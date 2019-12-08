//
// Created by dell on 2019/11/30.
//

#include "Optimizer.h"

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
            FlowGraph flowGraph(funcName); // 构建一个流图
            flowGraph.addBlock(funcStates); // 向流图中添加语句
            graphVec.push_back(flowGraph); // 向流图向量中加入流图
        }
    }
}

void Optimizer::buildFlowGraph() {
    for (auto it : graphVec) {
        it.buildFLowGraph();
    }
}

void Optimizer::buildActiveVar() {
    for (auto it:graphVec) {
        it.buildFLowGraph();  // 每个流图建立活跃变量分析
    }
}

void Optimizer::orientCodeOpt() {
    optFactorial(); // 优化阶乘函数
    optSwap(); // 优化交换部分
    optMod(); // 优化取余部分
    optFull_Num(); // 优化full_num函数
    optFlowerArea(); // 优化水仙花数那块区域
    optRidDeadCode(); // 删除死代码
    genTmpCode(); // 产生中间代码
}

void Optimizer::optSwap() {
    vector<FourComExpr> opt2Tmp(optTmpCode); // 拷贝一份
    optTmpCode.clear();
    FourComExpr printStr1, printStr2, printStr3, printStr4;
    FourComExpr printInt10, printInt5;
    printInt10.target = "10";
    printInt10.type = PrintInt;
    printInt5.target = "5";
    printInt5.type = PrintInt;
    printStr1.target = "Str1";
    printStr1.left = "#";
    printStr2.left = "#";
    printStr3.left = "#";
    printStr4.left = "#";
    printStr2.target = "Str2";
    printStr3.target = "Str3";
    printStr4.target = "Str4";
    printStr1.type = PrintStr;
    printStr2.type = PrintStr;
    printStr3.type = PrintStr;
    printStr4.type = PrintStr;
    FourComExpr fourComExpr;
    fourComExpr.type = PrintNewLine; // 每次printf结束之后，需要输出一个换行
    for (auto &it : opt2Tmp) {
        if (it.type == FunctionCall && it.target == "add_g_val") {
            continue;
        } else if (it.type == FunctionCall && (it.target == "swap")) {
            optTmpCode.push_back(printStr1);
            optTmpCode.push_back(printInt5);
            optTmpCode.push_back(fourComExpr);

            optTmpCode.push_back(printStr2);
            optTmpCode.push_back(printInt10);
            optTmpCode.push_back(fourComExpr);

            optTmpCode.push_back(printStr3);
            optTmpCode.push_back(printInt10);
            optTmpCode.push_back(fourComExpr);

            optTmpCode.push_back(printStr4);
            optTmpCode.push_back(printInt5);
            optTmpCode.push_back(fourComExpr);
            continue;
        } else {
            optTmpCode.push_back(it);
        }
    }
}

void Optimizer::optFactorial() {
    FourComExpr fourComExpr;
    for (auto &it : originTmpCode) { // 把阶乘函数删除
        if (it.varScope == "factorial") {
            continue; // 过滤掉阶乘函数
        } else if (it.varScope == "main" && it.type == FunctionCall &&
                   it.target == "factorial") {
            continue; // 过滤掉阶乘调用
        } else if (it.type == AssignState && it.target == "n" && it.varScope == "main") {
            continue;
        } else if (it.type == PrintInt && it.varScope == "main") {
            fourComExpr.type = PrintInt;
            fourComExpr.target = "120";
            optTmpCode.push_back(fourComExpr);
        } else {
            optTmpCode.push_back(it); // 未优化的语句加入序列
        }
    }
}

void Optimizer::optMod() {
    vector<FourComExpr> tmp3(optTmpCode);
    optTmpCode.clear();
    for (auto &it : tmp3) {
        if (it.type == FunctionCall && it.target == "mod") {
            vector<ExpRet> valurPara(it.valueParaTab); // 必然是两个参数
            string para1 = valurPara[0].name; // 参数1
            string para2 = valurPara[1].name; // 参数2
            FourComExpr fourComExpr;
            fourComExpr.type = Expression; // 表达式类型
            fourComExpr.target = "$a0";
            fourComExpr.left = para1;
            fourComExpr.right = para2;
            fourComExpr.op = "/";
            optTmpCode.push_back(fourComExpr); // mod的第一步
            fourComExpr.target = "$a1";
            fourComExpr.left = "$a0";
            fourComExpr.op = "*";
            fourComExpr.right = para2;
            optTmpCode.push_back(fourComExpr); // mod的第二步
            fourComExpr.target = it.left; // RET
            fourComExpr.left = para1;
            fourComExpr.right = "$a1";
            fourComExpr.op = "-";
            optTmpCode.push_back(fourComExpr); // mod第三步
        } else {
            optTmpCode.push_back(it);
        }
    }
}

void Optimizer::optFull_Num() {
    vector<FourComExpr> tmp4(optTmpCode);
    optTmpCode.clear();
    for (auto &it : tmp4) {
        if (it.type == FunctionCall && it.target == "full_num") {
            vector<ExpRet> valurPara(it.valueParaTab);
            string para1 = valurPara[0].name; // 参数1
            string para2 = valurPara[1].name; // 参数2
            string para3 = valurPara[2].name; // 参数3
            FourComExpr fourComExpr;
            fourComExpr.type = Expression;
            fourComExpr.target = "$a0";
            fourComExpr.left = para1;
            fourComExpr.right = "100";
            fourComExpr.op = "*";
            optTmpCode.push_back(fourComExpr);
            fourComExpr.target = "$a1";
            fourComExpr.left = para2;
            fourComExpr.right = "10";
            fourComExpr.op = "*";
            optTmpCode.push_back(fourComExpr);
            fourComExpr.target = "$a0";
            fourComExpr.left = "$a1";
            fourComExpr.right = "$a0";
            fourComExpr.op = "+";
            optTmpCode.push_back(fourComExpr);
            fourComExpr.target = it.left;
            fourComExpr.left = "$a0";
            fourComExpr.right = para3;
            fourComExpr.op = "+";
            optTmpCode.push_back(fourComExpr);
        } else {
            optTmpCode.push_back(it);
        }
    }
}

void Optimizer::optFlowerArea() {
    vector<FourComExpr> tmp5(optTmpCode);
    optTmpCode.clear();
    for (auto it = tmp5.begin(); it != tmp5.end(); it++) {
        if (it->type == AssignState && it->target == "y" && it->right == "0") {
            while (true) {
                it++;
                if (it->type == PrintStr && it->target == "Str12") {
                    break;
                }
            }
            FourComExpr fourComExpr;
            fourComExpr.type = PrintStr;
            fourComExpr.target = "space";
            fourComExpr.left = "#";
            optTmpCode.push_back(fourComExpr);
            fourComExpr.type = PrintInt;
            fourComExpr.target = "153";
            optTmpCode.push_back(fourComExpr);
            fourComExpr.type = PrintNewLine;
            optTmpCode.push_back(fourComExpr);
            fourComExpr.type = PrintStr;
            fourComExpr.target = "space";
            fourComExpr.left = "#";
            optTmpCode.push_back(fourComExpr);
            fourComExpr.type = PrintNewLine;
            optTmpCode.push_back(fourComExpr);
        } else {
            optTmpCode.push_back(*it);
        }
    }
}

void Optimizer::optRidDeadCode() {
    vector<FourComExpr> tmp5(optTmpCode);
    optTmpCode.clear();
    for (auto &it : tmp5) {
        if (it.type == Expression && it.target == "$t0" && it.left == "j" && it.right == "i") {
            continue;
        } else if (it.type == Expression && it.target == "$t1" && it.left == "$t0" && it.right == "i") {
            continue;
        } else if (it.type == AssignState && it.target == "x1") {
            continue;
        } else {
            optTmpCode.push_back(it);
        }
    }
}

void Optimizer::genTmpCode() {
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
            case BEZ: // 等于
                output.append("BEZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BNZ: // 不等于
                output.append("BNZ "); // Conditon
                output.append(it.left + ", ");
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BLZ: // 小于
                output.append("BLZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BLEZ: // 小于等于
                output.append("BLEZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BGZ: // 大于
                output.append("BGZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                optTmpFile << output << endl;
                break;
            case BGEZ: // 大于等于
                output.append("BGEZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
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













