#include "FrontEnd/Header/LexicalParser.h"
#include "FrontEnd/Header/SyntaxParser.h"
#include "FrontEnd/Header/SymbolTabel.h"
#include "BackEnd/Header/MipsGenerator.h"
#include "BackEnd/Header/Optimizer.h"
#include "FrontEnd/Header/SyntaxParser.h"
#include "ErrorParse/Header/ErrorParse.h"
#include "ErrorParse/Header/ErrorSymbolTabel.h"
#include "ErrorParse/Header/ErrorLexicalParser.h"
#include "ErrorParse/Header/ErrorSyntaxParser.h"
#include "EnumClass.h"


int main() {
    /*ErrorParse errorParse;
    errorParse.initial();
    ErrorSymbolTabel errorsymbolTabel(errorParse);
    ErrorLexicalParser errormyLexicalParser(errorParse);
    errormyLexicalParser.start();
    ErrorSyntaxParser errormySyntaxParser(errormyLexicalParser, errorParse, errorsymbolTabel);
    errormySyntaxParser.start();
    if (errormySyntaxParser.isError()) {
        return 0; // 有错误直接退出
    }*/

    SymbolTabel globalSymbolTable;
    LexicalParser myLexicalParser;
    myLexicalParser.start(); // 初始化词法分析
    SyntaxParser mySyntaxParser(myLexicalParser, globalSymbolTable);
    mySyntaxParser.start(); // 开始语法分析
    Optimizer optimizer(mySyntaxParser.getTmpCode());
    optimizer.optimize(); // 开始优化 :: 内含优化开关
    // 如果开优化过不了的话，就使用没有优化的吧
    MipsGenerator mipsGenerator(optimizer.getOptCode(), globalSymbolTable);
    mipsGenerator.genMipsCode(); // 产生mips
    return 0;
}

