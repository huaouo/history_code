#ifndef __EMSH_FS_DBLK
#define __EMSH_FS_DBLK

#include "const.h"

namespace emsh::fs {

struct DirEntry {
  char filename[MAX_FILENAME_LEN];
  uint16_t iaddr; // index of inode
};

struct DirBlk {
  DirEntry entries[DBLK_ENTRY_CNT];
};
}
#endif
