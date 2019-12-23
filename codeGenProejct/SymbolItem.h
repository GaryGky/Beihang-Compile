//
// Created by dell on 2019/10/17.
//
#include <string>
#include <vector>
#include "EnumClass.h"
#include <map>

using namespace std;
#ifndef COMPILEROO_SYMBOLITEM_H
#define COMPILEROO_SYMBOLITEM_H

class SymbolItem {
    // 符号表项
private:
    ItemType itemType; // 符号种类
    string name; // 符号名称
    RetType retType; // 符号类型
    int level; // 符号位于的层数
    long arraySize; // 数组大小
    int paraNum; // 函数参数个数
    string value; // 常量或者或者数组的大小 :: 如果为0 表示普通变量
    vector<SymbolItem> funcParaVec; // 函数参数的集合
    int number; // 该符号表项的数值
    char character; // 符号表项的字符值
    string itemScope; // 符号的作用域
    int order; // 符号表在当前作用域内是第几个
public:
    SymbolItem(ItemType itemType1, string &name1, int level1, RetType retType1, string &value1, int order,
               string &varScope, const vector<SymbolItem> &paraset, long arraySize1 = 0, int paraNum1 = 0);

    int getScopeOrder() {
        return order; // 返回符号表在当前作用域内的序号
    }

    ItemType getItemType() {
        return itemType;
    }

    string getName() {
        return name;
    }

    RetType getRetType() {
        return retType;
    }

    string getScope() {
        return itemScope;
    }

    long getSize() {
        return arraySize;
    }

    int getParaNum() {
        return paraNum;
    }

    int getLevel() {
        return level;
    }

    vector<SymbolItem> &getParaVec() {
        return funcParaVec;
    }

    string getValue() {
        return value;
    }

    void setNum(int num) {
        number = num;
    }

    void setCharacter(char ch) {
        character = ch;
    }


};

#endif //COMPILEROO_SYMBOLITEM_H
