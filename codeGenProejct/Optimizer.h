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

    // void orientCodeOpt(); // 面向代码的优化
    // 将语句序列按照函数作用域划分
    void divideByFunc();

    // 总控开关 :: 控制每个流图开始优化
    void optimize();

    void genOptiTmpCode(); // 生成中间代码

    void genOptCode(); // 产生优化后的代码
    vector<FourComExpr> &getOptCode() {
        return optTmpCode;
    }
};


#endif //CODEORIENTOPT_OPTIMIZER_H
