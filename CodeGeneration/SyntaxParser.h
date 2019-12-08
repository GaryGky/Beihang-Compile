//
// Created by dell on 2019/10/10.
//
#include "LexicalParser.h"
#include "SymbolItem.h"
#include "SymbolTabel.h"
#include "FourComExpr.h"
#include <queue>
#include <string>
#include <set>
#include <map>

#ifndef OOP_SYNTAXPARSER_H
#define OOP_SYNTAXPARSER_H


class SyntaxParser {
private:
    LexicalParser &lexicalParser; // 词法分析器
    SymbolTabel &symbolTabel;
    int level; // 记录语法分析的层数
    int labelCount; // 用于产生标号
    int varCount; // 用于产生变量
    int stringCount; // 用于产生字符串
    int globalRegCnt; // 全局寄存器计数
    ofstream tmpCode; // 用来输出中间代码的程序
    TmpCodeType condRelation; // 表达式中的关系运算符 --- 默认与0作比较
    vector<FourComExpr> tmpCodeVector; // 用来保存中间代码的输出
    map<string, TmpCodeType> relationMap; // 建立关系运算符到四元式的映射
    string varScope; // 变量作用域
    int order; // 记录局部变量在其作用域内是第几个 -- 全局变量不用管
    map<string, SymbolItem> localVarSpace; // 函数局部变量的空间
public:
    explicit SyntaxParser(LexicalParser &lex, SymbolTabel &symbolTabel1);// 构造器

    string generateLabel(); // 产生标签
    string genVar(); // 产生变量
    string genString(); // 产生字符串标号
    string genGlobalReg(); // 产生全局寄存器

    void constState(); // 常量说明
    void constDef(); // 常量定义
    void constCharDef(); // char常量说明
    void constIntDef(); // int常量说明

    void varState(); // 变量说明
    void varDef(); // 变量定义

    void funcState(); // 函数说明
    vector<SymbolItem> paraParse(); // 参数表解析：返回函数参数个数

    ExpRet exprParse(); // 表达式分析
    ExpRet termParse(); // 项分析
    ExprCompose factorParse(); // 因子分析

    bool stateColParse(RetType retType = Void); // 语句列分析
    void stateParse(RetType retType = Void); // 语句分析
    void ifParse(RetType retType = Void); // 条件语句
    string condParse(); // 条件解析
    void cmpBranch(const string &condRes, const string &labelA); // 比较并且跳转

    // 循环语句
    void doWhileParse(RetType retType = Void); // do-while
    void whileParse(RetType retType = Void); // while
    void forParse(RetType retType = Void); // for

    void assignParse(const string &idenName); // 赋值
    RetType returnParse(); // 返回
    void scanfParse(); // 读语句
    void printfParse(); // 写语句

    string funcRefParse(int flag, const string &funcName); // 函数 :: 返回保存函数返回值的临时变量
    vector<ExpRet> valueTabParse(const string &funcName); // 值参数表解析
    void mainParse(); // 主函数解析
    void comStateParse(RetType retType = Void); // 复合语句解析

    bool isVarState(); // 判断变量说明是否结束
    bool isMain(); // 判断是否为main函数
    bool isConstState();

    void start();

    void genTmpCode(); // 生成中间代码

    vector<ExprCompose> revPolTransfer(vector<ExprCompose> &infixExpr); // 将中缀表达式转换为后缀表达式
    string revPolCal(vector<ExprCompose> &revPolExpr); // 逆波兰表达式的计算

    vector<FourComExpr> &getTmpCode() {
        return tmpCodeVector;
    }
};

#endif //OOP_LEXICALPARSER_H