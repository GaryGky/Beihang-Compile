//
// Created by dell on 2019/10/17.
//
#include <string>
#include <vector>
#include <map>

using namespace std;
#ifndef COMPILEROO_SYMBOLITEM_H
#define COMPILEROO_SYMBOLITEM_H


enum ItemType {
    Constant,//常量
    Variable,//变量
    Function,//函数
    Parament//参数
};

enum RetType { // 返回值类型
    Integer,
    Character,
    Void,
    NullType, //未定义的类型
};

class SymbolItem {
    // 符号表项
private:
    ItemType itemType; // 符号种类
    string name; // 符号名称
    RetType retType; // 符号类型
    int level; // 符号位于的层数
    long arraySize; // 数组大小
    int paraNum; // 函数参数个数
    string value; // 常量或者变量的数值
    vector<SymbolItem> funcParaVec; // 函数参数的集合
public:
    SymbolItem(ItemType itemType1, string &name1, int level1, RetType retType1, string &value1,
               const vector<SymbolItem> &paraset, long arraySize1 = 0, int paraNum1 = 0);

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

    vector<SymbolItem> getParaVec() {
        return funcParaVec;
    }

    string getValue() {
        return value;
    }
};

#endif //COMPILEROO_SYMBOLITEM_H
