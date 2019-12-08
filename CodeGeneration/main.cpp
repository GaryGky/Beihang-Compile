#include "LexicalParser.h"
#include "SyntaxParser.h"
#include "SymbolTabel.h"
#include "MipsGenerator.h"
#include "Optimizer.h"


int main() {
    SymbolTabel symbolTabel;
    LexicalParser myLexicalParser;
    myLexicalParser.start(); // 初始化词法分析
    SyntaxParser mySyntaxParser(myLexicalParser, symbolTabel);
    mySyntaxParser.start(); // 开始语法分析
    Optimizer optimizer(mySyntaxParser.getTmpCode());
    optimizer.orientCodeOpt(); // 开始优化
    MipsGenerator mipsGenerator(optimizer.getOptCode(), symbolTabel);
    mipsGenerator.genMipsCode(); // 产生mips
}

