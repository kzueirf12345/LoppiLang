#pragma once

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "../Utils/Utils.h"

using func_parameters = std::vector<std::pair<std::string, std::string>>;

struct VarTID {
   private:
    // name, type
    std::map<std::string, std::string> _variables;

   public:
    void insert(std::string name, std::string type);
    void erase(std::string name);

    std::string get_type(std::string name) const;

    std::map<std::string, std::string> get_vars() const { return _variables; }
};

struct FuncTID {
   private:
    // name, return_type, params
    std::map<std::string, std::pair<std::string, func_parameters>> _functions;

   public:
    void insert(std::string name, std::string return_type, func_parameters params);

    std::string get_return_type(std::string name) const;
    std::string get_return_type(std::string name, std::vector<std::string> param_types) const;

    std::vector<std::pair<std::string, std::string>> get_parametrs(std::string name) const;

    std::map<std::string, std::pair<std::string, func_parameters>> get_funcs() const {
        return _functions;
    }
};

struct TypeTID {
   private:
    // name, fields
    std::map<std::string, std::map<std::string, std::string>> _types;

   public:
    void insert(std::string name, std::map<std::string, std::string> fields);

    std::map<std::string, std::string>& get_fields(std::string name);

    std::map<std::string, std::map<std::string, std::string>> get_types() const { return _types; }
};

class TID {
   private:
    VarTID _vars;
    FuncTID _funcs;
    TypeTID _types;

   public:
    VarTID get_vars() const { return _vars; }
    FuncTID get_funcs() const { return _funcs; }
    TypeTID get_types() const { return _types; }

    void push_var(std::string name, std::string type) { _vars.insert(name, type); }

    void push_func(std::string name, std::string return_type,
                   std::vector<std::pair<std::string, std::string>> params) {
        _funcs.insert(name, return_type, params);
    }

    void push_type(std::string name, std::map<std::string, std::string> fields) {
        _types.insert(name, fields);
    }

    void delete_var(std::string name) { _vars.erase(name); }

    std::string find_var(std::string name) const { return _vars.get_type(name); }
    std::string find_func(std::string name, std::vector<std::string> param_types) const {
        return _funcs.get_return_type(name, param_types);
    }
    std::string find_func(std::string name) const { return _funcs.get_return_type(name); }
    std::map<std::string, std::string>& find_type(std::string name) {
        return _types.get_fields(name);
    }
};