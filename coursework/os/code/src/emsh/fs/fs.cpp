#include <string.h>
#include <algorithm>

#include "fs.h"
#include "const.h"
#include "stream.h"
#include "dirblk.h"
#include "../util.h"

namespace emsh::fs {

FS::FS() : io(IO::get_instance()), free_blk_id(0) {
  if_fatal_error(!io.is_open(), "Fatal Error: 'disk' doesn't exist.");
  auto _sblk = io.read_block(SBLK_ID);
  sblk = *(SBlk *) &_sblk;
}

bool FS::can_alloc_dblk(uint16_t dblk_num) {
  return sblk.dblk_avail >= dblk_num + RESERVED_BLK_CNT;
}

bool FS::can_alloc_inode(uint16_t inode_num) {
  return sblk.inode_avail >= inode_num;
}

FS &FS::get_instance() {
  static FS fs = FS();
  return fs;
}

ItemInfo FS::get_iteminfo(uint16_t inode_id, const std::string &filename) {
  INode file_inode = read_inode(inode_id);
  return {file_inode.size,
          file_inode.lnk_cnt,
          file_inode.uid,
          file_inode.gid,
          file_inode.stamp,
          file_inode.status,
          filename};
}

SBlk FS::get_sblk() {
  return sblk;
}

std::pair<StatusCode, INode> FS::get_inode(const Path &path) {
  auto[code, inode_id] = find_file(path, 0, 0);
  if (code != SUCCESS)
    return std::make_pair(code, INode{});
  return std::make_pair(code, read_inode(inode_id));
}

// returns (StatusCode, inode_id)
std::pair<StatusCode, uint16_t> FS::find_file(const Path &path, emgid_t gid, emuid_t uid) {
  std::string t_inode_filename = "/";
  uint16_t t_inode_id = sblk.root_iaddr;
  const std::string blank;
  for (const auto &i : path) {
    INode t_inode = read_inode(t_inode_id);
    if (!((uid == 0)
        || (uid == t_inode.uid && t_inode.status.u_x)
        || (uid != t_inode.uid && gid == t_inode.gid && t_inode.status.g_x)
        || (uid != t_inode.uid && gid != t_inode.gid && t_inode.status.o_x)))
      return std::make_pair(PERMISSION_DENIED, t_inode_id);

    if (!t_inode.status.is_dir)
      return std::make_pair(NO_SUCH_FILE_OR_DIRECTORY, t_inode_id);

    Stream s(t_inode_id);
    DirBlk dir_blk{};
    while (true) {
      s.read((char *) &dir_blk, sizeof(dir_blk));
      for (const auto &entry : dir_blk.entries) {
        if (std::string(entry.filename).empty())
          return std::make_pair(NO_SUCH_FILE_OR_DIRECTORY, t_inode_id);

        if (i == entry.filename) {
          t_inode_id = entry.iaddr;
          t_inode_filename = entry.filename;
          goto out1;
        }
      }
    }
    out1:;
  }
  return std::make_pair(SUCCESS, t_inode_id);
}

INode FS::read_inode(uint16_t i) {
  IF_INTERNAL_ERROR(i >= INODE_CNT);

  auto blk_id = (uint16_t) (i / INODE_PER_BLK + IBLK_BEG_ID);
  auto offset = (uint16_t) (i % INODE_PER_BLK);

  Blk blk = io.read_block(blk_id);
  auto iblk = (IBlk *) &blk;
  return iblk->i[offset];
}

void FS::write_inode(INode *inode, uint16_t i) {
  IF_INTERNAL_ERROR(i >= INODE_CNT);

  auto blk_id = (uint16_t) (i / INODE_PER_BLK + IBLK_BEG_ID);
  auto offset = (uint16_t) (i % INODE_PER_BLK);

  Blk blk = io.read_block(blk_id);
  ((IBlk *) &blk)->i[offset] = *inode;
  io.write_block(&blk, blk_id);
}

void FS::reload_free_blk() {
  if (sblk.stack_top != 0)
    return;

  for (uint16_t id = DBLK_BEG_ID; id != LAST_FREEBLK_ID; id += DBLK_GROUP_SIZE) {
    auto blk = io.read_block(id);
    free_blk = *(FreeBlk *) &blk;
    free_blk_id = id;
    if (free_blk.stack_top != 0)
      break;
  }
}

uint16_t FS::alloc_inode() {
  uint16_t i = 0;
  int n, offset;
  bool fail = true;
  for (; i < INODE_CNT; ++i) {
    n = i / 8;
    offset = i % 8;
    if (!NTH_BIT(sblk.inode_bitmap[n], offset)) {
      SET_NTH_BIT(sblk.inode_bitmap[n], offset);
      fail = false;
      break;
    }
  }

  if (fail)
    return INODE_FAIL_ID;
  --sblk.inode_avail;
  io.write_block((Blk *) &sblk, SBLK_ID);
  return i;
}

void FS::free_inode(uint16_t i) {
  IF_INTERNAL_ERROR(i >= INODE_CNT);
  int n = i / 8;
  int offset = i % 8;
  CLEAR_NTH_BIT(sblk.inode_bitmap[n], offset);
  ++sblk.inode_avail;
  io.write_block((Blk *) &sblk, SBLK_ID);
}

uint16_t FS::alloc_dblk() {
  uint16_t id;
  if (sblk.stack_top != 0) {
    id = sblk.free_stack[sblk.stack_top--];
  } else {
    if (free_blk_id == 0 || free_blk.stack_top == 0)
      reload_free_blk();
    if (free_blk.stack_top == 0)
      return DBLK_FAIL_ID;
    id = free_blk.free_stack[free_blk.stack_top--];
    io.write_block((Blk *) &free_blk, free_blk_id);
  }
  --sblk.dblk_avail;
  io.write_block((Blk *) &sblk, SBLK_ID);

  auto blk = io.read_block(id);
  memset(&blk, 0, sizeof(blk));
  io.write_block(&blk, id);
  return id;
}

void FS::free_dblk(uint16_t i) {
  auto manage_blk_id = (uint16_t) ((i / DBLK_GROUP_SIZE - 1) * DBLK_GROUP_SIZE);
  if (manage_blk_id == 0) {
    sblk.free_stack[++sblk.stack_top] = i;
  } else {
    auto blk = io.read_block(manage_blk_id);
    auto fblk = (FreeBlk *) &blk;
    fblk->free_stack[++fblk->stack_top] = i;
    io.write_block(&blk, manage_blk_id);
  }
  ++sblk.dblk_avail;
  io.write_block((Blk *) &sblk, SBLK_ID);
}

StatusCode FS::dir_add_entry(uint16_t dir_inode_id, const DirEntry &entry) {
  Stream dir_stream(dir_inode_id);
  DirBlk dir_blk{};
  std::string add_filename(entry.filename);
  bool need_new_dir_blk = true;
  while (true) {
    if (dir_stream.read((char *) &dir_blk, sizeof(dir_blk)) == 0)
      break;

    for (auto &e : dir_blk.entries) {
      if (e.filename == add_filename)
        return FILE_EXISTS;

      if (e.filename[0] == '\0') {
        memcpy(&e, &entry, sizeof(entry));
        dir_stream.seekw(dir_stream.get_rpos() - sizeof(dir_blk));
        dir_stream.write((char *) &dir_blk, sizeof(dir_blk));
        need_new_dir_blk = false;
        goto out;
      }
    }
  }

  out:
  if (need_new_dir_blk) {
    memset(&dir_blk, 0, sizeof(dir_blk));
    memcpy(&dir_blk, &entry, sizeof(entry));
    dir_stream.seekw(dir_stream.size());
    if (dir_stream.write((char *) &dir_blk, sizeof(dir_blk)) == 0) {
      return RUN_OUT_OF_DBLK;
    }
  }

  auto t_inode = read_inode(dir_inode_id);
  ++t_inode.lnk_cnt;
  t_inode.stamp = time(nullptr);
  write_inode(&t_inode, dir_inode_id);
  return SUCCESS;
}

StatusCode FS::dir_remove_entry(uint16_t dir_inode_id, const std::string &filename) {
  Stream dir_stream(dir_inode_id);
  DirEntry entry{};
  std::vector<DirEntry> entry_buffer;
  while (true) {
    size_t entry_pos = dir_stream.get_rpos();
    if (dir_stream.read((char *) &entry, sizeof(entry)) == 0 || entry.filename[0] == '\0')
      return NO_SUCH_FILE_OR_DIRECTORY;
    if (filename == entry.filename) {
      while (dir_stream.read((char *) &entry, sizeof(entry))) {
        if (entry.filename[0] == '\0')
          break;
        else
          entry_buffer.push_back(entry);
      }

      dir_stream.seekw(entry_pos);
      for (const auto &e : entry_buffer)
        dir_stream.write((char *) &e, sizeof(e));
      DirEntry blank_entry{};
      dir_stream.write((char *) &blank_entry, sizeof(blank_entry));

      dir_stream.seekw(dir_stream.get_wpos() - sizeof(blank_entry));
      if (dir_stream.get_wpos() % BLK_SIZE == 0)
        dir_stream.truncate(dir_stream.size() - BLK_SIZE);

      auto t_inode = read_inode(dir_inode_id);
      --t_inode.lnk_cnt;
      t_inode.stamp = time(nullptr);
      write_inode(&t_inode, dir_inode_id);
      return SUCCESS;
    }
  }
}

std::pair<StatusCode, std::vector<ItemInfo>> FS::ls(const Path &path, emuid_t uid, emgid_t gid) {
  auto[status_code, t_inode_id] = find_file(path, uid, gid);
  std::vector<ItemInfo> result;
  if (status_code != SUCCESS)
    return std::make_pair(status_code, result);

  auto t_inode = read_inode(t_inode_id);
  if (t_inode.status.is_dir) {
    if (!((uid == 0)
        || (uid == t_inode.uid && t_inode.status.u_r)
        || (uid != t_inode.uid && gid == t_inode.gid && t_inode.status.g_r)
        || (uid != t_inode.uid && gid != t_inode.gid && t_inode.status.o_r)))
      return std::make_pair(PERMISSION_DENIED, result);

    DirBlk dir_blk{};
    Stream s(t_inode_id);
    while (true) {
      if (s.read((char *) &dir_blk, sizeof(dir_blk)) == 0)
        break;
      for (const auto &entry : dir_blk.entries) {
        if (entry.filename[0] == '\0')
          goto out2;
        result.push_back(get_iteminfo(entry.iaddr, entry.filename));
      }
    }
    out2:
    std::sort(result.begin(), result.end(), [](const ItemInfo &l, const ItemInfo &r) {
      return l.filename < r.filename;
    });
  } else {
    result.push_back({t_inode.size,
                      t_inode.lnk_cnt,
                      t_inode.uid,
                      t_inode.gid,
                      t_inode.stamp,
                      t_inode.status,
                      path.back()});
  }
  return std::make_pair(SUCCESS, result);
}

StatusCode FS::mkdir(const Path &path, IStatus umask, emuid_t uid, emgid_t gid) {
  if (!can_alloc_dblk(1))
    return RUN_OUT_OF_DBLK;

  if (path.empty())
    return FILE_EXISTS;

  if (path.back().size() >= MAX_FILENAME_LEN)
    return FILENAME_TOO_LONG;

  Path dir_in(path);
  dir_in.pop_back();
  auto[status_code, t_inode_id] = find_file(dir_in, uid, gid);
  if (status_code != SUCCESS)
    return status_code;
  auto t_inode = read_inode(t_inode_id);

  if (!((uid == 0)
      || (uid == t_inode.uid && t_inode.status.u_w && t_inode.status.u_x)
      || (uid != t_inode.uid && gid == t_inode.gid && t_inode.status.g_w && t_inode.status.g_x)
      || (uid != t_inode.uid && gid != t_inode.gid && t_inode.status.o_w && t_inode.status.o_x)))
    return PERMISSION_DENIED;

  uint16_t alloc_inode_id = alloc_inode();
  if (alloc_inode_id == INODE_FAIL_ID)
    return RUN_OUT_OF_INODE;

  uint16_t alloc_dblk_id = alloc_dblk();
  if (alloc_dblk_id == DBLK_FAIL_ID) {
    free_inode(alloc_inode_id);
    return RUN_OUT_OF_DBLK;
  }

  DirEntry entry{
      .iaddr = alloc_inode_id
  };
  memcpy(&entry.filename, path.back().c_str(), path.back().size() + 1);
  if (StatusCode code = dir_add_entry(t_inode_id, entry); code != SUCCESS) {
    free_dblk(alloc_dblk_id);
    free_inode(alloc_inode_id);
    return code;
  }

  uint16_t max_status_num = PRIVILEGE_MAX + IS_DIRECTORY;
  max_status_num &= ~*(uint16_t *) &umask;
  INode dir_inode{
      .size = BLK_SIZE,
      .lnk_cnt = 2,
      .stamp = time(nullptr),
      .uid = uid,
      .gid = gid,
      .d_addr = {alloc_dblk_id},
      .j_addr = 0,
      .jj_addr = 0,
      .status = *(IStatus *) &max_status_num
  };
  write_inode(&dir_inode, alloc_inode_id);

  DirBlk created_dir_blk{
      .entries = {
          {
              .filename = {'.'},
              .iaddr = alloc_inode_id
          },
          {
              .filename = {'.', '.'},
              .iaddr = t_inode_id
          }
      }
  };
  io.write_block((Blk *) &created_dir_blk, alloc_dblk_id);
  return SUCCESS;
}

StatusCode FS::rmdir(const Path &path, emuid_t uid, emgid_t gid) {
  if (path.empty())
    return PRESERVE_ROOT;

  auto[status_code, inode_id] = find_file(path, uid, gid);
  if (status_code != SUCCESS)
    return status_code;

  auto path_in = path;
  path_in.pop_back();
  auto [in_status_code, in_inode_id] = find_file(path_in, uid, gid);
  auto in_inode = read_inode(in_inode_id);
  if (!((uid == 0)
      || (uid == in_inode.uid && in_inode.status.u_w)
      || (uid != in_inode.uid && gid == in_inode.gid && in_inode.status.g_w)
      || (uid != in_inode.uid && gid != in_inode.gid && in_inode.status.o_w)))
    return PERMISSION_DENIED;

  auto inode = read_inode(inode_id);
  if (!inode.status.is_dir)
    return NOT_DIR;

  if (inode.lnk_cnt != 2)
    return DIR_NOT_EMPTY;

  Stream dir_s(inode_id);
  dir_s.truncate(0);
  free_inode(inode_id);

  dir_remove_entry(in_inode_id, path.back());
  return SUCCESS;
}

StatusCode FS::touch(const Path &path, IStatus umask, emuid_t uid, emgid_t gid) {
  auto[status_code, inode_id] = find_file(path, uid, gid);

  if (status_code == PERMISSION_DENIED) {
    return status_code;
  } else if (status_code == SUCCESS) {
    auto inode = read_inode(inode_id);
    inode.stamp = time(nullptr);
    write_inode(&inode, inode_id);
    return status_code;
  } else {
    auto path_in = path;
    path_in.pop_back();
    auto[_, in_inode_id] = find_file(path_in, uid, gid);
    auto in_inode = read_inode(in_inode_id);
    if (!((uid == 0)
        || (uid == in_inode.uid && in_inode.status.u_w)
        || (uid != in_inode.uid && gid == in_inode.gid && in_inode.status.g_w)
        || (uid != in_inode.uid && gid != in_inode.gid && in_inode.status.o_w)))
      return PERMISSION_DENIED;

    auto new_inode_id = alloc_inode();
    if (new_inode_id == INODE_FAIL_ID)
      return RUN_OUT_OF_INODE;

    uint16_t max_status_num = FILE_DEFAULT_PRIVILEGE;
    max_status_num &= ~*(uint16_t *) &umask;
    INode new_inode{
        .size = 0,
        .lnk_cnt = 1,
        .stamp = time(nullptr),
        .uid = uid,
        .gid = gid,
        .d_addr = {},
        .j_addr = 0,
        .jj_addr = 0,
        .status = *(IStatus *) &max_status_num
    };

    DirEntry entry{
        .iaddr = new_inode_id
    };
    memcpy(&entry.filename, path.back().c_str(), path.back().size() + 1);
    StatusCode code;
    if ((code = dir_add_entry(in_inode_id, entry)) == SUCCESS)
      write_inode(&new_inode, new_inode_id);
    else
      free_inode(new_inode_id);
    return code;
  }
}

StatusCode FS::rm(const Path &path, emuid_t uid, emgid_t gid) {
  auto[code, inode_id] = find_file(path, uid, gid);
  if (code != SUCCESS)
    return code;

  auto inode = read_inode(inode_id);
  if (inode.status.is_dir)
    return IS_DIR;

  auto path_in = path;
  path_in.pop_back();
  auto[in_code, in_inode_id] = find_file(path_in, uid, gid);
  auto in_inode = read_inode(in_inode_id);
  if (!((uid == 0)
      || (uid == in_inode.uid && in_inode.status.u_w)
      || (uid != in_inode.uid && gid == in_inode.gid && in_inode.status.g_w)
      || (uid != in_inode.uid && gid != in_inode.gid && in_inode.status.o_w)))
    return PERMISSION_DENIED;

  if (inode.lnk_cnt == 1) {
    Stream s(inode_id);
    s.truncate(0);
    free_inode(inode_id);
  } else {
    --inode.lnk_cnt;
    write_inode(&inode, inode_id);
  }
  dir_remove_entry(in_inode_id, path.back());
  return SUCCESS;
}

StatusCode FS::mv(const Path &dst, const Path &src, emuid_t uid, emgid_t gid) {
  if (dst == src)
    return SAME_FILE;
  if (dst.begin_with(src))
    return MOV_TO_SUB_DIR;

  auto[code_src, inode_id_src] = find_file(src, uid, gid);
  if (code_src != SUCCESS)
    return code_src;
  auto src_in = src;
  src_in.pop_back();
  auto[in_code_src, in_inode_id_src] = find_file(src_in, uid, gid);
  auto in_inode_src = read_inode(in_inode_id_src);
  if (!((uid == 0)
      || (uid == in_inode_src.uid && in_inode_src.status.u_w)
      || (uid != in_inode_src.uid && gid == in_inode_src.gid && in_inode_src.status.g_w)
      || (uid != in_inode_src.uid && gid != in_inode_src.gid && in_inode_src.status.o_w)))
    return PERMISSION_DENIED;

  auto[code_dst, inode_id_dst] = find_file(dst, uid, gid);
  uint16_t add_entry_inode_id;
  bool dst_is_dir = false;
  if (code_dst == PERMISSION_DENIED)
    return code_dst;

  if (code_dst == SUCCESS) {
    auto inode_dst = read_inode(inode_id_dst);
    if (!inode_dst.status.is_dir)
      return FILE_EXISTS;
    add_entry_inode_id = inode_id_dst;
    dst_is_dir = true;
  } else { // code_dst == NO_SUCH_FILE_OR_DIRECTORY
    auto dst_in = dst;
    dst_in.pop_back();
    auto[in_code_dst, in_inode_id_dst] = find_file(dst_in, uid, gid);
    if (in_code_dst != SUCCESS)
      return in_code_dst;
    auto in_inode_dst = read_inode(in_inode_id_dst);
    if (!in_inode_dst.status.is_dir)
      return NOT_DIR;
    add_entry_inode_id = in_inode_id_dst;
  }

  auto add_entry_inode = read_inode(add_entry_inode_id);
  if (!((uid == 0)
      || (uid == add_entry_inode.uid && add_entry_inode.status.u_w)
      || (uid != add_entry_inode.uid && gid == add_entry_inode.gid && add_entry_inode.status.g_w)
      || (uid != add_entry_inode.uid && gid != add_entry_inode.gid && add_entry_inode.status.o_w)))
    return PERMISSION_DENIED;

  DirEntry entry_to_add{
      .iaddr = inode_id_src
  };
  if (dst_is_dir)
    memcpy(entry_to_add.filename, src.back().c_str(), src.back().size() + 1);
  else
    memcpy(entry_to_add.filename, dst.back().c_str(), dst.back().size() + 1);

  if (auto code = dir_add_entry(add_entry_inode_id, entry_to_add); code != SUCCESS)
    return code;
  dir_remove_entry(in_inode_id_src, src.back());
  return SUCCESS;
}

StatusCode FS::cp(const Path &dst, const Path &src, emuid_t uid, emgid_t gid) {
  if (dst == src)
    return SAME_FILE;

  auto[src_code, data] = cat(src, uid, gid);
  if (src_code != SUCCESS)
    return src_code;

  auto[code_dst, inode_id_dst] = find_file(dst, uid, gid);
  uint16_t add_entry_inode_id;
  bool dst_is_dir = false;
  if (code_dst == PERMISSION_DENIED)
    return code_dst;

  if (code_dst == SUCCESS) {
    auto inode_dst = read_inode(inode_id_dst);
    if (!inode_dst.status.is_dir)
      return FILE_EXISTS;
    add_entry_inode_id = inode_id_dst;
    dst_is_dir = true;
  } else { // code_dst == NO_SUCH_FILE_OR_DIRECTORY
    auto dst_in = dst;
    dst_in.pop_back();
    auto[in_code_dst, in_inode_id_dst] = find_file(dst_in, uid, gid);
    if (in_code_dst != SUCCESS)
      return in_code_dst;
    auto in_inode_dst = read_inode(in_inode_id_dst);
    if (!in_inode_dst.status.is_dir)
      return NOT_DIR;
    add_entry_inode_id = in_inode_id_dst;
  }

  auto add_entry_inode = read_inode(add_entry_inode_id);
  if (!((uid == 0)
      || (uid == add_entry_inode.uid && add_entry_inode.status.u_w)
      || (uid != add_entry_inode.uid && gid == add_entry_inode.gid && add_entry_inode.status.g_w)
      || (uid != add_entry_inode.uid && gid != add_entry_inode.gid && add_entry_inode.status.o_w)))
    return PERMISSION_DENIED;

  auto new_inode_id = alloc_inode();
  if (new_inode_id == INODE_FAIL_ID)
    return RUN_OUT_OF_INODE;

  auto[_, src_inode_id] = find_file(src, uid, gid);
  auto src_inode = read_inode(src_inode_id);

  INode new_inode = {
      .size = 0,
      .lnk_cnt = 1,
      .stamp = time(nullptr),
      .uid = src_inode.uid,
      .gid = src_inode.gid,
      .d_addr = {},
      .j_addr = 0,
      .jj_addr = 0,
      .status = src_inode.status
  };

  DirEntry entry_to_add{
      .iaddr = new_inode_id
  };

  if (dst_is_dir)
    memcpy(entry_to_add.filename, src.back().c_str(), src.back().size() + 1);
  else
    memcpy(entry_to_add.filename, dst.back().c_str(), dst.back().size() + 1);

  if (auto code = dir_add_entry(add_entry_inode_id, entry_to_add); code != SUCCESS) {
    free_inode(new_inode_id);
    return code;
  }

  write_inode(&new_inode, new_inode_id);
  Stream s(new_inode_id);
  if (s.write(data.c_str(), data.size()) != data.size()) {
    free_inode(new_inode_id);
    return RUN_OUT_OF_DBLK;
  }
  return SUCCESS;
}

StatusCode FS::ln(const Path &dst, const Path &src, emuid_t uid, emgid_t gid) {
  if (dst == src)
    return SAME_FILE;

  auto[code_src, inode_id_src] = find_file(src, uid, gid);
  if (code_src != SUCCESS)
    return code_src;
  auto inode_src = read_inode(inode_id_src);
  if (inode_src.status.is_dir)
    return HARDLINK_NOT_ALLOWED_FOR_DIR;

  auto[code_dst, inode_id_dst] = find_file(dst, uid, gid);
  uint16_t add_entry_inode_id;
  bool dst_is_dir = false;
  if (code_dst == PERMISSION_DENIED)
    return code_dst;

  if (code_dst == SUCCESS) {
    auto inode_dst = read_inode(inode_id_dst);
    if (!inode_dst.status.is_dir)
      return FILE_EXISTS;
    add_entry_inode_id = inode_id_dst;
    dst_is_dir = true;
  } else { // code_dst == NO_SUCH_FILE_OR_DIRECTORY
    auto dst_in = dst;
    dst_in.pop_back();
    auto[in_code_dst, in_inode_id_dst] = find_file(dst_in, uid, gid);
    if (in_code_dst != SUCCESS)
      return in_code_dst;
    auto in_inode_dst = read_inode(in_inode_id_dst);
    if (!in_inode_dst.status.is_dir)
      return NOT_DIR;
    add_entry_inode_id = in_inode_id_dst;
  }

  auto add_entry_inode = read_inode(add_entry_inode_id);
  if (!((uid == 0)
      || (uid == add_entry_inode.uid && add_entry_inode.status.u_w)
      || (uid != add_entry_inode.uid && gid == add_entry_inode.gid && add_entry_inode.status.g_w)
      || (uid != add_entry_inode.uid && gid != add_entry_inode.gid && add_entry_inode.status.o_w)))
    return PERMISSION_DENIED;

  DirEntry entry_to_add{
      .iaddr = inode_id_src
  };
  if (dst_is_dir)
    memcpy(entry_to_add.filename, src.back().c_str(), src.back().size() + 1);
  else
    memcpy(entry_to_add.filename, dst.back().c_str(), dst.back().size() + 1);
  if (auto code = dir_add_entry(add_entry_inode_id, entry_to_add); code != SUCCESS)
    return code;
  inode_src.lnk_cnt++;
  write_inode(&inode_src, inode_id_src);
  return SUCCESS;
}

StatusCode FS::chown(const Path &path, emuid_t to_uid, emuid_t uid, emgid_t gid) {
  if (uid != 0)
    return PERMISSION_DENIED;

  auto[code, inode_id] = find_file(path, uid, gid);
  if (code != SUCCESS)
    return code;

  auto inode = read_inode(inode_id);
  inode.uid = to_uid;
  write_inode(&inode, inode_id);
  return SUCCESS;
}

StatusCode FS::chgrp(const Path &path, emgid_t to_gid, emuid_t uid, emgid_t gid) {
  auto[code, inode_id] = find_file(path, uid, gid);
  if (code != SUCCESS)
    return code;

  auto inode = read_inode(inode_id);
  if (!(uid == 0 || uid == inode.uid))
    return PERMISSION_DENIED;

  inode.gid = to_gid;
  write_inode(&inode, inode_id);
  return SUCCESS;
}

StatusCode FS::chmod(const Path &path, IStatus mode, emuid_t uid, emgid_t gid) {
  auto[code, inode_id] = find_file(path, uid, gid);
  if (code != SUCCESS)
    return code;

  auto inode = read_inode(inode_id);
  if (!(uid == 0 || uid == inode.uid))
    return PERMISSION_DENIED;

  if (inode.status.is_dir)
    mode.is_dir = true;
  inode.status = mode;
  write_inode(&inode, inode_id);
  return SUCCESS;
}

StatusCode FS::append_str(const Path &path, const std::string &str, emuid_t uid, emgid_t gid) {
  auto[code, inode_id] = find_file(path, uid, gid);
  if (code != SUCCESS)
    return code;

  auto inode = read_inode(inode_id);
  if (inode.status.is_dir)
    return IS_DIR;
  if (!((uid == 0)
      || (uid == inode.uid && inode.status.u_w)
      || (uid != inode.uid && gid == inode.gid && inode.status.g_w)
      || (uid != inode.uid && gid == inode.gid && inode.status.o_w)))
    return PERMISSION_DENIED;

  Stream s(inode_id);
  s.seekw(s.size());
  if (s.write(str.c_str(), str.size()) == 0) {
    return RUN_OUT_OF_DBLK;
  }
  return SUCCESS;
}

StatusCode FS::append_n_star(const Path &path, size_t n, emuid_t uid, emgid_t gid) {
  std::string str(n, '*');
  auto[code, inode_id] = find_file(path, uid, gid);
  if (code != SUCCESS)
    return code;

  auto inode = read_inode(inode_id);
  if (inode.status.is_dir)
    return IS_DIR;
  if (!((uid == 0)
      || (uid == inode.uid && inode.status.u_w)
      || (uid != inode.uid && gid == inode.gid && inode.status.g_w)
      || (uid != inode.uid && gid == inode.gid && inode.status.o_w)))
    return PERMISSION_DENIED;

  Stream s(inode_id);
  s.seekw(s.size());
  if (s.write(str.c_str(), str.size()) == 0) {
    return RUN_OUT_OF_DBLK;
  }
  return SUCCESS;
}

std::pair<StatusCode, std::string> FS::cat(const Path &path, emuid_t uid, emgid_t gid) {
  auto[code, inode_id] = find_file(path, uid, gid);
  if (code != SUCCESS)
    return std::make_pair(code, "");

  auto inode = read_inode(inode_id);
  if (inode.status.is_dir)
    return std::make_pair(IS_DIR, "");

  if (!((uid == 0)
      || (uid == inode.uid && inode.status.u_r)
      || (uid != inode.uid && gid == inode.gid && inode.status.g_r)
      || (uid != inode.uid && gid != inode.gid && inode.status.o_r)))
    return std::make_pair(PERMISSION_DENIED, "");

  Stream s(inode_id);
  std::string result;

  char buf[BLK_SIZE + 1];
  while (true) {
    memset(buf, 0, sizeof(buf));
    size_t read_cnt = s.read(buf, BLK_SIZE);
    result.append(buf);
    if (read_cnt != BLK_SIZE)
      break;
  }

  return std::make_pair(SUCCESS, result);
}
}