#include "TypeOpStack.h"

tos_elem TypeOpStack::check_bin(tos_elem lex1, tos_elem lex2, tos_elem lex3) const {
    auto s1 = utils::const_parser(lex1._lex);
    auto s2 = utils::const_parser(lex2._lex);
    auto op = lex3._lex;

    if (!utils::_built_in_types.count(s1) || !utils::_built_in_types.count(s2))
        throw std::logic_error("incorrect types: " + s1 + " " + s2);

    if (s1 != s2 && (s1 == "string" || s2 == "string" || s1 == "array" || s2 == "array"))
        throw std::logic_error(s1 + " " + op + " " + s2);

    if (s1 == s2 && s1 == "string") {
        if (op == "+")
            return tos_elem("string");
        else if (utils::_comp_ops.count(op))
            return tos_elem("byte");
    } else if (s1 == s2 && s1 == "array") {
        if (op == "==" || op == "!=") return tos_elem("byte");
    } else {
        if (utils::_comp_ops.count(op) || utils::_logic_bin_ops.count(op))
            return tos_elem("byte");
        else if (utils::_bin_ops.count(op))
            return utils::max_byte(s1, s2);
    }
    throw std::logic_error(s1 + " " + op + " " + s2);
}

tos_elem TypeOpStack::check_uno(tos_elem lex1, tos_elem lex2) const {
    auto s = lex1._lex;
    auto op = lex2._lex;

    if (!utils::_const_built_in_types.count(s) && !utils::_built_in_types.count(s))
        throw std::invalid_argument("incorrect type: " + s);

    if (!utils::_uno_ops.count(op) && !utils::_logic_uno_ops.count(op))
        throw std::invalid_argument("incorrect operation: " + op);

    if (s == "string" || s == "array") throw std::logic_error(s + " " + op);

    if (lex1._type == expr_type::rvalue && (op == "++" || op == "--"))
        throw std::logic_error(s + " " + op);

    if (utils::_logic_uno_ops.count(op)) return tos_elem("byte");

    return s;
}

tos_elem TypeOpStack::check_assignment(tos_elem lex1, tos_elem lex2, tos_elem lex3) const {
    auto s1 = lex1._lex, s2 = lex2._lex, op = lex3._lex;

    if (lex1._type == expr_type::rvalue) throw std::logic_error("you can't change const types");

    auto old1 = s1, old2 = s2;

    if (!utils::_built_in_types.count(s1) || !utils::_built_in_types.count(s2))
        throw std::logic_error("incorrect types: " + s1 + " " + s2);

    if (s1 != s2 && (s1 == "string" || s2 == "string" || s1 == "array" || s2 == "array"))
        throw std::logic_error(s1 + " " + op + " " + s2);

    if (s1 == s2 && s1 == "string") {
        if (op == "+=" || op == "=") return tos_elem("string");
    } else {
        if (utils::_assign_ops.count(op) /* && max_byte(s1, s2) == s1 */)
            return tos_elem(utils::max_byte(s1, s2));
    }
    throw std::logic_error(old1 + " " + op + " " + old2);
}

std::string TypeOpStack::check_bin() {
    if ((int)_stack.size() < 3) throw std::logic_error("too few elements");

    auto s2 = _stack.back();
    _stack.pop_back();
    auto op = _stack.back();
    _stack.pop_back();
    auto s1 = _stack.back();
    _stack.pop_back();

    auto ans = check_bin(s1, s2, op);
    _stack.push_back(ans);
    return ans._lex;
}

std::string TypeOpStack::check_uno() {
    if ((int)_stack.size() < 2) throw std::logic_error("too few elements");

    auto op = _stack.back();
    _stack.pop_back();
    auto s = _stack.back();
    _stack.pop_back();

    auto ans = check_uno(s, op);
    _stack.push_back(ans);
    return ans._lex;
}

std::string TypeOpStack::check_assignment() {
    if ((int)_stack.size() < 3) throw std::logic_error("too few elements");

    auto s2 = _stack.back();
    _stack.pop_back();
    auto op = _stack.back();
    _stack.pop_back();
    auto s1 = _stack.back();
    _stack.pop_back();

    auto ans = check_assignment(s1, s2, op);
    _stack.push_back(ans);
    return ans._lex;
}

std::string TypeOpStack::eq_bool() const {
    if (_stack.empty()) throw std::logic_error("tmkyoo few elements");

    std::string s = back();
    if (utils::is_op(s)) throw std::logic_error("top of the stack: " + s);

    std::string new_s = utils::const_parser(s);
    while (new_s.size() > 4) new_s.pop_back();
    if (new_s == "byte") return s;

    throw std::logic_error(s + " isn't bool(byte)");
}

std::string TypeOpStack::eq_void() const {
    if (_stack.empty()) throw std::logic_error("too few elements");

    std::string s = back();
    if (utils::is_op(s)) throw std::logic_error("top of the stack: " + s);

    if (s == "void") return s;

    throw std::logic_error(s + " isn't void");
}

std::string TypeOpStack::eq_array() const {
    std::string s = back();
    if (s != "array" || s != "const_array")
        throw std::logic_error(s + " isn't array or const_array");
    return s;
}

void TypeOpStack::check_comma() {
    if ((int)_stack.size() < 2) throw std::logic_error("too few elements");

    std::string s, op;
    s = back();
    _stack.pop_back();
    op = back();
    _stack.pop_back();

    if (op == ",") return;
    _stack.push_back(op);
    _stack.push_back(s);
}

void TypeOpStack::check_init_list(const std::string& type,
                                  const std::vector<std::string>& other_types) const {
    for (auto& el : other_types) utils::check_cast(el, type);
}

void TypeOpStack::push(std::string lexem) { _stack.push_back(tos_elem(lexem)); }

void TypeOpStack::clear() {
    while (!_stack.empty()) _stack.pop_back();
}

std::string TypeOpStack::back() const {
    if (_stack.size() == 0) throw std::logic_error("stack is empty");
    return _stack.back()._lex;
}

std::deque<std::string> TypeOpStack::get_stack() const {
    std::deque<std::string> res;
    for (auto& el : _stack) {
        res.push_front(el._lex);
    }

    return res;
}

std::string TypeOpStack::pop_back() {
    if (_stack.size() == 0) throw std::logic_error("stack is empty");
    std::string ans = back();
    _stack.pop_back();
    return ans;
}