#include "headers/Lexer.h"

Lexer::Lexer(const std::string& filename) : lexem_ind(0), line_num(1)
{
    const std::string alnums = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM0123456789_\n\t '#";
    for (const char chr : alnums)
    {
        symbols.insert(chr);
    }

    std::ifstream grammar_words_file(filename);
    if (!grammar_words_file.is_open())
    {
        std::cerr << "Could not open the file - '" << filename << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string grammar_word_str;
    while (std::getline(grammar_words_file, grammar_word_str))
    {
        std::stringstream ss(grammar_word_str);
        std::string word;
        uint64_t num;
        ss >> word;
        ss >> num;
        for (const char chr : word)
        {
            symbols.insert(chr);
        }
        if (num == 6)
        {
            for (size_t i = 1; i < word.size(); ++i)
            {
                operator_symbols.insert(word[i]);
            }
        }
        tree.insert(word, num);
    }
    grammar_words_file.close();
}

void Lexer::Lexing(const std::string& programm_filename)
{
    std::string programm_text(std::move(readFileIntoString(programm_filename) + "\n"));
    std::string lexem;

    bool flag1 = false, flag2 = false;;
    for (const auto &chr : programm_text)
    {
        if (!lexem.empty() && lexem[0] == '\'' && chr != '\'')
        {
            lexem += chr;
            flag1 = true;
        }
        else if (!lexem.empty() && lexem[0] == '\'' && chr == '\'')
        {
            PushLexem(lexem + chr);
            lexem = "";
            flag1 = false;
            flag2 = false;
        }
        else if (lexem[0] == '#')
        {
            if (chr == '\n')
            {
                ++line_num;
                lexem = "";
            }
            continue;
        }
        else if (symbols.find(chr) == symbols.end())
        {
            std::cerr << "Symbol - '" << chr << "' is incorrect on " << line_num << " line" << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (chr == ' ' || chr == '\t' || chr == '\n')
        {
            if (!lexem.empty())
            {
                PushLexem(lexem);
                lexem = "";
            }
        }
        else if (
            ((Type(lexem) == 1 || Type(lexem) == 2 || Type(lexem) == 8) && (isalnum(chr) || chr == '_'))
            || (Type(lexem) == 3 && isdigit(chr))
            || (Type(lexem) == 4 && isalpha(chr))
            || (lexem == ":" && chr == ':')
            || (Type(lexem) == 6 && operator_symbols.find(chr) != operator_symbols.end())
            || Type(lexem) == -1)
        {
            lexem += chr;
        }
        else
        {
            PushLexem(lexem);
            lexem = chr;
        }

        line_num += (chr == '\n');
    }
    if (flag1 != flag2)
    {
        std::cerr << "Some string is not close" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cerr << "Lexer is OK" << std::endl;
}

uint64_t Lexer::Type(const std::string &lexem)
{
    if (tree.count(lexem))
        return tree.get_type(lexem);
    else if (lexem.size() >= 2 && lexem[0] == '\'' && lexem[lexem.size() - 1] == '\'')
    {
        return 5;
    }
    else if (isNum(lexem))
    {
        return 3;
    }
    else if ((isalpha(lexem[0]) || lexem[0] == '_') && isAlphaOrNum(lexem))
        return 2;
    else
        return -1;
}

void Lexer::PushLexem(const std::string &lexem, bool flag)
{
    uint64_t type = Type(lexem);
    if (type == -1)
    {
        std::cerr << "Lexem - '" << lexem << "' is incorrect on " << line_num << " line" << std::endl;
        exit(EXIT_FAILURE);
    }
    dict.emplace_back(Lexem(lexem, Type(lexem), flag, line_num));
}

Lexem Lexer::gc()
{
/*     if (lexem_ind == dict.size() - 1)
    {
        std::cerr << "Lexems dictionary is ended" << std::endl;
        Lexem res("", -1);
        res.is_eof = true;
        return res;
        exit(EXIT_FAILURE);
    } */
    return dict[lexem_ind++];
}

[[nodiscard]] std::string Lexer::readFileIntoString(const std::string &path)
{
    std::ifstream input_file(path);
    if (!input_file.is_open())
    {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}