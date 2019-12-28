//
// Created by dell on 2019/10/10.
//
#include "ErrorLexicalParser.h"
#include "ErrorSymbolItem.h"
#include "ErrorParse.h"
#include "ErrorSymbolTabel.h"
#include <queue>
#include <string>
#include <set>


#ifndef CODEGENPROEJCT_ERRORSYNTAXPARSER_H
#define CODEGENPROEJCT_ERRORSYNTAXPARSER_H


class ErrorSyntaxParser {
private:
    ErrorLexicalParser &lexicalParser; // 词法分析器
    ErrorParse &errorParse;
    ErrorSymbolTabel globalSymbolTable;
    int level; // 记录语法分析的层数
    bool curOwnsRet; // 用于标识当前正在分析的函数是否拥有了正确的返回值
    // map<string, ErrorSymbolItem> localSymbolTable; // 局部的符号表
    // string varScope;
    bool error;

    static bool isConstChar(char ch) {
        return isalnum(ch) || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_';
    }

public:
    explicit ErrorSyntaxParser(ErrorLexicalParser &lex, ErrorParse &parse, ErrorSymbolTabel &symbolTabel1);// 构造器

    bool isError() { return error; }

    void readError(); // 读取error.txt :: 并设置error

    //void clearQueue(); // 将队列中的所有信息输出
    void constState(); // 常量说明
    void constDef(); // 常量定义
    void constCharDef(); // char常量说明
    void constIntDef(); // int常量说明

    void varState(); // 变量说明
    void varDef(); // 变量定义

    void funcState(); // 函数说明
    vector<ErrorSymbolItem> paraParse(); // 参数表解析：返回函数参数个数

    bool exprParse(); // 表达式分析
    bool termParse(); // 项分析
    bool factorParse(); // 因子分析

    bool stateColParse(RetType retType = Void); // 语句列分析
    void stateParse(RetType retType = Void); // 语句分析
    void ifParse(RetType retType = Void); // 条件语句
    void condParse();

    // 循环语句
    void doWhileParse(RetType retType = Void); // do-while
    void whileParse(RetType retType = Void); // while
    void forParse(RetType retType = Void); // for

    void assignParse(); // 赋值
    RetType returnParse(); // 返回
    void scanfParse(); // 读语句
    void printfParse(); // 写语句

    void funcRefParse(int flag, string funcName); // 函数
    void valueTabParse(string funcName); // 值参数表解析
    void excValueTabParse(); // 未定义的函数的值参数表解析
    void mainParse(); // 主函数解析
    void comStateParse(RetType retType = Void); // 复合语句解析

    bool isVarState(); // 判断变量说明是否结束
    bool isMain(); // 判断是否为main函数
    bool isConstState();

    void start();
};

#endif //OOP_LEXICALPARSER_H