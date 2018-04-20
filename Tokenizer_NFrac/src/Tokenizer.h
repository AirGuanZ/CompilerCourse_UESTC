#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <list>
#include <ostream>
#include <string>
#include <vector>

enum class TokenType
{
    // 关键字
    Integer,
    Begin, End,
    If, Then, Else,
    Function,

    // 符号
    Semicolon,
    LeftBrac, RightBrac,
    LessEqual,
    Minus, Times,
    Assign,

    // 标识符和整数字面量
    Identifier,
    IntLiteral,

    // 结束标志
    EndMark
};

std::ostream &operator<<(std::ostream &out, TokenType type);

struct Token
{
    TokenType type;
    std::string tokenStr;
};

inline bool operator==(const Token &L, const Token &R)
{
    return L.type == R.type && L.tokenStr == R.tokenStr;
}

// 用来表示词法分析错误
class TokenizerException
{
public:
    TokenizerException(const std::string &msg)
        : msg_(msg)
    {
        
    }

    const char *What(void) const
    {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class Tokenizer
{
public:
    using TokenStream = std::list<Token>;

    using TokenIdxStream = std::vector<std::pair<TokenType, size_t>>;
    using TokenTable  = std::vector<Token>;

    TokenStream    Tokenize(const std::string &src);
    TokenIdxStream Tokenize(const std::string &src, TokenTable &tab);
};

#endif // TOKENIZER_H
