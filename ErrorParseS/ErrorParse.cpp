//
// Created by dell on 2019/10/19.
//

#include <iostream>
#include "ErrorParse.h"
#include <string>

void ErrorParse::syntaxParseError(int lineno, SyntaxError errorType) {
    string errorInfo;
    if (errorType == LackRbrack) {
        errorInfo = to_string(lineno) + " m"; // 缺少右中]括号
    } else if (errorType == LackRparent) {
        errorInfo = to_string(lineno) + " l"; // 缺少右小)括号
    } else if (errorType == LackSemicn) {
        errorInfo = to_string(lineno) + " k"; // 缺少分;号
    } else if (errorType == LackWhile) {
        errorInfo = to_string(lineno) + " n"; // 缺少while
    } else if (errorType == arrayDefError) {
        errorInfo = to_string(lineno) + " i"; // 数组下标只能是整型表达式
    } else if (errorType == ConstDefError) {
        errorInfo = to_string(lineno) + " o"; // 常量定义中=后面只能是整型或字符型常量
    }else if(errorType == LexError){
        errorInfo = to_string(lineno) + " a"; // 不符合词法
    }
    outfile << errorInfo << endl;
}

// 语义错误分析程序
void ErrorParse::semanticError(int lineno, SemanticError errorType) {
    string errorInfo;
    if (errorType == Redeclare) {
        errorInfo = to_string(lineno) + " b"; // 名字重定义
    } else if (errorType == Nodeclare) {
        errorInfo = to_string(lineno) + " c"; // 标识符未定义
    } else if (errorType == ParaNumError) {
        errorInfo = to_string(lineno) + " d"; // 参数个数不匹配
    } else if (errorType == ParaTypeError) {
        errorInfo = to_string(lineno) + " e"; // 参数类型不匹配
    } else if (errorType == ConstAssError) {
        errorInfo = to_string(lineno) + " j"; // 不能改变常量值
    } else if (errorType == CondParseError) {
        errorInfo = to_string(lineno) + " f"; // 条件判断非法
    }
    outfile << errorInfo << endl;
}

void ErrorParse::returnError(int lineno, ReturnError errorType) {
    string errorInfo;
    if (errorType == voidReturn) {
        errorInfo = to_string(lineno) + " g"; // 无返回值的函数存在不匹配的return语句
    } else if (errorType == lackReturn) {
        errorInfo = to_string(lineno) + " h"; // 有返回值的函数缺少return语句
    } else if (errorType == retTypeError) {
        errorInfo = to_string(lineno) + " h"; // 有返回值的函数存在不匹配的return语句
    }
    outfile << errorInfo << endl;
}

void ErrorParse::initial() {
    outfile.open("error.txt", ios::out);
}



