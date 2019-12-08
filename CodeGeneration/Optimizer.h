//
// Created by dell on 2019/11/30.
//

#ifndef CODEORIENTOPT_OPTIMIZER_H
#define CODEORIENTOPT_OPTIMIZER_H

#include "FourComExpr.h"
#include <iostream>
#include <fstream>
#include <map>
#include "BasicBlock.h"
#include "FlowGraph.h"

using namespace std;

class Optimizer { // 顶端的程序层
private:
    vector<FourComExpr> originTmpCode; // 原始中间代码
    vector<FourComExpr> optTmpCode; // 优化后的中间代码
    ofstream optTmpFile;
    map<string, FlowGraph> funcToGraph; // 通过函数名称来寻找流图 :: 两者也可以通过下标进行捆绑索引
    vector<string> funcVec; // 函数名称
    vector<FlowGraph> graphVec; // 函数对应流图
public:
    explicit Optimizer(vector<FourComExpr> &tmpCode);

    void orientCodeOpt(); // 面向代码的优化
    // 将语句序列按照函数作用域划分
    void divideByFunc();

    // 总控开关 :: 控制每一个函数的流通进行数据流动
    void buildFlowGraph();

    // 总控开关 :: 控制每一个函数进行活跃变量分析
    void buildActiveVar();

    // 控制每一个函数(流图)的定义-使用 冲突关系
    void buildDefConv();

    // 构建函数内联关系
    void buildInlineFunc();

    /* 以下为面向代码的优化 :: 稍作了函数内联 */
    void optFactorial(); // 优化第一步 --- 阶乘递归的优化 减少了300
    void optSwap(); // 优化第二补 --- 交换函数的优化
    void optMod(); // 优化取余函数
    void optFull_Num(); // 优化这个函数
    void optFlowerArea(); // 优化这块区域
    void optRidDeadCode(); // 删除循环体内的死代码
    void genTmpCode();

    vector<FourComExpr> &getOptCode() {
        return optTmpCode;
    }
};


#endif //CODEORIENTOPT_OPTIMIZER_H
