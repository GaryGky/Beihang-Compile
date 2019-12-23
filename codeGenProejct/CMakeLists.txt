cmake_minimum_required(VERSION 3.14)
project(codeGenProejct)

set(CMAKE_CXX_STANDARD 14)

add_executable(codeGenProejct main.cpp LexicalParser.cpp LexicalParser.h SymbolItem.h SymbolItem.cpp
        SymbolTabel.cpp SymbolTabel.h SyntaxParser.cpp SyntaxParser.h
        FourComExpr.h FourComExpr.cpp
        MipsGenerator.h MipsGenerator.cpp
        Optimizer.h Optimizer.cpp
        BasicBlock.cpp BasicBlock.h
        FlowGraph.cpp FlowGraph.h
        Error/ErrorParse.h Error/ErrorParse.cpp
        Error/ErrorLexicalParser.h
        Error/ErrorLexicalParser.cpp
        Error/ErrorSymbolItem.cpp Error/ErrorSymbolItem.h Error/ErrorSymbolTabel.cpp
        Error/ErrorSymbolTabel.h Error/ErrorSyntaxParser.cpp Error/ErrorSyntaxParser.h
        EnumClass.h
        )