//
// Created by dell on 2019/10/17.
//
#include<string>
#include <vector>
#include <map>
#include "../../EnumClass.h"

using namespace std;
#ifndef CODEGENPROEJCT_ERRORSYMBOLITEM_H
#define CODEGENPROEJCT_ERRORSYMBOLITEM_H


class ErrorSymbolItem {
    // 符号表项
private:
    ItemType itemType; // 符号种类
    string name; // 符号名称
    RetType retType; // 符号类型
    int level; // 符号位于的层数
    long arraySize; // 数组大小
    int paraNum; // 函数参数个数
    string value; // 常量或者变量的数值
    vector<ErrorSymbolItem> funcParaVec; // 函数参数的集合
public:
    ErrorSymbolItem(ItemType itemType1, string &name1, int level1, RetType retType1, string &value1,
                    const vector<ErrorSymbolItem> &paraset, long arraySize1 = 0, int paraNum1 = 0);

    ItemType getItemType() {
        return itemType;
    }

    string getName() {
        return name;
    }

    RetType getRetType() {
        return retType;
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

    vector<ErrorSymbolItem> getParaVec() {
        return funcParaVec;
    }

    string getValue() {
        return value;
    }
};

#endif //COMPILEROO_SYMBOLITEM_H
