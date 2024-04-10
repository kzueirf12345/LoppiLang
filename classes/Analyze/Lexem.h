#pragma once

#include <string>

/**
    @brief Класс для хранения лексемы и её данных
*/
struct Lexem
{
    Lexem() : name(""), type(-1), is_eof(false) {};
    Lexem(std::string name, long long type) : name(std::move(name)), type(type), is_eof(false){}
    Lexem(std::string name, long long type, bool eof) : name(std::move(name)), type(type), is_eof(eof){}
    Lexem(std::string name, long long type, bool eof, long long line) : name(std::move(name)), type(type), is_eof(eof), line(line){}
    /// Имя лексемы
    std::string name;
    /// На какой линии в исходном коде находится лексема
    long long line;
    /// Тип лексемы из Grammar_words.txt
    long long type;
    /// Является ли последней лексемой
    bool is_eof;
};