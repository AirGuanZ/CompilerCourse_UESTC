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
    TokenizerException(const std::string &msg, const std::string &filename, int line)
        : msg_(msg), filename_(filename), line_(line)
    {
        
    }

    const char *What(void) const
    {
        return msg_.c_str();
    }

    int Line(void) const
    {
        return line_;
    }

    const std::string &Filename(void) const
    {
        return filename_;
    }

private:
    std::string msg_;
    std::string filename_;
    int line_;
};

class Tokenizer
{
public:
    using TokenStream = std::list<Token>;

    using TokenIdxStream = std::vector<std::pair<TokenType, size_t>>;
    using TokenTable  = std::vector<Token>;

    Tokenizer(const std::string &src, const std::string &filename);

    TokenStream    Tokenize(void);
    TokenIdxStream Tokenize(TokenTable &tab);

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
