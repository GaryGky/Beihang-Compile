//
// Created by dell on 2019/11/30.
//
#include <algorithm>
#include "BasicBlock.h"


BasicBlock::BasicBlock(int blockNumber, int start, set<string> &localVar) {
    this->blockNum = blockNumber;
    this->start = start;
    this->localVar = localVar;
}

void BasicBlock::addState(const FourComExpr &fourComExpr) {
    this->stateVec.push_back(fourComExpr);
}

void BasicBlock::genUseDef() {
    // 右边加入use，左边加入def
    // 加入use之前检查def
    // 加入def之前检查use
    for (auto &it:stateVec) {
        if (it.type == AssignState) {
            if (localVar.find(it.right) != localVar.end() && defSet.find(it.right) == defSet.end()) {
                useSet.insert(it.right); // 不在def集合中的局部变量加入use集合
            }
            if (localVar.find(it.target) != localVar.end() && useSet.find(it.target) == useSet.end()) {
                defSet.insert(it.target); // 不在use集合中的局部变量加入def集合
            }
        } else if (it.type == Expression) {
            if (localVar.find(it.right) != localVar.end() && defSet.find(it.right) == defSet.end()) {
                useSet.insert(it.right);
            }
            if (localVar.find(it.left) != localVar.end() && defSet.find(it.left) == defSet.end()) {
                useSet.insert(it.left);
            }
            if (localVar.find(it.target) != localVar.end() && useSet.find(it.target) == useSet.end()) {
                defSet.insert(it.target);
            }
        } else if (it.type == PrintInt || it.type == PrintChar) { // 打印是一种use
            if (localVar.find(it.target) != localVar.end() && defSet.find(it.target) == defSet.end()) {
                useSet.insert(it.target);
            }
        } else if (it.type == ReadInt || it.type == ReadChar) { // 输入是一种def
            if (localVar.find(it.target) != localVar.end() && useSet.find(it.target) == useSet.end()) {
                defSet.insert(it.target);
            }
        } else if (it.type == BLE || it.type == BLT || it.type == BGE ||
                   it.type == BGT || it.type == BEQ || it.type == BNE) { // 跳转语句也是一种use
            if (localVar.find(it.left) != localVar.end() && defSet.find(it.left) == defSet.end()) {
                useSet.insert(it.left); // 第一个比较操作数
            }
            if (localVar.find(it.index1) != localVar.end() && defSet.find(it.index1) == defSet.end()) {
                useSet.insert(it.index1); // 第二个比较操作数
            }
        } else if (it.type == FunctionCall) { // 函数调用的参数是一种使用
            vector<ExpRet> valueTab = it.valueParaTab; // 参数表要加入useSet
            for (auto &it1:valueTab) {
                if (localVar.find(it1.name) != localVar.end() && defSet.find(it1.name) == defSet.end()) {
                    useSet.insert(it1.name); // 加到使用集合中
                }
            }
        }
    }
}

void BasicBlock::addLiveOut(const string &varName) {
    outLiveSet.insert(varName);
}

void BasicBlock::genLiveIn() {
    set<string> outDiffdef;
    // 第一步 : outDiffdef = out[B] - def[B]
    for (auto &it:outLiveSet) {
        if (defSet.find(it) == defSet.end()) {
            outDiffdef.insert(it);
        }
    }
    // 第二步 : in = use[B] + outDiffdef
    inLiveSet.insert(useSet.begin(), useSet.end());
    inLiveSet.insert(outDiffdef.begin(), outDiffdef.end());
}
