//
// Created by dell on 2019/11/30.
//


#ifndef CODEGENPROEJCT_BASICBLOCK_H
#define CODEGENPROEJCT_BASICBLOCK_H

#include <vector>
#include <set>
#include "FourComExpr.h"

class BasicBlock {
private:
    int blockNum; // 基本块的序号
    int start; // 入口语句的标号
    int end; // 结束语句的标号
    set<string> localVar; // 基本块所在函数的局部变量
    vector<FourComExpr> stateVec; // 基本块内的中间代码
    set<string> useSet; // 基本块内使用的数据信息
    set<string> defSet; // 基本块内定义的数据信息
    set<string> inLiveSet; // 活跃变量分析的in集合
    set<string> outLiveSet; // 活跃变量分析的out集合

public:
    BasicBlock(int blockNumber, int start, set<string> &localVar); // 输入序号
    void addState(const FourComExpr& fourComExpr); // 向基本块内添加语句

    // void genDAG(); // 产生DAG图
    // void genOptCode(); // 从DAG图中导出代码

    void genUseDef(); // 产生基本块的use和def集合 :: 填充use集合

    // 活跃变量分析相关
    void addLiveOut(const string &varName); // 向活跃变量的out集合添加一个变量
    void genLiveIn(); // 每次都会刷新一次in集合
    set<string> &getInLiveSet() { return inLiveSet; } // 获取本集合的in :: 最终将成为活跃变量的查询点
    set<string> &getOutLiveSet() { return outLiveSet; } // 获取本基本块的out
    set<string> &getDefSet(){ return defSet;}

    int getBlockStart() { return start; }

    void setEnd(int inputEnd) { this->end = inputEnd; }

    int getEnd() { return end; }
};


#endif //CODEGENPROEJCT_BASICBLOCK_H
