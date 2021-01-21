#ifndef __EMSH_FS_FREEBLK
#define __EMSH_FS_FREEBLK

#include "const.h"

namespace emsh::fs {

  enum FREEBLK_AVAIL {
    AVAIL,
    UNAVAIL
  };

  struct FreeBlk {
    FREEBLK_AVAIL avail_status;
    uint16_t free_stack[FREEBLK_STACK_LEN];
    uint16_t stack_top;
    uint8_t _padding[250];
  };
}
#endif
