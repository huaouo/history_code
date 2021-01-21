#include <string.h>

#include "io.h"
#include "const.h"
#include "../util.h"

namespace emsh::fs {

IO::IO() : disk_stream(DISK_PATH, std::ios::binary | std::ios::in | std::ios::out),
           cache_size(CACHE_SIZE) {
  IF_INTERNAL_ERROR(cache_size == 0);
}

IO &IO::get_instance() {
  static IO instance = IO();
  return instance;
}

IO::~IO() {
  close();
}

IO::operator bool() {
  return !is_open();
}

bool IO::is_open() {
  return disk_stream.is_open();
}

void IO::close() {
  sync();
  disk_stream.close();
}

void IO::write_back(Blk *data, uint16_t block_id) {
  disk_stream.seekp(BLK_SIZE * block_id);
  disk_stream.write((char *) data, BLK_SIZE);
}

Blk IO::read_block(uint16_t block_id) {
  IF_INTERNAL_ERROR(!is_open());

  if (auto it = cache.find(block_id); it != cache.end()) {
    auto qit = std::get<1>(it->second);
    cache_q.erase(qit);
    cache_q.push_back(it->first);
    std::get<1>(it->second) = --cache_q.end();
    return std::get<0>(it->second);
  }

  Blk block{};
  disk_stream.seekg(BLK_SIZE * block_id);
  disk_stream.read((char *) &block, BLK_SIZE);

  if (cache_q.size() == cache_size) {
    uint16_t removed_id = cache_q.front();
    cache_q.pop_front();

    auto cached_data = cache.at(removed_id);
    if (std::get<2>(cached_data))
      write_back(&std::get<0>(cached_data), removed_id);
    cache.erase(removed_id);
  }

  cache_q.push_back(block_id);
  cache[block_id] = std::make_tuple(block, --cache_q.end(), false);
  return block;
}

void IO::write_block(Blk *data, uint16_t block_id) {
  IF_INTERNAL_ERROR(!is_open());

  auto it = cache.find(block_id);
  if (it == cache.end()) {
    read_block(block_id);
    it = cache.find(block_id);
  }

  std::get<0>(it->second) = *data;
  std::get<2>(it->second) = true;

  auto qit = std::get<1>(it->second);
  cache_q.erase(qit);
  cache_q.push_back(it->first);
  std::get<1>(it->second) = --cache_q.end();
}

void IO::clear_block(uint16_t block_id) {
  Blk blk = read_block(block_id);
  memset(&blk, 0, sizeof(blk));
  write_block(&blk, sizeof(blk));
}

void IO::sync() {
  for (auto &p : cache) {
    if (std::get<2>(p.second)) {
      write_back(&std::get<0>(p.second), p.first);
      std::get<2>(p.second) = false;
    }
  }
}
}
