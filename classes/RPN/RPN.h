#pragma once

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "../Utils/Utils.h"

using std::cout;
using std::map;
using std::pair;
using std::stack;
using std::string;
using std::vector;

using func_parameters = vector<pair<string, string>>;

enum rpn_elem_type { _operation, _operand, _command, _address };
// _command: { !, F!, T!, }

struct rpn_elem {
    string _name = "";
    rpn_elem_type _type = rpn_elem_type::_operation;
};

/*
 *   blanks:
 *   1. for
 *   2. while
 *   3. if else
 *   4. jump
 */

class RPN {
   private:
    vector<rpn_elem> _data;
    map<string, int> _scope_addresses;           // [path, id]
    map<string, vector<int>> _undefined_scopes;  // [path, jumps to this path]

   public:
    // push lex
    void push(const string& s);
    // blank
    void blank(const string& type = "!", const string& name = "");

    int size() const noexcept { return _data.size(); }

    rpn_elem& operator[](int id);

    void print_rpn() const;
};

struct func_table_elem {
    string _return_type;
    func_parameters _params;
    int _rpn_id;
};

class GlobalRPN {
   private:
    vector<RPN> _all_data;
    map<string, pair<int, int>> _scope_addresses;           // [path, { rpn_id, id }]
    map<string, vector<pair<int, int>>> _undefined_scopes;  // [path, jumps to this path]

    map<string, func_table_elem> _func_table;

    int _current_rpn_id = 0;
    stack<int> _rpn_ids;

   public:
    GlobalRPN();

    // push lexem
    void push(const string& s) { _all_data[_current_rpn_id].push(s); }
    // push and memorize scope
    void push_scope(const string& path);
    // blank
    void blank(const string& type = "!") { _all_data[_current_rpn_id].blank(type); }
    // blank and memorize name
    void jump_blank(const string& name, const string& type = "!");

    // create new rpn for func
    void push_func(const string& name, const string& return_type, const func_parameters& params);
    // back to previous rpn
    void leave_func();

    int size() const { return _all_data[_current_rpn_id].size(); }

    rpn_elem& operator[](int id);
    rpn_elem& back();

    void print_global_rpn() const;
};