#include <utility>
#include <iostream>
#include <iomanip>

#include "repl.h"
#include "fs/iblk.h"

namespace emsh {

REPL::REPL(emuid_t uid, emgid_t gid, UserInfo uinfo, GroupInfo ginfo, UserConf uconf)
    : uid(uid), gid(gid), uinfo(std::move(uinfo)), ginfo(std::move(ginfo)),
      uconf(std::move(uconf)), work_dir(this->uconf.get_uinfo(this->uid).home_path),
      home(work_dir), filesystem(fs::FS::get_instance()), io(fs::IO::get_instance()),
      umask({.o_w = true, .g_w = true}) {

  auto[_status_code, inode_id] = filesystem.find_file(work_dir, this->uid, this->gid);
  if (_status_code != SUCCESS) {
    print_error("cd: '" + work_dir.to_string() + "'", _status_code);
    work_dir.clear();
  }

  const auto get_status_str = [](fs::IStatus status) {
    std::string result;
    result.append(status.is_dir ? "d" : "-");
    result.append(status.u_r ? "r" : "-");
    result.append(status.u_w ? "w" : "-");
    result.append(status.u_x ? "x" : "-");
    result.append(status.g_r ? "r" : "-");
    result.append(status.g_w ? "w" : "-");
    result.append(status.g_x ? "x" : "-");
    result.append(status.o_r ? "r" : "-");
    result.append(status.o_w ? "w" : "-");
    result.append(status.o_x ? "x" : "-");
    return result;
  };

  function_map = {
      {"ls", [this](const Params &params) {
        this->function_map["ll"](params);
      }},
      {"ll", [this, &get_status_str](const Params &params) {
        const auto get_time_str = [](time_t stamp) {
          const char format[] = "%b %d %R";
          tm lt{};
          char res[32];

          localtime_r(&stamp, &lt);
          strftime(res, sizeof(res), format, &lt);
          return std::string(res);
        };

        auto params_op = params;
        if (params.size() == 1)
          params_op.emplace_back(".");

        for (size_t i = 1; i < params_op.size(); ++i) {
          auto[code, x] = filesystem.ls(get_abs_path(params_op[i]), this->uid, this->gid);
          if (x.empty()) {
            print_error("ls: cannot access '" + params_op[i] + "'", code);
            continue;
          }

          int width[4]{};
          try {
            for (const auto &entry : x) {
              width[0] = std::max(width[0], (int) std::to_string(entry.lnk_cnt).size());
              width[1] = std::max(width[1], (int) this->uconf.get_uinfo(entry.uid).username.size());
              width[2] = std::max(width[2], (int) this->uconf.get_ginfo(entry.gid).groupname.size());
              width[3] = std::max(width[3], (int) std::to_string(entry.size).size());
            }

            for (const auto &entry : x) {
              std::cout << get_status_str(entry.status) << ' '
                        << std::setw(width[0]) << entry.lnk_cnt << ' '
                        << std::left << std::setw(width[1]) << this->uconf.get_uinfo(entry.uid).username << ' '
                        << std::setw(width[2]) << this->uconf.get_ginfo(entry.gid).groupname << ' '
                        << std::right << std::setw(width[3]) << entry.size << ' '
                        << get_time_str(entry.stamp) << ' ';
              if (entry.status.is_dir)
                std::cout << "\e[1m\e[34m";
              else if (entry.status.u_x || entry.status.g_x || entry.status.o_x)
                std::cout << "\e[1m\e[92m";
              std::cout << entry.filename;
              std::cout << "\e[0m";
              if (entry.status.is_dir)
                std::cout << '/';
              else if (entry.status.u_x || entry.status.g_x || entry.status.o_x)
                std::cout << '*';
              std::cout << std::endl;
            }
          } catch (...) {
            print_error("ls", INTERNAL_ERROR);
            return;
          }
        }
      }},
      {"chmod", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("chmod", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("chmod", TOO_MANY_OPERANDS);
          return;
        }

        const auto &umask_str = params[1];
        auto print_err_msg = [&umask_str]() {
          std::cout << "chmod: " << umask_str << ": invalid mode" << std::endl;
        };
        if (umask_str.size() > 4) {
          print_err_msg();
          return;
        }
        for (const auto c : umask_str) {
          if (c > '7' || c < '0') {
            print_err_msg();
            return;
          }
        }
        auto umask_num = static_cast<uint16_t>(std::stoul(umask_str, nullptr, 8));
        if (umask_num > PRIVILEGE_MAX) {
          print_err_msg();
          return;
        }
        auto mode = *(fs::IStatus *) &umask_num;
        StatusCode code = filesystem.chmod(get_abs_path(params[2]), mode, this->uid, this->gid);
        print_error("chmod", code);
      }},
      {"chown", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("chown", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("chown", TOO_MANY_OPERANDS);
          return;
        }

        fs::Path file = get_abs_path(params[2]);
        emuid_t to_uid{};
        try {
          to_uid = this->uconf.get_uid(params[1]);
        } catch (...) {
          print_error("chown", INVALID_USERNAME);
          return;
        }
        StatusCode code = filesystem.chown(file, to_uid, this->uid, this->gid);
        print_error("chown", code);
      }},
      {"chgrp", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("chgrp", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("chgrp", TOO_MANY_OPERANDS);
          return;
        }

        fs::Path file = get_abs_path(params[2]);
        emgid_t to_gid{};
        try {
          to_gid = this->uconf.get_gid(params[1]);
        } catch (...) {
          print_error("chgrp", INVALID_GROUPNAME);
          return;
        }
        StatusCode code = filesystem.chgrp(file, to_gid, this->uid, this->gid);
        print_error("chgrp", code);
      }},
      {"pwd", [this](const Params &params) {
        std::cout << work_dir.to_string() << std::endl;
      }},
      {"cd", [this](const Params &params) {
        auto back_up = work_dir;
        switch (params.size()) {
          case 1:
            work_dir = home;
            break;
          case 2:
            if (params[1][0] == '/')
              work_dir.clear();
            work_dir.concat_back(params[1]);
            break;
          default:
            print_error("cd", TOO_MANY_OPERANDS);
            return;
        }

        auto[_code, _inode_id] = filesystem.find_file(work_dir, this->uid, this->gid);
        if (_code == SUCCESS) {
          auto item_info = filesystem.get_iteminfo(_inode_id, "");
          if (!item_info.status.is_dir)
            _code = NOT_DIR;
          else if (!((this->uid == item_info.uid && item_info.status.u_x)
              || (this->uid != item_info.uid && this->gid == item_info.gid && item_info.status.g_x)
              || (this->uid != item_info.uid && this->gid != item_info.gid && item_info.status.o_x)))
            _code = PERMISSION_DENIED;
        }
        if (_code != SUCCESS) {
          print_error("cd: '" + params[1] + "'", _code);
          work_dir = back_up;
        }
      }},
      {"mkdir", [this](const Params &params) {
        if (params.size() == 1) {
          print_error("mkdir", MISSING_OPERAND);
          return;
        }

        for (size_t i = 1; i < params.size(); ++i) {
          fs::Path dir_create = get_abs_path(params[i]);
          StatusCode code = filesystem.mkdir(dir_create, umask, this->uid, this->gid);
          print_error("mkdir: cannot create directory '" + params[i] + "'", code);
        }
      }},
      {"rmdir", [this](const Params &params) {
        if (params.size() == 1) {
          print_error("rmdir", MISSING_OPERAND);
          return;
        }

        for (size_t i = 1; i < params.size(); ++i) {
          fs::Path dir_rm = get_abs_path(params[i]);
          StatusCode code = filesystem.rmdir(dir_rm, this->uid, this->gid);
          print_error("rmdir: failed to remove '" + params[i] + "'", code);
        }
      }},
      {"umask", [this](const Params &params) {
        if (params.size() == 1) {
          std::cout << std::setfill('0') << std::setw(4) << std::oct
                    << *(uint16_t *) &umask << std::endl;
        } else {
          const auto &umask_str = params[1];
          auto print_err_msg = [&umask_str]() {
            std::cout << "\e[1m\e[31m";
            std::cout << "umask: " << umask_str << ": invalid umask number";
            std::cout << "\e[0m";
          };
          if (umask_str.size() > 4) {
            print_err_msg();
            return;
          }
          for (const auto c : umask_str) {
            if (c > '7' || c < '0') {
              print_err_msg();
              return;
            }
          }
          auto umask_num = static_cast<uint16_t>(std::stoul(umask_str, nullptr, 8));
          if (umask_num > PRIVILEGE_MAX) {
            print_err_msg();
            return;
          }
          umask = *(fs::IStatus *) &umask_num;
        }
      }},
      {"touch", [this](const Params &params) {
        if (params.size() == 1) {
          print_error("touch", MISSING_OPERAND);
          return;
        }

        for (size_t i = 1; i < params.size(); ++i) {
          fs::Path dir_touch = get_abs_path(params[i]);
          StatusCode code = filesystem.touch(dir_touch, umask, this->uid, this->gid);
          print_error("touch: cannot touch '" + params[i] + "'", code);
        }
      }},
      {"mv", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("mv", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("mv", TOO_MANY_OPERANDS);
          return;
        }

        fs::Path src = get_abs_path(params[1]);
        fs::Path dst = get_abs_path(params[2]);
        StatusCode code = filesystem.mv(dst, src, this->uid, this->gid);
        print_error("mv: cannot mv '" + params[1] + " to '" + params[2] + "'", code);
      }},
      {"cp", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("cp", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("cp", TOO_MANY_OPERANDS);
          return;
        }

        fs::Path src = get_abs_path(params[1]);
        fs::Path dst = get_abs_path(params[2]);
        StatusCode code = filesystem.cp(dst, src, this->uid, this->gid);
        print_error("cp: cannot cp '" + params[1] + " to '" + params[2] + "'", code);

      }},
      {"rm", [this](const Params &params) {
        if (params.size() == 1) {
          print_error("rm", MISSING_OPERAND);
          return;
        }

        for (size_t i = 1; i < params.size(); ++i) {
          fs::Path dir_touch = get_abs_path(params[i]);
          StatusCode code = filesystem.rm(dir_touch, this->uid, this->gid);
          print_error("rm: cannot remove '" + params[i] + "'", code);
        }
      }},
      {"ln", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("ln", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("ln", TOO_MANY_OPERANDS);
          return;
        }

        fs::Path src = get_abs_path(params[1]);
        fs::Path dst = get_abs_path(params[2]);
        StatusCode code = filesystem.ln(dst, src, this->uid, this->gid);
        print_error("ln: cannot ln '" + params[1] + " to '" + params[2] + "'", code);
      }},
      {"cat", [this](const Params &params) {
        if (params.size() == 1) {
          print_error("cat", MISSING_OPERAND);
          return;
        }

        for (size_t i = 1; i < params.size(); ++i) {
          fs::Path file = get_abs_path(params[i]);
          auto[code, data] = filesystem.cat(file, this->uid, this->gid);
          std::cout << data;
          print_error("touch: cannot cat '" + params[i] + "'", code);
        }
      }},
      {"passwd", [this](const Params &params) {
        if (params.size() > 2) {
          print_error("astr", TOO_MANY_OPERANDS);
          return;
        }

        emuid_t update_uid;
        try {
          update_uid = params.size() == 1 ? this->uid : this->uconf.get_uid(params[1]);
        } catch (...) {
          print_error("passwd", INVALID_USERNAME);
          return;
        }

        if (this->uid != 0 && update_uid != this->uid) {
          print_error("passwd", PERMISSION_DENIED);
          return;
        }

        auto update_uname = this->uconf.get_uname(update_uid);
        std::cout << "Changing password for " << update_uname << std::endl;

        if (this->uid != 0) {
          std::cout << "(current) UNIX password: ";
          std::string input_cur_passwd;
          set_stdin_echo(false);
          std::getline(std::cin, input_cur_passwd);
          set_stdin_echo(true);
          std::cout << std::endl;
          if (!this->uconf.validate(update_uname, input_cur_passwd)) {
            print_error("passwd", AUTH_FAIL);
            return;
          }
        }

        std::string new_passwd, new_passwd_check;
        std::cout << "Enter new UNIX password: ";
        set_stdin_echo(false);
        std::getline(std::cin, new_passwd);
        std::cout << std::endl;
        std::cout << "Retype new UNIX password: ";
        std::getline(std::cin, new_passwd_check);
        set_stdin_echo(true);
        std::cout << std::endl;
        if (new_passwd != new_passwd_check) {
          print_error("passwd", PASSWD_MISMATCH);
          return;
        }

        this->uconf.get_uinfo(update_uid).password_sha256 = sha256_with_salt(new_passwd);
        this->uconf.write_file();
        std::cout << "passwd: password changed successfully" << std::endl;
      }},
      {"astr", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("astr", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("astr", TOO_MANY_OPERANDS);
          return;
        }

        StatusCode code = filesystem.append_str(get_abs_path(params[1]), params[2], this->uid, this->gid);
        print_error("astr", code);
      }},
      {"anstar", [this](const Params &params) {
        if (params.size() < 3) {
          print_error("anstr", MISSING_OPERAND);
          return;
        } else if (params.size() > 3) {
          print_error("anstr", TOO_MANY_OPERANDS);
          return;
        }

        StatusCode code = filesystem.append_n_star(get_abs_path(params[1]), stoul(params[2]), this->uid, this->gid);
        print_error("anstr", code);
      }},
      {"psblk", [this](const Params &params) {
        if (params.size() > 1) {
          print_error("psblk", TOO_MANY_OPERANDS);
          return;
        }

        auto sblk = filesystem.get_sblk();
        std::cout << "INode Available: " << sblk.inode_avail << std::endl;
        std::cout << "Data Block Available: " << sblk.dblk_avail << std::endl;
        std::cout << "Root INode Id: " << sblk.root_iaddr << std::endl;
        std::cout << "Free Stack #1:" << std::endl;
        size_t i = 0;
        for (; i <= sblk.stack_top; ++i) {
          std::cout << sblk.free_stack[i] << ' ';
          if (i % 8 == 7)
            std::cout << std::endl;
        }
        if (--i % 8 != 7)
          std::cout << std::endl;
      }},
      {"pinode", [this, &get_status_str](const Params &params) {
        if (params.size() > 2) {
          print_error("anstr", TOO_MANY_OPERANDS);
          return;
        }

        auto path = params.size() == 1 ? work_dir : get_abs_path(params[1]);
        auto[code, inode] = filesystem.get_inode(path);
        if (code != SUCCESS) {
          print_error("pinode", code);
        } else {
          std::cout << "Size: " << inode.size << std::endl;
          std::cout << "Link Count: " << inode.lnk_cnt << std::endl;
          std::cout << "Time Stamp: " << inode.stamp << std::endl;
          std::cout << "Uid: " << inode.uid << std::endl;
          std::cout << "Gid: " << inode.gid << std::endl;
          std::cout << "Direct Addr(Block Id):";
          for (const auto addr : inode.d_addr)
            std::cout << ' ' << addr;
          std::cout << std::endl;
          std::cout << "Indirect Addr(Block Id) I: " << inode.j_addr << std::endl;
          std::cout << "Indirect Addr(Block Id) II: " << inode.jj_addr << std::endl;
          std::cout << "Mode: " << get_status_str(inode.status) << std::endl;
        }
      }},
      {"sync", [this](const Params &params) {
        io.sync();
      }},
      {"help", [](const Params &params) {
        std::cout << "Suppoerted commands:" << std::endl;
        std::cout << "cd" << std::endl;
        std::cout << "ll" << '\t' << "Similar to Linux command `ls -l`" << std::endl;
        std::cout << "ls" << '\t' << "Alias for `ll`" << std::endl;
        std::cout << "mv" << std::endl;
        std::cout << "cp" << std::endl;
        std::cout << "rm" << std::endl;
        std::cout << "ln" << std::endl;
        std::cout << "cat" << std::endl;
        std::cout << "pwd" << std::endl;
        std::cout << "sync" << '\t' << "Sync block cache, usage: `sync`" << std::endl;
        std::cout << "exit" << std::endl;
        std::cout << "help" << std::endl;
        std::cout << "astr" << '\t'
                  << "Append str to file, usage: `astr <file> <str>`" << std::endl;
        std::cout << "psblk" << '\t' << "Print info of super block, usage: `psblk`" << std::endl;
        std::cout << "clear" << std::endl;
        std::cout << "pinode" << '\t'
                  << "Print info of inode, usage: `pinode` or `pinode <file>`" << std::endl;
        std::cout << "anstar" << '\t'
                  << "Append n star(*) to file, usage: `anstar <file> <n>`" << std::endl;
        std::cout << "chmod" << std::endl;
        std::cout << "chown" << std::endl;
        std::cout << "chgrp" << std::endl;
        std::cout << "mkdir" << std::endl;
        std::cout << "rmdir" << std::endl;
        std::cout << "umask" << std::endl;
        std::cout << "touch" << std::endl;
        std::cout << "passwd" << std::endl;
      }},
      {"clear", [](const Params &params) {
        clear_screen();
      }},
      {"exit", [this](const Params &params) {
        exit = true;
      }}
  };
}

bool REPL::is_exit() {
  return exit;
}

fs::Path REPL::get_working_path() {
  return work_dir;
}

fs::Path REPL::get_abs_path(const std::string &input_path) {
  fs::Path dir;
  if (input_path[0] != '/')
    dir.concat_back(work_dir);
  dir.concat_back(input_path);
  return dir;
}

void REPL::print_main_prompt() {
  std::cout << "\e[1m\e[92m";
  std::cout << uconf.get_uinfo(uid).username << "@" << uconf.get_hostname();
  std::cout << "\e[0m";
  std::cout << ':';

  std::cout << "\e[1m\e[34m";
  fs::Path working_path = get_working_path();
  const auto &w = working_path.to_string();
  const auto &h = home.to_string();
  if (w.compare(0, h.size(), h) == 0)
    std::cout << "~" << w.substr(h.size());
  else
    std::cout << w;
  std::cout << "\e[0m";

  if (uid == 0)
    std::cout << "# ";
  else
    std::cout << "$ ";
}

void REPL::print_error(const std::string &leading, StatusCode code) {
  if (code == SUCCESS)
    return;
  std::cout << "\e[1m\e[31m";
  std::cout << leading << ": ";
  switch (code) {
    case RUN_OUT_OF_INODE:
      std::cout << "run out of inode";
      break;
    case RUN_OUT_OF_DBLK:
      std::cout << "run out of data block";
      break;
    case PERMISSION_DENIED:
      std::cout << "permission denied";
      break;
    case FILE_EXISTS:
      std::cout << "file exists";
      break;
    case FILENAME_TOO_LONG:
      std::cout << "filename too long";
      break;
    case PRESERVE_ROOT:
      std::cout << "preserve root";
      break;
    case NO_SUCH_FILE_OR_DIRECTORY:
      std::cout << "no such file or directory";
      break;
    case MISSING_OPERAND:
      std::cout << "missing operand";
      break;
    case DIR_NOT_EMPTY:
      std::cout << "directory not empty";
      break;
    case NOT_DIR:
      std::cout << "not a directory";
      break;
    case TOO_MANY_OPERANDS:
      std::cout << "too many operands";
      break;
    case SAME_FILE:
      std::cout << "same file";
      break;
    case MOV_TO_SUB_DIR:
      std::cout << "cannot mov a directory to its sub directory";
      break;
    case INVALID_ARGS:
      std::cout << "invalid arguments";
      break;
    case INVALID_USERNAME:
      std::cout << "invalid username";
      break;
    case INTERNAL_ERROR:
      std::cout << "internal error";
      break;
    case INVALID_GROUPNAME:
      std::cout << "invalid groupname";
      break;
    case IS_DIR:
      std::cout << "is a directory";
      break;
    case HARDLINK_NOT_ALLOWED_FOR_DIR:
      std::cout << "hard link not allowed for directory";
      break;
    case AUTH_FAIL:
      std::cout << "authentication failed";
      break;
    case PASSWD_MISMATCH:
      std::cout << "password mismatch";
  }
  std::cout << "\e[0m";
  std::cout << std::endl;
}

void REPL::do_loop() {
  print_main_prompt();

  std::string line_of_command;
  std::getline(std::cin, line_of_command);
  auto command = split_ignore_empty_slice(line_of_command, ' ');
  if (command.empty())
    return;

  decltype(function_map)::iterator func_iter;
  if ((func_iter = function_map.find(command[0])) != function_map.end()) {
    func_iter->second(command);
  } else {
    std::cout << "\e[1m\e[31m";
    std::cout << command[0] << ": command not found" << std::endl;
    std::cout << "\e[0m";
  }
}
}