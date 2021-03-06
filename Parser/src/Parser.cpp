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
    try
    {
        ParseProgram();
        if(!Match(TokenType::EndMark))
            Error("program end expected");
    }
    catch(const ParserException &err)
    {
        errs_.push_back(err);
    }
}

const VarTable &Parser::GetVars(void) const
{
    return vars_;
}

const ProcTable &Parser::GetProcs(void) const
{
    return procs_;
}

const Parser::Errs &Parser::GetErrs(void) const
{
    return errs_;
}

void Parser::Error(const std::string &msg) const
{
    throw ParserException(filename_, Current().line, msg);
}

void Parser::ErrorRecWithDef(void)
{
    while(Current().type != TokenType::Semicolon)
    {
        if(Current().type == TokenType::EndMark ||
           Current().type == TokenType::End)
            break;
        Next();
    }
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
    // 当前正在分析的过程还未被加入过程名表中
    // 所以这里单独比较一下，以允许递归调用
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
        try{
            if(!Match(TokenType::Integer))
                Error("'integer' expected");
            
            if(Match(TokenType::Function))
                ParseProcDef();
            else
                ParseVarDef(paramName, procName);
            
            if(!Match(TokenType::Semicolon) && !Match(TokenType::End))
                Error("';' expected");
        }
        catch(const ParserException &err)
        {
            errs_.push_back(err);
            ErrorRecWithDef();
            Match(TokenType::Semicolon);
        }

    } while(Current().type == TokenType::Integer);
}

void Parser::ParseExecs()
{
    do {
        try
        {
            ParseExec();
        }
        catch(const ParserException &err)
        {
            errs_.push_back(err);
            ErrorRecWithDef();
        }
    } while(Match(TokenType::Semicolon));
}

void Parser::ParseVarDef(const std::string &paramName,
                         const std::string &procName)
{
    if(Current().type != TokenType::Identifier)
        Error("variable name expected");
    const std::string &newVarName = Current().tokenStr;

    Next();

    // 不能在同一作用域内重复定义变量，也不允许定义和当前过程名相同的变量
    auto it = std::find_if(vars_.begin(), vars_.end(),
    [&](const Var &var)->bool
    {
        return var.name == newVarName && var.level == this->level_;
    });
    if(it != vars_.end() || newVarName == containingProc_)
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

    --level_;

    if(!Match(TokenType::End))
        Error("'end' expected");

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
    else if(Current().type == TokenType::Identifier)
    {
        if(Current().tokenStr != containingProc_)
            CheckVarDef(Current().tokenStr);
        Next();

        if(!Match(TokenType::Assign))
            Error("':=' expected");
        
        ParseArithExpr();
    }
    else
        Error("unnknown statement type");
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
