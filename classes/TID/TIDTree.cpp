#include "TIDTree.h"

void Node::erase_child(std::string name) {
    if (!_children.count(name)) {
        throw std::invalid_argument("space " + name + " doesn't exist");
    }
    delete _children[name];
    _children.erase(name);
}

std::string TIDTree::find_var(Node* cur, std::string& name, const std::vector<std::string>& path,
                              int position) const {
    if (position == (int)path.size()) {
        return cur->_tid.find_var(name);
    }

    if (!cur->_children.count(path[position])) {
        throw std::invalid_argument("path incorrect");
    }

    auto down_type = cur->_children.find(path[position])->second->_type;
    if (down_type == NodeType::SCOPE || down_type == NodeType::BODY) {
        return find_var(cur->_children[path[position]], name, path, position + 1);
    }
    throw std::invalid_argument("path incorrect");
}

std::string TIDTree::find_var(Node* cur, std::string& name) const {
    if (cur->_parent->_type == NodeType::ROOT) {
        return cur->_tid.find_var(name);
    }

    try {
        return cur->_tid.find_var(name);
    } catch (...) {
        return find_var(cur->_parent, name);
    }
}

std::string TIDTree::find_func(Node* cur, std::string& name, std::vector<std::string>& param_types,
                               const std::vector<std::string>& path, int position) const {
    if (position == (int)path.size()) {
        return cur->_tid.find_func(name, param_types);
    }

    if (!cur->_children.count(path[position])) {
        throw std::invalid_argument("path incorrect");
    }

    auto down_type = cur->_children.find(path[position])->second->_type;
    if (down_type == NodeType::SCOPE) {
        return find_func(cur->_children[path[position]], name, param_types, path, position + 1);
    }
    throw std::invalid_argument("path incorrect");
}

std::string TIDTree::find_func(Node* cur, std::string& name,
                               std::vector<std::string>& param_types) const {
    if (cur->_parent->_type == NodeType::ROOT) {
        return cur->_tid.find_func(name, param_types);
    }

    try {
        return cur->_tid.find_func(name, param_types);
    } catch (...) {
        return find_func(cur->_parent, name, param_types);
    }
}

std::map<std::string, std::string>& TIDTree::find_type(Node* cur, std::string name,
                                                       std::vector<std::string> path,
                                                       int position) const {
    if (position == (int)path.size()) {
        return cur->_tid.find_type(name);
    }

    if (!cur->_children.count(path[position])) {
        throw std::invalid_argument("path incorrect");
    }

    auto down_type = cur->_children.find(path[position])->second->_type;
    if (down_type == NodeType::SCOPE) {
        return find_type(cur->_children[path[position]], name, path, position + 1);
    }
    throw std::invalid_argument("path incorrect");
}

std::map<std::string, std::string>& TIDTree::find_type(Node* cur, std::string name) const {
    if (cur->_parent->_type == NodeType::ROOT) {
        return cur->_tid.find_type(name);
    }

    try {
        return cur->_tid.find_type(name);
    } catch (...) {
        return find_type(cur->_parent, name);
    }
}

std::string TIDTree::find_field(std::string& type, std::vector<std::string>& path,
                                int position) const {
    auto fields = get_type_fields(type);

    if (!fields.count(path[position])) {
        throw std::invalid_argument("Type " + type + " hasn't field " + path[position]);
    }

    if (position == (int)path.size() - 1) return fields[path[position]];

    return find_field(fields[path[position]], path, position + 1);
}

std::string TIDTree::check_return_type(Node* cur, const std::string& type) const {
    if (cur->_type == NodeType::ROOT)
        throw std::logic_error("you can't return " + type + " not in fucntions");
    if (cur->_type != NodeType::FUNC) return check_return_type(cur->_parent, type);

    std::string return_type = cur->_parent->_tid.find_func(cur->_name);
    return_type = utils::const_parser(return_type);
    auto new_type = utils::const_parser(type);

    if (return_type == new_type &&
        (return_type == "string" || return_type == "array" || return_type == "void"))
        return return_type;

    while (return_type.size() > 4) return_type.pop_back();
    while (new_type.size() > 4) new_type.pop_back();

    if (return_type == new_type) return type;
    throw std::logic_error("type " + type + " does not match the return type: " +
                           cur->_parent->_tid.find_func(cur->_name));
}

void TIDTree::erase_var(Node* cur, std::string name, std::vector<std::string> path, int position) {
    if (position == (int)path.size()) {
        cur->_tid.delete_var(name);
        return;
    }

    if (!cur->_children.count(path[position])) {
        throw std::invalid_argument("path incorrect");
    }

    auto down_type = cur->_children.find(path[position])->second->_type;
    if (down_type == NodeType::SCOPE || down_type == NodeType::BODY) {
        erase_var(cur->_children[path[position]], name, path, position + 1);
        return;
    }
    throw std::invalid_argument("path incorrect");
}

void TIDTree::erase_var(Node* cur, std::string name) {
    if (cur->_parent->_type == NodeType::ROOT) {
        cur->_tid.delete_var(name);
        return;
    }

    try {
        cur->_tid.delete_var(name);
    } catch (...) {
        erase_var(cur->_parent, name);
        return;
    }
}

void TIDTree::check_scope_path(Node* cur, const std::string& name,
                               const std::vector<std::string>& path, int position) const {
    if (position == (int)path.size()) {
        if (!cur->_children.count(name)) {
            std::string message = "";
            for (auto& el : path) message += el + "::";
            throw std::logic_error("path: " + message + name + " incorrect");
        }
        return;
    }

    if (!cur->_children.count(path[position])) {
        std::string message = "";
        for (auto& el : path) message += el + "::";
        throw std::logic_error("path: " + message + name + " incorrect");
    }

    check_scope_path(cur->_children[path[position]], name, path, position + 1);
}

void TIDTree::check_scope(std::string name) const {
    auto [aim, path] = utils::path_parser(name);
    check_scope_path(_root, aim, path, 0);
}

void TIDTree::create_tid(NodeType type, std::string name) {
    if (_current_node->_children.count(name)) {
        throw std::invalid_argument("space " + name + " already exists");
    }

    if (type == NodeType::ROOT) {
        throw std::logic_error("you can't create ROOT");
    }
    if (_current_node->_type == NodeType::BODY && type != NodeType::BODY) {
        throw std::logic_error("in BODY you can create only BODY");
    }
    if (_current_node->_type == NodeType::TYPE) {
        throw std::logic_error("ypu can't create spaces in TYPE");
    }
    if (_current_node->_type == NodeType::FUNC && type != NodeType::BODY) {
        throw std::logic_error("in FUNC you can create only BODY");
    }

    if (type == NodeType::FUNC) {
        _current_node->_tid.find_func(name);
    }

    Node* new_node = new Node(type, _current_node, name);
    _current_node->_children[name] = new_node;
    _current_node = new_node;

    if (type == NodeType::FUNC) {
        auto parent = _current_node->_parent;
        auto params = parent->_tid.get_funcs().get_parametrs(name);
        for (auto& [var, var_type] : params) {
            push_var(var, var_type);
        }
    }
}

void TIDTree::leave_tid() {
    if (_current_node->_parent == nullptr) {
        throw std::logic_error("there is no way back");
    }

    std::string name = _current_node->_name;
    NodeType type = _current_node->_type;
    _current_node = _current_node->_parent;

    if (type == NodeType::BODY) {
        _current_node->erase_child("");
    }
}

std::string TIDTree::get_var_type(std::string name, std::string fields) const {
    auto [short_name, path] = utils::path_parser(name);
    auto field_path = utils::path_parser(fields, '.');

    std::string name_type;

    if (path.size() == 0)
        name_type = find_var(_current_node, short_name);
    else
        name_type = find_var(_root, short_name, path, 0);

    if (field_path.size() == 0) return name_type;

    return check_fields(name_type, field_path);
}

std::string TIDTree::get_func_type(std::string name, std::vector<std::string> param_types) const {
    auto [short_name, path] = utils::path_parser(name);

    if (path.size() == 0) {
        return find_func(_current_node, short_name, param_types);
    }
    return find_func(_root, short_name, param_types, path, 0);
}

std::map<std::string, std::string> TIDTree::get_type_fields(std::string name) const {
    if (utils::_all_built_in_types.count(name)) {
        return {};
    }

    auto [short_name, path] = utils::path_parser(name);

    if (path.size() == 0) {
        return find_type(_current_node, short_name);
    }
    return find_type(_root, short_name, path, 0);
}

std::string TIDTree::check_fields(std::string type, std::vector<std::string> path) const {
    return find_field(type, path, 0);
}

void TIDTree::check_jumps() const {
    for (auto& aim : _jump_aims) {
        check_scope(aim);
    }
}

std::string TIDTree::check_return(std::string type) const {
    return check_return_type(_current_node, type);
}

void TIDTree::push_var(std::string name, std::string type) {
    if (_current_node->_type == NodeType::ROOT) {
        throw std::logic_error("you can't create global variables");
    }
    get_type_fields(type);
    _current_node->_tid.push_var(name, type);
}

void TIDTree::push_func(std::string name, std::string return_type, func_parameters params) {
    if (_current_node->_type == NodeType::ROOT) {
        throw std::logic_error("you can't create global functions");
    }

    if (_current_node->_type != NodeType::SCOPE) {
        throw std::logic_error("you can create funnctions directly in scopes");
    }

    if (return_type != "void") get_type_fields(return_type);

    for (auto& [var, var_type] : params) {
        get_type_fields(var_type);
    }

    _current_node->_tid.push_func(name, return_type, params);
}

void TIDTree::push_type(std::string name, std::map<std::string, std::string> fields) {
    if (_current_node->_type == NodeType::ROOT) {
        throw std::logic_error("you can't create global types");
    }

    if (_current_node->_type != NodeType::SCOPE) {
        throw std::logic_error("you can create types directly on scopes");
    }

    if (utils::_all_built_in_types.count(name))
        throw std::logic_error("Type " + name + " is a built-in type");

    for (auto& [var, var_type] : fields) {
        get_type_fields(var_type);
    }

    _current_node->_tid.push_type(name, fields);
}

void TIDTree::push_field(std::string type, std::string name, std::string field_type) {
    auto fields = get_type_fields(type);

    if (fields.count(name)) {
        throw std::invalid_argument("Field " + name + " already exists in type " + type);
    }

    fields[name] = field_type;
}

void TIDTree::delete_var(std::string name) {
    auto [short_name, path] = utils::path_parser(name);

    if (path.size() == 0) {
        erase_var(_current_node, short_name);
        return;
    }
    erase_var(_root, short_name, path, 0);
}