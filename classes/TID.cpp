#include "headers/TID.h"

std::string const_parser(const std::string& s) {
    if (s.size() <= 6) return s;

    std::string pref = "";
    for (int i = 0; i < 6; ++i) pref += s[i];

    if (pref != "const_") return s;

    std::string res = "";
    for (int i = 6; i < (int)s.size(); ++i) res += s[i];
    return res;
}

std::string check_cast(const std::string& s1, const std::string s2) {
    auto f = const_parser(s1), s = const_parser(s2);

    if (f == s && (f == "string" || f == "array")) return f;

    while (f.size() > 4) f.pop_back();
    while (s.size() > 4) s.pop_back();

    if (f == s) return f;
    throw std::logic_error("type " + s1 +
                           " does not match the return type: " + s2);
}

void VarTID::insert(std::string name, std::string type) {
    if (_variables.count(name)) {
        throw std::invalid_argument("Variable " + name + " already exists");
    }
    _variables[name] = type;
}

void VarTID::erase(std::string name) {
    if (!_variables.count(name)) {
        throw std::invalid_argument("Variable " + name + " doesn't exist");
    }
    _variables.erase(name);
}

std::string VarTID::get_type(std::string name) const {
    if (!_variables.count(name)) {
        throw std::invalid_argument("Variable " + name + " doesn't exist");
    }
    return _variables.at(name);
}

void FuncTID::insert(std::string name, std::string return_type,
                     func_parameters params) {
    if (_functions.count(name)) {
        throw std::invalid_argument("Function " + name + " already exists");
    }
    _functions[name] = {return_type, params};
}

std::string FuncTID::get_return_type(std::string name) const {
    if (!_functions.count(name)) {
        throw std::invalid_argument("Function " + name + " doesn't exist");
    }
    return _functions.at(name).first;
}

func_parameters FuncTID::get_parametrs(std::string name) const {
    if (!_functions.count(name)) {
        throw std::invalid_argument("Function " + name + " doesn't exist");
    }
    return _functions.at(name).second;
}

std::string FuncTID::get_return_type(
    std::string name, std::vector<std::string> param_types) const {
    if (!_functions.count(name)) {
        throw std::invalid_argument("Function " + name + " doesn't exist");
    }

    if (param_types.size() != _functions.at(name).second.size())
        throw std::invalid_argument("Parametrs are incorrect");

    for (int i = 0; i < (int)param_types.size(); ++i) {
        check_cast(_functions.at(name).second[i].second, param_types[i]);
    }

    return _functions.at(name).first;
}

void TypeTID::insert(std::string name,
                     std::map<std::string, std::string> fields) {
    if (_types.count(name)) {
        throw std::invalid_argument("Type " + name + " already exists");
    }
    _types[name] = fields;
}

std::map<std::string, std::string>& TypeTID::get_fields(std::string name) {
    if (!_types.count(name)) {
        throw std::invalid_argument("Type " + name + " doesn't exist");
    }
    return _types.at(name);
}