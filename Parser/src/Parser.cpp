#include <algorithm>

#include "Parser.h"

Parser::Parser(const Tokenizer::TokenStream &toks,
               const std::string &filename)
    : filename_(filename), level_(0)
{
    for(auto &t : toks)
    {
        if(t.type != TokenType::NewLine)
            toks_.push_back(t);
    }
    cur_ = toks_.begin();
}

void Parser::Parse(void)
{
    ParseProgram();
    if(!Match(TokenType::EndMark))
        Error("program end expected");
}

const VarTable &Parser::GetVars(void) const
{
    return vars_;
}

const ProcTable &Parser::GetProcs(void) const
{
    return procs_;
}

void Parser::Error(const std::string &msg) const
{
    throw ParserException(filename_, Current().line, msg);
}

bool Parser::Match(TokenType type)
{
    if(cur_->type == type)
    {
        if(cur_ != toks_.end())
            ++cur_;
        return true;
    }
    return false;
}

const Token &Parser::Current(void) const
{
    return *cur_;
}

void Parser::Next(void)
{
    ++cur_;
}

void Parser::CheckVarDef(const std::string &v) const
{
    auto it = std::find_if(vars_.begin(), vars_.end(),
    [&](const Var &var)->bool
    {
        return var.name == v && var.level <= level_;
    });
    if(it == vars_.end())
        Error("undefined variable: " + v);
}

void Parser::CheckProcDef(const std::string &p) const
{
    if(p == containingProc_)
        return;

    auto it = std::find_if(procs_.begin(), procs_.end(),
    [&](const Proc &proc)->bool
    {
        return proc.name == p && proc.level <= level_;
    });
    if(it == procs_.end())
        Error("undefined procedure: " + p);
}

void Parser::ParseProgram(void)
{
    return ParseSubprogram();
}

void Parser::ParseSubprogram()
{
    if(!Match(TokenType::Begin))
        Error("'begin' expected");

    ++level_;

    ParseDefs();
    ParseExecs();

    if(!Match(TokenType::End))
        Error("'end' expected");

    --level_;
}

void Parser::ParseDefs(const std::string &paramName,
                       const std::string &procName)
{
    do {
        if(!Match(TokenType::Integer))
            Error("'integer' expected");
        
        if(Match(TokenType::Function))
            ParseProcDef();
        else
            ParseVarDef(paramName, procName);
        
        Match(TokenType::Semicolon);

    } while(Current().type == TokenType::Integer);
}

void Parser::ParseExecs()
{
    do {
        ParseExec();
    } while(Match(TokenType::Semicolon));
}

void Parser::ParseVarDef(const std::string &paramName,
                         const std::string &procName)
{
    if(Current().type != TokenType::Identifier)
        Error("variable name expected");
    const std::string &newVarName = Current().tokenStr;

    Next();

    auto it = std::find_if(vars_.begin(), vars_.end(),
    [&](const Var &var)->bool
    {
        return var.name == newVarName && var.level == this->level_;
    });
    if(it != vars_.end())
        Error("Variale redefined: " + newVarName);
    
    Var newVar =
    {
        newVarName, procName,
        (paramName == newVarName ? VarKind::Parameter :
                                   VarKind::Variable),
        VarType::Integer,
        level_,
        vars_.size()
    };
    vars_.push_back(newVar);
}

void Parser::ParseProcDef(void)
{
    // 取得函数名
    if(Current().type != TokenType::Identifier)
        Error("function name expected");
    const std::string newProcName = Current().tokenStr;

    Next();

    // 检查是否重定义
    auto it = std::find_if(procs_.begin(), procs_.end(),
    [&](const Proc &proc)->bool
    {
        return proc.name == newProcName && proc.level == level_;
    });
    if(it != procs_.end())
        Error("Procedure redefined: " + newProcName);
    
    // 保存变量开始位置
    size_t procVarBegin = vars_.size();

    // 识别参数列表
    // 等等，纳尼，只支持一个参数？
    if(!Match(TokenType::LeftBrac))
        Error("'(' expected");
    if(Current().type != TokenType::Identifier)
        Error("parameter expected");
    std::string paramName = Current().tokenStr;
    Next();
    if(!Match(TokenType::RightBrac))
        Error("')' expected");
    
    if(!Match(TokenType::Semicolon))
        Error("';' expected");

    ++level_;
    
    if(!Match(TokenType::Begin))
        Error("'begin' expected");
    
    ParseDefs(paramName, newProcName);

    // 检查参数类型定义了没
    CheckVarDef(paramName);

    std::string oldCon = containingProc_;
    containingProc_ = newProcName;

    ParseExecs();

    containingProc_ = oldCon;

    if(!Match(TokenType::End))
        Error("'end' expected");

    --level_;

    size_t procVarEnd = vars_.size();

    Proc newProc =
    {
        newProcName,
        VarType::Integer,
        level_,
        procVarBegin,
        procVarEnd
    };
    procs_.push_back(newProc);
}

void Parser::ParseExec(void)
{
    if(Match(TokenType::Read) || Match(TokenType::Write))
    {
        if(!Match(TokenType::LeftBrac))
            Error("'(' expected");

        if(Current().type != TokenType::Identifier)
            Error("'variable expected'");
        CheckVarDef(Current().tokenStr);

        Next();

        if(!Match(TokenType::RightBrac))
            Error("')' expected");
    }
    else if(Match(TokenType::If))
    {
        ParseArithExpr();
        if(!Match(TokenType::Less) &&
           !Match(TokenType::LessEqual) &&
           !Match(TokenType::Equal) &&
           !Match(TokenType::GreaterEqual) &&
           !Match(TokenType::Greater) &&
           !Match(TokenType::NotEqual))
           Error("Comparation operator expected");
        ParseArithExpr();

        if(!Match(TokenType::Then))
            Error("'then' expected");
        
        ParseExec();

        if(!Match(TokenType::Else))
            Error("'else' expected");
        
        ParseExec();
    }
    else
    {
        if(Current().type != TokenType::Identifier)
            Error("variable name expected");
        if(Current().tokenStr != containingProc_)
            CheckVarDef(Current().tokenStr);
        Next();

        if(!Match(TokenType::Assign))
            Error("':=' expected");
        
        ParseArithExpr();
    }
}

void Parser::ParseArithExpr(void)
{
    ParseItem();
    while(Match(TokenType::Minus))
        ParseItem();
}

void Parser::ParseItem(void)
{
    ParseFactor();
    while(Match(TokenType::Times))
        ParseFactor();
}

void Parser::ParseFactor(void)
{
    if(Match(TokenType::IntLiteral))
        return;
    
    if(Current().type != TokenType::Identifier)
        Error("variable/procedure name expected");
    const std::string &refName = Current().tokenStr;
    Next();

    if(Match(TokenType::LeftBrac)) // 是个函数调用而非变量引用
    {
        CheckProcDef(refName);
        
        ParseArithExpr();
    
        if(!Match(TokenType::RightBrac))
            Error("')' expected");
    }
    else
        CheckVarDef(refName);
}
