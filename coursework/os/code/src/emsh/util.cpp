#include <openssl/sha.h>
#include <openssl/evp.h>

#include <algorithm>

#include "util.h"

namespace emsh {

std::vector<std::string> split(const std::string &str, char c) {
  std::vector<std::string> splitted;
  size_t pos = str.find(c);

  if (pos >= str.size()) {
    splitted.emplace_back(str);
    return splitted;
  } else {
    splitted.push_back(str.substr(0, pos));
  }

  do {
    size_t prev_pos = pos + 1;
    pos = str.find(c, prev_pos);
    if (pos > str.size())
      pos = str.size();
    splitted.push_back(str.substr(prev_pos, pos - prev_pos));
  } while (pos < str.size());
  return splitted;
}

std::vector<std::string> split_ignore_empty_slice(const std::string &path, const char gap) {
  auto splitted = split(path, gap);
  decltype(splitted) result;
  std::copy_if(splitted.begin(), splitted.end(), std::back_inserter(result), [](const auto& s) {
    return s != "";
  });
  return result;
}

std::string sha256_with_salt(const std::string& str) {
  std::string str_with_salt = "emsh" + str + "2018";
  unsigned char hash[SHA256_DIGEST_LENGTH];
  unsigned char encoded[45];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, str_with_salt.c_str(), str_with_salt.size());
  SHA256_Final(hash, &sha256);

  EVP_EncodeBlock((unsigned char*)encoded, hash, SHA256_DIGEST_LENGTH);
  return std::string((char*)encoded);
}

void if_fatal_error(bool expr, const std::string &msg) {
  if (expr) {
    std::cerr << msg << '\n';
    exit(-1);
  }
}

void trim(std::string &str) {
  size_t start = str.find_first_not_of(' ');
  if (start >= str.size())
    str.clear();
  size_t end = str.find_last_not_of(' ');
  str = str.substr(start, end - start + 1);
}

void clear_screen() {
  std::cout << "\033[H\033[J";
}

void set_stdin_echo(bool enable) {
  termios tty{};
  tcgetattr(STDIN_FILENO, &tty);
  if (!enable)
    tty.c_lflag &= ~ECHO;
  else
    tty.c_lflag |= ECHO;

  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}
}