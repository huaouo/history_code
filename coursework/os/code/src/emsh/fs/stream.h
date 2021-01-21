#ifndef __EMSH_FS_STREAM
#define __EMSH_FS_STREAM

#include <stddef.h>

#include "io.h"
#include "fs.h"
#include "iblk.h"

namespace emsh::fs {

class Stream {
  IO& io;
  FS& fs;
  INode inode;
  size_t read_pos;
  size_t write_pos;
  uint16_t iaddr;
 public:
  explicit Stream(uint16_t iaddr);
  bool truncate(size_t len);
  size_t write(const char *data, size_t len);
  size_t read(char *data, size_t len);
  void seekw(size_t pos);
  void seekr(size_t pos);
  size_t get_wpos();
  size_t get_rpos();
  size_t size();
};
}
#endif
