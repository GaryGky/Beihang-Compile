//
// Created by dell on 2019/11/30.
//

#include "FlowGraph.h"
#include <stack>
#include <iostream>
#include <algorithm> // 集合运算

using namespace std;


FlowGraph::FlowGraph(const string &funcName) {
    this->funcName = funcName;
    varSize = 0;
}

void FlowGraph::optStart() {
    findLocalVar(); // 建立函数的局部变量集合
    buildFLowGraph(); // 构建流图
    genUseDef(); // 控制每个基本块产生自己的use def集合
    activeVarBuild(); // 活跃变量定义
    buildVarOrderMap(); // 构建活跃变量的编号映射关系
    buildVarConvGraph(); // 构建冲突定义图
    disGlobalReg(); // 分配全局寄存器
    convertOrderToVar(); // 便于上层取用变量
    optimizeTmpCode(); // 直接用全局寄存器代替局部变量
    peepOptmize(); // 窥孔优化
}

void FlowGraph::addBlock(vector<FourComExpr> &funcStates) {
    this->graphStates = funcStates;
}

void FlowGraph::buildBlock() {
    int blockNo = 0;
    // 第一步 :: 划分基本块内的语句
    // 入口语句：从一开始到一个label
    BasicBlock basicBlock1(blockNo++, 0, localVar); // 开始语句
    basicBlockVec.push_back(basicBlock1); // 入口语句到第一个label
    for (int i = 1; i < (int) graphStates.size(); i++) {
        if (graphStates[i].type == Label) {
            labelToBlock.insert(pair<string, int>(graphStates[i].target, blockNo));
            BasicBlock basicBlock(blockNo++, i, localVar); // 构建一个基本快
            basicBlockVec.push_back(basicBlock); // 加入基本快向量中
        } else if (graphStates[i].type == BLE || graphStates[i].type == BLT || graphStates[i].type == BGE ||
                   graphStates[i].type == BGT || graphStates[i].type == BEQ || graphStates[i].type == BNE ||
                   graphStates[i].type == Return || graphStates[i].type == Jump) {
            if (i < graphStates.size() - 1 && graphStates[i + 1].type != Label) {
                BasicBlock basicBlock(blockNo++, i + 1, localVar); // 构建一个基本快
                i++; // 避免重复创建基本块
                basicBlockVec.push_back(basicBlock); // 加入基本快向量中
            } // 如果接在跳转语句之后的不是label才创建
        }
    }
    exitBlockNo = blockNo; // 设置出口基本块
    BasicBlock basicBlock(exitBlockNo, graphStates.size(), localVar); // 第三步: 加入一条出口语句
    basicBlockVec.push_back(basicBlock); // 加入一条出口
    // 第二步 :: 填补基本块内的语句
    for (int i = 0; i < (int) basicBlockVec.size() - 1; i++) {
        int start = basicBlockVec[i].getBlockStart(); // 也要把基本块的开始语句加入
        int end = basicBlockVec[i + 1].getBlockStart() - 1; // 本基本块的结束等于下一基本块开始的前一条语句
        basicBlockVec[i].setEnd(end);
        for (; start <= end; start++) {
            basicBlockVec[i].addState(graphStates[start]);
        }
    }
}

void FlowGraph::buildFLowGraph() {
    buildBlock(); // 建立流图之前划分基本块
    for (int i = 0; i < (int) basicBlockVec.size() - 1; i++) {
        int end = basicBlockVec[i].getEnd();
        FourComExpr look = graphStates[end];
        if (graphStates[end].type == Jump) {
            string label = graphStates[end].target; // 跳转到的目标
            int blockNo = labelToBlock.find(label)->second;
            flowGraph[i][blockNo] = 1;
        } else if (graphStates[end].type == Return) {
            // 直接到达出口
            flowGraph[i][exitBlockNo] = 1;
        } else if (graphStates[end].type == BLE || graphStates[end].type == BLT || graphStates[end].type == BGE ||
                   graphStates[end].type == BGT || graphStates[end].type == BEQ || graphStates[end].type == BNE) {
            string label = graphStates[end].target; // 跳转到的目标
            int blockNo = labelToBlock.find(label)->second;
            flowGraph[i][blockNo] = 1; // 指向跳转语句的后继
            flowGraph[i][i + 1] = 1; // 指向顺序执行的后继
        } else {
            flowGraph[i][i + 1] = 1; // 这是属于被label结束的情况
        }
    }
}

void FlowGraph::activeVarBuild() {
    while (true) {
        int cnt = 0; // 记录out集合被改变的次数
        for (int index = exitBlockNo; index >= 0; index--) {
            set<int> suffixSet = getSuffix(index); // 找到前驱
            basicBlockVec[index].genLiveIn(); // 计算一次当前基本块的in
            // 将suffixSet中的in集合加入到index所指集合的out中
            for (auto it:suffixSet) {
                cnt += addBlokcInToBlockOut(index, it); // 计算当前基本块的out集合
            }
        }
        // 如果所有out集合都没有改变，说明基本块的活跃变量已经没有改变
        if (cnt == 0) {
            break;
        }
    }
}

void FlowGraph::genUseDef() {
    for (auto &it : basicBlockVec) {
        it.genUseDef();
    }
}

set<int> FlowGraph::getSuffix(int blockNo) {
    set<int> suffix;
    for (int i = 0; i < exitBlockNo; i++) {
        if (flowGraph[i][blockNo] == 1) {
            suffix.insert(i); // 找到当前基本块的前驱
        }
    }
    return suffix;
}

bool FlowGraph::addBlokcInToBlockOut(int blockNo, int proBlock) {
    // 把blockno的in集合加入problock的out中
    set<string> proLiveOut = basicBlockVec[proBlock].getOutLiveSet(); // 得到集合的in集合
    set<string> blockIn = basicBlockVec[blockNo].getInLiveSet(); // 当前基本块的out集合
    bool isOutChange = false;
    // 把in集合加入当前基本块的out集合 :: 不断地更新out集合
    for (auto &it:blockIn) {
        if (proLiveOut.find(it) == proLiveOut.end()) { // 如果不在out集合中，才加入
            isOutChange = true;
            basicBlockVec[proBlock].addLiveOut(it);
        }
    }
    return isOutChange;
}

void FlowGraph::buildVarOrderMap() {
    for (auto &it : basicBlockVec) {
        for (auto &var : it.getInLiveSet()) {// 把活跃变量加入
            if (varNameToOrder.find(var) != varNameToOrder.end()) { // 已经在表中了
                continue;
            } else {
                varNameToOrder.insert(pair<string, int>(var, varSize));
                orderToVarname.insert(pair<int, string>(varSize, var));
                varSize++;
            }
        }
        for (auto &var : it.getDefSet()) { // 把基本块内定义的变量加入
            if (varNameToOrder.find(var) != varNameToOrder.end()) {
                continue;
            } else {
                varNameToOrder.insert(pair<string, int>(var, varSize));
                orderToVarname.insert(pair<int, string>(varSize, var));
                varSize++;
            }
        }
    }
}

void FlowGraph::buildVarConvGraph() {
    for (auto &it : basicBlockVec) {
        set<string> defLiveinSet; // 基本块的def和活跃变量集合
        defLiveinSet.insert(it.getDefSet().begin(), it.getDefSet().end());
        defLiveinSet.insert(it.getInLiveSet().begin(), it.getInLiveSet().end());
        addEdgeToMap(defLiveinSet); // 取得入口处的活跃变量
        // cout << "--------------------------" << endl;
    }
}

void FlowGraph::addEdgeToMap(set<string> &activeVarSet) {
    vector<int> orderVec; // 记录活跃变量的标号
    orderVec.reserve(activeVarSet.size()); // 分配活跃变量set的大小

    for (auto &it:activeVarSet) {
        orderVec.push_back(varNameToOrder.find(it)->second); // 标号
    }
    for (int i = 0; i < (int) orderVec.size(); i++) {
        for (int j = i + 1; j < (int) orderVec.size(); j++) {
            convUseMap[orderVec[i]][orderVec[j]] = 1;
            convUseMap[orderVec[j]][orderVec[i]] = 1;
            colorMap[orderVec[i]][orderVec[j]] = 1;
            colorMap[orderVec[j]][orderVec[i]] = 1;
        }
    }
}

void FlowGraph::disGlobalReg() { // 使用colorMap这个临时的辅助数组
    int remain = varSize; // 记录剩余的结点数量
    stack<int> recStack; // 记录结点被移走的顺序
    set<int> varMoved; // 被移走的变量
    if (remain == 0) return;
    while (remain > 1) {
        int moveVar = getDegLess8(varMoved); // 获取 一个度数小于8的结点
        if (moveVar != -1) { // 仍然可以移走结点
            remain--;
            recStack.push(moveVar); // 把这个结点移走 :: 加入栈中
            varMoved.insert(moveVar); // 加入到集合中 因为移走之后它的度会变成0
            moveNode(moveVar); // 移走该结点
        } else {
            // 把度数最大的结点移走 :: 并且标记不能为他分配全局寄存器
            moveVar = getDegMax(varMoved);
            remain--;
            recStack.push(moveVar);
            varMoved.insert(moveVar);
            moveNode(moveVar);
            globalFailVar.insert(moveVar); // 不能为该结点分配全局寄存器
        }
    }

    // 为最后剩下的变量分配全局寄存器
    int lastVar = disRegToLast(varMoved);
    varOrderToRegOrder.insert(pair<int, int>(lastVar, 0));
    // 为之前加入map的变量分配寄存器
    while (!recStack.empty()) {
        int varOrder = recStack.top(); // 当前需要考虑的变量
        if (globalFailVar.find(varOrder) != globalFailVar.end()) {
            recStack.pop(); // 要将这个不分配寄存器的变量直接弹出
            continue; // 如果被标记为不分配全局寄存器那么就不要分配
        }
        disRegToVar(varOrder); // 为这个变量分配寄存器
        recStack.pop(); // 弹出栈顶元素
    }
}

int FlowGraph::getDegMax(set<int> &movedVar) {
    vector<int> degOfNode; // 记录每个结点的度
    int maxValue = 0; // 记录最大值
    int maxIndex = 0; // 记录最大值对应的下标
    for (int i = 0; i < varSize; i++) {
        if (movedVar.find(i) != movedVar.end()) {
            degOfNode.push_back(0);
            continue; // 如果已经是被移除的结点，那么可以直接跳过
        }
        int deg = 0;
        for (int j = 0; j < varSize; j++) {
            if (movedVar.find(j) != movedVar.end()) {
                continue; // 如果已经是被移除的结点,不能计算度
            }
            deg += colorMap[i][j];
        }
        if (deg > maxValue) {
            maxValue = deg;
            maxIndex = i;
        }
        degOfNode.push_back(deg);
    }
    return maxIndex;
}


int FlowGraph::getDegLess8(set<int> &movedVar) {
    for (int i = 0; i < varSize; i++) {
        if (movedVar.find(i) != movedVar.end()) {
            continue; // 如果已经是被移除的结点，那么可以直接跳过
        }
        int deg = 0;
        for (int j = 0; j < varSize; j++) {
            deg += colorMap[i][j];
        }
        if (deg < 8) {
            return i;
        }
    }
    return -1; // 如果没找到度数小于8的那么就设置为-1
}

void FlowGraph::moveNode(int node) {
    for (int i = 0; i < varSize; i++) {
        // 去掉与其相关联的边
        colorMap[node][i] = 0;
        colorMap[i][node] = 0;
    }
}

int FlowGraph::disRegToLast(set<int> &movedVar) {
    for (int i = 0; i < varSize; i++) {
        if (movedVar.find(i) == movedVar.end()) {
            return i;
        }
    }
    return -1;
}

int FlowGraph::disRegToVar(int varOrder) {
    set<int> adjcentReg; // 邻居占用的寄存器
    for (int i = 0; i < varSize; i++) {
        if (convUseMap[varOrder][i] == 1 && varOrderToRegOrder.find(i) != varOrderToRegOrder.end()) {
            // 该邻居已经分配了寄存器
            adjcentReg.insert(varOrderToRegOrder.find(i)->second);
        }
    }
    for (int i = 0; i <= 8; i++) {
        if (adjcentReg.find(i) == adjcentReg.end()) {
            varOrderToRegOrder.insert(pair<int, int>(varOrder, i));
            return i;
        }
    }
    return -1;
}

void FlowGraph::convertOrderToVar() {
    for (auto &it:varOrderToRegOrder) {
        string varName = orderToVarname.find(it.first)->second;
        int regNo = it.second;
        string globalReg = "$s";
        globalReg.append(to_string(regNo));
        varToGlobalReg.insert(pair<string, string>(varName, globalReg));
    }
}

void FlowGraph::findLocalVar() { // 找到函数所有的局部变量
    for (const auto &it: graphStates) {
        if (it.type == VaribleDef) {
            localVar.insert(it.target); // 填充函数的局部变量
        }
    }
}

void FlowGraph::optimizeTmpCode() {
    for (auto it:graphStates) {
        if (it.type == ConstantDef || it.type == VaribleDef || it.type == ArrayDef) {
            optTmpCode.push_back(it); // 放到优化后的中间代码中
            continue; // 跳过常量定义和变量定义
        }
        if (varToGlobalReg.find(it.target) != varToGlobalReg.end()) {
            it.target = varToGlobalReg.find(it.target)->second;
        }
        if (varToGlobalReg.find(it.right) != varToGlobalReg.end()) {
            it.right = varToGlobalReg.find(it.right)->second;
        }
        if (varToGlobalReg.find(it.left) != varToGlobalReg.end()) {
            it.left = varToGlobalReg.find(it.left)->second;
        }

        if (it.type == FunctionCall) { // 处理函数的值参数表
            for (auto &it1 : it.valueParaTab) {
                if (varToGlobalReg.find(it1.name) != varToGlobalReg.end()) {
                    it1.name = varToGlobalReg.find(it1.name)->second;
                }
            }
        }

        if (varToGlobalReg.find(it.index) != varToGlobalReg.end()) {
            it.index = varToGlobalReg.find(it.index)->second;
        }

        if (varToGlobalReg.find(it.index1) != varToGlobalReg.end()) {
            it.index1 = varToGlobalReg.find(it.index1)->second;
        }

        if (varToGlobalReg.find(it.index2) != varToGlobalReg.end()) {
            it.index2 = varToGlobalReg.find(it.index2)->second;
        }

        optTmpCode.push_back(it);
    }
}

void FlowGraph::peepOptmize() {
    vector<FourComExpr> tmpOptCode(optTmpCode);
    optTmpCode.clear();
    for (int i = 0; i < (int) tmpOptCode.size(); i++) {
        if (i < tmpOptCode.size() - 1 && tmpOptCode[i + 1].type == AssignState && // 下一条语句是赋值语句
            tmpOptCode[i + 1].target[0] == '$' &&
            tmpOptCode[i + 1].target[1] == 's' && // 下一条语句的target是全局寄存器
            tmpOptCode[i].target == tmpOptCode[i + 1].right) { // 该语句的target等于赋值语句的right
            tmpOptCode[i].target = tmpOptCode[i + 1].target; // 把当前语句的target赋为下一条语句的target
            optTmpCode.push_back(tmpOptCode[i]); // 将表达式语句加入opt
            i++; // 去掉赋值语句
        } else {
            optTmpCode.push_back(tmpOptCode[i]);
        }
    }

}









