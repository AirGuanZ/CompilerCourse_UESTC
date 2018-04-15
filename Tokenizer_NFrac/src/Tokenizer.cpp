#include <cctype>
#include <map>
#include <vector>

#include "Tokenizer.h"

namespace
{
    // 跳过从src[idx]起的空白字符
    void SkipWhitespaces(const std::string &src, size_t &idx)
    {
        while(std::isspace(src[idx]))
            ++idx;
    }

    bool MatchSymbol(const std::string &src, size_t &idx, const std::string &sym)
    {
        size_t i = idx, j = 0;
        while(sym[j] && src[i] == sym[j])
            ++i, ++j;
        if(sym[j] == '\0')
        {
            idx = i;
            return true;
        }
        return false;
    }

    // 从src[idx]开始，返回下一个词法单元
    Token NextToken(const std::string &src, size_t &idx)
    {
        using namespace std;

        SkipWhitespaces(src, idx);

        // 结束标志

        if(src[idx] == '\0')
            return Token{ TokenType::EndMark, "" };

        // 符号

        static const vector<pair<string, TokenType>> specialSymbols =
        {
            { "<=", TokenType::LessEqual },
            { ":=", TokenType::Assign    },
            { ";",  TokenType::Semicolon },
            { "(",  TokenType::LeftBrac  },
            { ")",  TokenType::RightBrac },
            { "-",  TokenType::Minus     },
            { "*",  TokenType::Times     },
        };

        for(auto &sym : specialSymbols)
        {
            if(MatchSymbol(src, idx, sym.first))
                return Token{ sym.second, sym.first };
        }

        // 整形字面量

        if(src[idx] == '0')
        {
            // 0打头的只能有一个数字，后面不能跟数字字母下划线
            if(!isalnum(src[++idx]) && src[idx] != '_')
                return Token{ TokenType::IntLiteral, "0" };
            throw TokenizerException("invalid integer literal");
        }

        if(isdigit(src[idx]))
        {
            string digits(1, src[idx++]);
            while(isdigit(src[idx]))
                digits += src[idx++];
            return Token{ TokenType::IntLiteral, digits };
        }

        // 标识符 & 关键字

        if(isalpha(src[idx]) || src[idx] == '_')
        {
            string iden(1, src[idx++]);
            while(isalnum(src[idx]) || src[idx] == '_')
                iden += src[idx++];

            static const map<string, TokenType> keywords =
            {
                { "integer",  TokenType::Integer  },
                { "begin",    TokenType::Begin    },
                { "end",      TokenType::End      },
                { "if",       TokenType::If       },
                { "then",     TokenType::Then     },
                { "else",     TokenType::Else     },
                { "function", TokenType::Function },
            };
            auto it = keywords.find(iden);
            if(it != keywords.end())
                return Token{ it->second, iden };
            return Token{ TokenType::Identifier, iden };
        }

        throw TokenizerException(string("unknown token ") + src[idx]);
        return Token{ TokenType::EndMark, "" };
    }
}

std::ostream &operator<<(std::ostream &out, TokenType type)
{
    static const std::string tokenTypeNames[] =
    {
        "Integer",
        "Begin",
        "End",
        "If",
        "Then",
        "Else",
        "Function",

        "Semicolon",
        "LeftBrac",
        "RightBrac",
        "LessEqual",
        "Minus",
        "Times",
        "Assign",

        "Identifier",
        "IntLiteral",

        "EndMark",
    };
    return out << tokenTypeNames[std::underlying_type_t<TokenType>(type)];
}

Tokenizer::TokenStream Tokenizer::Tokenize(const std::string &src)
{
    TokenStream rt;
    size_t idx = 0;
    do
    {
        rt.push_back(NextToken(src, idx));
    } while(rt.back().type != TokenType::EndMark);
    return rt;
}
