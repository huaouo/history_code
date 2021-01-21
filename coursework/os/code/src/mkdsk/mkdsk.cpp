#include <time.h>
#include <stdlib.h>

#include <fstream>

#include "../emsh/fs/const.h"
#include "../emsh/fs/io.h"
#include "../emsh/fs/sblk.h"
#include "../emsh/fs/iblk.h"
#include "../emsh/fs/freeblk.h"
#include "../emsh/fs/dirblk.h"
#include "../emsh/fs/fs.h"

using namespace emsh::fs;

int main() {
  system("fallocate -l 32MiB disk");

  IO &io = IO::get_instance();
  SBlk sblk{
      .magic = SBLK_MAGIC,
      .dblk_avail = DBLK_CNT - 1,
      .inode_avail = INODE_CNT - 1,
      .root_iaddr = 0,
      .stack_top = FREEBLK_STACK_LEN - 2,
      .inode_bitmap = {1},
  };
  for (uint16_t i = 0; i < 128; ++i)
    sblk.free_stack[i] = (uint16_t) (i + DBLK_BEG_ID);
  io.write_block((Blk *) &sblk, SBLK_ID);

  uint16_t status_num = 01755;
  IBlk iblk{
      .i = {
          {
              .size = BLK_SIZE,
              .lnk_cnt = 2,
              .stamp = time(nullptr),
              .uid = 0,
              .gid = 0,
              .d_addr = {127 + DBLK_BEG_ID},
              .j_addr = 0,
              .jj_addr = 0,
              .status = *(IStatus *) &status_num
          }
      }
  };
  io.write_block((Blk *) &iblk, IBLK_BEG_ID);

  FreeBlk fblk{
      .avail_status = AVAIL,
      .stack_top = FREEBLK_STACK_LEN - 1
  };

  for (uint16_t id = DBLK_BEG_ID; id < LAST_FREEBLK_ID; id += DBLK_GROUP_SIZE) {
    for (uint16_t i = 0; i < FREEBLK_STACK_LEN; ++i) {
      fblk.free_stack[i] = (uint16_t) (id + DBLK_GROUP_SIZE + i);
    }
    io.write_block((Blk *) &fblk, id);
  }

  FreeBlk last_fblk{
      .avail_status = UNAVAIL
  };
  io.write_block((Blk *) &last_fblk, LAST_FREEBLK_ID);

  DirBlk dir_blk{
      .entries = {
          {
              .filename = {'.'},
              .iaddr = 0
          },
          {
              .filename = {'.', '.'},
              .iaddr = 0
          }
      }
  };
  io.write_block((Blk *) &dir_blk, 127 + DBLK_BEG_ID);

  IStatus umask000{};

  IStatus umask022 = {
      .o_w = true,
      .g_w = true
  };

  IStatus umask077 = {
      .o_x = true,
      .o_w = true,
      .o_r = true,
      .g_x = true,
      .g_w = true,
      .g_r = true
  };

  FS& fs = FS::get_instance();
  fs.mkdir(Path("/bin"), umask022, 0, 0);
  fs.mkdir(Path("/boot"), umask022, 0, 0);
  fs.mkdir(Path("/dev"), umask022, 0, 0);
  fs.mkdir(Path("/etc"), umask022, 0, 0);
  fs.mkdir(Path("/home"), umask022, 0, 0);
  fs.mkdir(Path("/lib"), umask022, 0, 0);
  fs.mkdir(Path("/media"), umask022, 0, 0);
  fs.mkdir(Path("/mnt"), umask022, 0, 0);
  fs.mkdir(Path("/opt"), umask022, 0, 0);
  fs.mkdir(Path("/root"), umask077, 0, 0);
  fs.mkdir(Path("/run"), umask022, 0, 0);
  fs.mkdir(Path("/sbin"), umask022, 0, 0);
  fs.mkdir(Path("/srv"), umask022, 0, 0);
  fs.mkdir(Path("/tmp"), umask000, 0, 0);
  fs.mkdir(Path("/usr"), umask022, 0, 0);
  fs.mkdir(Path("/var"), umask022, 0, 0);
  fs.mkdir(Path("/home/huaouo"), umask022, 0, 0);
  fs.chown(Path("/home/huaouo"), 1, 0, 0);
  fs.chgrp(Path("/home/huaouo"), 1, 0, 0);
}