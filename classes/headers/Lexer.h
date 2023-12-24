#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "AVLTree.h"
#include "Lexem.h"

class Lexer {
   public:
    Lexer(const std::string &filename);
    long long lexem_ind;
    long long line_num;

    void Lexing(const std::string &programm_filename);
    long long Type(const std::string &lexem);
    void PushLexem(const std::string &lexem, bool flag = false);

    Lexem gc();

    [[nodiscard]] std::vector<Lexem> &Dict() noexcept { return dict; }
    [[nodiscard]] const std::vector<Lexem> &Dict() const noexcept {
        return dict;
    }
    AVLTree tree;

   private:
    std::vector<Lexem> dict;
    std::unordered_set<char> symbols;
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