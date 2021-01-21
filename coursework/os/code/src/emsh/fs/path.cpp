#include "path.h"

namespace emsh::fs {

template <typename Iterable>
void Path::_concat_back(const Iterable& x) {
  for (const auto& i : x) {
    if (i == "..")
      pop_back();
    else if (i != ".")
      v.push_back(i);
  }
}

Path::Path(const std::string &abs_path) {
  auto splitted_abs_path = split_ignore_empty_slice(abs_path, '/');
  v.reserve(splitted_abs_path.size());
  _concat_back(splitted_abs_path);
}

Path::Path(const std::string &rel_path, const std::string &pwd) {
  auto splitted_pwd = split_ignore_empty_slice(pwd, '/');
  auto splitted_rel_path = split_ignore_empty_slice(rel_path, '/');
  v.reserve(splitted_pwd.size() + splitted_rel_path.size());
  _concat_back(splitted_pwd);
  _concat_back(splitted_rel_path);
}

bool Path::operator==(const Path &rhs) const {
  return v == rhs.v;
}

const std::string &Path::to_string() {
  if (!cache.empty())
    return cache;

  for (const auto &i : v) {
    cache.append("/");
    cache.append(i);
  }
  if (cache.empty())
    cache.append("/");
  return cache;
}

void Path::concat_back(const Path &rel_path) {
  cache.clear();
  v.reserve(v.size() + rel_path.size());
  _concat_back(rel_path);
}

void Path::concat_back(const std::string &rel_path) {
  cache.clear();
  auto splitted = split_ignore_empty_slice(rel_path, '/');
  v.reserve(v.size() + splitted.size());
  _concat_back(splitted);
}

void Path::pop_back() {
  if (!v.empty()) {
    cache.clear();
    v.pop_back();
  }
}

void Path::clear() {
  cache.clear();
  v.clear();
}

std::string &Path::back() {
  return v.back();
}

const std::string &Path::back() const {
  return v.back();
}

size_t Path::size() const {
  return v.size();
}

bool Path::empty() const {
  return size() == 0;
}

bool Path::begin_with(const Path &sub) const {
  if (sub.size() > size())
    return false;

  for (size_t i = 0; i < sub.size(); ++i) {
    if (v[i] != sub.v[i])
      return false;
  }
  return true;
}

Path::CIt Path::begin() const {
  return v.cbegin();
}

Path::CIt Path::end() const {
  return v.cend();
}
}