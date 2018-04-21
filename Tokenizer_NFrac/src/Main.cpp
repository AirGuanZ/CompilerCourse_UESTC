#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <utility>

#include "Tokenizer.h"

using namespace std;

map<TokenType, int> TOKENTYPE_TO_INDEX =
{
    { TokenType::Integer, 3 },
    { TokenType::Begin, 1 },
    { TokenType::End, 2 },
    { TokenType::If, 4 },
    { TokenType::Then, 5 },
    { TokenType::Else, 6 },
    { TokenType::Function, 7 },
    { TokenType::Read, 8 },
    { TokenType::Write, 9 },
    { TokenType::Semicolon, 23 },
    { TokenType::LeftBrac, 21 },
    { TokenType::RightBrac, 22 },
    { TokenType::LessEqual, 14 },
    { TokenType::Minus, 18 },
    { TokenType::Times, 19 },
    { TokenType::Assign, 20 },
    { TokenType::Equal, 12 },
    { TokenType::NotEqual, 13 },
    { TokenType::Less, 15 },
    { TokenType::GreaterEqual, 16 },
    { TokenType::Greater, 17 },
    { TokenType::Identifier, 10 },
    { TokenType::IntLiteral, 11 },
    { TokenType::EndMark, 25 },
    { TokenType::NewLine, 24 }
};

bool ReadFile(const std::string &filename, std::string &output)
{
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = std::string(std::istreambuf_iterator<char>(fin),
                         std::istreambuf_iterator<char>());
    return true;
}

std::string ReplaceFileType(const std::string &name, const std::string &type)
{
    size_t finalPnt = name.rfind(".");
    if(finalPnt == std::string::npos)
        return name + "." + type;
    return name.substr(0, finalPnt) + "." + type;
}

int main(int argc, char *argv[])
{
    // 源代码读入
    
    if(argc < 2)
    {
        cout << "Usage: tokenizer filename" << endl;
        return -1;
    }

    std::string src;
    if(!ReadFile(argv[1], src))
    {
        cout << "Cannot open file: "
             << argv[1] << endl;
        return -1;
    }

    vector<TokenizerException> errs;
    Tokenizer::TokenStream toks = Tokenizer(src, "TestFilename").Tokenize(errs);
    
    if(errs.size())
    {
        std::ofstream fout(ReplaceFileType(argv[1], "err"),
                           std::ofstream::out | std::ofstream::trunc);
        for(auto &e : errs)
        {
            fout << "Lex error around line "
                 << e.Line() << " in "
                 << e.Filename() << ": "
                 << e.What() << endl;
            cout << "Lex error around line "
                 << e.Line() << " in "
                 << e.Filename() << ": "
                 << e.What() << endl;
        }
        return -1;
    }

    std::ofstream fout(ReplaceFileType(argv[1], "dyd"),
                       std::ofstream::out | std::ofstream::trunc);
    if(!fout)
    {
        cout << "Failed to open output file" << endl;
        return -1;
    }
    for(auto &t : toks)
    {
        fout << string(max(0, 16 - (int)t.tokenStr.length()), ' ')
             << t.tokenStr
             << " "
             << TOKENTYPE_TO_INDEX[t.type]
             << endl;
    }
}
