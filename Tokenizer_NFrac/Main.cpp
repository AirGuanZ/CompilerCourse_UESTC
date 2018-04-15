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

    // 调用词法分析
    Tokenizer::TokenStream toks;
    try
    {
        toks = Tokenizer().Tokenize(src);
    }
    catch(const TokenizerException &err)
    {
        cout << "Lex error: " << err.What() << endl;
    }

    // 输出分析结果
    for(auto &tok : toks)
        cout << "[" << tok.type << ", " << tok.tokenStr << "]" << endl;
}
