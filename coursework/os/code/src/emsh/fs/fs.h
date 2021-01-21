#ifndef __EMSH_FS_FS
#define __EMSH_FS_FS

#include "io.h"
#include "sblk.h"
#include "iblk.h"
#include "path.h"
#include "dirblk.h"
#include "freeblk.h"
#include "iteminfo.h"

namespace emsh::fs {

class FS {
  friend class Stream;
 public:
  using FilePair = std::pair<std::string, uint16_t>;
 private:
  IO &io;
  SBlk sblk;
  uint16_t free_blk_id;
  FreeBlk free_blk{};

 public:
  void reload_free_blk();
  INode read_inode(uint16_t i); // i: index of inode
  void write_inode(INode *inode, uint16_t i); // i: index of inode
  uint16_t alloc_inode(); // return index of inode, different from blk_id
  void free_inode(uint16_t i);
  uint16_t alloc_dblk(); // return blk_id
  void free_dblk(uint16_t i);
  StatusCode dir_add_entry(uint16_t dir_inode_id, const DirEntry &entry);
  StatusCode dir_remove_entry(uint16_t dir_inode_id, const std::string &filename);
  explicit FS();
 public:
  FS(const FS &) = delete;
  FS(FS &&) = delete;
  FS operator=(const FS &) = delete;
  FS operator=(FS &&) = delete;

  static FS &get_instance();
  ItemInfo get_iteminfo(uint16_t inode_id, const std::string &filename);
  bool can_alloc_dblk(uint16_t dblk_num);
  bool can_alloc_inode(uint16_t inode_num);
  std::pair<StatusCode, uint16_t> find_file(const Path &path, emgid_t gid, emuid_t uid);
  std::pair<StatusCode, std::vector<ItemInfo>> ls(const Path &path, emuid_t uid, emgid_t gid);
  StatusCode mkdir(const Path &path, IStatus umask, emuid_t uid, emgid_t gid);
  StatusCode rmdir(const Path &path, emuid_t uid, emgid_t gid);
  StatusCode touch(const Path &path, IStatus umask, emuid_t uid, emgid_t gid);
  StatusCode rm(const Path &path, emuid_t uid, emgid_t gid);
  StatusCode mv(const Path &dst, const Path &src, emuid_t uid, emgid_t gid);
  StatusCode cp(const Path &dst, const Path &src, emuid_t uid, emgid_t gid);
  StatusCode ln(const Path &dst, const Path &src, emuid_t uid, emgid_t gid);
  StatusCode chown(const Path &path, emuid_t to_uid, emuid_t uid, emgid_t gid);
  StatusCode chgrp(const Path &path, emgid_t to_gid, emuid_t uid, emgid_t gid);
  StatusCode chmod(const Path &path, IStatus mode, emuid_t uid, emgid_t gid);
  StatusCode append_str(const Path &path, const std::string &str, emuid_t uid, emgid_t gid);
  StatusCode append_n_star(const Path &path, size_t n, emuid_t uid, emgid_t gid);
  std::pair<StatusCode, std::string> cat(const Path &path, emuid_t uid, emgid_t gid);

  // use for demonstration
  SBlk get_sblk();
  std::pair<StatusCode, INode> get_inode(const Path &path);
};
}
#endif
