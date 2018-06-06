#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <list>
#include <ostream>
#include <string>
#include <vector>

enum class TokenType
{
    // 关键字
    Integer      = 3,
    Begin        = 1,
    End          = 2,
    If           = 4,
    Then         = 5,
    Else         = 6,
    Function     = 7,
    Read         = 8,
    Write        = 9,

    // 符号
    Semicolon    = 23,
    LeftBrac     = 21,
    RightBrac    = 22,
    LessEqual    = 14,
    Minus        = 18,
    Times        = 19,
    Assign       = 20,
    Equal        = 12,
    NotEqual     = 13,
    Less         = 15,
    GreaterEqual = 16,
    Greater      = 17,

    // 换行
    NewLine      = 24,

    // 标识符和整数字面量
    Identifier   = 10,
    IntLiteral   = 11,

    // 结束标志
    EndMark = 25
};

constexpr int MAX_IDENTIFIER_LENGTH = 16;

struct Token
{
    TokenType type;
    std::string tokenStr;
    int line;
};

// 用来表示词法分析错误
struct TokenizerException
{
public:
    TokenizerException(const std::string &msg, const std::string &filename,
                       int line, int skipLen)
        : msg(msg), filename(filename),
          line(line), skipLen(skipLen)
    {
        
    }

    std::string msg;
    std::string filename;
    int line;
    int skipLen;
};

class Tokenizer
{
public:
    using TokenStream = std::list<Token>;

    Tokenizer(const std::string &src, const std::string &filename);

    TokenStream Tokenize(std::vector<TokenizerException> &errs);

private:

    void SkipWhitespaces(void);

    bool MatchSymbol(const std::string &sym);

    Token NextToken(void);

private:

    std::string src_;
    int idx_;

    std::string filename_;
    int line_;
};

#endif // TOKENIZER_H
