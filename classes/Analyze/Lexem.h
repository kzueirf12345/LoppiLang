#pragma once

#include <string>

struct Lexem
{
    Lexem() : name(""), type(-1), is_eof(false) {};
    Lexem(std::string name, long long type) : name(std::move(name)), type(type), is_eof(false){}
    Lexem(std::string name, long long type, bool eof) : name(std::move(name)), type(type), is_eof(eof){}
    Lexem(std::string name, long long type, bool eof, long long line) : name(std::move(name)), type(type), is_eof(eof), line(line){}
    std::string name;
    long long line;
    long long type;
    bool is_eof;
};