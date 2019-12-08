//
// Created by dell on 2019/11/30.
//
#include <vector>

#ifndef CODEGENPROEJCT_FLOWGRAPH_H
#define CODEGENPROEJCT_FLOWGRAPH_H

#include "BasicBlock.h"

#define MATMAX 200


class FlowGraph {
private:
    string funcName; // 指向的函数名称
    int exitBlockNo; // 流图出口基本块的编号
    // 数据流定向关系 :: 有向图关系 : 由前驱指向后继
    int flowGraph[MATMAX][MATMAX];
    // 数据流的逆向分析图 :: 有向图关系 : 由后继指向前驱
    int revFlowGraph[MATMAX][MATMAX];
    // 每个函数需要维护自己的语句序列
    vector<FourComExpr> graphStates;
    // 每个函数需要维护自己的基本块划分
    vector<BasicBlock> basicBlockVec;
    map<string, int> labelToBlock; // 由标签映射到基本块
    // 由变量名映射到变量的活跃范围
    map<string, set<int>> varToActiveBlock;
public:
    explicit FlowGraph(const string &funcName);

    // 向流图中添加基本块 :: 一次性添加所有语句，且未划分基本块
    void addBlock(vector<FourComExpr> &funcStates);

    // 根据流图中的语句构建基本块
    void buildBlock();

    // 构建流图 :: 根据基本块的流动方向以及跳转语句来构造基本块的转移方式
    void buildFLowGraph();

    // 产生基本块的use和def集合
    void genUseDef();

    // 活跃变量分析 ::
    set<int> getSuffix(int blockNo); // 寻找基本块的后继
    bool addBlokcInToBlockOut(int blockNo, int proBlock); // 将后继集合的in加入到当前集合的out中
    void activeVarBuild();

    // 构建定义-使用冲突图 :: 如果一个变量在另一个变量使用活跃，那么他们之间应该有一条边
    void buildVarConvGraph();
};


#endif //CODEGENPROEJCT_FLOWGRAPH_H
