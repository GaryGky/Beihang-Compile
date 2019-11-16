//
// Created by dell on 2019/11/11.
//

#ifndef CODEGENPROEJCT_OPTIMIZER_H
#define CODEGENPROEJCT_OPTIMIZER_H

#include "FourComExpr.h"
#include "SymbolTabel.h"
#include <fstream>
#include <map>

struct LocalVarible{
    RetType varType; // 变量的类型
    string varName; // 变量的名称
    string varScope; // 变量的作用域
    int order; // 变量的序号
};

class Optimizer {
private:
    vector<FourComExpr> tmpCodeVector; // 保存中间代码的向量
    SymbolTabel symbolTabel; // 全局符号表
    ofstream mipsFile;
    unsigned strMemSize = 0;//字符串占据的大小
    string codeScope; // 当前正在分析的区域
    int varCnt; // 函数局部变量的计数器
    map<string, LocalVarible> nameToLocalVar; // 函数局部变量的索引
public:
    explicit Optimizer(vector<FourComExpr> &tmpCode, SymbolTabel &symbolTabel1);

    void genMipsCode();

    void getNewLine(); // 换行
    void getSpace(); // 空格 :: 是关于MIPs IO 的输出
    void genDataSegment(); // 产生data段的数据
    void genTextSegment(); // 产生text段的数据
    void genAssignCode(FourComExpr &fourComExpr); // 产生赋值语句的mips
    void genExprCode(FourComExpr &fourComExpr);

    void genFuncDefCode(FourComExpr &fourComExpr); // 产生函数定义的mips代码
    void genReturnCode(FourComExpr &fourComExpr); // 产生函数返回的代码
    void genFuncRefCode(FourComExpr &fourComExpr); // 产生函数调用的代码

    string getGloabalAddr(const string &varName, const string &dstReg); // 获取全局变量的地址 --- 赋值语句的左边
    string getLocalAddr(const string &varScope, int order, const string &dstReg); // 获取局部变量的地址 --- 赋值语句的左边
    string getTempReg(const string &tempReg, const string &dstReg); // 获取临时变量的寄存器值
    void initialStack(const string &funcName); // 初始化函数的运行栈
    void keepScene(); // 保存现场
    void getArrayValue(const string &arrayName, const string &offset,const string &dstReg); // 获取数组元素的值
    void getArrayAddr(const string &arrayName, const string &offset,const string &dstReg); // 获取数组元素的地址
    void getParaValue(int paraOrder, const string &dstReg);
};


#endif //CODEGENPROEJCT_OPTIMIZER_H
