#ifndef __EMSH_REPL
#define __EMSH_REPL

#include <vector>
#include <functional>

#include "userconf.h"
#include "fs/fs.h"
#include "fs/path.h"

namespace emsh {

class REPL {
  using Params = std::vector<std::string>;

  fs::IO &io;
  fs::FS &filesystem;
  const emuid_t uid;
  const emgid_t gid;
  UserInfo uinfo;
  GroupInfo ginfo;
  bool exit = false;
  UserConf uconf;
  fs::Path work_dir; // uconf should be declared before work_dir ...
  fs::Path home;
  fs::IStatus umask;
  std::map<std::string, std::function<void(const Params &)>> function_map;

  void print_main_prompt();
  void print_error(const std::string& leading, StatusCode code);
  fs::Path get_working_path();
  fs::Path get_abs_path(const std::string& input_path);
 public:
  explicit REPL(emuid_t uid, emgid_t gid, UserInfo uinfo, GroupInfo ginfo, UserConf uconf);
  bool is_exit();
  void do_loop();
};
}

#endif
