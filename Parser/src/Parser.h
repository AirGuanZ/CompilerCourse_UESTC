#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

#include "Tokenizer.h"

enum class VarKind
{
    Parameter,
    Variable
};

enum class VarType
{
    Integer
};

struct Var
{
    std::string name;
    std::string proc;

    VarKind kind;
    VarType type;

    int level;
    size_t posInTable;
};

struct Proc
{
    std::string name;

    VarType returnType;

    int level;
    size_t varPosBegin, varPosEnd; // 有效范围是[begin, end)
};

using VarTable  = std::vector<Var>;
using ProcTable = std::vector<Proc>;

struct ParserException
{
    ParserException(const std::string &filename, int line,
                    const std::string msg)
        : filename(filename), line(line), msg(msg)
    {

    }

    std::string filename;
    int line;

    std::string msg;
};

class Parser
{
public:
    using Errs = std::vector<ParserException>;

    Parser(const Tokenizer::TokenStream &toks,
           const std::string &filename);

    void Parse(void);

    const VarTable &GetVars(void) const;

    const ProcTable &GetProcs(void) const;

private:

    void Error(const std::string &msg) const;

    bool Match(TokenType type);

    const Token &Current(void) const;

    void Next(void);

    void CheckVarDef(const std::string &var) const;

    void CheckProcDef(const std::string &var) const;

    void ParseProgram(void);

    void ParseSubprogram(void);

    void ParseDefs(const std::string &paramName = "",
                   const std::string &procName = "");

    void ParseVarDef(const std::string &paramName,
                     const std::string &procName);

    void ParseProcDef(void);

    void ParseExecs(void);

    void ParseExec(void);

    void ParseArithExpr(void);

    void ParseItem(void);

    void ParseFactor(void);

private:

    Tokenizer::TokenStream toks_;
    Tokenizer::TokenStream::iterator cur_;

    VarTable vars_;
    ProcTable procs_;

    std::string filename_;
    int level_;

    std::string containingProc_;
};

#endif /* PARSER_H */
