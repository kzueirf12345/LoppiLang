#pragma once

#include "Lexer.h"

class Syntaxer
{
public:
    Syntaxer(Lexer &lexer) : lexer(&lexer), lexem() {}

    void Programm()
    {
        lexer->PushLexem("Lol", true);
        get();
        Scope();
        get();
        Subprogramm();
        std::cerr << "Syntax is OK" << std::endl;
    }

    void Scope()
    {
        if (lexem.name != "scope")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.type == 2)
        {
            get();
        }
        if (lexem.name != "{")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Statements();
        get();
        if (lexem.name != "}")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Subprogramm()
    {
        while (!lexem.is_eof)
        {
            Scope();
            get();
        }
    }

    void Statements()
    {
        Statement();
        get();
        Other_statements();
    }

    void Statement()
    {
        if (lexem.name == "function")
        {
            Function();
        }
        else if (lexem.name == "if" ||
                 lexem.name == "while" ||
                 lexem.name == "for" ||
                 lexem.name == "lopin" ||
                 lexem.name == "lopout" ||
                 lexem.name == "delete")
        {
            Complicated_operator();
        }
        else if (lexem.name == "type")
        {
            Complicated_type();
        }
        else if (lexem.type == 8)
        {
            Var();
        }
        else if (lexem.name == "jump")
        {
            Jump();
        }
        else
        {
            Expr();
            get();
            if (lexem.name != ";")
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
            // throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Other_statements()
    {

        while (lexem.name == "function" ||
               lexem.name == "if" ||
               lexem.name == "while" ||
               lexem.name == "for" ||
               lexem.name == "lopin" ||
               lexem.name == "lopout" ||
               lexem.name == "delete" ||
               lexem.type == 2 ||
               lexem.name == "type" ||
               lexem.type == 8 ||
               lexem.name == "jump")
        {
            Statement();
            get();
        }
        --lexer->lexem_ind;
    }

    void Function()
    {
        if (lexem.name != "function")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.type != 2)
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name == ":")
        {
            get();
            Type();
            get();
        }
        if (lexem.name == "(")
        {
            get();
            Var_enum();
            get();
            if (lexem.name != ")")
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
            get();
        }
        if (lexem.name != "{")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Func_statements();
        get();
        if (lexem.name != "}")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Complicated_operator()
    {
        if (lexem.name == "if")
        {
            If();
        }
        else if (lexem.name == "while")
        {
            While();
        }
        else if (lexem.name == "for")
        {
            For();
        }
        else if (lexem.name == "lopin")
        {
            Lopin();
        }
        else if (lexem.name == "lopout")
        {
            Lopout();
        }
        else if (lexem.name == "delete")
        {
            Delete();
        }
        /*         else if (lexem.type == 2)
                {
                    Call_func();
                    get();
                    if (lexem.name != ";")
                    {
                        throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
                    }
                } */
        else
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Complicated_type()
    {
        if (lexem.name != "type")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.type != 2)
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "{")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Vars();
        get();
        if (lexem.name != "}")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Var()
    {
        Type();
        get();
        if (lexem.type != 2)
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name == "=")
        {
            get();
            Expr();
            get();
        }
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Jump()
    {
        if (lexem.name != "jump")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Call_name();
        get();
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Var_enum()
    {
        Type();
        get();
        if (lexem.type != 2)
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Other_var_enum();
    }

    void Func_statements()
    {
        Func_statement();
        get();
        Other_func_statements();
    }

    void If()
    {
        if (lexem.name != "if")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "(")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Expr();
        get();
        if (lexem.name != ")")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "{")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Body_statements();
        get();
        if (lexem.name != "}")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name == "else")
        {
            get();
            if (lexem.name == "{")
            {
                get();
                Body_statements();
                get();
                if (lexem.name != "}")
                {
                    throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
                }
            }
            else if (lexem.name == "if")
            {
                If();
            }
            else
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
        }
        else
        {
            --lexer->lexem_ind;
        }
    }

    void While()
    {
        if (lexem.name != "while")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "(")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Expr();
        get();
        if (lexem.name != ")")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "{")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Body_statements();
        get();
        if (lexem.name != "}")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name == "else")
        {
            get();
            if (lexem.name == "{")
            {
                get();
                Body_statements();
                get();
                if (lexem.name != "}")
                {
                    throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
                }
            }
        }
        else
        {
            --lexer->lexem_ind;
        }
    }

    void For()
    {
        if (lexem.name != "for")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "(")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Vars();
        get();
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Expr();
        get();
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Expr();
        get();
        if (lexem.name != ")")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "{")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Body_statements();
        get();
        if (lexem.name != "}")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }

        get();
        if (lexem.name == "else")
        {
            get();
            if (lexem.name == "{")
            {
                get();
                Body_statements();
                get();
                if (lexem.name != "}")
                {
                    throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
                }
            }
        }
        else
        {
            --lexer->lexem_ind;
        }
    }

    void Lopin()
    {
        if (lexem.name != "lopin")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "(")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Call_name();
        get();
        if (lexem.name != ")")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Lopout()
    {
        if (lexem.name != "lopout")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != "(")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Exprs();
        get();
        if (lexem.name != ")")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Delete()
    {
        if (lexem.name != "delete")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Call_name();
        get();
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Call_func()
    {
        Call_name();
        get();
        if (lexem.name != "(")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Exprs();
        get();
        if (lexem.name != ")")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Vars()
    {
        Var();
        get();
        Other_vars();
    }

    void Expr()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Assigment_expr();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_expr();
        if (cnt != 0)
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Call_name()
    {
        if (lexem.type != 2)
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Other_call_name();
    }

    void Other_var_enum()
    {

        while (lexem.name == ",")
        {
            get();
            Var_enum();
            get();
        }
        --lexer->lexem_ind;
    }

    void Func_statement()
    {
        if (lexem.name == "if" ||
            lexem.name == "while" ||
            lexem.name == "for" ||
            lexem.name == "lopin" ||
            lexem.name == "lopout" ||
            lexem.name == "delete")
        {
            Complicated_operator();
        }
        else if (lexem.type == 8)
        {
            Var();
        }
        else if (lexem.name == "return")
        {
            Return();
        }
        else if (lexem.name == "type")
        {
            Complicated_type();
        }
        else
        {
            Expr();
            get();
            if (lexem.name != ";")
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
            // throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Other_func_statements()
    {

        while (lexem.name == "if" ||
               lexem.name == "while" ||
               lexem.name == "for" ||
               lexem.name == "lopin" ||
               lexem.name == "lopout" ||
               lexem.name == "delete" ||
               lexem.type == 2 ||
               lexem.type == 8 ||
               lexem.name == "return" ||
               lexem.name == "type")
        {
            Func_statement();
            get();
        }
        --lexer->lexem_ind;
    }

    void Body_statements()
    {

        while (lexem.name == "if" ||
               lexem.name == "while" ||
               lexem.name == "for" ||
               lexem.name == "lopin" ||
               lexem.name == "lopout" ||
               lexem.name == "delete" ||
               lexem.type == 2 ||
               lexem.type == 8 ||
               lexem.name == "type" ||
               lexem.name == "break" ||
               lexem.name == "continue" ||
               lexem.name == "return") // для return в функциях
        {
            Body_statement();
            get();
        }
        --lexer->lexem_ind;
    }

    void Exprs()
    {
        Assigment_expr();
        get();
        Other_exprs();
    }

    void Other_vars()
    {

        while (lexem.type == 8)
        {
            Vars();
            get();
        }
        --lexer->lexem_ind;
    }

    void Assigment_expr()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Logical_implication();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_assigment_expr();
    }

    void Other_expr()
    {

        while (lexem.name == ",")
        {
            get();
            Assigment_expr();
            get();
        }
        --lexer->lexem_ind;
    }

    void Other_call_name()
    {

        while (lexem.name == "::")
        {
            get();
            if (lexem.type != 2)
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
            get();
        }
        --lexer->lexem_ind;
    }

    void Call_field()
    {

        while (lexem.name == ".")
        {
            get();
            if (lexem.type != 2)
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
            get();
        }
        --lexer->lexem_ind;
    }

    void Return()
    {
        if (lexem.name != "return")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.type == 2 ||
            lexem.type == 3 ||
            lexem.type == 4 ||
            lexem.type == 5 ||
            lexem.name == "{")
        {
            Expr();
            get();
        }
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Body_statement()
    {
        if (lexem.name == "if" ||
            lexem.name == "while" ||
            lexem.name == "for" ||
            lexem.name == "lopin" ||
            lexem.name == "lopout" ||
            lexem.name == "delete")
        {
            Complicated_operator();
        }
        else if (lexem.name == "type")
        {
            Complicated_type();
        }
        else if (lexem.type == 8)
        {
            Var();
        }
        else if (lexem.name == "break" ||
                 lexem.name == "continue")
        {
            Jump_operator();
        }
        else if (lexem.name == "return")
        {
            Return();
        }
        else
        {
            Expr();
            get();
            if (lexem.name != ";")
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
            // throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Other_exprs()
    {

        while (lexem.name == ",")
        {
            get();
            Assigment_expr();
            get();
        }
        --lexer->lexem_ind;
    }

    void Logical_implication()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Logical_or();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_logical_implication();
    }

    void Other_assigment_expr()
    {

        while (lexem.name == "=" ||
               lexem.name == "+=" ||
               lexem.name == "->=" ||
               lexem.name == "-=" ||
               lexem.name == "*=" ||
               lexem.name == "**=" ||
               lexem.name == "/=" ||
               lexem.name == "^=" ||
               lexem.name == "&=" ||
               lexem.name == "|=" ||
               lexem.name == "%=")
        {
            get();
            Assigment_expr();
            get();
        }
        --lexer->lexem_ind;
    }

    void Jump_operator()
    {
        if (lexem.name != "break" && lexem.name != "continue")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        if (lexem.name != ";")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Logical_or()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Logical_and();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_logical_or();
    }

    void Other_logical_implication()
    {

        while (lexem.name == "->")
        {
            get();
            Logical_or();
            get();
        }
        --lexer->lexem_ind;
    }

    void Logical_and()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Bitwise_or();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_logical_and();
    }

    void Other_logical_or()
    {

        while (lexem.name == "||")
        {
            get();
            Logical_and();
            get();
        }
        --lexer->lexem_ind;
    }

    void Bitwise_or()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Bitwise_xor();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_bitwise_or();
    }

    void Other_logical_and()
    {

        while (lexem.name == "&&")
        {
            get();
            Bitwise_or();
            get();
        }
        --lexer->lexem_ind;
    }

    void Bitwise_xor()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Bitwise_and();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_bitwise_xor();
    }

    void Other_bitwise_or()
    {

        while (lexem.name == "|")
        {
            get();
            Bitwise_xor();
            get();
        }
        --lexer->lexem_ind;
    }

    void Bitwise_and()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Comparison_equality();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_bitwise_and();
    }

    void Other_bitwise_xor()
    {

        while (lexem.name == "^")
        {
            get();
            Bitwise_and();
            get();
        }
        --lexer->lexem_ind;
    }

    void Comparison_equality()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Comparison_comparison();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_comparison_equality();
    }

    void Other_bitwise_and()
    {

        while (lexem.name == "&")
        {
            get();
            Comparison_equality();
            get();
        }
        --lexer->lexem_ind;
    }

    void Comparison_comparison()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Sumsub();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_comparison_comparison();
    }

    void Other_comparison_equality()
    {

        while (lexem.name == "==" ||
               lexem.name == "!=")
        {
            get();
            Comparison_comparison();
            get();
        }
        --lexer->lexem_ind;
    }

    void Sumsub()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Muldiv();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_sumsub();
    }

    void Other_comparison_comparison()
    {

        while ((lexem.name == ">" ||
                lexem.name == ">=" ||
                lexem.name == "<" ||
                lexem.name == "<=") &&
               flag_compar)
        {
            get();
            Sumsub();
            get();
        }
        --lexer->lexem_ind;
    }

    void Muldiv()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Power();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_muldiv();
    }

    void Other_sumsub()
    {

        while (lexem.name == "+" || lexem.name == "-")
        {
            get();
            Muldiv();
            get();
        }
        --lexer->lexem_ind;
    }

    void Power()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Unary();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        Other_power();
    }

    void Other_muldiv()
    {

        while (lexem.name == "/" ||
               lexem.name == "*" ||
               lexem.name == "%")
        {
            get();
            Power();
            get();
        }
        --lexer->lexem_ind;
    }

    void Unary()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        while (lexem.name == "++" ||
               lexem.name == "--" ||
               lexem.name == "+" ||
               lexem.name == "-" ||
               lexem.name == "!" ||
               lexem.name == "~")
        {
            get();
            while (lexem.name == "(")
            {
                ++cnt;
                get();
            }
        }
        Call_expr();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        --lexer->lexem_ind;
    }

    void Other_power()
    {

        while (lexem.name == "**")
        {
            get();
            Power();
            get();
        }
        --lexer->lexem_ind;
    }

    void Call_expr()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        Gensec();
        get();
        Other_call_expr();
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        --lexer->lexem_ind;
    }

    void Gensec()
    {
        while (lexem.name == "(")
        {
            ++cnt;
            get();
        }
        if (lexem.type != 3 &&
            lexem.type != 4 &&
            lexem.type != 5)
        {
            if (lexem.name == "{")
            {
                Init_list();
            }
            else if (lexem.type == 2)
            {
                Call_name();
                get();
                if (lexem.name == "(")
                {
                    get();
                    if (lexem.name != ")")
                    {
                        Exprs();
                        get();
                        if (lexem.name != ")")
                        {
                            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
                        }
                    }
                }
                else if (lexem.name == ".")
                {
                    Call_field();
                }
                else
                {
                    --lexer->lexem_ind;
                }
            }
            else
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
        }
        get();
        while (lexem.name == ")" && cnt > 0)
        {
            --cnt;
            get();
        }
        --lexer->lexem_ind;
    }

    void Other_call_expr()
    {

        while (lexem.name == "[")
        {
            get();
            Expr();
            get();
            if (lexem.name != "]")
            {
                throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
            }
        }
        --lexer->lexem_ind;
    }

    void Init_list()
    {
        if (lexem.name != "{")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
        get();
        Init_list_content();
        get();
        if (lexem.name != "}")
        {
            throw lexem.name + " number " + std::to_string(lexer->lexem_ind) + " on " + std::to_string(lexem.line) + " line";
        }
    }

    void Init_list_content()
    {
        Expr();
        get();
        Other_init_list_content();
    }

    void Other_init_list_content()
    {

        while (lexem.name == ",")
        {
            get();
            Expr();
            get();
        }
        --lexer->lexem_ind;
    }

    void Type()
    {
        if (lexem.name == "array" || lexem.name == "const_array")
        {
            get();
            if (lexem.name != "<")
            {
                throw lexem.name + " " + std::to_string(lexer->lexem_ind);
            }
            get();
            Type();
            get();
            if (lexem.name != ",")
            {
                throw lexem.name + " " + std::to_string(lexer->lexem_ind);
            }
            get();
            flag_compar = false;
            Expr();
            flag_compar = true;
            get();
            if (lexem.name != ">")
            {
                throw lexem.name + " " + std::to_string(lexer->lexem_ind);
            }
        }
        else if (lexem.type != 8)
        {
            throw lexem.name = " " + std::to_string(lexer->lexem_ind);
        }
    }

private:
    int64_t cnt = 0;
    Lexem lexem;
    Lexer *lexer;

    bool flag_compar = true;
    bool flag_boper = true;

    void get()
    {
        lexem = lexer->gc();
    }
};