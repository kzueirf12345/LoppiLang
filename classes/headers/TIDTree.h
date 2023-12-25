#pragma once

#include "TID.h"

enum NodeType { ROOT, SCOPE, FUNC, BODY, TYPE };

struct Node {
   public:
    NodeType _type;
    TID _tid;
    Node* _parent;
    std::string _name;
    std::map<std::string, Node*> _children;

   public:
    Node() : _type(NodeType::ROOT), _parent(nullptr), _name("") {}
    Node(NodeType type) : _type(type), _parent(nullptr), _name("") {}
    Node(NodeType type, Node* parent)
        : _type(type), _parent(parent), _name("") {}
    Node(NodeType type, Node* parent, std::string name)
        : _type(type), _parent(parent), _name(name) {}

    void erase_child(std::string name);
};

class TIDTree {
   private:
    Node* _root;
    Node* _current_node;

    std::vector<std::string> _jump_aims;

    const std::set<std::string> _built_in_types = {
        "string",     "array",       "byte",         "byte2",
        "byte4",      "byte8",       "const_string", "const_array",
        "const_byte", "const_byte2", "const_byte4",  "const_byte8",
    };

   private:
    std::string const_parser(const std::string& s) const;

    std::pair<std::string, std::vector<std::string>> path_parser(
        std::string raw) const;
    std::vector<std::string> path_parser(std::string raw, char sep) const;

    std::string find_var(Node* cur, std::string& name,
                         const std::vector<std::string>& path,
                         int position) const;
    std::string find_var(Node* cur, std::string& name) const;

    std::string find_func(Node* cur, std::string& name,
                          std::vector<std::string>& param_types,
                          const std::vector<std::string>& path,
                          int position) const;
    std::string find_func(Node* cur, std::string& name,
                          std::vector<std::string>& param_types) const;

    std::map<std::string, std::string>& find_type(Node* cur, std::string name,
                                                  std::vector<std::string> path,
                                                  int position) const;
    std::map<std::string, std::string>& find_type(Node* cur,
                                                  std::string name) const;

    std::string find_field(std::string& type, std::vector<std::string>& path,
                           int position) const;

    std::string check_return_type(Node* cur, const std::string& type) const;

    void erase_var(Node* cur, std::string name, std::vector<std::string> path,
                   int position);
    void erase_var(Node* cur, std::string name);

    void check_scope_path(Node* cur, const std::string& name,
                          const std::vector<std::string>& path,
                          int position) const;
    void check_scope(std::string name) const;

    //	DON'T USE!!! DANGEROUS!!!
    void push_field(std::string type, std::string name, std::string var_type);
    //	DON'T USE!!! DANGEROUS!!!

   public:
    TIDTree() : _root(new Node(NodeType::ROOT)), _current_node(_root) {}

    void create_tid(NodeType type, std::string name = "");
    void leave_tid();

    std::string get_var_type(std::string name, std::string fields = "") const;
    std::string get_func_type(std::string name,
                              std::vector<std::string> param_types) const;
    std::map<std::string, std::string> get_type_fields(std::string name) const;

    std::string check_fields(std::string type,
                             std::vector<std::string> path) const;
    void check_jumps() const;
    std::string check_return(std::string type) const;

    void push_var(std::string name, std::string type);
    void push_func(std::string name, std::string return_type,
                   func_parameters params);
    void push_type(std::string name,
                   std::map<std::string, std::string> fields = {});
    void push_jump(std::string name) { _jump_aims.push_back(name); }

    void delete_var(std::string name);

    Node* get_current_node() const noexcept { return _current_node; }
};
