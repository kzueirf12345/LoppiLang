#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <exception>

#include "../Utils/AVLTree.h"
#include "Lexem.h"

/**
    @brief Класс для лексического анализа
    @throw CouldNotOpenFile Не удалось открыть файл
    @throw LexemIsIncorrect Некорректная лексема на определённой строчке
    @throw SomeStringIsNotClose Незакрытая строка
    @throw SymbolIsIncorrect Некорректный символ на определённой строчке
*/
class Lexer {
   public:
    /**
        @brief Конструктор, который парсит файл типов лексем
        @param filename Строка с именем входного файла
        @throw CouldNotOpenFile Не удалось открыть файл
    */
    Lexer(const std::string &filename);
    /// Порядковый индекс лексемы
    long long lexem_ind;
    /// Номер строки, на которой находится лексема
    long long line_num;

    /**
        @brief Функция лексического анализа
        @param programm_filename Строка с именем входного файла
        @throw SomeStringIsNotClose Незакрытая строка
        @throw SymbolIsIncorrect Некорректный символ на определённой строчке
        @throw LexemIsIncorrect Некорректная лексема на определённой строчке
    */
    void Lexing(const std::string &programm_filename);

    /**
        @brief Функция, определеяющая тип лексемы
        @param lexem Данная лексема
        @return Возвращает значение типа лексемы, соответсвующее Grammar_words,
        либо -1, если тип определить невозможно
    */
    long long Type(const std::string &lexem);

    /**
        @brief Функция, добовляющая лексему в @ref dict "dict"
        @param lexem Данная лексема
        @param flag Является ли данная лексема последней
        @throw LexemIsIncorrect Некорректная лексема на определённой строчке
    */
    void PushLexem(const std::string &lexem, bool flag = false);

    /**
        @brief Функция, возвращающая текущую лексему и переходящая на следующую
    */
    Lexem gc();

    [[nodiscard]] std::vector<Lexem> &Dict() noexcept { return dict; }
    [[nodiscard]] const std::vector<Lexem> &Dict() const noexcept {
        return dict;
    }
    /// AVL-дерево, для быстрого поиска лексем и их типов
    AVLTree tree;

   private:
    /// Список всех лексем исходной программы
    std::vector<Lexem> dict;
    /// набор допустимых символов без операторов
    std::unordered_set<char> symbols;
    /// набор допустимых символов для операторов
    std::unordered_set<char> operator_symbols;

    [[nodiscard]] std::string readFileIntoString(const std::string &path);

    bool isNum(std::string const &str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
    }

    bool isAlphaOrNum(std::string const &str) {
        return !str.empty() &&
               std::all_of(str.begin(), str.end(), [](const auto &chr) {
                   return isalnum(chr) || chr == '_';
               });
    }
};