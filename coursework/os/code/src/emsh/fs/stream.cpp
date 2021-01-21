#include <math.h>
#include <string.h>

#include "stream.h"
#include "lnkblk.h"

namespace emsh::fs {

Stream::Stream(const uint16_t iaddr)
    : io(IO::get_instance()), fs(FS::get_instance()),
      iaddr(iaddr), inode(fs.read_inode(iaddr)), read_pos(0), write_pos(0) {}

bool Stream::truncate(size_t len) {
  if (len > inode.size)
    return false;

  auto original_blk_index = static_cast<int>(inode.size / BLK_SIZE);
  if (inode.size % BLK_SIZE == 0)
    --original_blk_index;
  auto target_blk_index = static_cast<int>(len / BLK_SIZE);
  if (len % BLK_SIZE == 0)
    --target_blk_index;

  if (target_blk_index < original_blk_index) {
    for (auto blk_index = original_blk_index; blk_index > target_blk_index; --blk_index) {
      if (blk_index < INODE_DADDR_BLK_CNT) {
        fs.free_dblk(inode.d_addr[blk_index]);
        inode.d_addr[blk_index] = 0;
      } else if (blk_index < INODE_DJADDR_BLK_CNT) {
        Blk _j_blk = io.read_block(inode.j_addr);
        auto j_blk = (LnkBlk *) &_j_blk;
        fs.free_dblk(j_blk->addr[blk_index - INODE_DADDR_BLK_CNT]);
        j_blk->addr[blk_index - INODE_DADDR_BLK_CNT] = 0;
        io.write_block(&_j_blk, inode.j_addr);

        if (blk_index == INODE_DADDR_BLK_CNT) {
          fs.free_dblk(inode.j_addr);
          inode.j_addr = 0;
        }
      } else {
        auto blk_index_remain = static_cast<uint16_t>(blk_index - INODE_DJADDR_BLK_CNT);
        auto blk_index_id = static_cast<uint16_t>(blk_index_remain / LNK_ENTRY_PER_BLK);
        auto blk_index_offset = static_cast<uint16_t>(blk_index_remain % LNK_ENTRY_PER_BLK);
        Blk _j_blk = io.read_block(inode.jj_addr);
        auto j_blk = (LnkBlk *) &_j_blk;
        Blk _jj_blk = io.read_block(j_blk->addr[blk_index_id]);
        auto jj_blk = (LnkBlk *) &_jj_blk;

        fs.free_dblk(jj_blk->addr[blk_index_offset]);
        jj_blk->addr[blk_index_offset] = 0;
        io.write_block(&_jj_blk, j_blk->addr[blk_index_id]);

        if (blk_index_offset == 0) {
          fs.free_dblk(j_blk->addr[blk_index_id]);
          j_blk->addr[blk_index_id] = 0;
          io.write_block(&_j_blk, inode.jj_addr);
        }

        if (blk_index_remain == 0) {
          fs.free_dblk(inode.jj_addr);
          inode.jj_addr = 0;
        }
      }
    }
  }
  inode.size = len;
  inode.stamp = time(nullptr);
  fs.write_inode(&inode, iaddr);
  return true;
}

// If cannot write the whole len bytes, it just prevent writing
// any bytes and return 0
size_t Stream::write(const char *data, size_t len) {
  if (!fs.can_alloc_dblk(static_cast<uint16_t>(len/BLK_SIZE)))
    return 0;

  size_t data_pos = 0;

  auto current_blk_left_bytes = write_pos - write_pos / BLK_SIZE * BLK_SIZE;
  auto new_alloc_blk_num = static_cast<uint16_t>(
      ceil(static_cast<double>(len - current_blk_left_bytes) / BLK_SIZE));
  if (!fs.can_alloc_dblk(new_alloc_blk_num))
    return 0;

  while (true) {
    bool alloc_fail = false;
    auto blk_index = static_cast<uint16_t>(write_pos / BLK_SIZE);
    auto offset = static_cast<uint16_t>(write_pos % BLK_SIZE);
    auto write_cycle_size = static_cast<uint16_t>(
        std::min(static_cast<size_t>(BLK_SIZE - offset), len - data_pos));
    if (write_cycle_size == 0)
      break;

    uint16_t blk_id = 0;
    if (blk_index < INODE_DADDR_BLK_CNT) {
      if ((blk_id = inode.d_addr[blk_index]) == 0) {
        uint16_t new_blk_id = fs.alloc_dblk();
        if (new_blk_id == DBLK_FAIL_ID) {
          alloc_fail = true;
        } else {
          blk_id = inode.d_addr[blk_index] = new_blk_id;
          fs.write_inode(&inode, iaddr);
        }
      }
    } else if (blk_index < INODE_DJADDR_BLK_CNT) {
      bool prev_alloc = false;
      if (inode.j_addr == 0) {
        uint16_t new_j_addr = fs.alloc_dblk();
        if (new_j_addr == DBLK_FAIL_ID) {
          alloc_fail = true;
        } else {
          prev_alloc = true;
          inode.j_addr = new_j_addr;
          fs.write_inode(&inode, iaddr);
          io.clear_block(new_j_addr);
        }
      }

      if (!alloc_fail) {
        Blk _j_blk = io.read_block(inode.j_addr);
        auto j_blk = (LnkBlk *) &_j_blk;
        if ((blk_id = j_blk->addr[blk_index - INODE_DADDR_BLK_CNT]) == 0) {
          uint16_t new_blk_id = fs.alloc_dblk();
          if (new_blk_id == DBLK_FAIL_ID) {
            if (prev_alloc) {
              fs.free_dblk(inode.j_addr);
              inode.j_addr = 0;
              fs.write_inode(&inode, iaddr);
            }
            alloc_fail = true;
          } else {
            blk_id = j_blk->addr[blk_index - INODE_DADDR_BLK_CNT] = new_blk_id;
            io.write_block(&_j_blk, inode.j_addr);
          }
        }
      }
    } else {
      bool pprev_alloc = false, prev_alloc = false;
      auto blk_index_remain = static_cast<uint16_t>(blk_index - INODE_DJADDR_BLK_CNT);
      auto blk_index_id = static_cast<uint16_t>(blk_index_remain / LNK_ENTRY_PER_BLK);
      auto blk_index_offset = static_cast<uint16_t>(blk_index_remain % LNK_ENTRY_PER_BLK);

      if (inode.jj_addr == 0) {
        uint16_t new_j_blk_id = fs.alloc_dblk();
        if (new_j_blk_id == DBLK_FAIL_ID) {
          alloc_fail = true;
        } else {
          inode.jj_addr = new_j_blk_id;
          fs.write_inode(&inode, iaddr);
          io.clear_block(new_j_blk_id);
          pprev_alloc = true;
        }
      }

      Blk _j_blk{};
      auto j_blk = (LnkBlk *) &_j_blk;
      if (!alloc_fail) {
        _j_blk = io.read_block(inode.jj_addr);
        if (j_blk->addr[blk_index_id] == 0) {
          uint16_t new_jj_blk_id = fs.alloc_dblk();
          if (new_jj_blk_id == DBLK_FAIL_ID) {
            if (pprev_alloc) {
              fs.free_dblk(inode.jj_addr);
              inode.jj_addr = 0;
              fs.write_inode(&inode, iaddr);
            }
            alloc_fail = true;
          } else {
            j_blk->addr[blk_index_id] = new_jj_blk_id;
            io.write_block(&_j_blk, inode.jj_addr);
            io.clear_block(new_jj_blk_id);
            prev_alloc = true;
          }
        }
      }

      if (!alloc_fail) {
        Blk _jj_blk = io.read_block(j_blk->addr[blk_index_id]);
        auto jj_blk = (LnkBlk *) &_jj_blk;
        if ((blk_id = jj_blk->addr[blk_index_offset]) == 0) {
          uint16_t new_blk_id = fs.alloc_dblk();
          if (new_blk_id == DBLK_FAIL_ID) {
            if (prev_alloc) {
              fs.free_dblk(j_blk->addr[blk_index_id]);
              j_blk->addr[blk_index_id] = 0;
              io.write_block(&_j_blk, inode.jj_addr);
            }
            if (pprev_alloc) {
              fs.free_dblk(inode.jj_addr);
              inode.jj_addr = 0;
              fs.write_inode(&inode, iaddr);
            }
            alloc_fail = true;
          } else {
            blk_id = jj_blk->addr[blk_index_offset] = new_blk_id;
            io.write_block(&_jj_blk, j_blk->addr[blk_index_id]);
          }
        }
      }
    }

    if (alloc_fail)
      break;

    //TODO:: exam this;
    IF_INTERNAL_ERROR(blk_id == 0);
    Blk blk = io.read_block(blk_id);
    memcpy((char *) &blk + offset, data + data_pos, write_cycle_size);
    io.write_block(&blk, blk_id);
    write_pos += write_cycle_size;
    data_pos += write_cycle_size;
    if (write_pos > inode.size)
      inode.size = write_pos;
  }
  inode.stamp = time(nullptr);
  fs.write_inode(&inode, iaddr);
  return data_pos;
}

size_t Stream::read(char *data, size_t len) {
  size_t data_pos = 0;

  while (true) {
    auto blk_index = static_cast<uint16_t>(read_pos / BLK_SIZE);
    auto offset = static_cast<uint16_t>(read_pos % BLK_SIZE);
    auto read_cycle_size = static_cast<uint16_t>(
        std::min(static_cast<size_t>(BLK_SIZE - offset),
                 std::min(inode.size - read_pos, len - data_pos)));
    if (read_cycle_size == 0)
      break;

    Blk blk{};
    if (blk_index < INODE_DADDR_BLK_CNT) {
      blk = io.read_block(inode.d_addr[blk_index]);
    } else if (blk_index < INODE_DJADDR_BLK_CNT) {
      Blk _j_blk = io.read_block(inode.j_addr);
      auto j_blk = (LnkBlk *) &_j_blk;
      blk = io.read_block(j_blk->addr[blk_index - INODE_DADDR_BLK_CNT]);
    } else {
      auto blk_index_remain = static_cast<uint16_t>(blk_index - INODE_DJADDR_BLK_CNT);
      auto blk_index_id = static_cast<uint16_t>(blk_index_remain / LNK_ENTRY_PER_BLK);
      auto blk_index_offset = static_cast<uint16_t>(blk_index_remain % LNK_ENTRY_PER_BLK);

      Blk _j_blk = io.read_block(inode.jj_addr);
      auto j_blk = (LnkBlk *) &_j_blk;
      Blk _jj_blk = io.read_block(j_blk->addr[blk_index_id]);
      auto jj_blk = (LnkBlk *) &_jj_blk;
      blk = io.read_block(jj_blk->addr[blk_index_offset]);
    }

    memcpy(data + data_pos, (char *) &blk + offset, read_cycle_size);
    read_pos += read_cycle_size;
    data_pos += read_cycle_size;
  }
  return data_pos;
}

void Stream::seekw(size_t pos) {
  if (pos > inode.size)
    pos = inode.size;
  write_pos = pos;
}

void Stream::seekr(size_t pos) {
  if (pos >= inode.size)
    pos = inode.size;
  read_pos = pos;
}

size_t Stream::get_wpos() {
  return write_pos;
}

size_t Stream::get_rpos() {
  return read_pos;
}

size_t Stream::size() {
  return inode.size;
}
}