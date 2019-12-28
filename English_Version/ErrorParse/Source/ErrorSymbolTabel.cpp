//
// Created by dell on 2019/10/22.
//

#include "../Header/ErrorSymbolTabel.h"
#include "../Header/ErrorLexicalParser.h"

ErrorSymbolTabel::ErrorSymbolTabel(ErrorParse &errorParse) : errorParse(errorParse) {
    curlevel = 0;
    curpos = 0;
}

bool ErrorSymbolTabel::checkSymTable(ErrorSymbolItem &item) {
    if (item.getLevel() > 0) { // 局部
        if (localSymbolMap.find(item.getName()) != localSymbolMap.end()) {
            // 重定义
            if (item.getItemType() == Parament) {
                // 如果是参数的话，行号保存在value中
                errorParse.semanticError(stoi(item.getValue()), Redeclare);
            } else {
                errorParse.semanticError(lineNo, Redeclare); // 名字重定义
            }
            return true;
        }
    } else { // 全局
        if (globalSymbolMap.find(item.getName()) != globalSymbolMap.end()) {
            if (item.getItemType() == Parament) {
                // 如果是参数的话，行号保存在value中
                errorParse.semanticError(stoi(item.getValue()), Redeclare);
            } else {
                errorParse.semanticError(lineNo, Redeclare); // 名字重定义
            }
            return true;
        }
    }
    return false;
}

bool ErrorSymbolTabel::pushSymTabel(ErrorSymbolItem item) {
    if (checkSymTable(item)) {
        // 重定义 返回错误信息
        return false;
    } else {
        if (item.getLevel() > 0) {
            localSymbolMap.insert(pair<string, ErrorSymbolItem>(item.getName(), item));
        } else {
            globalSymbolMap.insert(pair<string, ErrorSymbolItem>(item.getName(), item));
        }
        return true;
    }
}


bool ErrorSymbolTabel::dropSymTabel() {
    // 清除局部变量: 删除所有level不等于0的
    localSymbolMap.clear(); // 清楚局部符号表
    return true;
}

RetType ErrorSymbolTabel::checkIdenDef(const string &idenName) { // 使用的时候来查表
    if (localSymbolMap.find(idenName) != localSymbolMap.end()) {
        return localSymbolMap.find(idenName)->second.getRetType();
    } else if (globalSymbolMap.find(idenName) != globalSymbolMap.end()) {
        return globalSymbolMap.find(idenName)->second.getRetType();
    } else {
        return NullType; // 名称未定义
    }
}

vector<ErrorSymbolItem> ErrorSymbolTabel::getFuncPara(string &funcName) {
    if (globalSymbolMap.find(funcName) != globalSymbolMap.end()) {
        return globalSymbolMap.find(funcName)->second.getParaVec();
    } else {
        exit(-1); // 没有这个名称的函数
    }
}

ErrorSymbolItem ErrorSymbolTabel::getSymbol(string &symbName) {
    if (localSymbolMap.find(symbName) != localSymbolMap.end()) {
        return localSymbolMap.find(symbName)->second;
    } else if (globalSymbolMap.find(symbName) != globalSymbolMap.end()) {
        return globalSymbolMap.find(symbName)->second;
    } else {
        exit(-1); // 如果这里没找到就报错
    }
}

