#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <list>
#include <ostream>
#include <string>

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

    TokenStream Tokenize(const std::string &src);
};

#endif // TOKENIZER_H
