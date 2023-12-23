#include "headers/TID.h"

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
        if (_functions.at(name).second[i].second != param_types[i])
            throw std::invalid_argument("Parametrs are incorrect");
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