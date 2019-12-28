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
    // 保存函数的局部变量
    set<string> localVar; // 函数的局部变量
    // 每个函数需要维护自己的语句序列
    vector<FourComExpr> graphStates;
    // 每个函数需要维护自己的基本块划分
    vector<BasicBlock> basicBlockVec;
    map<string, int> labelToBlock; // 由标签映射到基本块
    // 最终寄存器在该函数内部的分配情况
    map<string, string> varToGlobalReg; // 由变量名映射到全局寄存器
    map<int, int> varOrderToRegOrder; // 变量序号映射到寄存器序号
    // 冲突-使用图 : 以变量标号作为结点
    int convUseMap[MATMAX][MATMAX];
    int colorMap[MATMAX][MATMAX]; // 辅助全局寄存器分配矩阵 :: 因为直接对冲突图操作的话不安全
    int varSize; // 为活跃变量指定标号
    // 这两个map应该在所有语句添加进来之后建立好
    map<string, int> varNameToOrder; // 变量名称对应到标号
    map<int, string> orderToVarname; // 标号映射到变量名称
    // 不能分配全局寄存器的结点
    set<int> globalFailVar; // 全局寄存器分配失败
    vector<FourComExpr> optTmpCode; // 优化后的中间代码
public:
    explicit FlowGraph(const string &funcName);

    void optStart(); // 优化启动函数 :: 启动该流图的优化

    void findLocalVar(); // 找到函数的局部变量

    // 向流图中添加基本块 :: 一次性添加所有语句，且未划分基本块
    void addBlock(vector<FourComExpr> &funcStates);

    // 根据流图中的语句构建基本块
    void buildBlock();

    // 构建流图 :: 根据基本块的流动方向以及跳转语句来构造基本块的转移方式
    void buildFLowGraph();

    void buildVarOrderMap(); // 建立序号到变量名称的映射 :: 在活跃变量分析完成之后

    // 产生基本块的use和def集合
    void genUseDef();

    // 活跃变量分析 :: 分为以下三个步骤
    set<int> getSuffix(int blockNo); // 寻找基本块的后继
    bool addBlokcInToBlockOut(int blockNo, int proBlock); // 将后继集合的in加入到当前集合的out中
    void activeVarBuild();

    // 构建定义-使用冲突图 :: 如果一个变量在另一个变量使用活跃，那么他们之间应该有一条边
    void buildVarConvGraph();

    void addEdgeToMap(set<string> &activeVarSet); // 活根据跃变量集合加边

    // 根据定义-使用冲突图构造全局寄存器的分配
    void disGlobalReg();

    int getDegLess8(set<int> &movedVar); // 找到度数小于7的一个点
    int getDegMax(set<int> &movedVar); // 找到度数最大的结点
    void moveNode(int node); // 移走该节点
    int disRegToLast(set<int> &movedVar); // 为最后剩余的变量分配寄存器
    int disRegToVar(int varOrder); // 为当前寄存器分配变量

    void convertOrderToVar(); // 将编号转换为字符串
    void optimizeTmpCode(); // 直接将中间代码中的局部变量用全局寄存器代替
    void peepOptmize(); // 窥孔优化 :: 将针对寄存器的重复赋值去掉
    vector<FourComExpr> &getOptCode() { return optTmpCode; }
};


#endif //CODEGENPROEJCT_FLOWGRAPH_H
