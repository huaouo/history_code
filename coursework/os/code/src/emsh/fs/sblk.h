#ifndef __EMSH_FS_SBLK
#define __EMSH_FS_SBLK

#include "const.h"

namespace emsh::fs {

struct SBlk {
  uint16_t magic;
  uint16_t dblk_avail;
  uint16_t inode_avail;
  uint16_t root_iaddr; // inode index of root
  uint16_t free_stack[FREEBLK_STACK_LEN];
  uint16_t stack_top;
  uint8_t inode_bitmap[126];
  uint8_t _padding[120];
};
}
#endif
