//
// Created by dell on 2019/10/22.
//
#include <map>
#include <string>
#include "ErrorSymbolItem.h"
#include "ErrorParse.h"

#ifndef CODEGENPROEJCT_ERRORSYMBOLTABEL_H
#define CODEGENPROEJCT_ERRORSYMBOLTABEL_H


class ErrorSymbolTabel {
private:
    ErrorParse &errorParse;
    int curlevel; // 辅助表建立层数
    int curpos; // 辅助表建立索引
    map<string, ErrorSymbolItem> globalSymbolMap; // 全局符号表
    map<string, ErrorSymbolItem> localSymbolMap; // 局部符号表实现分层
    // map<int, int> indexTabel; // 分程序符号表：记录程序段的位置 【辅助】
public:
    explicit ErrorSymbolTabel(ErrorParse &errorParse);

    bool checkSymTable(ErrorSymbolItem &item); // 填表前查表
    bool dropSymTabel(); // &当一个函数定义模块结束后，删除其符号表
    RetType checkIdenDef(const string &idenName); // 检查标识符的类型
    bool pushSymTabel(ErrorSymbolItem item); // 压入符号表
    vector<ErrorSymbolItem> getFuncPara(string &funcName); // 获取函数参数
    ErrorSymbolItem getSymbol(string &symbName); // 获取一个符号表项
};


#endif //COMPILEROO_SYMBOLTABEL_H
