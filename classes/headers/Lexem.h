#pragma once

#include <string>

struct Lexem
{
    Lexem() : name(""), type(-1), is_eof(false) {};
    Lexem(std::string name, uint64_t type) : name(std::move(name)), type(type), is_eof(false){}
    Lexem(std::string name, uint64_t type, bool eof) : name(std::move(name)), type(type), is_eof(eof){}
    Lexem(std::string name, uint64_t type, bool eof, uint64_t line) : name(std::move(name)), type(type), is_eof(eof), line(line){}
    std::string name;
    uint64_t line;
    uint64_t type;
    bool is_eof;
};