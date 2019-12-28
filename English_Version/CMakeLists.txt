cmake_minimum_required(VERSION 3.14)
project(codeGenProejct)

set(CMAKE_CXX_STANDARD 14)

add_executable(codeGenProejct main.cpp FrontEnd/Source/LexicalParser.cpp FrontEnd/Header/LexicalParser.h FrontEnd/Header/SymbolItem.h FrontEnd/Source/SymbolItem.cpp
        FrontEnd/Source/SymbolTabel.cpp FrontEnd/Header/SymbolTabel.h FrontEnd/Source/SyntaxParser.cpp FrontEnd/Header/SyntaxParser.h
        BackEnd/Header/FourComExpr.h BackEnd/Source/FourComExpr.cpp
        BackEnd/Header/MipsGenerator.h BackEnd/Source/MipsGenerator.cpp
        BackEnd/Header/Optimizer.h BackEnd/Source/Optimizer.cpp
        BackEnd/Source/BasicBlock.cpp BackEnd/Header/BasicBlock.h
        BackEnd/Source/FlowGraph.cpp BackEnd/Header/FlowGraph.h
        ErrorParse/Header/ErrorParse.h ErrorParse/Source/ErrorParse.cpp
        ErrorParse/Header/ErrorLexicalParser.h
        ErrorParse/Source/ErrorLexicalParser.cpp
        ErrorParse/Source/ErrorSymbolItem.cpp ErrorParse/Header/ErrorSymbolItem.h ErrorParse/Source/ErrorSymbolTabel.cpp
        ErrorParse/Header/ErrorSymbolTabel.h ErrorParse/Source/ErrorSyntaxParser.cpp ErrorParse/Header/ErrorSyntaxParser.h
        EnumClass.h
        )