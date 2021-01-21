#ifndef __EMSH_FS_IO
#define __EMSH_FS_IO

#include <map>
#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <fstream>

#include "blk.h"
#include "const.h"

namespace emsh::fs {

class IO {
  using QIt = std::list<uint16_t>::iterator;
  using CacheIt = std::map<uint16_t, std::tuple<Blk, QIt, bool>>::iterator;

  const size_t cache_size;
  std::fstream disk_stream;
  std::map<uint16_t, std::tuple<Blk, QIt, bool>> cache;
  std::list<uint16_t> cache_q;

  void write_back(Blk *data, uint16_t block_id);
  IO();
 public:
  IO(const IO &) = delete;
  IO(IO &&) = delete;
  IO operator=(const IO &) = delete;
  IO operator=(IO &&) = delete;

  static IO &get_instance();
  ~IO();
  explicit operator bool();
  bool is_open();
  void close();
  Blk read_block(uint16_t block_id);
  void write_block(Blk *data, uint16_t block_id);
  void clear_block(uint16_t block_id);
  void sync();
};
}
#endif