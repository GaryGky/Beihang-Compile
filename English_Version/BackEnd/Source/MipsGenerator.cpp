//
// Created by dell on 2019/11/11.
//

#include <iostream>
#include "../Header/MipsGenerator.h"

using namespace std;

MipsGenerator::MipsGenerator(vector<FourComExpr> &tmpCode, SymbolTabel &symbolTabel1) {
    this->tmpCodeVector = tmpCode; // 待处理
    symbolTabel = symbolTabel1;
    mipsFile.open("mips.txt", ios::out); // 输出的目标文件
    codeScope = "Global";
    localOffset = 0;
    regToPos.insert(pair<string, string>("$a0", "0"));
    regToPos.insert(pair<string, string>("$a1", "-4"));
    regToPos.insert(pair<string, string>("$a2", "-8"));
    regToPos.insert(pair<string, string>("$a3", "-12"));
    regToPos.insert(pair<string, string>("$t0", "-20"));
    regToPos.insert(pair<string, string>("$t1", "-24"));
    regToPos.insert(pair<string, string>("$t2", "-28"));
    regToPos.insert(pair<string, string>("$t3", "-32"));
    regToPos.insert(pair<string, string>("$t4", "-36"));
    regToPos.insert(pair<string, string>("$t5", "-40"));
    regToPos.insert(pair<string, string>("$t6", "-44"));
    regToPos.insert(pair<string, string>("$t7", "-48"));
    regToPos.insert(pair<string, string>("$s0", "-52"));
    regToPos.insert(pair<string, string>("$s1", "-56"));
    regToPos.insert(pair<string, string>("$s2", "-60"));
    regToPos.insert(pair<string, string>("$s3", "-64"));
    regToPos.insert(pair<string, string>("$s4", "-68"));
    regToPos.insert(pair<string, string>("$s5", "-72"));
    regToPos.insert(pair<string, string>("$s6", "-76"));
    regToPos.insert(pair<string, string>("$s7", "-80"));
}

void MipsGenerator::getNewLine() { // 换行
    mipsFile << "la $a0, newLine" << endl;
    mipsFile << "li $v0, 4" << endl;
    mipsFile << "syscall" << endl;
}

void MipsGenerator::genMipsCode() {
    genFuncDefReg(); // 构造每一个函数内部def的寄存器
    // genRecFunc(); // 判断递归函数
    genDataSegment();
    mipsFile << endl;
    genTextSegment();
}

void MipsGenerator::genDataSegment() {
    mipsFile << ".data" << endl;
    mipsFile << "space: " << ".asciiz " << "\" \"" << endl; // 产生空格
    mipsFile << "newLine: " << ".asciiz " << R"("\n")" << endl; // 产生换行符
    // 字符串
    for (auto &it : tmpCodeVector) {
        if (it.type == PrintStr && it.left != "#") {
            string str;
            int lineNo = 0;
            for (char i : it.left) {
                if (i == '\\') {
                    str += '\\';
                    lineNo++; // 反斜杠的数量+1
                }
                str += i;
            }
            mipsFile << it.target << ": " << ".asciiz " << "\"" << str << "\"" << endl; // 把字符串保存到data段
            strMemSize += it.left.size() + 1; // '\0' 占一个字符
        }
    }
    unsigned tmp = strMemSize;
    strMemSize = strMemSize + 4 - (strMemSize % 4); // 使字符串的大小对齐
    mipsFile << "takeUp: .space " << (strMemSize - tmp) % 4 << endl;
    // 数组
    for (auto it = tmpCodeVector.begin(); it != tmpCodeVector.end();) {
        if (it->type == ArrayDef && it->varScope == "Global") { // 全局数组分配空间
            int arraySize = stoi(it->left);
            mipsFile << it->target << ": " << ".space " << arraySize * 4 << endl; // int char 都按4处理
            it = tmpCodeVector.erase(it);
        } else {
            it++;
        }
    }
    // 全局变量
    for (auto it = tmpCodeVector.begin(); it != tmpCodeVector.end();) {
        if (it->type == VaribleDef && it->varScope == "Global") {
            mipsFile << it->target << ": .space 4" << endl; // 不论是char还是int统一赋值4字节
            it = tmpCodeVector.erase(it);
        } else {
            it++;
        }
    }
}

void MipsGenerator::genTextSegment() {
    string srcReg;
    string output;
    LocalVarible localVarible;
    mipsFile << ".text" << endl;
    mipsFile << "j main" << endl;
    // int i = 0;
    for (auto &it : tmpCodeVector) {
        switch (it.type) {
            case FunctionCall:
                genFuncRefCode(it);
                break;
            case AssignState:
                // 赋值语句
                genAssignCode(it);
                break;
            case Label:
                mipsFile << it.target << ":" << endl;
                break;
            case FunctionDef:
                codeScope = it.target; // 设置当前正在分析的代码域
                genFuncDefCode(it);
                break;
            case Expression:
                genExprCode(it);
                break;
            case Jump:
                mipsFile << "j " << it.target << endl; // 无条件跳转
                break;
            case BEQ: // 等于0时跳转
            case BNE: // 不等于0时跳转
            case BLE: // 小于等于0时跳转
            case BLT: // 小于0转移
            case BGT: // 大于0时跳转
            case BGE: // 大于等于0时跳转
                genBranchCode(it);
                break;
            case ReadChar: // 需要处理目的寄存器
                mipsFile << "li $v0,12" << endl;
                mipsFile << "syscall" << endl;
                if (nameToLocalVar.find(it.target) != nameToLocalVar.end()) {
                    // 局部变量读入
                    srcReg = getLocalAddr(it.target, nameToLocalVar.find(it.target)->second.localOffset, "$k0");
                    mipsFile << "sw $v0, 0(" << srcReg << ")" << endl; // 将读入的值写回
                } else if (symbolTabel.isParameter(codeScope, it.target)) {
                    // 对函数参数赋值
                    int paraOrder = symbolTabel.getParaOrder(codeScope, it.target);
                    if (paraOrder < 4) {
                        mipsFile << "move $a" << paraOrder << " $v0" << endl;
                    } else {
                        int offset = -(paraOrder - 3) * 4 - 84;
                        mipsFile << "addi $k0 $fp " << offset << endl; // 计算参数相对于fp的基地址
                        mipsFile << "sw $v0 0($k0)" << endl;
                    }
                } else if (symbolTabel.getItemScopeByName(it.target) == "Global") {
                    getGloabalAddr(it.target, "$k0"); // 加载赋值地址到$t0
                    mipsFile << "sw $v0, 0($k0)" << endl; // 将读入的值写回
                } else if (it.target[0] == '$') {
                    mipsFile << "move " << it.target << " $v0" << endl;
                }
                break;
            case ReadInt:
                mipsFile << "li $v0,5" << endl;
                mipsFile << "syscall" << endl;
                if (nameToLocalVar.find(it.target) != nameToLocalVar.end()) {
                    // 局部变量读入
                    srcReg = getLocalAddr(it.target, nameToLocalVar.find(it.target)->second.localOffset, "$k0");
                    mipsFile << "sw $v0, 0(" << srcReg << ")" << endl; // 将读入的值写回
                } else if (symbolTabel.isParameter(codeScope, it.target)) {
                    // 对函数参数赋值
                    int paraOrder = symbolTabel.getParaOrder(codeScope, it.target);
                    if (paraOrder < 4) {
                        mipsFile << "move $a" << paraOrder << " $v0" << endl;
                    } else {
                        int offset = -(paraOrder - 3) * 4 - 84;
                        mipsFile << "addi $k0 $fp " << offset << endl; // 计算参数相对于fp的基地址
                        mipsFile << "sw $v0 0($k0)" << endl;
                    }
                } else if (symbolTabel.getItemScopeByName(it.target) == "Global") {
                    getGloabalAddr(it.target, "$k0"); // 加载赋值地址到$t0
                    mipsFile << "sw $v0, 0($k0)" << endl; // 将读入的值写回
                } else if (it.target[0] == '$') {
                    mipsFile << "move " << it.target << " $v0" << endl;
                }
                break;
            case PrintStr:
                // mipsFile << "move $v1 $a0" << endl; // 用k0保护a0
                mipsFile << "la $a0, " << it.target << endl; // 加载字符串地址
                mipsFile << "li $v0, 4" << endl;
                mipsFile << "syscall" << endl;
                // mipsFile << "move $a0 $v1" << endl; // 恢复a0
                break;
            case PrintInt:
                // mipsFile << "move $v1 $a0" << endl; // 用k0保护a0
                srcReg = getEleValue(it.target, "$a0", "");
                if (srcReg != "$a0") {
                    if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') {
                        mipsFile << "li $a0 " << srcReg << endl;
                    } else {
                        mipsFile << "move $a0 " << srcReg << endl;
                    }
                }
                mipsFile << "li $v0, 1" << endl;
                mipsFile << "syscall" << endl;
                // mipsFile << "move $a0 $v1" << endl; // 恢复a0
                break;
            case PrintChar:
                // mipsFile << "move $v1 $a0" << endl; // 用k0保护a0
                srcReg = getEleValue(it.target, "$a0", "");
                if (srcReg != "$a0") mipsFile << "move $a0 " << srcReg << endl;
                mipsFile << "li $v0, 11" << endl;
                mipsFile << "syscall" << endl;
                // mipsFile << "move $a0 $v1" << endl; // 恢复a0
                break;
            case PrintNewLine:
                // mipsFile << "move $v1 $a0" << endl; // 用k0保护a0
                getNewLine(); // 换行
                // mipsFile << "move $a0 $v1" << endl; // 恢复a0
                break;
            case Return:
                genReturnCode(it);
                break;
            case VaribleDef:
                localVarible.varName = it.target;
                localVarible.varScope = it.varScope;
                localVarible.localOffset = localOffset; // 这里需要自增
                localOffset += 4; // 偏移增加
                localVarible.varType = it.valueType;
                localVarible.tmpCodeType = VaribleDef;
                nameToLocalVar.insert(pair<string, LocalVarible>(localVarible.varName, localVarible));
                break;
            case ArrayDef:
                localVarible.varName = it.target;
                localVarible.varScope = it.varScope;
                localVarible.varType = it.valueType;
                localVarible.localOffset = localOffset;
                localOffset += stoi(it.left) * 4; // 加上数组的大小，每个数组元素的大小为4
                localVarible.tmpCodeType = ArrayDef;
                nameToLocalVar.insert(pair<string, LocalVarible>(localVarible.varName, localVarible));
                break;
            case ConstantDef:
                localVarible.varName = it.target;
                localVarible.varScope = it.varScope;
                localVarible.varType = it.valueType;
                localVarible.tmpCodeType = ConstantDef;
                localVarible.constValue = it.left; // 常量值在left中
                nameToLocalVar.insert(pair<string, LocalVarible>(localVarible.varName, localVarible));
                break;
            default:
                break;
        }
    }
}

void MipsGenerator::genAssignCode(FourComExpr &fourComExpr) {
    // 第一步::确定赋值语句的右边 --- 放在$k1 中 :: 赋值语句中右边只有一个元素
    string srcReg = getEleValue(fourComExpr.right, "$k1", fourComExpr.index2); // 取值操作
    string dstAssign; // 要赋值的目标
    // 第二步::确定赋值语句的目标地址 --- 放在$k0中
    if (fourComExpr.target.at(0) == '$') {
        // 直接对寄存器赋值
        if (srcReg[0] == '$') {
            mipsFile << "move " << fourComExpr.target << " " << srcReg << endl;
        } else {
            mipsFile << "li " << fourComExpr.target << " " << srcReg << endl;
        }
    } else if (nameToLocalVar.find(fourComExpr.target) != nameToLocalVar.end()) { // 局部
        if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') { // 对数字进行特殊处理
            mipsFile << "li $k1 " << srcReg << endl;
            srcReg = "$k1";
        }
        if (fourComExpr.arrayOrVar == 0) { // 局部数组元素赋值
            getArrayAddr(fourComExpr.target, fourComExpr.index, "$k0"); // 获取数组地址
            mipsFile << "sw " << srcReg << " ,0($k0)" << endl; // 将变量的值写回存储器
        } else { //局部变量赋值
            getLocalAddr(fourComExpr.target, nameToLocalVar.find(fourComExpr.target)->second.localOffset, "$k0");
            mipsFile << "sw " << srcReg << " ,0($t8)" << endl; // 将变量的值写回存储器
        }
    } else if (symbolTabel.isParameter(codeScope, fourComExpr.target)) { // 对函数参数赋值
        int paraOrder = symbolTabel.getParaOrder(codeScope, fourComExpr.target);
        if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') { // 对数字进行特殊处理
            mipsFile << "li $k1 " << srcReg << endl;
            srcReg = "$k1";
        }
        if (paraOrder < 3) {
            mipsFile << "move $a" << paraOrder + 1 << " " << srcReg << endl;
        } else {
            int offset = -(paraOrder - 3) * 4 - 84;
            mipsFile << "addi $k0 $fp " << offset << endl; // 计算参数相对于fp的基地址
            mipsFile << "sw " << srcReg << " 0($k0)" << endl;
        }
    } else if (symbolTabel.getItemScopeByName(fourComExpr.target) == "Global") { // 全局
        if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') { // 对数字进行特殊处理
            mipsFile << "li $k1 " << srcReg << endl;
            srcReg = "$k1";
        }
        if (fourComExpr.arrayOrVar == 0) { // 全局数组赋值
            getArrayAddr(fourComExpr.target, fourComExpr.index, "$k0");
        } else { // 全局变量赋值
            getGloabalAddr(fourComExpr.target, "$k0");
        }
        mipsFile << "sw " << srcReg << " 0($k0)" << endl; // 将变量的值写回存储器
    }
}

void MipsGenerator::genExprCode(FourComExpr &fourComExpr) {
    // 第1步::确定表达式的右边第一项 --- 放在k0中
    string srcReg1 = getEleValue(fourComExpr.left, "$k0", fourComExpr.index1);
    // 第2步::确定表达式右边的第二项 --- 放在 $k1中
    string srcReg2 = getEleValue(fourComExpr.right, "$k1", fourComExpr.index2);
    // 第3步 :: 根据目标值进行计算
    if (isdigit(srcReg1[0]) || srcReg1[0] == '+' || srcReg1[0] == '-') {
        mipsFile << "li $k0 " << srcReg1 << endl;
        srcReg1 = "$k0";
    }
    string dstReg;
    dstReg = fourComExpr.target;
    if (fourComExpr.op == "+") {
        if (isdigit(srcReg2[0]) || srcReg2[0] == '+' || srcReg2[0] == '-') {
            mipsFile << "addi " << dstReg << " " << srcReg1 << " " << srcReg2 << endl;
        } else {
            mipsFile << "add " << dstReg << " " << srcReg1 << " " << srcReg2 << endl;
        }
    } else if (fourComExpr.op == "-") {
        if (srcReg2[0] == '+') {
            mipsFile << "addi " << dstReg << " " << srcReg1 << " -" << srcReg2.substr(1) << endl;
        } else if (isdigit(srcReg2[0])) {
            mipsFile << "addi " << dstReg << " " << srcReg1 << " -" << srcReg2 << endl;
        } else if (srcReg2[0] == '-') {
            mipsFile << "addi " << dstReg << " " << srcReg1 << " " << srcReg2.substr(1) << endl;
        } else {
            mipsFile << "sub " << dstReg << " " << srcReg1 << " " << srcReg2 << endl;
        }
    } else if (fourComExpr.op == "*") {
        if (isdigit(srcReg2[0]) || srcReg2[0] == '+' || srcReg2[0] == '-') {
            mipsFile << "li $k1 " << srcReg2 << endl;
            mipsFile << "mult " << srcReg1 << " $k1" << endl;
        } else {
            mipsFile << "mult " << srcReg1 << " " << srcReg2 << endl;
        }
        mipsFile << "mflo " << dstReg << endl;
    } else if (fourComExpr.op == "/") {
        if (isdigit(srcReg2[0]) || srcReg2[0] == '+' || srcReg2[0] == '-') {
            mipsFile << "li $k1 " << srcReg2 << endl;
            mipsFile << "div " << srcReg1 << " $k1" << endl;
        } else {
            mipsFile << "div " << srcReg1 << " " << srcReg2 << endl;
        }
        mipsFile << "mflo " << dstReg << endl;
    }
}

void MipsGenerator::genFuncDefCode(FourComExpr &fourComExpr) {
    localOffset = 84; // 将局部变量的偏移戳复位
    nameToLocalVar.clear(); // 清除
    codeScope = fourComExpr.target;
    mipsFile << fourComExpr.target << ":" << endl; // 产生函数的label
    initialStack(codeScope);  // !! main一定要存一下参数，否则第二层马上出错，但是main结束不用退栈
}


void MipsGenerator::keepScene(set<string> &funcDefSet, string &targetFunc) { // 保存现场 :: 函数调用之前
    /*// 保存函数的参数 --- 用到哪些就得保存哪些
    // 保存函数的参数 --- 用到哪些就得保存哪些
    mipsFile << "sw $a0, 0($fp)" << endl;
    mipsFile << "sw $a1, -4($fp)" << endl;
    mipsFile << "sw $a2, -8($fp)" << endl;
    mipsFile << "sw $a3, -12($fp)" << endl;
    // 保存函数的返回值
    // mipsFile << "sw $ra, -16($fp)" << endl;
    mipsFile << "sw $t0, -20($fp)" << endl;
    mipsFile << "sw $t1, -24($fp)" << endl;
    mipsFile << "sw $t2, -28($fp)" << endl;
    mipsFile << "sw $t3, -32($fp)" << endl;
    mipsFile << "sw $t4, -36($fp)" << endl;
    mipsFile << "sw $t5, -40($fp)" << endl;
    mipsFile << "sw $t6, -44($fp)" << endl;
    mipsFile << "sw $t7, -48($fp)" << endl;*/
    for (auto &it:funcDefSet) {
        mipsFile << "sw " << it << ", " << regToPos.find(it)->second << "($fp)" << endl;
    } // 保存函数用到的寄存器
    /*if (recFuncToDefstate.find(targetFunc) != recFuncToDefstate.end()) { // 保存a类寄存器
        set<string> paraReg = recFuncToDefstate.find(targetFunc)->second;
        for (auto &it : paraReg) {
            mipsFile << "sw " << it << ", " << regToPos.find(it)->second << "($fp)" << endl;
        }
    }*/
    // 保险起见，保存a类寄存器
    mipsFile << "sw $a1, -4($fp)" << endl;
    mipsFile << "sw $a2, -8($fp)" << endl;
    mipsFile << "sw $a3, -12($fp)" << endl;
}


void MipsGenerator::initialStack(const string &funcName) {
    // 保存现场
    mipsFile << "sw $fp, 0($sp)" << endl;  // 保存上一级函数的起始地点
    mipsFile << "addi $sp,$sp,-4" << endl; // 栈指针向下移动
    mipsFile << "move $fp,$sp" << endl; // 记录函数的开始栈点
    mipsFile << "addi $sp,$sp,-80" << endl; // 移动栈指针 --- 保存现场需要 84个字节的栈空间
    vector<SymbolItem> paraVec;
    if (funcName != "main") {
        paraVec = symbolTabel.getSymbol(funcName).getParaVec(); // 得到函数的参数表
    }
    int funcMemSize = 0; // 多余的函数参数所需空间 :: 多余的参数需要保存空间
    if (paraVec.size() > 3) { // 我的main函数是没有压栈的
        funcMemSize += ((int) paraVec.size() - 3) * 4;
    }
    localOffset += funcMemSize; // 局部变量定义之前，加上多余的参数所需空间
    // 计算函数局部变量+数组需要的空间
    for (auto &it : tmpCodeVector) {
        if (it.type == VaribleDef && it.varScope == funcName) {
            funcMemSize += 4; // 不管是char还是int都按照4字节增长
        } else if (it.type == ArrayDef && it.varScope == funcName) {
            funcMemSize += stoi(it.left) * 4; // 数组元素占位4
        }
    }
    if (funcMemSize > 0) {
        mipsFile << "addi $sp, $sp, " << "-" << funcMemSize << endl; // 分配局部变量空间
    }
    mipsFile << "addi $sp,$sp,-4" << endl; // 应该多-4 :: 移动到下一个空间
    mipsFile << "sw $ra, -16($fp)" << endl; // 这句很重要 进入函数时需要保存该层的ra
}

void MipsGenerator::restoreScene(set<string> &funcDefReg, string &function) {
    // 2. 恢复sp寄存器到上一级函数的结尾(也就是该层函数的开始)
    // 3. 恢复寄存器 --- 保存哪些就得恢复哪些
    /*mipsFile << "lw $ra,-16($fp)" << endl; // ra使用的是当前层 :: 不需要load
    mipsFile << "addi $sp,$fp,4" << endl; // 恢复栈指针 --- 这个地址上存放着上一层函数的起点
    mipsFile << "lw $fp, 4($fp)" << endl; // 把函数运行栈的寄存器恢复到上一层函数的开始位置
    // 依次将函数参数恢复
    mipsFile << "lw $a0, 0($fp)" << endl;
    mipsFile << "lw $a1, -4($fp)" << endl;
    mipsFile << "lw $a2, -8($fp)" << endl;
    mipsFile << "lw $a3, -12($fp)" << endl;
    mipsFile << "lw $t0, -20($fp)" << endl;
    mipsFile << "lw $t1, -24($fp)" << endl;
    mipsFile << "lw $t2, -28($fp)" << endl;
    mipsFile << "lw $t3, -32($fp)" << endl;
    mipsFile << "lw $t4, -36($fp)" << endl;
    mipsFile << "lw $t5, -40($fp)" << endl;
    mipsFile << "lw $t6, -44($fp)" << endl;
    mipsFile << "lw $t7, -48($fp)" << endl;
    // 4. 函数返回值点
    mipsFile << "jr $ra" << endl;*/
    // 2. 恢复sp寄存器到上一级函数的结尾(也就是该层函数的开始)
    // 3. 恢复寄存器 --- 保存哪些就得恢复哪些
    mipsFile << "lw $ra,-16($fp)" << endl; // ra使用的是当前层 :: 不需要load
    mipsFile << "addi $sp,$fp,4" << endl; // 恢复栈指针 --- 这个地址上存放着上一层函数的起点
    mipsFile << "lw $fp, 4($fp)" << endl; // 把函数运行栈的寄存器恢复到上一层函数的开始位置
    // 依次将函数参数恢复
    for (auto &it:funcDefReg) {
        mipsFile << "lw " << it << ", " << regToPos.find(it)->second << "($fp)" << endl;
    }
    // 保险起见，保存a类寄存器
    mipsFile << "lw $a1, -4($fp)" << endl;
    mipsFile << "lw $a2, -8($fp)" << endl;
    mipsFile << "lw $a3, -12($fp)" << endl;
    // 4. 函数返回值点
    mipsFile << "jr $ra" << endl;
}

void MipsGenerator::genReturnCode(FourComExpr &fourComExpr) {
    // 1. 产生函数的返回值 -- 保存在v0中(mips的约定)
    if (codeScope == "main") {
        mipsFile << "li $v0, 10" << endl;
        mipsFile << "syscall" << endl;
        return;
    } else if (symbolTabel.getRetTypeByName(codeScope) == Void) {
        // void 类型无返回值
        // 不用做处理
    } else {
        string srcReg = getEleValue(fourComExpr.target, "$v0", fourComExpr.index);
        if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') {
            mipsFile << "li $v0 " << srcReg << endl;
        } else if (srcReg != "$v0") mipsFile << "move $v0 " << srcReg << endl;
    }
    restoreScene(funcToDef.find(fourComExpr.varScope)->second, fourComExpr.varScope); // 恢复现场 :: 不能内联的函数才需要恢复现场
}

void MipsGenerator::genFuncRefCode(FourComExpr &fourComExpr) {
    // 1. 进行现场保存
    keepScene(funcToDef.find(fourComExpr.target)->second, fourComExpr.target); // 函数调用之前进行现场保存
    // 2. 产生函数调用的参数 :: 保存参数
    int index = 1;
    // 应该从函数的取保存过的参数
    for (auto &it : fourComExpr.valueParaTab) {  // 先把参数保存到a0~a3中
        if (index < 4) {
            string dstReg = "$a" + to_string(index); // 产生目标寄存器
            if (symbolTabel.isParameter(codeScope, it.name) && symbolTabel.getParaOrder(codeScope, it.name) < 3) {
                int order = symbolTabel.getParaOrder(codeScope, it.name) + 1;
                getRecurPara(order, dstReg); // 要从当前的fp取 a1-a3
            } else {
                string srcReg = getEleValue(it.name, dstReg, it.index);// 解析值参数表 :: 先把值暂时存放到 函数参数寄存器 offset为空
                if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') {
                    mipsFile << "li " << dstReg << " " << srcReg << endl;
                } else if (srcReg != dstReg) mipsFile << "move " << dstReg << " " << srcReg << endl;
            }
        } else {
            string srcReg;
            if (symbolTabel.isParameter(codeScope, it.name) &&
                symbolTabel.getParaOrder(codeScope, it.name) < 3) { // 如果是当前函数的参数 :: 从fp活动区上取
                int order = symbolTabel.getParaOrder(codeScope, it.name) + 1; // 得到参数的序号
                getRecurPara(order, "$k0"); //从fp中取参数放到k0中
                srcReg = "$k0";
            } else { // 如果不是当前函数的参数 :: 可以get它的值
                srcReg = getEleValue(it.name, "$k0", it.index); // 保存到$k0中
                if (srcReg[0] == '+' || srcReg[0] == '-' || isdigit(srcReg[0])) {
                    mipsFile << "li $k0 " << srcReg << endl;
                    srcReg = "$k0";
                } // get参数的值
            }
            // 往寄存器空间之上存
            int offset = (index - 3) * 4;
            offset = 0 - offset - 84; // 这里要保存到即将进入的栈空间当中
            mipsFile << "addi $t8 $sp " << offset << endl; // $t8中保存着该参数的地址
            mipsFile << "sw " << srcReg << " , 0($t8)" << endl; // 把参数地址存入栈空间中
        }
        index++;
    }
    // 判断函数是否可以内联，如果可以内联，把函数的代码拷贝到这里
    // 然后jal
    mipsFile << "jal " << fourComExpr.target << endl; // 函数调用
    if (symbolTabel.getRetTypeByName(fourComExpr.target) != Void) {
        mipsFile << "move " << fourComExpr.left << ", $v0" << endl; // 得到函数的返回值
    }
}

string MipsGenerator::getGloabalAddr(const string &varName, const string &dstReg) {
    // 获取全局变量的地址
    mipsFile << "la " << dstReg << ", " << varName << endl;
    return dstReg;
}

string MipsGenerator::getLocalAddr(const string &varName, int offset, const string &dstReg) {
    mipsFile << "addi $t8, $fp " << '-' << offset << endl;  // 计算局部变量的偏移
    // mipsFile << "lw  $t8, " << "0($t8)" << endl;
    // 取得局部变量的地址
    return "$t8";
}

string MipsGenerator::getTempReg(const string &tempReg, const string &dstReg) {
    return tempReg;
}

void MipsGenerator::getArrayValue(const string &arrayName, const string &offset, const string &dstReg) {
    string srcReg = getEleValue(offset, "$t9", ""); // 加载数组元素相对于数组基地址的偏移
    if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') {
        mipsFile << "li $t9 " << srcReg << endl;
        mipsFile << "sll $t9 $t9" << " 2" << endl;
    } else {
        mipsFile << "sll $t9 " << srcReg << " 2" << endl;
    }
    if (nameToLocalVar.find(arrayName) != nameToLocalVar.end()) {
        // 局部数组的处理
        int arrayBase = nameToLocalVar.find(arrayName)->second.localOffset; // 数组基地址相对于$fp的偏移
        mipsFile << "addi $t8 " << "$fp " << '-' << arrayBase << endl; // 注意 :: 是加负数
        mipsFile << "sub " << dstReg << " $t8 $t9" << endl; // 得到数组元素所在的地址
    } else {
        mipsFile << "la $t8 " << arrayName << endl; // 全局数组的基地址
        mipsFile << "add " << dstReg << " $t8 $t9" << endl; // 全局数组的偏移
    }
    mipsFile << "lw " << dstReg << ", 0(" << dstReg << ")" << endl; // 将数组的地址加载到dstReg
}

void MipsGenerator::getArrayAddr(const string &arrayName, const string &offset, const string &dstReg) {
    string srcReg = getEleValue(offset, "$t9", ""); // 加载数组元素相对于数组基地址的偏移
    if (isdigit(srcReg[0]) || srcReg[0] == '+' || srcReg[0] == '-') {
        mipsFile << "li $t9 " << srcReg << endl;
        mipsFile << "sll $t9 $t9" << " 2" << endl;
    } else {
        mipsFile << "sll $t9 " << srcReg << " 2" << endl;
    }
    if (nameToLocalVar.find(arrayName) != nameToLocalVar.end()) {
        // 局部数组的处理
        int arrayBase = nameToLocalVar.find(arrayName)->second.localOffset; // 数组基地址相对于$fp的偏移
        mipsFile << "addi $t8 " << "$fp " << '-' << arrayBase << endl;
        mipsFile << "sub " << dstReg << " $t8 $t9" << endl; // 得到数组元素所在的地址
    } else {
        mipsFile << "la $t8 " << arrayName << endl; // 全局数组的基地址
        mipsFile << "add " << dstReg << " $t8 $t9" << endl; // 全局数组的偏移
    }
}

string MipsGenerator::getParaValue(int paraOrder, const string &dstReg) {
    if (paraOrder < 3) {
        string srcReg = "$a" + to_string(paraOrder + 1); // 保存参数的寄存器
        return srcReg;
    } else {
        int offset = -(paraOrder - 3) * 4 - 84;
        mipsFile << "addi $t8 $fp " << offset << endl; // 计算参数相对于fp的基地址
        mipsFile << "lw " << dstReg << " 0($t8)" << endl;
        return dstReg;
    }
}

string MipsGenerator::getEleValue(const string &target, const string &dstReg, const string &offset) {
    if (nameToLocalVar.find(target) != nameToLocalVar.end()) { // 局部的
        if (nameToLocalVar.find(target)->second.tmpCodeType == ConstantDef) {
            if (nameToLocalVar.find(target)->second.varType == Integer) {
                mipsFile << "li " << dstReg + " " << nameToLocalVar.find(target)->second.constValue << endl;
            } else {
                mipsFile << "li " << dstReg << " \'" << nameToLocalVar.find(target)->second.constValue << "\'" << endl;
            }
        } else if (nameToLocalVar.find(target)->second.tmpCodeType == ArrayDef) {
            getArrayValue(target, offset, dstReg); // 按照数组的方法寻找
        } else {
            string srcReg = getLocalAddr(target, nameToLocalVar.find(target)->second.localOffset,
                                         dstReg); // 函数内部的局部变量
            if (srcReg[0] == '$' && srcReg[1] == 's') {
                // mipsFile << "move " << dstReg << " " << srcReg << endl;
                return srcReg;
            } else {
                mipsFile << "lw " << dstReg << ", 0(" << srcReg << ")" << endl;
            }
        }
    } else if (target[0] == '$') {
        return getTempReg(target, dstReg); // 属于临时寄存器
    } else if (symbolTabel.isParameter(codeScope, target)) {
        return getParaValue(symbolTabel.getParaOrder(codeScope, target), dstReg); // 函数参数
    } else if (symbolTabel.getItemScopeByName(target) == "Global") {
        if (symbolTabel.getItemtypeByName(target) == Constant) { // 全局的常量
            if (symbolTabel.getRetTypeByName(target) == Integer) {
                mipsFile << "li " << dstReg + " " << symbolTabel.getIntValueByName(target) << endl;
            } else {
                mipsFile << "li " << dstReg << " \'" << symbolTabel.getCharValueByName(target) << "\'" << endl;
            }
        } else {
            if (symbolTabel.getIntValueByName(target) > 0) {
                getArrayValue(target, offset, dstReg); // 全局数组
            } else {
                getGloabalAddr(target, dstReg); // 全局变量
                mipsFile << "lw " << dstReg << ", 0(" << dstReg << ")" << endl;
            }
        }
    } else {
        // mipsFile << "li " << dstReg << ", " << target << endl; // 属于字母或数字 -- 直接赋值
        if (target[0] == '\'') {
            mipsFile << "li " << dstReg << ", " << target << endl; // 字符得赋值
        } else {
            return target; // 数字直接返回
        }
    }
    return dstReg;
}

void MipsGenerator::genBranchCode(FourComExpr &fourComExpr) {
    // ex: ble $k0, -1, Label
    // bgt $k0, 1, Label
    // bgt $k0,$k1,Label
    string output;
    output.append(fourComExpr.right + " ");
    string srcReg1 = getEleValue(fourComExpr.left, "$k0", fourComExpr.index);
    if (srcReg1[0] != '$') { // 如果是字面量，要保存到寄存器中才能比较
        mipsFile << "li $k0 " << srcReg1 << endl;
        srcReg1 = "$k0";
    }
    string srcReg2 = getEleValue(fourComExpr.index1, "$k1", fourComExpr.index);
    output.append(srcReg1 + ", "); // 记得跳转语句这个地方只能是寄存器
    output.append(srcReg2 + ", ");
    output.append(fourComExpr.target);
    mipsFile << output << endl; // 输出
}

void MipsGenerator::genFuncDefReg() {
    for (int i = 0; i < (int) tmpCodeVector.size(); i++) {
        if (tmpCodeVector[i].type == FunctionDef) {
            string funcName = tmpCodeVector[i].target; // 函数名称
            set<string> funcDefSet;
            // cout << tmpCodeVector[i].varScope << endl;
            while (i < tmpCodeVector.size() && tmpCodeVector[i].varScope == funcName) {
                if (tmpCodeVector[i].type == AssignState && tmpCodeVector[i].target[0] == '$') {
                    if (tmpCodeVector[i].target != "$v1") funcDefSet.insert(tmpCodeVector[i].target);
                } else if (tmpCodeVector[i].type == Expression && tmpCodeVector[i].target[0] == '$') {
                    if (tmpCodeVector[i].target != "$v1") funcDefSet.insert(tmpCodeVector[i].target);
                } else if ((tmpCodeVector[i].type == ReadChar || tmpCodeVector[i].type == ReadInt) &&
                           tmpCodeVector[i].target[0] == '$') {
                    if (tmpCodeVector[i].target != "$v1") funcDefSet.insert(tmpCodeVector[i].target);
                } else if (tmpCodeVector[i].type == FunctionCall && tmpCodeVector[i].left[0] == '$') {
                    if (tmpCodeVector[i].left != "$v1") funcDefSet.insert(tmpCodeVector[i].left);
                }
                i++;
            }
            i--; // 不然会把下一个函数定义吃掉的
            funcToDef.insert(pair<string, set<string>>(funcName, funcDefSet));
        }
    }
}

void MipsGenerator::genRecFunc() { // 分析递归函数
    for (auto it = tmpCodeVector.begin(); it != tmpCodeVector.end();) {
        if (it->type == FunctionDef) {
            string funcName = it->varScope;
            vector<SymbolItem> paraTab = it->paraSet; // 获得该函数的参数表
            set<string> funcPara; // 函数的参数
            while (it != tmpCodeVector.end() && it->varScope == funcName) {
                it++;
                if (it->type == FunctionCall && it->target == funcName) { // 递归调用
                    if (!paraTab.empty()) {
                        funcPara.insert("$a1");
                    }
                    if (paraTab.size() >= 2) {
                        funcPara.insert("$a2");
                    }
                    if (paraTab.size() >= 3) {
                        funcPara.insert("$a3");
                    }
                    recFuncToDefstate.insert(pair<string, set<string>>(funcName, funcPara));
                    break; // 判断为递归函数就可跳过
                }
            }
        } else {
            it++;
        }
    }
}

string MipsGenerator::getRecurPara(int paraOrder, const string &dstReg) {
    paraOrder *= -4; // * -4
    mipsFile << "lw " << dstReg << " " << paraOrder << "($fp)" << endl;
    return "";
}






