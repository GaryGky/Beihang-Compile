//
// Created by dell on 2019/10/10.
//
#include "LexicalParser.h"
#include <queue>

#ifndef OOP_SYNTAXPARSER_H
#define OOP_SYNTAXPARSER_H


class SyntaxParser {
private:
    LexicalParser &lexicalParser; // 词法分析器
    // queue<string> advWord; // 提前读入的字符
    // string curString; // 当前正在处理的字符串

public:
    explicit SyntaxParser(LexicalParser &lex);// 构造器

    //void clearQueue(); // 将队列中的所有信息输出
    bool isConstState();
    void constState(); // 常量说明
    void constDef(); // 常量定义
    void constCharDef(); // char常量说明
    void constIntDef(); // int常量说明
    bool isVarState();

    void varState(); // 变量说明
    void varDef(); // 变量定义
    bool isMain(); // 判断是否为main函数
    void funcState(); // 函数说明
    void paraParse();

    void exprParse(); // 表达式分析
    void termParse(); // 项分析
    void factorParse(); // 因子分析
    void stateColParse(); // 语句列分析
    void stateParse(); // 语句分析
    void ifParse(); // 条件语句
    void condParse();

    // 循环语句
    void doWhileParse(); // do-while
    void whileParse(); // while
    void forParse(); // for
    void assignParse(); // 赋值
    void returnParse(); // 返回
    void scanfParse(); // 读语句
    void printfParse(); // 写语句
    void funcRefParse(int flag); // 函数
    void valueTabParse(); // 值参数表解析
    void mainParse(); // 主函数解析
    void comStateParse(); // 复合语句解析

    void start();
};

#endif //OOP_LEXICALPARSER_H