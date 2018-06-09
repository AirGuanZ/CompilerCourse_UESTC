#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <utility>

#include "Parser.h"
#include "Tokenizer.h"

using namespace std;

bool ReadFile(const string &filename, string &output)
{
    ifstream fin(filename, ifstream::in);
    if(!fin)
        return false;
    output = string(istreambuf_iterator<char>(fin),
                         istreambuf_iterator<char>());
    return true;
}

string ReplaceFileType(const string &name, const string &type)
{
    return name.substr(0, name.rfind(".")) + "." + type;
}

int main(int argc, char *argv[])
{
    // 源代码读入
    
    if(argc < 2)
    {
        cout << "Usage: tokenizer filename" << endl;
        return -1;
    }

    const std::string filename = argv[1];

    string src;
    if(!ReadFile(filename, src))
    {
        cout << "Cannot open file: "
             << filename << endl;
        return -1;
    }

    // 词法分析及其错误输出

    vector<TokenizerException> errs;
    Tokenizer::TokenStream toks = Tokenizer(src, filename).Tokenize(errs);

    if(errs.size())
    {
        ofstream fout(ReplaceFileType(filename, "err"), ofstream::out);
        for(auto &e : errs)
        {
            fout << "***LINE: " << e.line
                 << "  " << e.msg << endl;
            cout << "***LINE: " << e.line
                 << "  " << e.msg << endl;
        }
        return -1;
    }

    // 词法分析结果输出

    ofstream fout(ReplaceFileType(filename, "dyd"), ofstream::out);
    if(!fout)
    {
        cout << "Failed to open dyd file" << endl;
        return -1;
    }
    for(auto &t : toks)
    {
        fout << setw(16) << setfill(' ')
             << t.tokenStr
             << " "
             << setw(2) << setfill('0')
             << static_cast<int>(t.type)
             << endl;
    }

    fout.close();

    // 语法分析及其错误输出

    Parser parser(toks, filename);
    
    try
    {
        parser.Parse();
    }
    catch(const ParserException &e)
    {
        ofstream fout(ReplaceFileType(filename, "err"), ofstream::out);
        fout << "***LINE: " << e.line
             << "  " << e.msg << endl;
        cout << "***LINE: " << e.line
             << "  " << e.msg << endl;
        return -1;
    }

    // 语法分析结果输出

    fout.open(ReplaceFileType(filename, "dys"), ofstream::out);
    if(!fout)
    {
        cout << "Failed to open dys file" << endl;
        return -1;
    }
    for(auto &t : toks)
    {
        fout << setw(16) << setfill(' ')
             << t.tokenStr
             << " "
             << setw(2) << setfill('0')
             << static_cast<int>(t.type)
             << endl;
    }
    fout.close();

    fout.open(ReplaceFileType(filename, "varfil"), ofstream::out);
    if(!fout)
    {
        cout << "Failed to open varfil file" << endl;
        return -1;
    }

    for(const Var &v : parser.GetVars())
    {
        fout << "Var"                              << endl
             << "    Name      = " << v.name       << endl
             << "    Procedure = " << v.proc       << endl
             << "    Kind      = " <<
                (v.kind == VarKind::Variable ?
                    "Variable" : "Parameter")      << endl
             << "    Type      = " << "Integer"    << endl
             << "    Level     = " << v.level      << endl
             << "    Offset    = " << v.posInTable << endl;
    }

    fout.close();

    fout.open(ReplaceFileType(filename, "profil"), ofstream::out);
    if(!fout)
    {
        cout << "Failed to open profil file" << endl;
        return -1;
    }

    for(const Proc &p : parser.GetProcs())
    {
        fout << "Proc"                                << endl
             << "    Name      = " << p.name          << endl
             << "    Type      = " << "Integer"       << endl
             << "    Level     = " << p.level         << endl
             << "    FirstVar  = " << p.varPosBegin   << endl
             << "    LastVar   = " << p.varPosEnd - 1 << endl;
    }

    fout.close();

    cout << "Parsing succeeded" << endl;

    return 0;
}
