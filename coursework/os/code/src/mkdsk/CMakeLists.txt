add_executable(mkdsk mkdsk.cpp ../emsh/fs/io.h ../emsh/fs/io.cpp
        ../emsh/fs/sblk.h ../emsh/fs/iblk.h ../emsh/fs/freeblk.h
        ../emsh/fs/dirblk.h ../emsh/fs/fs.cpp ../emsh/fs/path.cpp
        ../emsh/fs/stream.cpp ../emsh/util.cpp)

target_link_libraries(mkdsk crypto)