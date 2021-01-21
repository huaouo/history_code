#include <set>
#include <map>
#include <queue>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <functional>

struct Production {
  std::string left, right;
  size_t pos = 0;

  bool operator<(const Production &rhs) const {
    if (pos == rhs.pos) {
      if (left == rhs.left)
        return right < rhs.right;
      return left < rhs.left;
    }
    return pos < rhs.pos;
  }

  bool operator==(const Production &rhs) const {
    return pos == rhs.pos && left == rhs.left && right == rhs.right;
  }
};

struct Node {
  size_t index = 0;
  std::vector<Production> productions;
  std::map<std::string, std::shared_ptr<Node>> next;

  // ignore index difference
  bool operator<(const Node &rhs) {
    if (productions == rhs.productions)
      return next < rhs.next;
    return productions < rhs.productions;
  }
};

class NodeAllocator {
  size_t next_index = 0;
  const std::map<std::string, std::set<std::string>> left_to_right;
  std::set<std::shared_ptr<Node>,
           std::function<bool(const std::shared_ptr<Node> &,
                              const std::shared_ptr<Node> &)>> allocated{
      [](const std::shared_ptr<Node> &lhs, const std::shared_ptr<Node> &rhs) {
        return lhs->productions[0] < rhs->productions[0];
      }
  };
 public:
  explicit NodeAllocator(const std::map<std::string, // NOLINT(modernize-pass-by-value)
                                        std::set<std::string>> &left_to_right)
      : left_to_right(left_to_right) {}

  auto get(const Production &init_p) {
    if (init_p.right.size() < init_p.pos)
      return std::make_shared<Node>();

    auto node = std::make_shared<Node>();
    node->productions.push_back(init_p);
    auto it = allocated.find(node);
    if (it != allocated.end())
      return *it;

    node->index = next_index++;
    if (init_p.pos != init_p.right.size()) {
      std::queue<std::string> non_terminal_queue;
      non_terminal_queue.push(init_p.right.substr(init_p.pos, 1));
      while (!non_terminal_queue.empty()) {
        auto front = non_terminal_queue.front();
        non_terminal_queue.pop();
        if (left_to_right.find(front) != left_to_right.end()) {
          for (const auto &right : left_to_right.at(front)) {
            node->productions.push_back({front, right, 0});
            if (isupper(right[0]))
              non_terminal_queue.push(right.substr(0, 1));
          }
        }
      }
    }
    allocated.insert(node);
    return node;
  }
};

int main() {
  size_t index;
  auto production_to_index = std::map<Production,
                                      size_t,
                                      std::function<bool(const Production &,
                                                         const Production &)>>{
      // ignore pos difference
      [](const Production &lhs, const Production &rhs) {
        if (lhs.right == rhs.right)
          return lhs.left < rhs.left;
        return lhs.right < rhs.right;
      }
  };
  std::map<std::string, std::set<std::string>> left_to_right;
  std::string production_str;
  std::ifstream fin("input.txt");
  while (fin >> index >> production_str) {
    std::string left = production_str.substr(0, 1);
    std::string right = production_str.substr(3);
    production_to_index[{left, right, 0}] = index;
    left_to_right[left].insert(right);
  }
  fin.close();

  NodeAllocator allocator(left_to_right);
  std::string end = *left_to_right["S"].begin();
  std::shared_ptr<Node> root = allocator.get({"S", end, 0});
  std::queue<std::shared_ptr<Node>> bfs_queue;
  std::set<std::shared_ptr<Node>> accessed_set;
  bfs_queue.push(root);

  std::ofstream fout("output.txt");
  fout << "line\tcolumn\tcontent\n";
  while (!bfs_queue.empty()) {
    auto this_node = bfs_queue.front();
    bfs_queue.pop();
    if (accessed_set.find(this_node) != accessed_set.end())
      continue;
    accessed_set.insert(this_node);

    for (const auto &p : this_node->productions) {
      std::string move_char = p.right.substr(p.pos, 1);
      if (p.pos < p.right.size()) {
        auto next_p = p;
        ++next_p.pos;
        auto next_node = allocator.get(next_p);
        this_node->next[move_char] = next_node;
        if (this_node->index != next_node->index)
          bfs_queue.push(next_node);
        fout << this_node->index << '\t' << move_char << "\tS" << next_node->index << '\n';
      } else {
        if (p.right == end) {
          fout << this_node->index << "\t#\t" << "acc" << '\n';
        } else {
          fout << this_node->index << "\t*\tr" << production_to_index[p] << '\n';
        }
      }
    }
  }
}