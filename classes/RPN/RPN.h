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

/**
 * @brief Структура, хранящая имя и тип элемента нотации Бэкуса-Наура
 */
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

/**
    @brief Вспомогательный класс для GlobalRPN
*/
class RPN {
   private:
    /// @brief Список элементов Бэкуса-Науры
    vector<rpn_elem> _data;
    /// @brief Адресы каждого scope
    map<string, int> _scope_addresses;  // [path, id]
    /// @brief Список scope'ов, которых ещё нет, но на которые может быть совершён прыжок.
    map<string, vector<int>> _undefined_scopes;  // [path, jumps to this path]

   public:
    /// Функция добавления элемента в конец _data
    void push(const string& s);
    /// @brief Функция добавления условного/безусловного перехода
    /// @param type Тип перехода
    /// @param name Параметр перед переходом в полизе
    void blank(const string& type = "!", const string& name = "");

    int size() const noexcept { return _data.size(); }

    rpn_elem& operator[](int id);

    /// @brief Функция вывода полиза в консоль
    void print_rpn() const;
};

/**
 * @brief Структура для хранения функций исходного кода
 */
struct func_table_elem {
    /// @brief Тип возвращаемого значения
    string _return_type;
    /// @brief Параметры функции с их типами
    func_parameters _params;
    /// @brief Номер полиза
    int _rpn_id;
};

class GlobalRPN {
   private:
    /// @brief Список всех полизов
    vector<RPN> _all_data;
    /// @brief Адресы каждого scope
    map<string, pair<int, int>> _scope_addresses;  // [path, { rpn_id, id }]
    /// @brief Список scope'ов, которых ещё нет, но на которые может быть совершён прыжок.
    map<string, vector<pair<int, int>>> _undefined_scopes;  // [path, jumps to this path]

    /// @brief Список функций
    map<string, func_table_elem> _func_table;

    /// @brief Номер текущего полиза
    int _current_rpn_id = 0;
    /// @brief Стэк полизов
    stack<int> _rpn_ids;

   public:
    GlobalRPN();

    /// Функция добавления элемента в конец текущего полиза
    void push(const string& s) { _all_data[_current_rpn_id].push(s); }
    /// Функция добавления scope'а в конец текущего полиза
    void push_scope(const string& path);
    /// @brief Функция добавления условного/безусловного перехода в текущий полиз
    /// @param type Тип перехода
    /// @param name Параметр перед переходом в полизе
    void blank(const string& type = "!") { _all_data[_current_rpn_id].blank(type); }
    /// @brief Функция добавления jump'a
    /// @param name Имя scope'a в который осуществляется прыжок
    /// @param type Тип перехода
    void jump_blank(const string& name, const string& type = "!");

    /// Функция создания полиза новой функции
    void push_func(const string& name, const string& return_type, const func_parameters& params);
    /// Функция выхода из полиза функции
    void leave_func();

    int size() const { return _all_data[_current_rpn_id].size(); }

    rpn_elem& operator[](int id);
    rpn_elem& back();

    /// @brief Функция вывода всех полизов в консоль
    void print_global_rpn() const;
};