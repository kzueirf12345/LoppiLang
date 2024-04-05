#include "Utils.h"

const std::unordered_set<std::string> utils::_comp_ops = {"==", "!=", ">", "<", ">=", "<="};
const std::unordered_set<std::string> utils::_logic_bin_ops = {"||", "&&"};
const std::unordered_set<std::string> utils::_logic_uno_ops = {"!"};
const std::unordered_set<std::string> utils::_bin_ops = {"->", "|", "^", "&", "+",
                                                         "-",  "/", "*", "%", "**"};
const std::unordered_set<std::string> utils::_uno_ops = {"++", "--", "~", "-", "+"};
const std::unordered_set<std::string> utils::_assign_ops = {
    "=", "+=", "->=", "-=", "*=", "**=", "/=", "^=", "&=", "|=", "%="};

const std::unordered_set<std::string> utils::_built_in_types = {"string", "array", "byte",
                                                                "byte2",  "byte4", "byte8"};
const std::unordered_set<std::string> utils::_const_built_in_types = {
    "const_string", "const_array", "const_byte", "const_byte2", "const_byte4", "const_byte8",
};
const std::unordered_set<std::string> utils::_all_built_in_types = {
    "string",       "array",       "byte",       "byte2",       "byte4",       "byte8",
    "const_string", "const_array", "const_byte", "const_byte2", "const_byte4", "const_byte8",
};

std::string utils::const_parser(const std::string& s) {
    if ((int)s.size() <= 6) return s;

    std::string pref = "";
    for (int i = 0; i < 6; ++i) pref += s[i];

    if (pref != "const_") return s;

    std::string res = "";
    for (int i = 6; i < (int)s.size(); ++i) res += s[i];
    return res;
}

std::string utils::max_byte(const std::string& l, const std::string& r) {
    if (l.size() == 4 && r.size() == 4)
        return "byte";
    else if (l.size() == 4)
        return r;
    else if (r.size() == 4)
        return l;
    else
        return l.back() > r.back() ? l : r;
}

std::string utils::check_cast(const std::string& s1, const std::string& s2) {
    auto f = const_parser(s1), s = const_parser(s2);

    if (f == s && (f == "string" || f == "array")) return f;

    while (f.size() > 4) f.pop_back();
    while (s.size() > 4) s.pop_back();

    if (f == s) return f;
    throw std::logic_error("type " + s1 + " does not match the return type: " + s2);
}

bool utils::is_op(const std::string& s) {
    if (s == "void") return true;
    if (s == ",") return true;
    if (_built_in_types.count(s) || _const_built_in_types.count(s)) return false;

    if (_comp_ops.count(s) || _logic_bin_ops.count(s) || _logic_uno_ops.count(s) ||
        _bin_ops.count(s) || _uno_ops.count(s) || _assign_ops.count(s))
        return true;

    throw std::invalid_argument("incorrect lexem: " + s);
}

std::pair<std::string, std::vector<std::string>> utils::path_parser(const std::string& raw_name) {
    std::vector<std::string> path = {""};
    for (int i = 0; i < (int)raw_name.size(); ++i) {
        if (raw_name[i] == ':') {
            ++i;
            path.push_back("");
        } else {
            path.back().push_back(raw_name[i]);
        }
    }

    std::string name = path.back();
    path.pop_back();

    return {name, path};
}

std::vector<std::string> utils::path_parser(const std::string& raw, char sep) {
    std::vector<std::string> path;
    for (int i = 0; i < (int)raw.size(); ++i) {
        if (raw[i] == sep) {
            path.push_back("");
        } else {
            path.back().push_back(raw[i]);
        }
    }

    return path;
}