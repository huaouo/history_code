add_executable(test catch.hpp test.cpp
        ../emsh/util.h ../emsh/util.cpp ../emsh/fs/io.h
        ../emsh/fs/io.cpp ../emsh/fs/blk.h
        ../emsh/fs/fs.cpp ../emsh/fs/fs.h
        ../emsh/fs/stream.cpp
        ../emsh/fs/path.cpp)

target_link_libraries(test crypto)