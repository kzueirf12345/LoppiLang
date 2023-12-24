#include "headers/TypeOpStack.h"

std::string TypeOpStack::const_parser(const std::string& s) const {
    if (s.size() <= 6) return s;

    std::string pref = "";
    for (int i = 0; i < 6; ++i) pref += s[i];

    if (pref != "const_") return s;

    std::string res = "";
    for (int i = 6; i < (int)s.size(); ++i) res += s[i];
    return res;
}

std::string TypeOpStack::max_byte(const std::string& l,
                                  const std::string r) const {
    if (l.size() == 4 && r.size() == 4)
        return "byte";
    else if (l.size() == 4)
        return r;
    else if (r.size() == 4)
        return l;
    else
        return l.back() > r.back() ? l : r;
}

std::string TypeOpStack::check_bin(std::string s1, std::string s2,
                                   std::string op) const {

    s1 = const_parser(s1);
    s2 = const_parser(s2);

    if (!_built_in_types.count(s1) || !_built_in_types.count(s2))
        throw std::logic_error("incorrect types: " + s1 + " " + s2);

    if (s1 != s2 &&
        (s1 == "string" || s2 == "string" || s1 == "array" || s2 == "array"))
        throw std::logic_error(s1 + " " + op + " " + s2);

    if (s1 == s2 && s1 == "string") {
        if (op == "+")
            return "string";
        else if (_comp_ops.count(op))
            return "byte";
    } else if (s1 == s2 && s1 == "array") {
        if (op == "==" || op == "!=") return "byte";
    } else
    {    if (_comp_ops.count(op) || _logic_bin_ops.count(op))
            return "byte";
        else if (_bin_ops.count(op))
            return max_byte(s1, s2);
    }
        throw std::logic_error(s1 + " " + op + " " + s2);
}

std::string TypeOpStack::check_uno(std::string s, std::string op) const {
    if (!_const_built_in_types.count(s) && !_built_in_types.count(s))
        throw std::invalid_argument("incorrect type: " + s);

    if (!_uno_ops.count(op) && !_logic_uno_ops.count(op))
        throw std::invalid_argument("incorrect operation: " + op);

    if (s == "string" || s == "array") throw std::logic_error(s + " " + op);

    if (s != const_parser(s) && (op == "++" || op == "--"))
        throw std::logic_error(s + " " + op);

    if (_logic_uno_ops.count(op)) return "byte";

    return s;
}

std::string TypeOpStack::check_assignment(std::string s1, std::string s2,
                                          std::string op) const {
    if (_const_built_in_types.count(s1))
        throw std::logic_error("you can't change const types");

    s1 = const_parser(s1);
    s2 = const_parser(s2);

    if (!_built_in_types.count(s1) || !_built_in_types.count(s2))
        throw std::logic_error("incorrect types: " + s1 + " " + s2);

    if (s1 != s2 &&
        (s1 == "string" || s2 == "string" || s1 == "array" || s2 == "array"))
        throw std::logic_error(s1 + " " + op + " " + s2);

    if (s1 == s2 && s1 == "string") {
        if (op == "+=" || op == "=") return "string";
    } else {
        if (_assign_ops.count(op))
            return max_byte(s1, s2);
    }
    throw std::logic_error(s1 + " " + op + " " + s2);
}

bool TypeOpStack::is_op(std::string s) const {
    if (s == ",") return true;
    if (_built_in_types.count(s) || _const_built_in_types.count(s))
        return false;

    if (_comp_ops.count(s) || _logic_bin_ops.count(s) ||
        _logic_uno_ops.count(s) || _bin_ops.count(s) || _uno_ops.count(s) ||
        _assign_ops.count(s))
        return true;

    throw std::invalid_argument("incorrect lexem: " + s);
}

void TypeOpStack::push(std::string lexem) {
    is_op(lexem);
    _stack.push_back(lexem);
}

std::string TypeOpStack::check_bin() {
    if (_stack.size() < 3) throw std::logic_error("too few elements");

    std::string s1, s2, op;
    s2 = _stack.back();
    _stack.pop_back();
    op = _stack.back();
    _stack.pop_back();
    s1 = _stack.back();
    _stack.pop_back();

    std::string ans = check_bin(s1, s2, op);
    _stack.push_back(ans);
    return ans;
}

std::string TypeOpStack::check_uno() {
    if (_stack.size() < 2) throw std::logic_error("too few elements");

    std::string s, op;
    s = _stack.back();
    _stack.pop_back();
    op = _stack.back();
    _stack.pop_back();

    std::string ans = check_uno(s, op);
    _stack.push_back(ans);
    return ans;
}

std::string TypeOpStack::check_assignment() {
    if (_stack.size() < 3) throw std::logic_error("too few elements");

    std::string s1, s2, op;
    s2 = _stack.back();
    _stack.pop_back();
    op = _stack.back();
    _stack.pop_back();
    s1 = _stack.back();
    _stack.pop_back();

    std::string ans = check_assignment(s1, s2, op);
    _stack.push_back(ans);
    return ans;
}

std::string TypeOpStack::eq_bool() const {
    if (_stack.empty()) throw std::logic_error("too few elements");

    std::string s = _stack.back();
    if (is_op(s)) throw std::logic_error("top of the stack: " + s);

    std::string new_s = const_parser(s);
    while (new_s.size() > 4) new_s.pop_back();
    if (new_s == "byte") return s;

    throw std::logic_error(s + " isn't bool(byte)");
}

std::string TypeOpStack::eq_void() const {
    if (_stack.empty()) throw std::logic_error("too few elements");

    std::string s = _stack.back();
    if (is_op(s)) throw std::logic_error("top of the stack: " + s);

    if (s == "void") return s;

    throw std::logic_error(s + " isn't void");
}

std::string TypeOpStack::eq_array() const {
    std::string s = back();
    if (s != "array" || s != "const_array")
        throw std::logic_error(s + " isn't array or const_array");
    return s;
}

void TypeOpStack::clear() {
    while (!_stack.empty()) _stack.pop_back();
}

void TypeOpStack::check_comma() {
    if (_stack.size() < 2) throw std::logic_error("too few elements");

    std::string s, op;
    s = _stack.back();
    _stack.pop_back();
    op = _stack.back();
    _stack.pop_back();

    if (op == ",") return;
    _stack.push_back(op);
    _stack.push_back(s);
}

std::string TypeOpStack::back() const {
    if (_stack.size() == 0) throw std::logic_error("stack is empty");
}