#include <vector>
#include <fstream>
#include <iostream>

#include "util.h"
#include "userconf.h"

namespace emsh {

const std::string UserConf::CONF_PATH = "conf/";
const std::string UserConf::GROUP_CONF_PATH = CONF_PATH + "group.conf";
const std::string UserConf::PASSWD_CONF_PATH = CONF_PATH + "passwd.conf";
const std::string UserConf::HOSTNAME_CONF_PATH = CONF_PATH + "hostname.conf";

UserConf UserConf::load() {
  std::string line;
  UserConf conf;

  std::ifstream group_in(UserConf::GROUP_CONF_PATH);
  if_fatal_error(group_in.fail(), "Fatal Error: Cannot find 'group.conf'.");
  while (std::getline(group_in, line)) {
    trim(line);
    if (line.empty() || line[0] == '#')
      continue;

    std::vector<std::string> fields = split(line, ':');
    if_fatal_error(fields.size() != 3, "Fatal Error: Incorrect format of 'group.conf'");
    const std::string &groupname = fields[0];
    const auto gid = static_cast<emgid_t>(stoul(fields[1]));
    const auto member_usernames_vector = split(fields[2], ',');
    const std::set<std::string> member_usernames(member_usernames_vector.begin(), member_usernames_vector.end());
    conf.gname_to_gid[groupname] = gid;
    conf.group_map[gid] = {
        groupname,
        member_usernames
    };
  }
  group_in.close();

  std::ifstream passwd_in(PASSWD_CONF_PATH);
  if_fatal_error(passwd_in.fail(), "Fatal Error: Cannot find 'passwd.conf'.");
  while (std::getline(passwd_in, line)) {
    trim(line);

    if (line.empty() || line[0] == '#')
      continue;

    std::vector<std::string> fields = split(line, ':');
    if_fatal_error(fields.size() != 5, "Fatal Error: Incorrect format of 'passwd.conf'");
    const std::string &username = fields[0];
    const std::string &password_sha256 = fields[1];
    const auto uid = static_cast<emuid_t>(stoul(fields[2]));
    const auto gid = static_cast<emgid_t>(stoul(fields[3]));
    const std::string &home_path = fields[4];
    conf.uname_to_uid[username] = uid;
    conf.user_map[uid] = {
        gid,
        username,
        password_sha256,
        home_path
    };
  }
  passwd_in.close();

  std::ifstream hostname_in(HOSTNAME_CONF_PATH);
  if_fatal_error(hostname_in.fail(), "Fatal Error: Cannot find 'hostname.conf'.");
  do {
    std::getline(hostname_in, conf.hostname);
    trim(conf.hostname);
  } while (hostname_in && !conf.hostname.empty());
  if_fatal_error(conf.hostname.empty(), "Fatal Error: 'hostname.conf' is empty.");
  hostname_in.close();

  return conf;
}

void UserConf::write_file() {
  std::ofstream passwd_out(UserConf::PASSWD_CONF_PATH);
  for (const auto &p : user_map)
    passwd_out << p.second.username << ':' << p.second.password_sha256
               << ':' << p.first << ':' << p.second.gid << ':'
               << p.second.home_path << std::endl;
  passwd_out.close();
}

bool UserConf::validate(const std::string &username, const std::string &password) {
  decltype(uname_to_uid)::iterator it;
  if ((it = uname_to_uid.find(username)) == uname_to_uid.end())
    return false;

  return user_map[it->second].password_sha256 == sha256_with_salt(password);
}

emuid_t UserConf::get_uid(const std::string &username) {
  return uname_to_uid.at(username);
}

emgid_t UserConf::get_gid(const std::string &groupname) {
  return gname_to_gid.at(groupname);
}

const std::string &UserConf::get_hostname() {
  return hostname;
}

UserInfo &UserConf::get_uinfo(emuid_t uid) {
  return user_map.at(uid);
}

const GroupInfo &UserConf::get_ginfo(emgid_t gid) {
  return group_map.at(gid);
}

const std::string& UserConf::get_uname(emuid_t uid) {
  return user_map.at(uid).username;
}
}