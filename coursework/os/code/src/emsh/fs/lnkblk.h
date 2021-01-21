#ifndef __EMSH_FS_LNKBLK
#define __EMSH_FS_LNKBLK

#include <stdint.h>

#include "const.h"

namespace emsh::fs {

struct LnkBlk {
  uint16_t addr[LNK_ENTRY_PER_BLK];
};
}
#endif
