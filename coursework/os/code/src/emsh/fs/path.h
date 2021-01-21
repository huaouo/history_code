#ifndef __EMSH_FS_PATH
#define __EMSH_FS_PATH

#include <vector>
#include <string>

#include "../util.h"

namespace emsh::fs {

class Path {
  std::vector<std::string> v;
  std::string cache;

  template <typename Iterable>
  void _concat_back(const Iterable& x);
 public:
  using CIt = decltype(v)::const_iterator;
  Path() = default;
  explicit Path(const std::string &abs_path);
  Path(const std::string &rel_path, const std::string &pwd);
  bool operator==(const Path &rhs) const;
  const std::string& to_string();
  void concat_back(const Path& rel_path);
  void concat_back(const std::string& rel_path);
  void pop_back();
  void clear();
  std::string& back();
  const std::string& back() const;
  size_t size() const;
  bool empty() const;
  bool begin_with(const Path &sub) const;
  CIt begin() const;
  CIt end() const;
};
}

#endif
