#ifndef __EMSH_UESRCONFIGURATION
#define __EMSH_UESRCONFIGURATION

#include <map>
#include <set>
#include <string>

#include "util.h"

namespace emsh {

struct GroupInfo {
  std::string groupname;
  std::set<std::string> member_usernames;
};

struct UserInfo {
  emgid_t gid;
  std::string username;
  std::string password_sha256;
  std::string home_path;
};

class UserConf {
 public:
  const static std::string CONF_PATH;
  const static std::string GROUP_CONF_PATH;
  const static std::string PASSWD_CONF_PATH;
  const static std::string HOSTNAME_CONF_PATH;

 private:
  std::string hostname;
  std::map<emuid_t, UserInfo> user_map;
  std::map<std::string, emuid_t> uname_to_uid;
  std::map<emgid_t, GroupInfo> group_map;
  std::map<std::string, emgid_t> gname_to_gid;

  UserConf() = default;
 public:
  static UserConf load();
  void write_file();
  bool validate(const std::string &username, const std::string &password);
  emuid_t get_uid(const std::string &username);
  emgid_t get_gid(const std::string &groupname);
  const std::string &get_hostname();
  UserInfo &get_uinfo(emuid_t uid);
  const GroupInfo &get_ginfo(emgid_t gid);
  const std::string& get_uname(emuid_t uid);
};
}

#endif