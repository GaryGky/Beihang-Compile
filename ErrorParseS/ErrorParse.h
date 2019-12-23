//
// Created by dell on 2019/10/19.
//
#include <fstream>

using namespace std;

#ifndef COMPILEROO_ERRORPARSE_H
#define COMPILEROO_ERRORPARSE_H
//语法分析错误种类枚举
/*
1.文件为空error
2.main函数后多余内容
3.没有void main函数
4.语法缺少部分结构,或者结构不匹配
5.数组声明中数组的长度不是无符号整数(>0)
6.0的前面不可以有正负号修饰
*/
enum SyntaxError {
    LexError, // 在语法分析中不符合词法
    EmptyFile, // 空文件
    RedantCont, // main函数后有多余内容
    LackMain, // 缺少main函数
    arrayDefError, // 数组的下标不为整型
    LackSemicn, // 缺少分号
    LackRparent, // 缺少右小括号
    LackRbrack, // 缺少右中括号
    LackWhile, // do-while语句缺少while
    ConstDefError, // 常量定义时，只能是整数或者字符常量
};

//语义分析的相关错误
/*
1.声明定义冲突(作用域问题)
2.引用变量，变量未定义
3.引用数组，数组越界
4.引用函数，函数未定义
5.单个语句只能是函数
6.调用函数,无参却传参数
7.调用函数，值参数个数不对
8.调用函数，值参数类型不对
9.赋值对象不是普通变量(包含了多种错误情况)
10.赋值类型不匹配(int 给 char赋值)
11.引用标识符,标识符对应类型不匹配
12.值参数表不可以为空
13.case出现相同的值
14.除数不可以为0
*/
enum SemanticError {

    Redeclare, // 重复定义
    Nodeclare, // 引用未定义的变量或函数
    IndexError, // 数组越界
    ParaNumError, // 函数参数个数不对
    ParaTypeError, // 函数参数类型不对
    ConstAssError, // 对常量赋值
    AssTypeError, // 赋值类型不同
    ZeroError, // 除数不可以为0
    CondParseError, // 条件判断不符合类型
};

/*与return语句有关的error
1.无返回值存在有返回值的return
2.有返回值存在无返回值的return
3.有返回值不存在return
4.有返回值存在return类型不兼容
5.有返回值存在分支导致无返回值
*/
enum ReturnError {
    voidReturn, // void类型函数有返回值
    lackReturn, // 缺少return返回语句
    retTypeError, // 返回值类型错误
    // 有返回值存在分支导致无返回值
};

class ErrorParse {
private:
    ofstream outfile;
public:
    void initial();

    void syntaxParseError(int lineno, SyntaxError errorType); // 语法分析错误处理程序
    void semanticError(int lineno, SemanticError errorType); // 语义分析错误处理程序
    void returnError(int lineno, ReturnError errorTypr); // 返回值相关错误处理程序
};


#endif //COMPILEROO_ERRORPARSE_H
