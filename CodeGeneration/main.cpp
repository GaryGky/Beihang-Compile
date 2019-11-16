#include "LexicalParser.h"
#include "SyntaxParser.h"
#include "SymbolTabel.h"
#include "Optimizer.h"
#include <iostream>


int main() {
    SymbolTabel symbolTabel;
    LexicalParser myLexicalParser;
    myLexicalParser.start();
    SyntaxParser mySyntaxParser(myLexicalParser, symbolTabel);
    mySyntaxParser.start();
    Optimizer optimizer(mySyntaxParser.getTmpCode(), symbolTabel);
    optimizer.genMipsCode(); // 产生mips
}

