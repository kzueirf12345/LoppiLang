#pragma once

#include "TID.h"

enum NodeType { ROOT, SCOPE, FUNC, BODY, TYPE };

/**
 * Вершина для дерева TID'ов
*/
struct Node {
   public:
    /// @brief Тип вершины
    NodeType _type;
    /// @brief TID
    TID _tid;
    /// Указатель на родителя
    Node* _parent;
    /// @brief Имя
    std::string _name;
    /// @brief Список указателей на детей
    std::map<std::string, Node*> _children;

   public:
    Node() : _type(NodeType::ROOT), _parent(nullptr), _name("") {}
    Node(NodeType type) : _type(type), _parent(nullptr), _name("") {}
    Node(NodeType type, Node* parent) : _type(type), _parent(parent), _name("") {}
    Node(NodeType type, Node* parent, std::string name)
        : _type(type), _parent(parent), _name(name) {}

    void erase_child(std::string name);
};

class TIDTree {
   private:
    /// @brief Указатель на корневую вершину
    Node* _root;
    /// @brief Указатель на текущую вершину
    Node* _current_node;

    /// @brief Список jump'ов
    std::vector<std::string> _jump_aims;

   private:
    /// @brief Функция поиск переменной
    /// @param cur Указатель на текущую вершину
    /// @param name Имя переменной
    /// @param path Путь до переменной
    /// @param position Позиция переменной
    /// @return Полный путь до переменной
    std::string find_var(Node* cur, std::string& name, const std::vector<std::string>& path,
                         int position) const;
    /// @brief Функция поиска переменной
    /// @param cur Указатель на текущую вершину
    /// @param name Имя переменной
    /// @return Полный путь до переменной
    std::string find_var(Node* cur, std::string& name) const;

    /// @brief Функция поиска функции
    /// @param cur Указатель на текущую вершину
    /// @param name Имя функции
    /// @param param_types Список параметров функции
    /// @param path Путь до функции
    /// @param position Поизиция функции
    /// @return Полный путь до функции
    std::string find_func(Node* cur, std::string& name, std::vector<std::string>& param_types,
                          const std::vector<std::string>& path, int position) const;
    /// @brief Функция поиск функции
    /// @param cur Указатель на текущую вершину
    /// @param name Имя функции
    /// @param param_types Список параметров функции 
    /// @return Полный путь до функции
    std::string find_func(Node* cur, std::string& name,
                          std::vector<std::string>& param_types) const;

    // TODO: REDESIGN
    std::map<std::string, std::string>& find_type(Node* cur, std::string name,
                                                  std::vector<std::string> path,
                                                  int position) const;
    // TODO: REDESIGN
    std::map<std::string, std::string>& find_type(Node* cur, std::string name) const;

    // TODO: REDESIGN
    std::string find_field(std::string& type, std::vector<std::string>& path, int position) const;

    /// @brief Функция, проверяющая возвращаемое значение функции
    /// @param cur Указатель на текущую вершину
    /// @param type Тип, возвращаемого из функции значения
    /// @return Возвращаемый тип проверяемой функции
    std::string check_return_type(Node* cur, const std::string& type) const;

    /// @brief Функция удаления переменной
    /// @param cur Указатель на текущий TID
    /// @param name Имя переменной
    /// @param path Путь до переменной
    /// @param position Позиция переменой
    void erase_var(Node* cur, std::string name, std::vector<std::string> path, int position);
    /// @brief Фукнция удаления переменной
    /// @param cur Указатель на текущий TID
    /// @param name Имя переменной
    void erase_var(Node* cur, std::string name);

    /// @brief Функция, проверяющая правильность пути
    /// @param cur Указатель на текущий TID
    /// @param name Имя проверяемого объекта
    /// @param path Проверяемый путь
    /// @param position Позиция объекта
    void check_scope_path(Node* cur, const std::string& name, const std::vector<std::string>& path,
                          int position) const;
    /// @brief Функция проверяющая правильность пути
    /// @param name Полный путь с именем
    void check_scope(std::string name) const;

    // TODO: REDESIGN
    //	DON'T USE!!! DANGEROUS!!!
    void push_field(std::string type, std::string name, std::string var_type);
    //	DON'T USE!!! DANGEROUS!!!

   public:
    TIDTree() : _root(new Node(NodeType::ROOT)), _current_node(_root) {}

    /// @brief Функция создания нового TID'а
    /// @param type Тип TID'а
    /// @param name Имя
    void create_tid(NodeType type, std::string name = "");
    /// @brief Функция выхода из TID'а
    void leave_tid();

    std::string get_var_type(std::string name, std::string fields = "") const;
    std::string get_func_type(std::string name, std::vector<std::string> param_types) const;
    // TODO: REDESIGN
    std::map<std::string, std::string> get_type_fields(std::string name) const;

    std::string check_fields(std::string type, std::vector<std::string> path) const;
    void check_jumps() const;
    std::string check_return(std::string type) const;

    void push_var(std::string name, std::string type);
    void push_func(std::string name, std::string return_type, func_parameters params);
    // TODO: REDESIGN
    void push_type(std::string name, std::map<std::string, std::string> fields = {});
    void push_jump(std::string name) { _jump_aims.push_back(name); }

    void delete_var(std::string name);

    Node* get_current_node() const noexcept { return _current_node; }
};