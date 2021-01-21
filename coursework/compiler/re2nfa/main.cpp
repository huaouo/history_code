#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <algorithm>

using Transfer = std::tuple<char, char, char>;

struct NFA {
    std::vector<char> start_symbols, end_symbols;
    std::vector<Transfer> transfers;
};

char current_state = 'A';

char GetNextState() {
    return current_state++;
}

class NodeBase {
public:
    virtual NFA GetNFA() = 0;

    virtual ~NodeBase() = default;
};

class Atom : public NodeBase {
    char value;
public:
    explicit Atom(char value) : value(value) {}

    NFA GetNFA() override {
        NFA ret;
        char s = GetNextState(), e = GetNextState();
        ret.start_symbols.push_back(s);
        ret.end_symbols.push_back(e);
        ret.transfers.emplace_back(s, value, e);
        return ret;
    }
};

class Alternation : public NodeBase {
    std::unique_ptr<NodeBase> lhs, rhs;
public:
    Alternation(std::unique_ptr<NodeBase> lhs, std::unique_ptr<NodeBase> rhs) :
            lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    NFA GetNFA() override {
        NFA ret, l_nfa = lhs->GetNFA(), r_nfa = rhs->GetNFA();
        char s = GetNextState(), e = GetNextState();
        ret.start_symbols.push_back(s);
        ret.end_symbols.push_back(e);
        ret.transfers.insert(ret.transfers.end(), l_nfa.transfers.begin(), l_nfa.transfers.end());
        for (auto l_nfa_s : l_nfa.start_symbols) {
            for (auto l_nfa_e : l_nfa.end_symbols) {
                ret.transfers.emplace_back(s, 'e', l_nfa_s);
                ret.transfers.emplace_back(l_nfa_e, 'e', e);
            }
        }
        for (auto r_nfa_s : r_nfa.start_symbols) {
            for (auto r_nfa_e : r_nfa.end_symbols) {
                ret.transfers.emplace_back(s, 'e', r_nfa_s);
                ret.transfers.emplace_back(r_nfa_e, 'e', e);
            }
        }
        ret.transfers.insert(ret.transfers.end(), r_nfa.transfers.begin(), r_nfa.transfers.end());
        return ret;
    }
};

class Concatenation : public NodeBase {
    std::unique_ptr<NodeBase> lhs, rhs;
public:
    Concatenation(std::unique_ptr<NodeBase> lhs, std::unique_ptr<NodeBase> rhs) :
            lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    NFA GetNFA() override {
        NFA ret, l_nfa = lhs->GetNFA(), r_nfa = rhs->GetNFA();
        ret.start_symbols.insert(ret.start_symbols.end(), l_nfa.start_symbols.begin(), l_nfa.start_symbols.end());
        ret.end_symbols.insert(ret.end_symbols.end(), r_nfa.end_symbols.begin(), r_nfa.end_symbols.end());
        ret.transfers.insert(ret.transfers.end(), l_nfa.transfers.begin(), l_nfa.transfers.end());
        for (auto s : l_nfa.end_symbols)
            for (auto e : r_nfa.start_symbols)
                ret.transfers.emplace_back(s, 'e', e);
        ret.transfers.insert(ret.transfers.end(), r_nfa.transfers.begin(), r_nfa.transfers.end());
        return ret;
    }
};

class Closure : public NodeBase {
    std::unique_ptr<NodeBase> repeated_element;
public:
    explicit Closure(std::unique_ptr<NodeBase> repeated_element) :
            repeated_element(std::move(repeated_element)) {}

    NFA GetNFA() override {
        NFA ret, r_nfa = repeated_element->GetNFA();
        char s = GetNextState(), e = GetNextState();
        ret.start_symbols.push_back(s);
        ret.end_symbols.push_back(e);
        for (auto nfa_e : r_nfa.start_symbols)
            ret.transfers.emplace_back(s, 'e', nfa_e);
        ret.transfers.insert(ret.transfers.end(), r_nfa.transfers.begin(), r_nfa.transfers.end());
        for (auto nfa_s : r_nfa.start_symbols)
            for (auto nfa_e : r_nfa.end_symbols)
                ret.transfers.emplace_back(nfa_e, 'e', nfa_s);
        for (auto nfa_e : r_nfa.end_symbols)
            ret.transfers.emplace_back(nfa_e, 'e', e);
        ret.transfers.emplace_back(s, 'e', e);
        return ret;
    }
};

bool keep_symbol = false;
int current_char = EOF;
std::string symbols, re;

int GetNextChar() {
    static size_t index = 0;
    while (index < re.size() && re[index] == '.')
        ++index;

    if (index == re.size())
        return current_char = EOF;
    return current_char = re[index++];
}

void Error(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
}

std::unique_ptr<NodeBase> ParseRE(int stop = EOF) {
    std::unique_ptr<NodeBase> ret;
    std::unique_ptr<NodeBase> cur;
    int next_char;
    while ((next_char = (keep_symbol ? current_char : GetNextChar())) != stop
           && next_char != EOF && next_char != ')') {
        if (next_char != '*' && (!keep_symbol || next_char != '|')) {
            if (ret == nullptr)
                ret = std::move(cur);
            else
                ret = std::make_unique<Concatenation>(std::move(ret), std::move(cur));
        }
        keep_symbol = false;
        switch (next_char) {
            case '(':
                cur = ParseRE();
                if (current_char != ')')
                    Error("Expect ')'.");
                break;
            case '|':
                if (ret == nullptr)
                    Error("Expect item before '|'.");
                cur = ParseRE('|');
                if (cur == nullptr)
                    Error("Expect item after '|'.");
                ret = std::make_unique<Alternation>(std::move(ret), std::move(cur));
                if (current_char != '|')
                    goto end;
                else
                    keep_symbol = true;
                break;
            case '*':
                if (cur == nullptr)
                    Error("Expect item before '*'.");
                cur = std::make_unique<Closure>(std::move(cur));
                break;
            default:
                cur = std::make_unique<Atom>(next_char);
        }
    }

    end:
    if (cur != nullptr) {
        if (ret == nullptr)
            ret = std::move(cur);
        else
            ret = std::make_unique<Concatenation>(std::move(ret), std::move(cur));
    }

    return ret;
}

int main() {
    std::ifstream in("input.txt");
    std::getline(in, symbols);
    std::getline(in, re);

    auto nfa = ParseRE()->GetNFA();
    std::ofstream out("output.txt");
    for (char c = 'A'; c < current_state; ++c)
        out << c;
    out << '\n';
    out << symbols << '\n';
    std::sort(nfa.start_symbols.begin(), nfa.start_symbols.end());
    std::sort(nfa.end_symbols.begin(), nfa.end_symbols.end());
    for (auto c : nfa.start_symbols)
        out << c;
    out << '\n';
    for (auto c : nfa.end_symbols)
        out << c;
    out << '\n';
    for (auto tr : nfa.transfers) {
        auto[s, t, e] = tr;
        out << '(' << s << ", " << t << ", " << e << ')';
    }
    out << '\n';

    std::ofstream out_graph("output.gv");
    out_graph << "digraph G {\n";
    out_graph << "rankdir = LR;\n";
    out_graph << "\"\" [shape=none];\n";
    out_graph << "node [shape = doublecircle]; \n";
    for (auto c : nfa.start_symbols)
        out_graph << c << ';';
    for (auto c : nfa.end_symbols)
        out_graph << c << ';';
    out_graph << '\n';
    out_graph << "node [shape = circle];\n";
    for (auto c : nfa.start_symbols)
        out_graph << "\"\"->" << c << " [label = \"Start\"]\n";
    for (auto tr : nfa.transfers) {
        auto[s, t, e] = tr;
        out_graph << s << "->" << e << " [label = \"" << t << "\"]\n";
    }
    out_graph << "}\n";
};