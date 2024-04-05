#pragma once

#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

class utils {
   public:
    static const std::unordered_set<std::string> _comp_ops;
    static const std::unordered_set<std::string> _logic_bin_ops;
    static const std::unordered_set<std::string> _logic_uno_ops;
    static const std::unordered_set<std::string> _bin_ops;
    static const std::unordered_set<std::string> _uno_ops;
    static const std::unordered_set<std::string> _assign_ops;

    static const std::unordered_set<std::string> _built_in_types;
    static const std::unordered_set<std::string> _const_built_in_types;
    static const std::unordered_set<std::string> _all_built_in_types;

    static std::string const_parser(const std::string& s);
    static std::string max_byte(const std::string& l, const std::string& r);
    static std::string check_cast(const std::string& s1, const std::string& s2);
    static bool is_op(const std::string& s);
    static std::pair<std::string, std::vector<std::string>> path_parser(const std::string& raw);
    static std::vector<std::string> path_parser(const std::string& raw, char sep);
};