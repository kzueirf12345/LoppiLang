#pragma once

#include "../RPN/RPN.h"
#include "../TID/TIDTree.h"
#include "../TypeOpStack/TypeOpStack.h"
#include "Lexer.h"

using std::to_string;

/**
    @brief Класс для синтаксического анализа рекурсивным спуском
    @throw SyntaxerError неправильный порядок лексем
*/
class Syntaxer {
   public:
    /**
        @brief Конструктор, инциализирующий приватные поля
        @details @ref TIDTree "tid_tree", @ref TypeOpStack "type_stack", @ref GlobalRPN "rpn"
        инциализируются соответсвующим конструктором по умолчанию
    */
    Syntaxer(Lexer& lexer)
        : lexer(&lexer), tid_tree(TIDTree()), type_stack(TypeOpStack()), rpn(GlobalRPN()) {}

    [[nodiscard]] GlobalRPN& RPN() noexcept { return rpn; }
    [[nodiscard]] const GlobalRPN& RPN() const noexcept { return rpn; }

    /**
        @brief Функция, возвращающая scope, который мы в даннный момент обрабатываем
    */
    string str_scope() const {
        string res;
        for (int i = 0; i < scope.size() - 1; ++i) {
            res += scope[i] + "::";
        }
        res += scope.back();
        return res;
    }

    /**
        @brief Точка входа в рекурсивный спуск
    */
    void Programm() {
        lexer->PushLexem("Lol", true);
        get();
        Scope();
        get();
        Subprogramm();
        tid_tree.check_jumps();

        rpn.push("eof");

        std::cerr << "Syntax analyze is OK" << std::endl;
        std::cerr << "Semantic analyze is OK" << std::endl;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Scope() {
        if (lexem.name != "scope") {
            throw lexem;
        }
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;               // sem
        tid_tree.create_tid(NodeType::SCOPE, name);  // sem
        scope.push_back(name);                       // gen
        rpn.push_scope(str_scope());                 // gen
        get();
        if (lexem.name != "{") {
            throw lexem;
        }
        get();
        type_stack.clear();
        Statements();
        get();
        if (lexem.name != "}") {
            throw lexem;
        }
        scope.pop_back();      // gen
        tid_tree.leave_tid();  // sem
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Subprogramm() {
        while (!lexem.is_eof) {
            Scope();
            get();
        }
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Statements() {
        Statement();
        get();
        Other_statements();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Statement() {
        type_stack.clear();
        if (lexem.name == "function") {
            Function();
        } else if (lexem.name == "if" || lexem.name == "while" || lexem.name == "for" ||
                   lexem.name == "lopin" || lexem.name == "lopout" || lexem.name == "delete") {
            Complicated_operator();
        } else if (lexem.type == 8) {
            Var();
        } else if (lexem.name == "jump") {
            Jump();
        } else if (lexem.name == "scope") {
            Scope();
        } else if (lexem.type == 2) {
            auto pos = lexer->lexem_ind;
            try {
                Var();
            } catch (...) {
                lexer->lexem_ind = --pos;
                get();
                Expr();
                get();
                if (lexem.name != ";") {
                    throw lexem;
                }
                rpn.push(";");  // gen
            }
        } else {
            Expr();
            get();
            if (lexem.name != ";") {
                throw lexem;
            }
            rpn.push(";");  // gen
        }
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_statements() {
        while (lexem.name == "function" || lexem.name == "if" || lexem.name == "while" ||
               lexem.name == "for" || lexem.name == "lopin" || lexem.name == "lopout" ||
               lexem.name == "delete" || lexem.type == 2 || lexem.name == "type" ||
               lexem.type == 8 || lexem.name == "jump" || lexem.name == "scope") {
            Statement();
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Function() {
        if (lexem.name != "function") {
            throw lexem;
        }
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;  // sem
        std::string type = "void";      // sem
        get();
        if (lexem.name == ":") {
            get();
            type = Type();  // sem
            get();
        }
        func_parameters params;  // sem
        if (lexem.name == "(") {
            get();
            Var_enum(params);
            get();
            if (lexem.name != ")") {
                throw lexem;
            }
            get();
        }
        tid_tree.push_func(name, type, params);     // sem
        tid_tree.create_tid(NodeType::FUNC, name);  // sem
        rpn.push_func(name, type, params);          // gen
        if (lexem.name != "{") {
            throw lexem;
        }
        get();
        Func_statements();
        get();
        if (lexem.name != "}") {
            throw lexem;
        }
        tid_tree.leave_tid();  // sem
        rpn.leave_func();      // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Complicated_operator() {
        if (lexem.name == "if") {
            If();
        } else if (lexem.name == "while") {
            While();
        } else if (lexem.name == "for") {
            For();
        } else if (lexem.name == "lopin") {
            Lopin();
        } else if (lexem.name == "lopout") {
            Lopout();
        } else if (lexem.name == "delete") {
            Delete();
        } else {
            throw lexem;
        }
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    std::pair<std::string, std::string> Var() {
        std::string type = Type();  // sem
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;  // sem
        // std::cout << name << " " << type << std::endl;
        tid_tree.push_var(name, type);  // sem
        rpn.push(name);                 // gen
        get();
        if (lexem.name == "=") {
            rpn.back()._name += address_marker;
            get();
            type_stack.push(type);  // sem
            type_stack.push("=");   // sem
            Expr();
            type_stack.check_assignment();  // sem
            type_stack.clear();             // sem
            get();
            rpn.push("=");  // gen
        }
        if (lexem.name != ";") {
            throw lexem;
        }
        rpn.push(";");  // gen
        return {name, type};
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Jump() {
        if (lexem.name != "jump") {
            throw lexem;
        }
        get();
        std::string name;      // sem
        Call_name(name);       // sem
        rpn.jump_blank(name);  // gen
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
        rpn.push(";");             // gen
        tid_tree.push_jump(name);  // sem
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Var_enum(func_parameters& params) {
        std::string type = Type();  // sem
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;
        params.push_back({name, type});  // sem
        get();
        Other_var_enum(params);
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Func_statements() {
        Func_statement();
        get();
        Other_func_statements();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void If() {
        if (lexem.name != "if") {
            throw lexem;
        }
        get();
        if (lexem.name != "(") {
            throw lexem;
        }
        get();
        Expr();
        type_stack.eq_bool();  // sem
        type_stack.clear();    // sem
        get();
        if (lexem.name != ")") {
            throw lexem;
        }
        auto p1 = rpn.size();  // gen
        rpn.blank("F!");       // gen
        get();
        if (lexem.name != "{") {
            throw lexem;
        }
        get();
        tid_tree.create_tid(NodeType::BODY);  // sem
        Body_statements();
        get();
        if (lexem.name != "}") {
            throw lexem;
        }
        get();
        tid_tree.leave_tid();                        // sem
        auto p2 = rpn.size();                        // gen
        rpn.blank();                                 // gen
        rpn[p1]._name = std::to_string(rpn.size());  // gen

        if (lexem.name == "else") {
            get();
            if (lexem.name == "{") {
                tid_tree.create_tid(NodeType::BODY);
                get();
                Body_statements();
                get();
                if (lexem.name != "}") {
                    throw lexem;
                }
                tid_tree.leave_tid();
            } else if (lexem.name == "if") {
                If();
            } else {
                throw lexem;
            }
        } else {
            --lexer->lexem_ind;
        }

        rpn[p2]._name = std::to_string(rpn.size());  // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void While() {
        tid_tree.create_tid(NodeType::BODY);  // sem

        auto adress1 = rpn.size();  // gen

        if (lexem.name != "while") {
            throw lexem;
        }
        get();
        if (lexem.name != "(") {
            throw lexem;
        }
        get();
        type_stack.clear();  // sem
        Expr();
        type_stack.eq_bool();  // sem
        type_stack.clear();    // sem
        get();
        if (lexem.name != ")") {
            throw lexem;
        }
        get();

        auto p1 = rpn.size();  // gen
        rpn.blank("F!");       // gen

        if (lexem.name != "{") {
            throw lexem;
        }
        get();
        Body_statements();
        get();
        if (lexem.name != "}") {
            throw lexem;
        }
        tid_tree.leave_tid();  // sem

        auto p2 = rpn.size();                   // gen
        rpn.blank();                            // gen
        rpn[p2]._name = to_string(adress1);     // gen
        rpn[p1]._name = to_string(rpn.size());  // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void For() {
        tid_tree.create_tid(NodeType::BODY);  // sem
        if (lexem.name != "for") {
            throw lexem;
        }
        get();
        if (lexem.name != "(") {
            throw lexem;
        }
        get();
        std::map<std::string, std::string> params;  // sem
        while (lexem.type == 8) {
            std::pair<std::string, std::string> p = Var();
            if (params.find(p.first) != params.end()) {
                throw lexem;
            }
            params[p.first] = p.second;
            get();
        }
        if (lexem.name != ";") {
            throw lexem;
        }
        get();
        rpn.push(";");               // gen
        auto address4 = rpn.size();  // gen

        type_stack.clear();  // sem
        Expr();
        type_stack.eq_bool();  // sem
        type_stack.clear();    // sem
        get();

        auto p1 = rpn.size();        // gen
        rpn.blank("F!");             // gen
        auto p2 = rpn.size();        // gen
        rpn.blank("!");              // gen

        if (lexem.name != ";") {
            throw lexem;
        }
        rpn.push(";");  // gen
        get();
        auto address3 = rpn.size();  // gen

        Expr();
        get();
        if (lexem.name != ")") {
            throw lexem;
        }
        rpn.push(";");                          // gen

        auto p4 = rpn.size();                   // gen
        rpn.blank("!");                         // gen
        rpn[p4]._name = to_string(address4);    // gen
        rpn[p2]._name = to_string(rpn.size());  // gen

        get();
        if (lexem.name != "{") {
            throw lexem;
        }
        get();
        Body_statements();
        get();
        if (lexem.name != "}") {
            throw lexem;
        }

        auto p3 = rpn.size();                   // gen
        rpn.blank("!");                         // gen
        rpn[p3]._name = to_string(address3);    // gen
        rpn[p1]._name = to_string(rpn.size());  // gen

        // Stoyakovskiu::a_pochemy_vu_na(shkebede_toalete)? STOYAKOVSKY
        // get(); //TODO почему закоментили то блять
        tid_tree.leave_tid();  // sem
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Lopin() {
        if (lexem.name != "lopin") {
            throw lexem;
        }
        get();
        if (lexem.name != "(") {
            throw lexem;
        }
        get();
        std::vector<std::string> parametrs;

        Call_names(parametrs);
        get();
        if (lexem.name != ")") {
            throw lexem;
        }

        for (auto& el : parametrs) {  // gen
            rpn.push(el);
        }
        rpn.push(to_string(parametrs.size()));  // gen
        rpn.push("lopin");                      // gen

        get();
        if (lexem.name != ";") {
            throw lexem;
        }
        rpn.push(";");  // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Call_names(std::vector<std::string>& parametrs) {
        std::string name;
        Call_name(name);
        parametrs.push_back(name);
        get();
        Other_call_names(parametrs);
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_call_names(std::vector<std::string>& parametrs) {
        while (lexem.name == ",") {
            get();
            std::string name;
            Call_name(name);
            parametrs.push_back(name);
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Lopout() {
        if (lexem.name != "lopout") {
            throw lexem;
        }
        get();
        if (lexem.name != "(") {
            throw lexem;
        }
        get();
        std::vector<std::string> types;
        Exprs(types);
        get();
        if (lexem.name != ")") {
            throw lexem;
        }

        rpn.push(to_string(types.size()));  // gen
        rpn.push("lopout");                 // gen

        get();
        if (lexem.name != ";") {
            throw lexem;
        }

        rpn.push(";");  // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Delete() {
        if (lexem.name != "delete") {
            throw lexem;
        }
        get();
        std::string name;  // sem
        Call_name(name);
        tid_tree.delete_var(name);  // sem

        rpn.push(name);      // gen
        rpn.push("delete");  // gen

        get();
        if (lexem.name != ";") {
            throw lexem;
        }

        rpn.push(";");  // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Call_func() {
        std::string name;  // sem
        Call_name(name);
        get();
        if (lexem.name != "(") {
            throw lexem;
        }
        get();
        std::vector<std::string> types;  // sem
        Exprs(types);
        rpn.push(name);    // gen
        rpn.push("call");  // gen
        get();
        if (lexem.name != ")") {
            throw lexem;
        }
        tid_tree.get_func_type(name, types);
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Expr() {
        Assigment_expr();
        get();
        Other_expr();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Call_name(std::string& name) {
        if (lexem.type != 2) {
            throw lexem;
        }
        name += lexem.name;  // sem
        get();
        Other_call_name(name);
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_var_enum(func_parameters& params) {
        while (lexem.name == ",") {
            get();
            Var_enum(params);
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Func_statement() {
        if (lexem.name == "if" || lexem.name == "while" || lexem.name == "for" ||
            lexem.name == "lopin" || lexem.name == "lopout" || lexem.name == "delete") {
            Complicated_operator();
        } else if (lexem.type == 8) {
            Var();
        } else if (lexem.name == "return") {
            Return();
        } else if (lexem.type == 2) {
            auto pos = lexer->lexem_ind;
            try {
                Var();
            } catch (...) {
                lexer->lexem_ind = --pos;
                get();
                Expr();
                get();
                if (lexem.name != ";") {
                    throw lexem;
                }
                rpn.push(";");  // gen
            }
        } else {
            Expr();
            get();
            if (lexem.name != ";") {
                throw lexem;
            }
            rpn.push(";");  // gen
        }
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_func_statements() {
        while (lexem.name == "if" || lexem.name == "while" || lexem.name == "for" ||
               lexem.name == "lopin" || lexem.name == "lopout" || lexem.name == "delete" ||
               lexem.type == 2 || lexem.type == 8 || lexem.name == "return" ||
               lexem.name == "type") {
            type_stack.clear();
            Func_statement();
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Body_statements(std::map<std::string, std::string> params = {}) {
        while (lexem.name == "if" || lexem.name == "while" || lexem.name == "for" ||
               lexem.name == "lopin" || lexem.name == "lopout" || lexem.name == "delete" ||
               lexem.type == 2 || lexem.type == 8 || lexem.name == "type" ||
               lexem.name == "break" || lexem.name == "continue" ||
               lexem.name == "return")  // для return в функциях
        {
            Body_statement(params);
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Exprs(std::vector<std::string>& types) {
        Assigment_expr();
        types.push_back(type_stack.back());
        get();
        Other_exprs(types);
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Assigment_expr() {
        Logical_implication();
        get();
        Other_assigment_expr();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_expr() {
        while (lexem.name == ",") {
            type_stack.push(",");      // sem
            string name = lexem.name;  // gen
            get();
            Assigment_expr();
            type_stack.check_comma();
            rpn.push(name);  // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_call_name(std::string& name) {
        while (lexem.name == "::") {
            get();
            if (lexem.type != 2) {
                throw lexem;
            }
            name += "::" + lexem.name;  // sem
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Return() {
        if (lexem.name != "return") {
            throw lexem;
        }
        get();
        if (lexem.type == 2 || lexem.type == 3 || lexem.type == 4 || lexem.type == 5 ||
            lexem.name == "{") {
            Expr();
            get();
            tid_tree.check_return(type_stack.back());
        }

        rpn.push("ret");  // gen

        if (lexem.name != ";") {
            throw lexem;
        }
        rpn.push(";");  // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Body_statement(std::map<std::string, std::string>& params) {
        for (const auto& elem : params) {
            tid_tree.push_var(elem.first, elem.second);
        }
        if (lexem.name == "if" || lexem.name == "while" || lexem.name == "for" ||
            lexem.name == "lopin" || lexem.name == "lopout" || lexem.name == "delete") {
            Complicated_operator();
        } else if (lexem.type == 8) {
            Var();
        } else if (lexem.name == "break" || lexem.name == "continue") {
            Jump_operator();
        } else if (lexem.name == "return") {
            Return();
        } else if (lexem.type == 2) {
            auto pos = lexer->lexem_ind;
            try {
                Var();
            } catch (...) {
                lexer->lexem_ind = --pos;
                get();
                Expr();
                get();
                if (lexem.name != ";") {
                    throw lexem;
                }
                rpn.push(";");  // gen
            }
        } else {
            Expr();
            get();
            if (lexem.name != ";") {
                throw lexem;
            }
            rpn.push(";");  // gen
        }
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_exprs(std::vector<std::string>& types) {
        while (lexem.name == ",") {
            get();
            Assigment_expr();
            types.push_back(type_stack.back());
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Logical_implication() {
        Logical_or();
        get();
        Other_logical_implication();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_assigment_expr() {
        while (lexem.name == "=" || lexem.name == "+=" || lexem.name == "->=" ||
               lexem.name == "-=" || lexem.name == "*=" || lexem.name == "**=" ||
               lexem.name == "/=" || lexem.name == "^=" || lexem.name == "&=" ||
               lexem.name == "|=" || lexem.name == "%=") {
            rpn.back()._name += address_marker;
            type_stack.push(lexem.name);  // sem
            string name = lexem.name;     // gen
            get();
            Assigment_expr();
            type_stack.check_assignment();  // sem
            rpn.push(name);                 // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Jump_operator() {
        if (lexem.name != "break" && lexem.name != "continue") {
            throw lexem;
        }
        rpn.push(lexem.name);  // gen
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
        rpn.push(";");  // gen
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Logical_or() {
        Logical_and();
        get();
        Other_logical_or();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_logical_implication() {
        while (lexem.name == "->") {
            string name = lexem.name;  // gen
            type_stack.push("->");     // sem
            get();
            Logical_or();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Logical_and() {
        Bitwise_or();
        get();
        Other_logical_and();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_logical_or() {
        while (lexem.name == "||") {
            type_stack.push("||");     // sem
            string name = lexem.name;  // gen
            get();
            Logical_and();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Bitwise_or() {
        Bitwise_xor();
        get();
        Other_bitwise_or();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_logical_and() {
        while (lexem.name == "&&") {
            type_stack.push("&&");     // sem
            string name = lexem.name;  // gen
            get();
            Bitwise_or();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Bitwise_xor() {
        Bitwise_and();
        get();
        Other_bitwise_xor();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_bitwise_or() {
        while (lexem.name == "|") {
            type_stack.push("|");      // sem
            string name = lexem.name;  // gen
            get();
            Bitwise_xor();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Bitwise_and() {
        Comparison_equality();
        get();
        Other_bitwise_and();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_bitwise_xor() {
        while (lexem.name == "^") {
            type_stack.push("^");      // sem
            string name = lexem.name;  // gen
            get();
            Bitwise_and();
            rpn.push(name);          // gen
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Comparison_equality() {
        Comparison_comparison();
        get();
        Other_comparison_equality();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_bitwise_and() {
        while (lexem.name == "&") {
            type_stack.push("&");      // sem
            string name = lexem.name;  // gen
            get();
            Comparison_equality();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Comparison_comparison() {
        Sumsub();
        get();
        Other_comparison_comparison();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_comparison_equality() {
        while (lexem.name == "==" || lexem.name == "!=") {
            type_stack.push(lexem.name);  // sem
            string name = lexem.name;     // gen
            get();
            Comparison_comparison();
            rpn.push(name);          // gen
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Sumsub() {
        Muldiv();
        get();
        Other_sumsub();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_comparison_comparison() {
        while (lexem.name == ">" || lexem.name == ">=" || lexem.name == "<" || lexem.name == "<=") {
            string name = lexem.name;     // gen
            type_stack.push(lexem.name);  // sem
            get();
            Sumsub();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Muldiv() {
        Power();
        get();
        Other_muldiv();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_sumsub() {
        while (lexem.name == "+" || lexem.name == "-") {
            string name = lexem.name;     // gen
            type_stack.push(lexem.name);  // sem
            get();
            Muldiv();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Power() {
        Unary();
        get();
        Other_power();
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_muldiv() {
        while (lexem.name == "/" || lexem.name == "*" || lexem.name == "%") {
            string name = lexem.name;     // gen
            type_stack.push(lexem.name);  // sem
            get();
            Power();
            rpn.push(name);          // gen
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Unary() {
        std::vector<std::string> arr;  // gen sem
        while (lexem.name == "++" || lexem.name == "--" || lexem.name == "+" || lexem.name == "-" ||
               lexem.name == "!" || lexem.name == "~") {
            type_stack.push(lexem.name);  // sem
            arr.push_back(lexem.name);    // gen
            get();
        }
        Gensec();
        for (const auto& str : arr) {
            type_stack.check_uno();  // sem
            rpn.push(str);           // gen
        }
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Other_power() {
        while (lexem.name == "**") {
            std::string name = lexem.name;  // gen
            type_stack.push("**");          // sem
            get();
            Power();
            type_stack.check_bin();  // sem
            rpn.push(name);          // gen
            get();
        }
        --lexer->lexem_ind;
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    void Gensec() {
        if (lexem.name == "(") {
            get();
            Expr();
            get();
            if (lexem.name != ")") {
                throw lexem;
            }
        } else if (lexem.type == 3 || lexem.type == 4) {
            type_stack.push("byte4");  // sem
            rpn.push(lexem.name);      // gen
        } else if (lexem.type == 5) {
            type_stack.push("string");  // sem
            rpn.push(lexem.name);       // gen
        } else if (lexem.type == 2) {
            std::string name;  // sem
            Call_name(name);
            get();
            if (lexem.name == "(") {
                get();
                std::vector<std::string> types;  // sem
                auto prev_sz = type_stack.size();
                if (lexem.name != ")") {
                    Exprs(types);
                    get();
                    if (lexem.name != ")") {
                        throw lexem;
                    }
                    rpn.push(name);    // gen
                    rpn.push("call");  // gen
                }
                while (type_stack.size() > prev_sz) {
                    type_stack.pop_back();
                }
                type_stack.push(tid_tree.get_func_type(name, types));
            } else {
                type_stack.push(tid_tree.get_var_type(name));  // sem
                rpn.push(name);                                // gen
                --lexer->lexem_ind;
            }
        } else {
            throw lexem;
        }
    }
    /**
        @brief Функция рекурсивного спуска 
    */
    std::string Type() {
        std::string type = lexem.name;
        if (lexem.type != 8) {
            throw lexem;
        }
        return type;
    }

    /// Текущая лексема
    Lexem lexem;

   private:
    /// Символ, который обозначает использование адреса в нотации Бэкуса-Наура
    const string address_marker = "$";

    /// Стэк scope'ов, где мы сейчас находимся
    vector<string> scope;
    /// Указатель на лексер
    Lexer* lexer;

    /// Функция, переходящая к следующей лексеме
    Lexem get() { return lexem = lexer->gc(); }

    /// Дерево TID'ов для проверки семантики
    TIDTree tid_tree;
    /// Стэк типов и операций, для проверки на соответствие типов
    TypeOpStack type_stack;
    /// Объект класса для записи нотации Бэкуса-Наура
    GlobalRPN rpn;
};