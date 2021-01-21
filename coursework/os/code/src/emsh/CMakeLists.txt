add_executable(emsh emsh.cpp login.cpp
        userconf.cpp repl.cpp repl.h util.cpp fs/iblk.h
        fs/sblk.h fs/dirblk.h fs/freeblk.h fs/const.h
        fs/io.cpp fs/io.h fs/blk.h fs/fs.h fs/fs.cpp fs/lnkblk.h fs/iteminfo.h fs/stream.cpp fs/stream.h fs/path.cpp fs/path.h)

target_link_libraries(emsh crypto)