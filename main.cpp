#include "Syntaxer.h"

#define DEBUG true

int main() {
    std::string grammar_filename;
#if DEBUG
    grammar_filename = "Grammar_words.txt";
#else
    std::cout << "Введите имя файла с зарезервированными словами:\n";
    std::cin >> grammar_filename;
#endif
    Lexer lexer(grammar_filename);
    Syntaxer syntaxer(lexer);

    std::string programm_filename;
#if DEBUG
    programm_filename = "Programm.lop";
#else
    std::cout << "Введите имя файла с программой:\n";
    std::cin >> programm_filename;
#endif
    try {
        lexer.Lexing(programm_filename);
    } catch (const std::exception& e) {
        std::cerr << "Lexeical analyze error:\n" << e.what() << std::endl;
    }

    std::string lexer_out_filename;
#if DEBUG
    lexer_out_filename = "Lexems.txt";
#else
    std::cout << "Введите имя файла, куда будут загружены лексемы\n";
    std::cin >> lexer_out_filename;
#endif
    std::ofstream lexer_out_file(lexer_out_filename);
    for (const auto& elem : lexer.Dict())
        lexer_out_file << elem.name << " " << elem.type << "\n";
    lexer_out_file.close();

    try {
        syntaxer.Programm();
    } catch (const Lexem& e) {
        std::cerr << "Syntax analyze error:\n"
                  << "Lexem " << e.name << " on " << e.line << " line" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Semantic analyze error:\n"
                  << e.what() << " on " << syntaxer.lexem.line << " line"
                  << std::endl;
    }

    return 0;
}