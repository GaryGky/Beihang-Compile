//
// Created by dell on 2019/11/5.
//

#ifndef CODEGENPROEJCT_FOURCOMEXPR_H
#define CODEGENPROEJCT_FOURCOMEXPR_H

#include "SymbolItem.h"

/* 中间代码的种类枚举
1.值参传入push x,push y
2.调用函数 call add
3.赋值语句 i = ret  i = t1 + 1
4.条件判断 x == y  x<=y
5.纯标签Label1:
6.跳转语句 goto label1 bnz label1 ...
7.函数返回 ret x   ret
8.函数声明 int x
9.参数表 param x
10.print "xxxx"  print (表达式)
11.read int a, read char c // 输入语句的定义
*/
enum TmpCodeType {
    FunctionCall, // 函数调用
    AssignState, // 赋值语句
    Label, // 标签
    VaribleDef, // 变量定义
    ArrayDef, // 数组定义
    ConstantDef, // 常量定义
    FunctionDef, // 函数定义
    Expression, // 表达式
    Jump, // 无条件跳转语句
    BEZ, // 相等时 ==
    BNZ, // 不等时 !=
    BLZ, // 小于时 <
    BLEZ, // 小于等于时 <=
    BGZ, // 大于时 >
    BGEZ, // 大于等于时 >=
    ReadChar, // scanf 中的char
    ReadInt, // scanf中的int
    PrintStr, // 输出字符串
    PrintInt, // 输出整数
    PrintChar, // 输出字符
    PrintNewLine, // 换行
    Return, // 返回语句
    Default
};

enum FactorType {
    IdenFactor, // 单独的标识符
    ArrayFactor, // 数组标识符
    FuncRefFactor, // 函数调用,
    ExprFactor, // 表达式因子
    IntFactor, // 整数因子
    CharFactor, // 字符因子
    DefaultFac // 默认类型
};

// 表达式的组成成分
struct ExprCompose {
    FactorType factorType; // 因子的类型
    string op; // 运算符
    string target; // 标识符的名字, 函数名字, 保存表达式最终结果的临临时变量
    string index; // 数组下标的表达式, 函数返回值的临时变量
    /*int number; // 如果是整数的话记录其值
    char ch; // 字符型因子保存其值*/
    bool isOperator; // 是否为操作符
    RetType retType; // 因子是否为整型
};

//函数存在多返回值的结构体设计
//表达式返回值
struct ExpRet {
    string name; // 表达式返回的名字--->统一规定,表达式都需要有一个返回变量名(临时变量 --- 用来保存表达式的值)
    string index; // 数组下标
    bool isSurable; //是否确定
    RetType type = Character; // 值类型:: 一旦设置为Intege就不能再改变了
    int number;
    char character;
    bool isEmpty;//是否是空的
    vector<ExprCompose> termFactor; // 项中包含的因子
};


/*
 * 四元式整体表示：<操作符>, <操作数1>, <操作数2>, <结果>
 *
 */
struct FourComExpr {
    TmpCodeType type;
    RetType valueType; // 当前四元式的类型
    string target; // 运算结果, 函数名称, 变量名
    string index = ""; // 赋值语句的右边如果是数组元素，需要一个index来保存下标
    string left;  // 操作数1:: 数组定义时的大小，常量定义时的值
    string right;// 操作数2
    string index1 = ""; // 左边数组元素的下标
    string index2 = ""; // 右边数组元素的下标
    string op; // 运算符 --- 关系运算符|算数运算符
    int arrayOrVar{}; // 用来标记赋值的目标：0表示数组，1表示变量
    vector<SymbolItem> paraSet; // 函数的参数表
    vector<ExpRet> valueParaTab; // 函数调用的值参数表
    // int number{}; // 用来保存表达式计算结果
    // char character{}; // 赋值语句用
    string varScope; // 变量的作用域 -- 保存引用比较安全
};


#endif //CODEGENPROEJCT_FOURCOMEXPR_H
