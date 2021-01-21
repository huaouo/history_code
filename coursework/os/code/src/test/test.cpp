#define CATCH_CONFIG_MAIN

#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "catch.hpp"
#include "../emsh/util.h"
#include "../emsh/fs/iblk.h"
#include "../emsh/fs/dirblk.h"
#include "../emsh/fs/sblk.h"
#include "../emsh/fs/io.h"
#include "../emsh/fs/blk.h"
#include "../emsh/fs/freeblk.h"
#include "../emsh/fs/fs.h"

using namespace std;
using namespace emsh;
using namespace emsh::fs;

TEST_CASE("fs_test", "[alloc]") {
  FS& fs = FS::get_instance();
  uint16_t blk_id;
  set<uint16_t> s;

  while((blk_id = fs.alloc_dblk()) != DBLK_FAIL_ID) {
    if (s.find(blk_id) != s.end())
      std::cerr << "Fuck you" << std::endl;
    s.insert(blk_id);
  }

  auto size = s.size();
  for (auto i : s) {
    fs.free_dblk(i);
  }

  std::cout << "************************************" << std::endl;
  std::cout << "size = " << size << std::endl;
}