//
// Created by dell on 2019/10/17.
//

#include <iostream>
#include "LexicalParser.h"
#include "SymbolItem.h"

using namespace std;

SymbolItem::SymbolItem(ItemType itemType1, string &name1, int level1, RetType retType1, string &value1,
                       const vector<SymbolItem> &paraset, long arraySize1, int paraNum1) {
    this->itemType = itemType1;
    this->retType = retType1;
    this->name = name1;
    this->level = level1;
    this->value = value1;
    this->funcParaVec = paraset;
    this->arraySize = arraySize1;
    this->paraNum = paraNum1;
}
