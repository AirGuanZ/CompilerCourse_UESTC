#include <algorithm>
#include <cctype>
#include <map>
#include <vector>

#include "Tokenizer.h"

Tokenizer::Tokenizer(const std::string &src, const std::string &filename)
    : src_(src), idx_(0), filename_(filename), line_(1)
{
    
}

// 跳过从src[idx]起的空白字符
void Tokenizer::SkipWhitespaces(void)
{
    while(std::isspace(src_[idx_]) && src_[idx_] != '\n')
        ++idx_;
}

bool Tokenizer::MatchSymbol(const std::string &sym)
{
    size_t i = idx_, j = 0;
    while(sym[j] && src_[i] == sym[j])
        ++i, ++j;
    if(sym[j] == '\0')
    {
        idx_ = i;
        return true;
    }
    return false;
}

// 从src[idx]开始，返回下一个词法单元
Token Tokenizer::NextToken(void)
{
    using namespace std;
    SkipWhitespaces();

    // 结束标志
    if(src_[idx_] == '\0')
        return Token{ TokenType::EndMark, "EOF" };

    // 换行符
    if(src_[idx_] == '\n')
    {
        ++line_, ++idx_;
        return Token{ TokenType::NewLine, "EOLN" };
    }

    // 符号
    static const vector<pair<string, TokenType>> specialSymbols =
    {
        { "<=", TokenType::LessEqual    },
        { ">=", TokenType::GreaterEqual },
        { ":=", TokenType::Assign       },
        { "<>", TokenType::NotEqual     },
        { "<",  TokenType::Less         },
        { ">",  TokenType::Greater      },
        { "=",  TokenType::Equal        },
        { ";",  TokenType::Semicolon    },
        { "(",  TokenType::LeftBrac     },
        { ")",  TokenType::RightBrac    },
        { "-",  TokenType::Minus        },
        { "*",  TokenType::Times        },
    };

    for(auto &sym : specialSymbols)
    {
        if(MatchSymbol(sym.first))
            return Token{ sym.second, sym.first };
    }

    // 整形字面量
    if(src_[idx_] == '0')
    {
        // 0打头的只能有一个数字，后面不能跟数字字母下划线
        if(!isalnum(src_[++idx_]) && src_[idx_] != '_')
            return Token{ TokenType::IntLiteral, "0" };

        throw TokenizerException("invalid integer literal", filename_, line_, 0);
    }

    if(isdigit(src_[idx_]))
    {
        string digits(1, src_[idx_++]);
        while(isdigit(src_[idx_]))
            digits += src_[idx_++];

        return Token{ TokenType::IntLiteral, digits };
    }

    // 标识符 & 关键字
    if(isalpha(src_[idx_]) || src_[idx_] == '_')
    {
        string iden(1, src_[idx_++]);
        while(isalnum(src_[idx_]) || src_[idx_] == '_')
            iden += src_[idx_++];

        if(iden.length() > MAX_IDENTIFIER_LENGTH)
            throw TokenizerException("name length limit exceeded: " + iden, filename_, line_, 0);

        static const map<string, TokenType> keywords =
        {
            { "integer",  TokenType::Integer  },
            { "begin",    TokenType::Begin    },
            { "end",      TokenType::End      },
            { "if",       TokenType::If       },
            { "then",     TokenType::Then     },
            { "else",     TokenType::Else     },
            { "function", TokenType::Function },
            { "read",     TokenType::Read     },
            { "write",    TokenType::Write    }
        };

        auto it = keywords.find(iden);
        if(it != keywords.end())
            return Token{ it->second, iden };

        return Token{ TokenType::Identifier, iden };
    }

    throw TokenizerException(string("unknown token ") + src_[idx_], filename_, line_, 1);
    return Token{ TokenType::EndMark, "EOF" };
}

Tokenizer::TokenStream Tokenizer::Tokenize(std::vector<TokenizerException> &errs)
{
    TokenStream rt;
    do
    {
        try
        {
            rt.push_back(NextToken());
        }
        catch(const TokenizerException &err)
        {
            errs.push_back(err);
            idx_ += err.SkipLength();
        }
    } while(rt.back().type != TokenType::EndMark);

    return rt;
}
