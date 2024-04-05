#pragma once

#include "Lexer.h"
#include "../TID/TIDTree.h"
#include "../TypeOpStack/TypeOpStack.h"
#include "../RPN/RPN.h"

class Syntaxer {
   public:
    Syntaxer(Lexer& lexer)
        : lexer(&lexer), tid_tree(TIDTree()), type_stack(TypeOpStack()) {}

    void Programm() {
        lexer->PushLexem("Lol", true);
        get();
        Scope();
        get();
        Subprogramm();
        tid_tree.check_jumps();
        std::cerr << "Syntax analyze is OK" << std::endl;
        std::cerr << "Semantic analyze is OK" << std::endl;
    }

    void Scope() {  // sem
        if (lexem.name != "scope") {
            throw lexem;
            ;
        }
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;               // sem
        tid_tree.create_tid(NodeType::SCOPE, name);  // sem
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
        tid_tree.leave_tid();  // sem
    }

    void Subprogramm() {
        while (!lexem.is_eof) {
            Scope();
            get();
        }
    }

    void Statements() {
        Statement();
        get();
        Other_statements();
    }

    void Statement() {
        type_stack.clear();
        if (lexem.name == "function") {
            Function();
        } else if (lexem.name == "if" || lexem.name == "while" ||
                   lexem.name == "for" || lexem.name == "lopin" ||
                   lexem.name == "lopout" || lexem.name == "delete") {
            Complicated_operator();
        } else if (lexem.name == "type") {
            Complicated_type();
        } else if (lexem.type == 8) {
            Var();
        } else if (lexem.name == "jump") {
            Jump();
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
            }
        } else {
            Expr();
            get();
            if (lexem.name != ";") {
                throw lexem;
            }
        }
    }

    void Other_statements() {
        while (lexem.name == "function" || lexem.name == "if" ||
               lexem.name == "while" || lexem.name == "for" ||
               lexem.name == "lopin" || lexem.name == "lopout" ||
               lexem.name == "delete" || lexem.type == 2 ||
               lexem.name == "type" || lexem.type == 8 ||
               lexem.name == "jump") {
            Statement();
            get();
        }
        --lexer->lexem_ind;
    }

    void Function() {  // sem
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
    }

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

    void Complicated_type() {  // sem
        if (lexem.name != "type") {
            throw lexem;
        }
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;  // sem
        // lexer->tree.insert(name, 8);
        get();
        if (lexem.name != "{") {
            throw lexem;
        }
        std::vector<std::pair<std::string, std::string>> fields;  // sem
        get();
        Vars(fields);
        std::map<std::string, std::string> fields_map;
        for (const auto& elem : fields) {
            fields_map[elem.first] = elem.second;
        }
        if (fields.size() != fields_map.size()) {
            throw lexem;
        }
        get();
        if (lexem.name != "}") {
            throw lexem;
        }
        tid_tree.push_type(name, fields_map);  // sem
    }

    std::pair<std::string, std::string> Var() {  // sem
        std::string type = Type();               // sem
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;  // sem
        // std::cout << name << " " << type << std::endl;
        tid_tree.push_var(name, type);  // sem
        get();
        if (lexem.name == "=") {
            get();
            type_stack.push(type);  // sem
            type_stack.push("=");   // sem
            Expr();
            type_stack.check_assignment();  // sem
            type_stack.clear();             // sem
            get();
        }
        if (lexem.name != ";") {
            throw lexem;
        }
        return {name, type};
    }

    std::pair<std::string, std::string> Var_without_sem() {  // sem
        std::string type = Type();                           // sem
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;  // sem
        get();
        if (lexem.name == "=") {
            get();
            type_stack.push(type);  // sem
            type_stack.push("=");   // sem
            Expr();
            type_stack.check_assignment();  // sem
            type_stack.clear();             // sem
            get();
        }
        if (lexem.name != ";") {
            throw lexem;
        }
        return {name, type};
    }

    void Jump() {  // sem
        if (lexem.name != "jump") {
            throw lexem;
        }
        get();
        std::string name;  // sem
        Call_name(name);   // sem
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
        tid_tree.push_jump(name);  // sem
    }

    void Var_enum(func_parameters& params) {  // sem
        std::string type = Type();            // sem
        get();
        if (lexem.type != 2) {
            throw lexem;
        }
        std::string name = lexem.name;
        params.push_back({name, type});  // sem
        get();
        Other_var_enum(params);
    }

    void Func_statements() {
        Func_statement();
        get();
        Other_func_statements();
    }

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
        tid_tree.leave_tid();
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
    }

    void While() {                            // sem
        tid_tree.create_tid(NodeType::BODY);  // sem
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
        if (lexem.name != "{") {
            throw lexem;
        }
        get();
        Body_statements();
        get();
        if (lexem.name != "}") {
            throw lexem;
        }
        get();
        tid_tree.leave_tid();
        if (lexem.name == "else") {
            tid_tree.create_tid(NodeType::BODY);
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
            tid_tree.leave_tid();
        } else {
            --lexer->lexem_ind;
        }
    }

    void For() {                              // sem
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
        while (lexem.type == 8 || lexem.type == 2) {
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

        type_stack.clear();  // sem
        Expr();
        type_stack.eq_bool();  // sem
        type_stack.clear();    // sem
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
        get();

        Expr();
        get();
        if (lexem.name != ")") {
            throw lexem;
        }
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
        // Stoyakovskiu::a_pochemy_vu_na(shkebede_toalete)? //TODO STOYAKOVSKY
        get();
        tid_tree.leave_tid();
        if (lexem.name == "else") {
            tid_tree.create_tid(NodeType::BODY);
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
            tid_tree.leave_tid();
        } else {
            --lexer->lexem_ind;
        }
    }

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
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
    }

    void Call_names(std::vector<std::string>& parametrs) {
        std::string name;
        Call_name(name);
        parametrs.push_back(name);
        get();
        Other_call_names(parametrs);
    }

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
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
    }

    void Delete() {  // sem
        if (lexem.name != "delete") {
            throw lexem;
        }
        get();
        std::string name;  // sem
        Call_name(name);
        tid_tree.delete_var(name);  // sem
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
    }

    void Call_func() {     // sem
        std::string name;  // sem
        Call_name(name);
        get();
        if (lexem.name != "(") {
            throw lexem;
        }
        get();
        std::vector<std::string> types;  // sem
        Exprs(types);
        get();
        if (lexem.name != ")") {
            throw lexem;
        }
        tid_tree.get_func_type(name, types);
    }

    void Vars(
        std::vector<std::pair<std::string, std::string>>& params) {  // sem
        params.push_back(Var_without_sem());                         // sem
        get();
        Other_vars(params);
    }

    void Expr() {
        Assigment_expr();
        get();
        Other_expr();
    }

    void Call_name(std::string& name) {  // sem
        if (lexem.type != 2) {
            throw lexem;
        }
        name += lexem.name;  // sem
        get();
        Other_call_name(name);
    }

    void Other_var_enum(func_parameters& params) {
        while (lexem.name == ",") {
            get();
            Var_enum(params);
            get();
        }
        --lexer->lexem_ind;
    }

    void Func_statement() {
        if (lexem.name == "if" || lexem.name == "while" ||
            lexem.name == "for" || lexem.name == "lopin" ||
            lexem.name == "lopout" || lexem.name == "delete") {
            Complicated_operator();
        } else if (lexem.type == 8) {
            Var();
        } else if (lexem.name == "return") {
            Return();
        } else if (lexem.name == "type") {
            Complicated_type();
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
            }
        } else {
            Expr();
            get();
            if (lexem.name != ";") {
                throw lexem;
            }
        }
    }

    void Other_func_statements() {
        while (lexem.name == "if" || lexem.name == "while" ||
               lexem.name == "for" || lexem.name == "lopin" ||
               lexem.name == "lopout" || lexem.name == "delete" ||
               lexem.type == 2 || lexem.type == 8 || lexem.name == "return" ||
               lexem.name == "type") {
            type_stack.clear();
            Func_statement();
            get();
        }
        --lexer->lexem_ind;
    }

    void Body_statements(std::map<std::string, std::string> params = {}) {
        while (lexem.name == "if" || lexem.name == "while" ||
               lexem.name == "for" || lexem.name == "lopin" ||
               lexem.name == "lopout" || lexem.name == "delete" ||
               lexem.type == 2 || lexem.type == 8 || lexem.name == "type" ||
               lexem.name == "break" || lexem.name == "continue" ||
               lexem.name == "return")  // для return в функциях
        {
            Body_statement(params);
            get();
        }
        --lexer->lexem_ind;
    }

    void Exprs(std::vector<std::string>& types) {
        Assigment_expr();
        types.push_back(type_stack.back());
        get();
        Other_exprs(types);
    }

    void Other_vars(std::vector<std::pair<std::string, std::string>>& params) {
        while (lexem.type == 8 || lexem.type == 2) {
            Vars(params);
            get();
        }
        --lexer->lexem_ind;
    }

    void Assigment_expr() {
        Logical_implication();
        get();
        Other_assigment_expr();
    }

    void Other_expr() {  // sem
        while (lexem.name == ",") {
            type_stack.push(",");  // sem
            get();
            Assigment_expr();
            type_stack.check_comma();
            get();
        }
        --lexer->lexem_ind;
    }

    void Other_call_name(std::string& name) {  // sem
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

    void Call_field(std::string& fields) {
        while (lexem.name == ".") {
            fields += ".";
            get();
            if (lexem.type != 2) {
                throw lexem;
            }
            fields += lexem.name;
            get();
        }
        --lexer->lexem_ind;
    }

    void Return() {
        if (lexem.name != "return") {
            throw lexem;
        }
        get();
        if (lexem.type == 2 || lexem.type == 3 || lexem.type == 4 ||
            lexem.type == 5 || lexem.name == "{") {
            Expr();
            get();
        }
        if (lexem.name != ";") {
            throw lexem;
        }
        tid_tree.check_return(type_stack.back());
    }

    void Body_statement(std::map<std::string, std::string>& params) {  // sem
        for (const auto& elem : params) {
            tid_tree.push_var(elem.first, elem.second);
        }
        if (lexem.name == "if" || lexem.name == "while" ||
            lexem.name == "for" || lexem.name == "lopin" ||
            lexem.name == "lopout" || lexem.name == "delete") {
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
            }
        } else {
            Expr();
            get();
            if (lexem.name != ";") {
                throw lexem;
            }
        }
    }

    void Other_exprs(std::vector<std::string>& types) {
        while (lexem.name == ",") {
            get();
            Assigment_expr();
            types.push_back(type_stack.back());
            get();
        }
        --lexer->lexem_ind;
    }

    void Logical_implication() {
        Logical_or();
        get();
        Other_logical_implication();
    }

    void Other_assigment_expr() {  // sem
        while (lexem.name == "=" || lexem.name == "+=" || lexem.name == "->=" ||
               lexem.name == "-=" || lexem.name == "*=" ||
               lexem.name == "**=" || lexem.name == "/=" ||
               lexem.name == "^=" || lexem.name == "&=" || lexem.name == "|=" ||
               lexem.name == "%=") {
            type_stack.push(lexem.name);  // sem
            get();
            Assigment_expr();
            type_stack.check_assignment();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Jump_operator() {
        if (lexem.name != "break" && lexem.name != "continue") {
            throw lexem;
        }
        get();
        if (lexem.name != ";") {
            throw lexem;
        }
    }

    void Logical_or() {
        Logical_and();
        get();
        Other_logical_or();
    }

    void Other_logical_implication() {  // sem
        while (lexem.name == "->") {
            get();
            type_stack.push("->");  // sem
            Logical_or();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Logical_and() {
        Bitwise_or();
        get();
        Other_logical_and();
    }

    void Other_logical_or() {  // sem
        while (lexem.name == "||") {
            type_stack.push("||");  // sem
            get();
            Logical_and();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Bitwise_or() {
        Bitwise_xor();
        get();
        Other_bitwise_or();
    }

    void Other_logical_and() {  // sem
        while (lexem.name == "&&") {
            type_stack.push("&&");  // sem
            get();
            Bitwise_or();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Bitwise_xor() {
        Bitwise_and();
        get();
        Other_bitwise_xor();
    }

    void Other_bitwise_or() {  // sem
        while (lexem.name == "|") {
            type_stack.push("|");  // sem
            get();
            Bitwise_xor();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Bitwise_and() {
        Comparison_equality();
        get();
        Other_bitwise_and();
    }

    void Other_bitwise_xor() {  // sem
        while (lexem.name == "^") {
            type_stack.push("^");  // sem
            get();
            Bitwise_and();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Comparison_equality() {
        Comparison_comparison();
        get();
        Other_comparison_equality();
    }

    void Other_bitwise_and() {  // sem
        while (lexem.name == "&") {
            type_stack.push("&");  // sem
            get();
            Comparison_equality();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Comparison_comparison() {
        Sumsub();
        get();
        Other_comparison_comparison();
    }

    void Other_comparison_equality() {  // sem
        while (lexem.name == "==" || lexem.name == "!=") {
            type_stack.push(lexem.name);  // sem
            get();
            Comparison_comparison();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Sumsub() {
        Muldiv();
        get();
        Other_sumsub();
    }

    void Other_comparison_comparison() {  // sem
        while ((lexem.name == ">" || lexem.name == ">=" || lexem.name == "<" ||
                lexem.name == "<=") &&
               flag_compar) {
            type_stack.push(lexem.name);  // sem
            get();
            Sumsub();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Muldiv() {
        Power();
        get();
        Other_muldiv();
    }

    void Other_sumsub() {  // sem
        while (lexem.name == "+" || lexem.name == "-") {
            type_stack.push(lexem.name);  // sem
            get();
            Muldiv();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Power() {
        Unary();
        get();
        Other_power();
    }

    void Other_muldiv() {  // sem
        while (lexem.name == "/" || lexem.name == "*" || lexem.name == "%") {
            type_stack.push(lexem.name);  // sem
            get();
            Power();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Unary() {  // sem
        long long cnt = 0;
        while (lexem.name == "++" || lexem.name == "--" || lexem.name == "+" ||
               lexem.name == "-" || lexem.name == "!" || lexem.name == "~") {
            ++cnt;
            type_stack.push(lexem.name);  // check after //sem
            get();
        }
        Call_expr();
        while (cnt--) type_stack.check_uno();  // sem
    }

    void Other_power() {  // sem
        while (lexem.name == "**") {
            type_stack.push("**");  // sem
            get();
            Power();
            type_stack.check_bin();  // sem
            get();
        }
        --lexer->lexem_ind;
    }

    void Call_expr() {
        Gensec();
        get();
        Other_call_expr();
    }

    void Gensec() {  // sem
        if (lexem.name == "(") {
            get();
            Expr();
            get();
            if (lexem.name != ")") {
                throw lexem;
            }
        } else if (lexem.type == 3 || lexem.type == 4) {
            type_stack.push("byte8");  // sem
        } else if (lexem.type == 5) {
            type_stack.push("string");  // sem
        } else if (lexem.name == "{") {
            type_stack.push("array");  // sem
            Init_list();
        } else if (lexem.type == 2) {
            std::string name;  // sem
            Call_name(name);
            get();
            if (lexem.name == "(") {
                get();
                std::vector<std::string> types;  // sem
                if (lexem.name != ")") {
                    Exprs(types);
                    get();
                    if (lexem.name != ")") {
                        throw lexem;
                    }
                }
                type_stack.push(tid_tree.get_func_type(name, types));
            } else if (lexem.name == ".") {
                std::string fields;
                Call_field(fields);
                type_stack.push(
                    tid_tree.get_var_type(name, fields));  // sem //fields
            } else {
                type_stack.push(tid_tree.get_var_type(name));  // sem
                --lexer->lexem_ind;
            }
        } else {
            throw lexem;
        }
    }

    void Other_call_expr() {  // sem
        if (lexem.name == "[") {
            type_stack.eq_array();
            get();
            Expr();
            type_stack.eq_bool();  // sem
            get();
            if (lexem.name != "]") {
                throw lexem;
            }
        }
        --lexer->lexem_ind;
    }

    void Init_list() {  // TODO check to array type
        if (lexem.name != "{") {
            throw lexem;
        }
        get();
        Init_list_content();
        get();
        if (lexem.name != "}") {
            throw lexem;
        }
    }

    void Init_list_content() {
        Assigment_expr();
        type_stack.pop_back();
        get();
        Other_init_list_content();
    }

    void Other_init_list_content() {
        while (lexem.name == ",") {
            get();
            Assigment_expr();
            type_stack.pop_back();
            get();
        }
        --lexer->lexem_ind;
    }

    std::string Type() {
        std::string type = lexem.name;
        if (lexem.name == "array" || lexem.name == "const_array") {
            get();
            if (lexem.name != "<") {
                throw lexem;
            }
            get();
            if (lexem.name == "array" || lexem.name == "const_array") {
                throw lexem;
            }
            Type();
            get();
            if (lexem.name != ",") {
                throw lexem;
            }
            get();
            flag_compar = false;
            Expr();
            flag_compar = true;
            get();
            if (lexem.name != ">") {
                throw lexem;
            }
        } else if (lexem.type != 8 && lexem.type != 2) {
            throw lexem;
        }
        return type;
    }

    Lexem lexem;

   private:
    Lexer* lexer;
    bool flag_compar = true;

    Lexem get() { return lexem = lexer->gc(); }

    TIDTree tid_tree;
    TypeOpStack type_stack;
};