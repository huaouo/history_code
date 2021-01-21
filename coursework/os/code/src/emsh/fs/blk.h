#ifndef __EMSH_FS_BLK
#define __EMSH_FS_BLK

#include <stdint.h>

namespace emsh::fs {

struct Blk {
  uint8_t data[512];
};
}
#endif
