#include "LexicalParser.h"
#include "SyntaxParser.h"

int main() {
    LexicalParser myLexicalParser;
    myLexicalParser.start();
    SyntaxParser mySyntaxParser(myLexicalParser);
    mySyntaxParser.start();
}