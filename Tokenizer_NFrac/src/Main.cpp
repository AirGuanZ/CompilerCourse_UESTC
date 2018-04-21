#include <iostream>

#include "Tokenizer.h"

using namespace std;

int main(void)
{
    // 源代码
    std::string src =
        R"__(
        begin
            integer k;
            integer m;
            integer function F(n);
            begin
                integer n;
                if n <= 0 then F := 1
                          else F := n * F(n - 1)
            end;
            read(m);
            k := F(m);
            write(k)
        end
        )__";

    Tokenizer::TokenStream toks;
    try
    {
        toks = Tokenizer(src, "TestFilename").Tokenize();
    }
    catch(const TokenizerException &err)
    {
        cout << "Lex error around line "
             << err.Line() << " in "
             << err.Filename() << ": "
             << err.What() << endl;
    }

    for(auto &t : toks)
        cout << "[" << t.type << ", " << t.tokenStr << "]" << endl;



    /*// 调用词法分析
    Tokenizer::TokenIdxStream toks;
    Tokenizer::TokenTable  tab;
    try
    {
        toks = Tokenizer().Tokenize(src, tab);
    }
    catch(const TokenizerException &err)
    {
        cout << "Lex error: " << err.What() << endl;
    }

    // 输出分析结果
    for(auto &t : toks)
    {
        Token &tok = tab[t.second];
        cout << "[" << tok.type << ", " << tok.tokenStr << "]" << endl;
    }*/
}
