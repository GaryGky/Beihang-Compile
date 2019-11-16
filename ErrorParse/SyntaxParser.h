//
// Created by dell on 2019/10/10.
//
#include "LexicalParser.h"
#include "SymbolItem.h"
#include "ErrorParse.h"
#include "SymbolTabel.h"
#include <queue>
#include <string>
#include <set>


#ifndef OOP_SYNTAXPARSER_H
#define OOP_SYNTAXPARSER_H


class SyntaxParser {
private:
    LexicalParser &lexicalParser; // 词法分析器
    ErrorParse &errorParse;
    SymbolTabel symbolTabel;
    int level; // 记录语法分析的层数

public:
    explicit SyntaxParser(LexicalParser &lex, ErrorParse &parse, SymbolTabel &symbolTabel1);// 构造器

    //void clearQueue(); // 将队列中的所有信息输出
    void constState(); // 常量说明
    void constDef(); // 常量定义
    void constCharDef(); // char常量说明
    void constIntDef(); // int常量说明

    void varState(); // 变量说明
    void varDef(); // 变量定义

    void funcState(); // 函数说明
    vector<SymbolItem> paraParse(); // 参数表解析：返回函数参数个数

    bool exprParse(); // 表达式分析
    bool termParse(); // 项分析
    bool factorParse(); // 因子分析

    void stateColParse(RetType retType = Void); // 语句列分析
    void stateParse(bool &ownsRet, RetType retType = Void); // 语句分析
    void ifParse(); // 条件语句
    void condParse();

    // 循环语句
    void doWhileParse(); // do-while
    void whileParse(); // while
    void forParse(); // for

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