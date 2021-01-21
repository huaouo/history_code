#ifndef __EMSH_UTIL
#define __EMSH_UTIL

#include <stdint.h>
#include <crypt.h>
#include <unistd.h>
#include <termios.h>

#include <map>
#include <vector>
#include <iostream>

#define IF_INTERNAL_ERROR(EXPR) \
do { \
  if (EXPR) { \
    std::cerr << "Internal Error: '" << __FILE__ << "': LINE " << __LINE__ << '\n'; \
    exit(-1); \
  } \
} while(0)

#define NTH_BIT(VAL, N) (((VAL) >> (N))&1)
#define SET_NTH_BIT(VAL, N) ((VAL) |= (1 << (N)))
#define CLEAR_NTH_BIT(VAL, N) ((VAL) &= ~(1 << (N)))

namespace emsh {
using emuid_t = uint32_t;
using emgid_t = uint32_t;

std::vector<std::string> split(const std::string &str, char c);

std::vector<std::string> split_ignore_empty_slice(const std::string &path, char gap);

std::string sha256_with_salt(const std::string &str);

void if_fatal_error(bool expr, const std::string &msg);

void trim(std::string &str);

void clear_screen();

void set_stdin_echo(bool enable);
}

#endif