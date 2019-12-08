//
// Created by dell on 2019/11/30.
//

#include "FlowGraph.h"
#include <algorithm> // 集合运算


FlowGraph::FlowGraph(const string &funcName) {
    this->funcName = funcName;
}

void FlowGraph::addBlock(vector<FourComExpr> &funcStates) {
    this->graphStates = funcStates;
}

void FlowGraph::buildBlock() {
    int blockNo = 0;
    // 第一步 :: 划分基本块内的语句
    // 入口语句：从一开始到一个label
    BasicBlock basicBlock1(blockNo++, 0); // 开始语句
    basicBlockVec.push_back(basicBlock1); // 入口语句到第一个label
    for (int i = 0; i < (int) graphStates.size(); i++) {
        if (graphStates[i].type == Label) {
            labelToBlock.insert(pair<string, int>(graphStates[i].target, blockNo));
            BasicBlock basicBlock(blockNo++, i); // 构建一个基本快
            basicBlockVec.push_back(basicBlock); // 加入基本快向量中
        } else if (graphStates[i].type == BLEZ || graphStates[i].type == BLZ || graphStates[i].type == BGEZ ||
                   graphStates[i].type == BGZ || graphStates[i].type == BEZ || graphStates[i].type == BNZ ||
                   graphStates[i].type == Return || graphStates[i].type == Jump) {
            BasicBlock basicBlock(blockNo++, i + 1); // 构建一个基本快
            basicBlockVec.push_back(basicBlock); // 加入基本快向量中
        }
    }
    exitBlockNo = blockNo; // 设置出口基本块
    // 第二步 :: 填补基本块内的语句
    for (int i = 0; i < (int) basicBlockVec.size() - 1; i++) {
        int start = basicBlockVec[i].getBlockStart(); // 也要把基本块的开始语句加入
        int end = basicBlockVec[i + 1].getBlockStart();
        basicBlockVec[i].setEnd(end - 1);
        for (; start < end; start++) {
            basicBlockVec[i].addState(graphStates[i]);
        }
    }
    // 第三步: 加入一条出口语句
    BasicBlock basicBlock(-1, -1);
    basicBlockVec.push_back(basicBlock); // 加入一条出口
}

void FlowGraph::buildFLowGraph() {
    buildBlock(); // 建立流图之前划分基本块
    for (int i = 0; i < (int) basicBlockVec.size() - 1; i++) {
        int end = basicBlockVec[i].getEnd();
        if (graphStates[i].type == Jump) {
            string label = graphStates[i].target; // 跳转到的目标
            int blockNo = labelToBlock.find(label)->second;
            flowGraph[i][blockNo] = 1;
            revFlowGraph[blockNo][i] = 1;
        } else if (graphStates[i].type == Return) {
            // 直接到达出口
            flowGraph[i][exitBlockNo] = 1;
            revFlowGraph[exitBlockNo][i] = 1;
        } else if (graphStates[i].type == BLEZ || graphStates[i].type == BLZ || graphStates[i].type == BGEZ ||
                   graphStates[i].type == BGZ || graphStates[i].type == BEZ || graphStates[i].type == BNZ) {
            string label = graphStates[i].target; // 跳转到的目标
            int blockNo = labelToBlock.find(label)->second;
            flowGraph[i][blockNo] = 1; // 指向跳转语句的后继
            flowGraph[i][i + 1] = 1; // 指向顺序执行的后继
            revFlowGraph[blockNo][i] = 1;
            revFlowGraph[i + 1][i] = 1;
        } else {
            flowGraph[i][i + 1] = 1; // 这是属于被label结束的情况
            flowGraph[i + 1][i] = 1;
        }
    }
}

void FlowGraph::activeVarBuild() {
    while (true) {
        bool isOutChange = true;
        int cnt = 0; // 记录out集合被改变的次数
        for (int index = exitBlockNo; index >= 0; index--) {
            set<int> suffixSet = getSuffix(index); // 找到前驱
            basicBlockVec[index].genLiveIn(); // 计算一次当前基本块的in
            // 将suffixSet中的in集合加入到index所指集合的out中
            for (auto it:suffixSet) {
                cnt += addBlokcInToBlockOut(index, it); // 计算当前基本块的out集合
            }
        }
        if (cnt == 0) isOutChange = false; // 如果所有out集合都没有改变，说明基本块的活跃变量已经没有改变
        if (!isOutChange) {
            break;
        }
    }

}

void FlowGraph::genUseDef() {
    for (auto &it : basicBlockVec) {
        it.genUse();
        it.genDef();
    }
}

set<int> FlowGraph::getSuffix(int blockNo) {
    set<int> suffix;
    for (int i = 0; i < exitBlockNo; i++) {
        if (flowGraph[blockNo][i] == 1) {
            suffix.insert(i); // 找到当前基本块的前驱
        }
    }
    return suffix;
}

bool FlowGraph::addBlokcInToBlockOut(int blockNo, int proBlock) {
    set<string> proLiveIn = basicBlockVec[proBlock].getInLiveSet(); // 得到集合的in集合
    set<string> blockOut = basicBlockVec[blockNo].getOutLiveSet(); // 当前基本块的out集合
    bool isOutChange = false;
    // 把in集合加入当前基本块的out集合 :: 不断地更新out集合
    for (auto &it:proLiveIn) {
        if (blockOut.find(it) == blockOut.end()) { // 如果不在out集合中，才加入
            isOutChange = true;
            basicBlockVec[blockNo].addLiveOut(it);
        }
    }
    return isOutChange;
}

void FlowGraph::buildVarConvGraph() {

}


