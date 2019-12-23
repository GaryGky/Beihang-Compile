//
// Created by dell on 2019/10/22.
//

#include "SymbolTabel.h"
#include "LexicalParser.h"

SymbolTabel::SymbolTabel(ErrorParse &errorParse) : errorParse(errorParse) {
    curlevel = 0;
    curpos = 0;
}

bool SymbolTabel::checkSymTable(SymbolItem &item) {
    if (symbolMap.find(item.getName()) != symbolMap.end()) {
        if (item.getItemType() == Parament) {
            // 如果是参数的话，行号保存在value中
            errorParse.semanticError(stoi(item.getValue()), Redeclare);
        } else {
            errorParse.semanticError(lineNo, Redeclare); // 名字重定义
        }
        return true; // 名字重定义了
    }
    return false;
}

bool SymbolTabel::pushSymTabel(SymbolItem item) {
    if (checkSymTable(item)) {
        // 重定义 返回错误信息
        return false;
    } else {
        // 正常情况返回true信息
        symbolMap.insert(pair<string, SymbolItem>(item.getName(), item));
        return true;
    }
}


bool SymbolTabel::dropSymTabel(int level) {
    // 清除局部变量: 删除所有level不等于0的
    for (auto it = symbolMap.begin(); it != symbolMap.end();) {
        if (it->second.getLevel() != 0) {
            it = symbolMap.erase(it); // 保证删除的安全
        } else {
            it++;
        }
    }
    return false;
}

RetType SymbolTabel::checkIdenDef(const string &idenName) {
    if (symbolMap.find(idenName) != symbolMap.end()) {
        return symbolMap.find(idenName)->second.getRetType();
    } else {
        // 名称未定义
        return NullType;
    }
}

vector<SymbolItem> SymbolTabel::getFuncPara(string &funcName) {
    if (symbolMap.find(funcName) != symbolMap.end()) {
        return symbolMap.find(funcName)->second.getParaVec();
    } else {
        exit(-1); // 没有这个名称的函数
    }
}

SymbolItem SymbolTabel::getSymbol(string &symbName) {
    if (symbolMap.find(symbName) != symbolMap.end()) {
        return symbolMap.find(symbName)->second;
    } else {
        exit(-1); // 如果这里没找到就报错
    }
}

// 填表前查表

