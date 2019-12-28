//
// Created by dell on 2019/11/30.
//

#include "../Header/Optimizer.h"

Optimizer::Optimizer(vector<FourComExpr> &tmpCode) {
    this->originTmpCode = tmpCode;
    optTmpFile.open("optTmpFile.txt", ios::out);
}

void Optimizer::divideByFunc() {
    for (int i = 0; i < (int) originTmpCode.size(); i++) {
        if (originTmpCode[i].type == FunctionDef) {
            string funcName = originTmpCode[i].target; // 函数名称
            funcVec.push_back(originTmpCode[i].target); // 函数定义
            vector<FourComExpr> funcStates; // 函数内部的语句
            // 获得函数内部的序列
            while (i < originTmpCode.size() && originTmpCode[i].varScope == funcName) {
                funcStates.push_back(originTmpCode[i]);
                i++;
            }
            i--; // 此处减一是为了不让for语句的i++把下一个函数定义吃掉
            FlowGraph flowGraph(funcName); // 构建一个流图
            flowGraph.addBlock(funcStates); // 向流图中添加语句
            graphVec.push_back(flowGraph); // 向流图向量中加入流图
        }
    }
}

void Optimizer::optimize() {
    // orientCodeOpt();
    divideByFunc(); // 可以
    for (auto &it: graphVec) {
        it.optStart(); // 控制每一个流图开始优化
    }
    genOptCode(); // 将每个流图的优化代码加入代码生成器
    genOptiTmpCode();// 生成优化后的中间代码
}

void Optimizer::genOptCode() {
    for (auto &it :originTmpCode) { // 将全局变量和全局常量加入
        if (it.varScope == "Global" && it.type != PrintStr) {
            optTmpCode.push_back(it);
        }
    }
    for (auto it:graphVec) {
        vector<FourComExpr> tmp = it.getOptCode();
        optTmpCode.insert(optTmpCode.end(), it.getOptCode().begin(), it.getOptCode().end());
    }
}


void Optimizer::genOptiTmpCode() {
    // 生成中间代码
    string output; // 输出内容
    for (auto &it : optTmpCode) {
        switch (it.type) {
            case AssignState: // 赋值语句
                if (it.arrayOrVar == 0) {
                    output.append(it.target + "[" + it.index + "]"); // 标识符
                } else {
                    output.append(it.target);
                }
                output.append(it.op + " "); // =
                output.append(it.right); // 表达式的值
                optTmpFile << output << endl;
                break;
            case Label: // 标签
                output.append(it.target);// 标签输出
                output.append(":"); // 添加冒号
                optTmpFile << output << endl;
                break;
            case VaribleDef: // 变量定义
                output.append("var "); // 添加变量定义说明
                // 添加标识符类型
                if (it.valueType == Integer) {
                    output.append("int ");
                } else {
                    output.append("char ");
                }
                output.append(it.target); // 标识符名称
                optTmpFile << output << endl;
                break;
            case ArrayDef: // 数组定义 array int a[10];
                output.append("array ");
                if (it.valueType == Integer) {
                    output.append("int ");
                } else {
                    output.append("char ");
                }
                output.append(it.target); // 标识符名称
                output.append("[" + it.left + "]"); // 数组的大小
                optTmpFile << output << endl;
                break;
            case ConstantDef: // 常量定义
                output.append("const ");
                if (it.valueType == Integer) {
                    output.append("int ");
                } else {
                    output.append("char ");
                }
                output.append(it.target);
                output.append("= " + it.left);
                optTmpFile << output << endl;
                break;
            case FunctionDef: // 函数定义
                switch (it.valueType) {
                    case Void:
                        output.append("Void ");
                        break;
                    case Integer:
                        output.append("int ");
                        break;
                    case Character:
                        output.append("char ");
                        break;
                    case NullType:
                        break;
                }
                output.append(it.target + " () "); // 加入函数名称
                optTmpFile << output << endl;
                output.clear();
                for (auto &it1 : it.paraSet) {
                    output.append("para ");
                    switch (it1.getRetType()) {
                        case Integer:
                            output.append("int ");
                            break;
                        case Character:
                            output.append("char ");
                            break;
                        case Void:
                            // 不会出现void类型的参数
                            break;
                        case NullType:
                            break;
                    }
                    output.append(it1.getName());
                    optTmpFile << output << endl;
                    output.clear();
                }
                break;
            case Expression: // 表达式解析
                if (it.index1.length() > 0) {
                    output.append(it.target + " = " + it.left + "[" + it.index1 + "]");
                } else {
                    output.append(it.target + " = " + it.left + " ");
                }
                output.append(" " + it.op + " "); // 运算符
                if (it.index2.length() > 0) {
                    output.append(it.right + "[" + it.index2 + "] ");
                } else {
                    output.append(it.right);
                }
                optTmpFile << output << endl;
                break;
            case Jump: // 无条件跳转
                output.append("Goto ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BEQ: // 等于
                output.append("BEQ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.index1 + ", ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BNE: // 不等于
                output.append("BNE "); // Conditon
                output.append(it.left + ", ");
                output.append(it.index1 + ", ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BLT: // 小于
                output.append("BLT "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.index1 + ", ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BLE: // 小于等于
                output.append("BLE "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.index1 + ", ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BGT: // 大于
                output.append("BGT "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.index1 + ", ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BGE: // 大于等于
                output.append("BGE "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.index1 + ", ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case ReadChar: // 读入一个字符
                output.append("Rear Char ");
                output.append(it.target);
                optTmpFile << output << endl;
                break;
            case ReadInt: // 读入一个整数
                output.append("Read Int ");
                output.append(it.target);
                optTmpFile << output << endl;
                break;
            case PrintStr: // 打印字符串
                output.append("Print String ");
                output.append(it.target);
                optTmpFile << output << endl;
                break;
            case PrintInt: // 打印整型
                output.append("Print Int ");
                output.append(it.target);
                optTmpFile << output << endl;
                break;
            case PrintChar: // 打印字符
                output.append("Print Char ");
                output.append(it.target);
                optTmpFile << output << endl;
                break;
            case Return: // 返回语句
                if (it.valueType == Void || it.valueType == NullType) {
                    optTmpFile << "RET" << endl; // 直接返回
                    break;
                } else {
                    output.append("RET ");
                    output.append(it.target);
                    optTmpFile << output << endl;
                }
                break;
            case FunctionCall:
                // 函数调用
                // 函数值保存在left中
                for (auto &it1 : it.valueParaTab) {
                    // Push x
                    optTmpFile << "Push " + it1.name << endl;
                }
                optTmpFile << "Call " + it.target << endl; // 函数调用语句
                optTmpFile << it.left + " = RET" << endl; // 返回值
                break;
            default:
                break;
        }
        // 清空输出字符串
        output.clear();
    }
}
















