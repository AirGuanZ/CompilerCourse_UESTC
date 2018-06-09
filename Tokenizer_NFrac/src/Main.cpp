#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <utility>

#include "Tokenizer.h"

using namespace std;

map<TokenType, int> TOKENTYPE_TO_INDEX =
{
    { TokenType::Integer,      3  },
    { TokenType::Begin,        1  },
    { TokenType::End,          2  },
    { TokenType::If,           4  },
    { TokenType::Then,         5  },
    { TokenType::Else,         6  },
    { TokenType::Function,     7  },
    { TokenType::Read,         8  },
    { TokenType::Write,        9  },
    { TokenType::Semicolon,    23 },
    { TokenType::LeftBrac,     21 },
    { TokenType::RightBrac,    22 },
    { TokenType::LessEqual,    14 },
    { TokenType::Minus,        18 },
    { TokenType::Times,        19 },
    { TokenType::Assign,       20 },
    { TokenType::Equal,        12 },
    { TokenType::NotEqual,     13 },
    { TokenType::Less,         15 },
    { TokenType::GreaterEqual, 16 },
    { TokenType::Greater,      17 },
    { TokenType::Identifier,   10 },
    { TokenType::IntLiteral,   11 },
    { TokenType::EndMark,      25 },
    { TokenType::NewLine,      24 }
};

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

    string src;
    if(!ReadFile(argv[1], src))
    {
        cout << "Cannot open file: "
             << argv[1] << endl;
        return -1;
    }

    // 调用词法分析

    vector<TokenizerException> errs;
    Tokenizer::TokenStream toks = Tokenizer(src, argv[1]).Tokenize(errs);

    // 错误输出

    if(errs.size())
    {
        ofstream fout(ReplaceFileType(argv[1], "err"),
                           ofstream::out | ofstream::trunc);
        for(auto &e : errs)
        {
            fout << "***LINE: " << e.Line()
                 << "  " << e.What() << endl;
            cout << "***LINE: " << e.Line()
                 << "  " << e.What() << endl;
        }
        return -1;
    }

    // 分析结果输出

    ofstream fout(ReplaceFileType(argv[1], "dyd"),
                       ofstream::out | ofstream::trunc);
    if(!fout)
    {
        cout << "Failed to open output file" << endl;
        return -1;
    }
    for(auto &t : toks)
    {
        fout << setw(16) << setfill(' ')
             << t.tokenStr
             << " "
             << setw(2) << setfill('0')
             << TOKENTYPE_TO_INDEX[t.type]
             << endl;
    }
}
