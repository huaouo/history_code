#ifndef __EMSH_FS_IBLK
#define __EMSH_FS_IBLK

#include "../util.h"
#include "const.h"

namespace emsh::fs {

struct IStatus {
  bool o_x: 1;
  bool o_w: 1;
  bool o_r: 1;
  bool g_x: 1;
  bool g_w: 1;
  bool g_r: 1;
  bool u_x: 1;
  bool u_w: 1;
  bool u_r: 1;
  bool is_dir: 1;
};

struct INode {
  uint64_t size;
  uint64_t lnk_cnt;
  time_t stamp;
  emuid_t uid;
  emgid_t gid;
  uint16_t d_addr[INODE_DADDR_CNT]; // direct
  uint16_t j_addr; // means jump
  uint16_t jj_addr; // means jump jump ...
  IStatus status;
  uint8_t _padding[16];
};

struct IBlk {
  INode i[8];
};
}
#endif
