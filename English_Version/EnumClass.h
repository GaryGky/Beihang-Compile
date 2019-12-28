//
// Created by dell on 2019/12/14.
//

#ifndef CODEGENPROEJCT_ENUMCLASS_H
#define CODEGENPROEJCT_ENUMCLASS_H
enum ItemType {
    Constant,//常量
    Variable,//变量
    Function,//函数
    Parament,//参数
    NullItem, // 未知 :: 默认
};

enum RetType { // 返回值类型
    Integer,
    Character,
    Void,
    NullType, //未定义的类型
};
#endif //CODEGENPROEJCT_ENUMCLASS_H
