#include <iostream>

#include "login.h"
#include "util.h"

namespace emsh {

REPL login() {
  UserConf conf = UserConf::load();

  size_t attempts = 0;
  std::string username;
  do {
    std::cout << "Username: ";
    std::getline(std::cin, username);

    std::string password;
    std::cout << "Password: ";
    set_stdin_echo(false);
    std::getline(std::cin, password);
    set_stdin_echo(true);
    std::cout << '\n';
    if (conf.validate(username, password))
      break;
    ++attempts;
    std::cerr << "Error: Incorrect username or password.\n";
  } while (attempts < 3);

  if_fatal_error(attempts >= 3, "Fatal Error: Attempts reached 3.");

  clear_screen();
  emuid_t uid = conf.get_uid(username);
  UserInfo uinfo = conf.get_uinfo(uid);
  emgid_t gid = uinfo.gid;
  GroupInfo ginfo = conf.get_ginfo(gid);
  return REPL(uid, gid, uinfo, ginfo, conf);
}
}