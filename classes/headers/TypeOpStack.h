#pragma once

#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

class TypeOpStack {
   private:
    const std::unordered_set<std::string> _comp_ops = {"==", "!=", ">",
                                                       "<",  ">=", "<="};
    const std::unordered_set<std::string> _logic_bin_ops = {"||", "&&"};
    const std::unordered_set<std::string> _logic_uno_ops = {"!"};
    const std::unordered_set<std::string> _bin_ops = {
        "->", "|", "^", "&", "+", "-", "/", "*", "%", "**"};
    const std::unordered_set<std::string> _uno_ops = {"++", "--", "~", "-",
                                                      "+"};
    const std::unordered_set<std::string> _assign_ops = {
        "=", "+=", "->=", "-=", "*=", "**=", "/=", "^=", "&=", "|=", "%="};

    const std::unordered_set<std::string> _built_in_types = {
        "string", "array", "byte", "byte2", "byte4", "byte8"};
    const std::unordered_set<std::string> _const_built_in_types = {
        "const_string", "const_array", "const_byte",
        "const_byte2",  "const_byte4", "const_byte8",
    };

    std::string const_parser(const std::string& s) const;
    std::string max_byte(const std::string& l, const std::string r) const;

    std::string check_cast(const std::string& s1, const std::string s2) const;

    std::string check_bin(std::string s1, std::string s2, std::string op) const;
    std::string check_uno(std::string s, std::string op) const;
    std::string check_assignment(std::string s1, std::string s2,
                                 std::string op) const;

    bool is_op(std::string s) const;

   private:
    std::deque<std::string> _stack;

   public:
    void push(std::string lexem);
    std::string check_bin();
    std::string check_uno();
    std::string check_assignment();
    std::string eq_bool() const;
    std::string eq_void() const;
    std::string eq_array() const;
    void clear();
    void check_comma();

    void check_init_list(const std::string& type, const std::vector<std::string>& other_types) const;

    int size() const { return _stack.size(); }
    std::string back() const;
    std::deque<std::string> get_stack() const { return _stack; }
    std::string pop_back();
};
