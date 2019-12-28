//
// Created by dell on 2019/10/22.
//

#include "../Header/SymbolTabel.h"
#include "../Header/LexicalParser.h"
#include <algorithm>


bool SymbolTabel::isReDef(SymbolItem &item) {
    return symbolMap.find(item.getName()) != symbolMap.end(); //检查名字是否相同
}

bool SymbolTabel::pushSymTabel(SymbolItem item) {
    if (isReDef(item)) {
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

RetType SymbolTabel::getRetTypeByName(const string &idenName) {
    for (auto it: symbolMap) {
        if (it.second.getLevel() > 0 && it.second.getName() == idenName) { // 先找局部变量
            return it.second.getRetType();
        }
    }
    if (symbolMap.find(idenName) != symbolMap.end()) {
        return symbolMap.find(idenName)->second.getRetType();
    } else {
        // 名称未定义
        return NullType;
    }
}


SymbolItem &SymbolTabel::getSymbol(const string &symbName) {
    if (symbolMap.find(symbName) != symbolMap.end()) {
        return symbolMap.find(symbName)->second;
    } else {
        exit(-1); // 如果这里没找到就报错
    }
}

string SymbolTabel::getItemScopeByName(const string &symName) {
    if (symbolMap.find(symName) != symbolMap.end()) {
        return symbolMap.find(symName)->second.getScope();
    } else {
        return "null";
    }
}

int SymbolTabel::getItemOrderByName(const string &symName) {
    if (symbolMap.find(symName) != symbolMap.end()) {
        return symbolMap.find(symName)->second.getScopeOrder();
    } else {
        return -1; // 如果这里没找到就报错
    }
}

int SymbolTabel::getIntValueByName(const string &symName) {
    if (symbolMap.find(symName) != symbolMap.end()) {
        return stoi(symbolMap.find(symName)->second.getValue());
    } else {
        return -1; // 如果这里没找到就报错
    }
}

char SymbolTabel::getCharValueByName(const string &symName) {
    if (symbolMap.find(symName) != symbolMap.end()) {
        return symbolMap.find(symName)->second.getValue().at(0);
    } else {
        return '\0'; // 如果这里没找到就报错
    }
}

bool SymbolTabel::isParameter(const string &funcName, const string &idenName) {
    if (funcName.length() == 0 || funcName == "main") {
        return false;
    }
    vector<SymbolItem> paraVec = getSymbol(funcName).getParaVec();
    for (auto &it : paraVec) {
        if (it.getName() == idenName) {
            return true;
        }
    }
    return false;
}

int SymbolTabel::getParaOrder(const string &funcName, const string &idenName) {
    vector<SymbolItem> paraVec = getSymbol(funcName).getParaVec();
    for (auto &it : paraVec) {
        if (it.getName() == idenName) {
            return it.getScopeOrder();
        }
    }
    return -1;
}

ItemType SymbolTabel::getItemtypeByName(const string &symName) {
    if (symbolMap.find(symName) != symbolMap.end()) {
        return symbolMap.find(symName)->second.getItemType();
    } else {
        return NullItem; // 如果这里没找到就报错
    }
}

SymbolTabel::SymbolTabel() = default;
