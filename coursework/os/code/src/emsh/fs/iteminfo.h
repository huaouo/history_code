#ifndef __EMSH_FS_ITEMINFO
#define __EMSH_FS_ITEMINFO

#include <string>

#include "iblk.h"

namespace emsh::fs {

struct ItemInfo {
  size_t size;
  size_t lnk_cnt;
  emuid_t uid;
  emgid_t gid;
  time_t stamp;
  IStatus status;
  std::string filename;
};
}
#endif
