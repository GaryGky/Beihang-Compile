#include "LexicalParser.h"
#include "SyntaxParser.h"
#include "SymbolTabel.h"
#include <iostream>


int main() {
    // cout << "start compile:" << endl;
    ErrorParse errorParse;
    errorParse.initial();
    SymbolTabel symbolTabel(errorParse);
    LexicalParser myLexicalParser(errorParse);
    myLexicalParser.start();
    SyntaxParser mySyntaxParser(myLexicalParser, errorParse, symbolTabel);
    mySyntaxParser.start();
}

