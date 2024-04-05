#pragma once

#include <deque>
#include <stdexcept>
#include <string>

#include "../Utils/Utils.h"

enum expr_type { lvalue, rvalue };

struct tos_elem {
    std::string _lex = "";
    bool _is_op = false;
    expr_type _type = expr_type::lvalue;

    tos_elem(const std::string& lex) : _lex(utils::const_parser(lex)), _type(expr_type::lvalue) {
        _is_op = utils::is_op(_lex);
        if (!_is_op && _lex != lex) _type = expr_type::rvalue;
    }
};

class TypeOpStack {
   private:
    std::deque<tos_elem> _stack;

   private:
    tos_elem check_bin(tos_elem lex1, tos_elem lex2, tos_elem lex3) const;
    tos_elem check_uno(tos_elem lex1, tos_elem lex2) const;
    tos_elem check_assignment(tos_elem lex1, tos_elem lex2, tos_elem lex3) const;

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

    void check_init_list(const std::string& type,
                         const std::vector<std::string>& other_types) const;

    int size() const { return (int)_stack.size(); }
    std::string back() const;
    std::deque<std::string> get_stack() const;
    std::string pop_back();
};