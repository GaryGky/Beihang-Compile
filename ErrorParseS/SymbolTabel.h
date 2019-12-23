//
// Created by dell on 2019/10/22.
//
#include <map>
#include <string>
#include "SymbolItem.h"
#include "ErrorParse.h"

#ifndef COMPILEROO_SYMBOLTABEL_H
#define COMPILEROO_SYMBOLTABEL_H


class SymbolTabel {
private:
    ErrorParse &errorParse;
    int curlevel; // 辅助表建立层数
    int curpos; // 辅助表建立索引
    map<string, SymbolItem> symbolMap; // 全局符号表
    // map<int, int> indexTabel; // 分程序符号表：记录程序段的位置 【辅助】
public:
    explicit SymbolTabel(ErrorParse &errorParse);

    bool checkSymTable(SymbolItem &item); // 填表前查表
    bool dropSymTabel(int level); // &当一个函数定义模块结束后，删除其符号表
    RetType checkIdenDef(const string &idenName); // 检查标识符的类型
    bool pushSymTabel(SymbolItem item);

    vector<SymbolItem> getFuncPara(string &funcName);

    SymbolItem getSymbol(string &symbName);
};


#endif //COMPILEROO_SYMBOLTABEL_H
