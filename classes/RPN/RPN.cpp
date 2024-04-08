#include "RPN.h"

void RPN::push(const string& s) {
    rpn_elem el;
    el._name = s;

    try {
        if (utils::is_op(s))
            el._type = rpn_elem_type::_operation;
        else
            el._type = rpn_elem_type::_operand;
    } catch (std::logic_error) {
        el._type = rpn_elem_type::_operand;
    }

    _data.push_back(el);
}

void RPN::blank(const string& type, const string& name) {
    _data.push_back({name, rpn_elem_type::_address});
    _data.push_back({type, rpn_elem_type::_command});
}

rpn_elem& RPN::operator[](int id) {
    try {
        return _data[id];
    } catch (...) {
        throw std::logic_error("array index out of range");
    }
}

void RPN::print_rpn() const {
    cout << "[ ";
    for (int i = 0; i < (int)_data.size(); ++i) {
        auto& el = _data[i];
        cout << "(" << el._name << ", " << i << ") ";
    }
    cout << " ]";
}

GlobalRPN::GlobalRPN() { _all_data.push_back(RPN()); }

void GlobalRPN::push_scope(const string& path) {
    _scope_addresses[path] = {_current_rpn_id, _all_data[_current_rpn_id].size()};

    string name = std::to_string(_scope_addresses[path].first) + " : " +
                  std::to_string(_scope_addresses[path].second);
    for (auto& [rpn_id, id] : _undefined_scopes[path]) {
        _all_data[rpn_id][id]._name = name;
    }

    _undefined_scopes.erase(path);
}

void GlobalRPN::jump_blank(const string& path, const string& type) {
    string name = "";
    if (_scope_addresses.count(path))
        name = std::to_string(_scope_addresses[path].first) + " : " +
               std::to_string(_scope_addresses[path].second);
    else
        _undefined_scopes[path].push_back({_current_rpn_id, _all_data[_current_rpn_id].size()});
    _all_data[_current_rpn_id].blank(type, name);
}

void GlobalRPN::push_func(const string& name, const string& return_type,
                          const func_parameters& params) {
    _rpn_ids.push(_current_rpn_id);
    _current_rpn_id = _all_data.size();
    _all_data.push_back(RPN());

    _func_table[name] = {return_type, params, _current_rpn_id};

    for (auto& [f, s] : params) {
        push(s);
        push(f);
        push(";");
    }
}

void GlobalRPN::leave_func() {
    _current_rpn_id = _rpn_ids.top();
    _rpn_ids.pop();
}

rpn_elem& GlobalRPN::operator[](int id) {
    try {
        return _all_data[_current_rpn_id][id];
    } catch (...) {
        throw std::logic_error("array index out of range");
    }
}

rpn_elem& GlobalRPN::back() {
    return _all_data[_current_rpn_id][_all_data[_current_rpn_id].size() - 1];
}

void GlobalRPN::print_global_rpn() const {
    for (int i = 0; i < (int)_all_data.size(); ++i) {
        cout << i << " ";
        _all_data[i].print_rpn();
        cout << "\n";
    }
}