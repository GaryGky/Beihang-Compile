//
// Created by dell on 2019/10/22.
//
#include <map>
#include <string>
#include "SymbolItem.h"

#ifndef COMPILEROO_SYMBOLTABEL_H
#define COMPILEROO_SYMBOLTABEL_H


class SymbolTabel {
private:
    map<string, SymbolItem> symbolMap; // 全局符号表
public:
    explicit SymbolTabel();

    bool isReDef(SymbolItem &item); // 填表前查表
    bool dropSymTabel(int level); // &当一个函数定义模块结束后，删除其符号表
    RetType getRetTypeByName(const string &idenName); // 检查标识符的类型
    bool pushSymTabel(SymbolItem item);

    SymbolItem &getSymbol(const string &symbName); // 获取符号
    string getItemScopeByName(const string &symName); // 获取符号的作用域
    int getIntValueByName(const string &symName); // 获取符号的值value
    char getCharValueByName(const string &symName); // 获取符号的值value
    int getItemOrderByName(const string &symName); // 获取符号的序号
    bool isParameter(const string &funcName, const string &idenName); // 检查该标识符是否为当前函数的参数
    int getParaOrder(const string &funcName, const string &idenName); // 查找该参数的序号
    ItemType getItemtypeByName(const string &symName); // 获取符号的类型
};


#endif //COMPILEROO_SYMBOLTABEL_H
