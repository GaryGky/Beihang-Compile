cmake_minimum_required(VERSION 3.14)
project(ErrorParseS)

set(CMAKE_CXX_STANDARD 14)

add_executable(ErrorParseS main.cpp
        ErrorParse.cpp ErrorParse.h
        LexicalParser.h LexicalParser.cpp
        SymbolItem.cpp SymbolItem.h
        SymbolTabel.h SymbolTabel.cpp
        SyntaxParser.h SyntaxParser.cpp)