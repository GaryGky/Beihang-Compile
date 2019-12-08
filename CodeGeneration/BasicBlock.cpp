//
// Created by dell on 2019/11/30.
//
#include <algorithm>
#include "BasicBlock.h"


BasicBlock::BasicBlock(int blockNumber, int start) {
    this->blockNum = blockNumber;
    this->start = start;
}

void BasicBlock::addState(FourComExpr fourComExpr) {
    this->stateVec.push_back(fourComExpr);
}

void BasicBlock::genUse() {
    for (auto &it:stateVec) {
        if (it.type == AssignState) {
            if (it.right[0] != '$' && !isdigit(it.right[0])) {
                useSet.insert(it.right); // 不是数字也不是寄存器
            }
        } else if (it.type == Expression) {
            if (it.right[0] != '$' && !isdigit(it.right[0])) {
                useSet.insert(it.right); // 不是数字也不是寄存器
            }
            if (it.left[0] != '$' && !isdigit(it.left[0])) {
                useSet.insert(it.left);
            }
        }
    }
}

void BasicBlock::genDef() {
    for (auto &it : stateVec) {
        if (it.type == AssignState) {
            if (it.right[0] != '$' && !isdigit(it.right[0]) && useSet.find(it.right) == useSet.end()) {
                defSet.insert(it.right);
            }
        } else if (it.type == Expression) {
            if (it.right[0] != '$' && !isdigit(it.right[0]) && useSet.find(it.right) == useSet.end()) {
                defSet.insert(it.right);
            }
            if (it.left[0] != '$' && !isdigit(it.left[0]) && useSet.find(it.left) == useSet.end()) {
                defSet.insert(it.left);
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
    set_difference(outLiveSet.begin(), outDiffdef.end(), defSet.begin(), defSet.end(),
                   inserter(outDiffdef, outDiffdef.begin()));
    // 第二步 : in = use[B] + outDiffdef
    set_union(outDiffdef.begin(), outDiffdef.end(), useSet.begin(), useSet.end(),
              inserter(inLiveSet, inLiveSet.begin()));
}
