//
// Created by dell on 2019/10/10.
//

#include "SyntaxParser.h"
#include <stack>
#include <iostream>


string SyntaxParser::generateLabel() {
    labelCount++;
    char x[10] = {'\0'};
    sprintf(x, "%d", labelCount);
    return ("Label" + string(x)); // Label1
}

string SyntaxParser::genVar() {
    varCount++;
    char x[10] = {'\0'};
    sprintf(x, "%d", varCount % 8);
    return ("$t" + string(x)); // $t1
}

string SyntaxParser::genString() {
    stringCount++;
    char x[10] = {'\0'};
    sprintf(x, "%d", stringCount);
    return ("Str" + string(x)); // T1
}

string SyntaxParser::genGlobalReg() {
    globalRegCnt++;
    char x[10] = {'\0'};
    sprintf(x, "%d", globalRegCnt % 8);
    return ("$s" + string(x)); // $s1
}

SyntaxParser::SyntaxParser(LexicalParser &lex, SymbolTabel &symbolTabel1) :
        lexicalParser(lex), symbolTabel(symbolTabel1) {
    level = 0; // 初始化为0层 最外层的全局变量
    tmpCode.open("tmpCode.txt", ios::out);
    labelCount = 0;
    varCount = 0;
    stringCount = 0;
    condRelation = BGEZ; // 随便设置一个，在用时总会先赋值再使用
    varScope = "Global"; // 初始化为全局作用域
    order = 0;  // 初始化序号为0
    globalRegCnt = 0;
    // haveReturn = false; // 函数进入之前也会设置为false 表示不含return语句
    relationMap.insert(pair<string, TmpCodeType>("!=", BNZ));
    relationMap.insert(pair<string, TmpCodeType>("==", BEZ));
    relationMap.insert(pair<string, TmpCodeType>(">=", BGEZ));
    relationMap.insert(pair<string, TmpCodeType>("<=", BLEZ));
    relationMap.insert(pair<string, TmpCodeType>(">", BGZ));
    relationMap.insert(pair<string, TmpCodeType>("<", BLZ));
}

void SyntaxParser::start() {
    constState();
    varState();
    varScope = ""; // 防止遭遇不测把函数内的变量定义为全局
    funcState();
    varScope = "main"; // main函数的作用域
    mainParse();
    genTmpCode(); // 在此处输出中间代码
}

void SyntaxParser::constState() {
    // 常量说明
    if (!isConstState()) {
        return;
    }
    string token;
    while (true) {
        if (!isConstState()) {
            break;
        }
        constDef();
        if (lexicalParser.peekTop() == ';') {
            token = lexicalParser.getWord(); // ; 分号
        }
    }
}

void SyntaxParser::constDef() {
    string token;
    token = lexicalParser.getWord(); // const
    token = lexicalParser.getWord();
    if (token == "int") {
        constIntDef();
    } else if (token == "char") {
        constCharDef();
    } else {
        exit(-1); // 报错
    }
    token = lexicalParser.getWord(); // ;
}

void SyntaxParser::constIntDef() {
    // string global = "Global";
    vector<SymbolItem> tmp; // 构造一个空引用放到符号表中占位置
    string token;
    string iden;
    while (true) {
        FourComExpr fourComExpr;
        fourComExpr.valueType = Integer;
        fourComExpr.type = ConstantDef; // 类型为常量定义
        iden = lexicalParser.getWord(); // 标识符
        fourComExpr.target = iden; //标识符名称
        token = lexicalParser.getWord(); // =
        fourComExpr.op = token; // 等号运算符
        char c = lexicalParser.peekTop();
        if (c == '+' || c == '-') {
            token = lexicalParser.getWord(); // +/-
            c = lexicalParser.peekTop();
            if (isdigit(c)) {
                token.append(lexicalParser.getWord()); // 得到一个带正负号的整数
                SymbolItem item(Constant, iden, level, Integer, token, order, varScope, tmp); // 新建一个符号项
                symbolTabel.pushSymTabel(item); // 将符号压入符号表
                localVarSpace.insert(pair<string, SymbolItem>(iden, item)); // 加入局部变量/常量表
            }
            fourComExpr.left = token; // 加入带符号的数字
        } else if (isdigit(c)) {
            token = lexicalParser.getWord(); // 得到一个无符号整数
            SymbolItem item(Constant, iden, level, Integer, token, order, varScope, tmp); // 新建一个符号项
            symbolTabel.pushSymTabel(item); // 将符号压入符号表
            localVarSpace.insert(pair<string, SymbolItem>(iden, item));
            fourComExpr.left = token; // 无符号数字
        }
        fourComExpr.varScope = varScope;
        tmpCodeVector.push_back(fourComExpr);
        if (lexicalParser.peekTop() == ';') {
            break;
        }
        token = lexicalParser.getWord(); // , 逗号
    }
}

bool isConstChar(char ch) {
    return isalnum(ch) || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_';
}

void SyntaxParser::constCharDef() {
    // string global = "Global";
    string token;
    string iden;
    vector<SymbolItem> tmp;
    while (true) {
        FourComExpr fourComExpr;
        fourComExpr.type = ConstantDef; // 类型为常量定义
        fourComExpr.valueType = Character;
        iden = lexicalParser.getWord(); // 标识符
        fourComExpr.target = iden; // 设置常量名
        token = lexicalParser.getWord(); // =
        fourComExpr.op = token; // 设置运算符
        string value = lexicalParser.getWord(); // 字符
        fourComExpr.left = value; // 设置操作数: 第二个操作数为空
        fourComExpr.varScope = "Global";
        tmpCodeVector.push_back(fourComExpr);
        SymbolItem item(Constant, iden, level, Character, value, order, varScope, tmp); // 新建一个符号项
        symbolTabel.pushSymTabel(item); // 将符号压入符号表, 填表前自动查表
        localVarSpace.insert(pair<string, SymbolItem>(iden, item)); // 加入局部表
        if (lexicalParser.peekTop() == ';') {
            break;
        }
        token = lexicalParser.getWord();  // , 逗号
    }
}

void SyntaxParser::varState() {
    if (!isVarState()) {
        return;
    }
    string token;
    while (true) {
        if (!isVarState()) {
            break;
        }
        varDef();
    }
}

void SyntaxParser::varDef() {
    string token; // 类型标识符 int 或// char;
    string type = lexicalParser.getWord(); // 类型标识符
    char c;
    vector<SymbolItem> tmp; // 占位
    while (true) {
        FourComExpr fourComExpr;
        token = lexicalParser.getWord(); // 标识符
        fourComExpr.type = VaribleDef; // 变量定义
        fourComExpr.target = token; // 设置中间码的名称
        fourComExpr.varScope = varScope; // 设置变量的作用域
        string value = "0"; // 占位
        c = lexicalParser.peekTop();
        if (c == ',' || c == ';') {
            if (type == "int") {
                fourComExpr.valueType = Integer; // 设置中间码的类型
                SymbolItem item(Variable, token, level,
                                Integer, value, order, varScope, tmp);
                if (varScope == "Global") {
                    symbolTabel.pushSymTabel(item);
                }
                localVarSpace.insert(pair<string, SymbolItem>(token, item));
            } else if (type == "char") {
                fourComExpr.valueType = Character; // 设置中间码的类型
                SymbolItem item(Variable, token, level,
                                Character, value, order, varScope, tmp);
                if (varScope == "Global") {
                    symbolTabel.pushSymTabel(item);
                }
                localVarSpace.insert(pair<string, SymbolItem>(token, item));
            }
            order++; // 不是数组才能自增 --- 数组统一放在data区
            fourComExpr.varScope = varScope;
            tmpCodeVector.push_back(fourComExpr); // 局部变量定义加入中间代码表
            if (c == ';') break; // 分号的输出在while循环体外
            token = lexicalParser.getWord();
        } else if (c == '[') {
            fourComExpr.type = ArrayDef; // 数组定义
            token = lexicalParser.getWord(); // [
            value = lexicalParser.getWord(); // 无符号整数
            if (type == "int") {
                fourComExpr.valueType = Integer; // 设置中间码的类型
                SymbolItem item(Variable, fourComExpr.target, level,
                                Integer, value, order, varScope, tmp);
                symbolTabel.pushSymTabel(item);
                localVarSpace.insert(pair<string, SymbolItem>(fourComExpr.target, item));
            } else if (type == "char") {
                fourComExpr.valueType = Character; // 设置中间码的类型
                SymbolItem item(Variable, fourComExpr.target, level,
                                Character, value, order, varScope, tmp);
                symbolTabel.pushSymTabel(item);
                localVarSpace.insert(pair<string, SymbolItem>(fourComExpr.target, item));
            }
            fourComExpr.left = value; // 第一个操作数:: 数组大小
            token = lexicalParser.getWord(); // ]
            c = lexicalParser.peekTop();
            if (c == ';') {
                fourComExpr.varScope = varScope;
                tmpCodeVector.push_back(fourComExpr); // 加入全局中间代码表
                break;
            } else if (c == ',') {
                token = lexicalParser.getWord(); // 吃掉逗号
            }
            fourComExpr.varScope = varScope;
            tmpCodeVector.push_back(fourComExpr); // 局部数组加入全局中间代码表
        }
    }
    token = lexicalParser.getWord(); // 输出分号
}

void SyntaxParser::funcState() {
    // 函数说明
    string token;
    string type;
    string name;
    RetType retType = Void; // 默认为void
    vector<SymbolItem> paraVector; // 解析参数表得到的参数
    while (true) {
        // 不断的定义函数
        if (isMain()) {
            varScope = "main";
            break;
        }
        localVarSpace.clear(); // 清空上一个函数的局部变量空间
        // haveReturn = false; // 每个函数进入之前的return设置为无
        FourComExpr fourComExpr; // 中间代码生成的四元式
        type = lexicalParser.getWord(); // 函数类型
        name = lexicalParser.getWord(); // 函数名称
        varScope = name; // 改变函数的作用域
        order = 0; // 一个函数定义完之后order需要清零 -- 记录函数局部变量的序号
        fourComExpr.type = FunctionDef; // 函数定义
        fourComExpr.target = name;
        token = lexicalParser.getWord(); // (
        level++;
        paraVector = paraParse(); // 参数表解析 : 要吃掉右小括号
        fourComExpr.paraSet = paraVector; // 将函数的参数表加入四元表达式中
        token = lexicalParser.getWord(); // )
        level--; // 此处level的变化是为了将参数与函数层次分开
        string value1;
        if (type == "int") {
            retType = Integer;
            fourComExpr.valueType = Integer;
            value1 = to_string(paraVector.size());
            SymbolItem item(Function, name, level, Integer, value1, order, varScope, paraVector);
            symbolTabel.pushSymTabel(item);
        } else if (type == "char") {
            retType = Character;
            fourComExpr.valueType = Character;
            value1 = to_string(paraVector.size());
            SymbolItem item(Function, name, level, Character, value1, order, varScope, paraVector);
            symbolTabel.pushSymTabel(item);
        } else {
            value1 = to_string(paraVector.size());
            retType = Void;
            fourComExpr.valueType = Void;
            SymbolItem item(Function, name, level, Void, value1, order, varScope, paraVector);
            symbolTabel.pushSymTabel(item);
        }
        if (type == "void") {
            lexicalParser.addFunc(name, 0);
        } else {
            lexicalParser.addFunc(name, 1);
        }
        fourComExpr.varScope = varScope;
        tmpCodeVector.push_back(fourComExpr); // 将这个函数定义推入全局中间代码表中
        // 将参数表中的参数推入二级符号表
        for (const auto &it : paraVector) {
            symbolTabel.pushSymTabel(it);
        }
        level++; // level 变成1
        token = lexicalParser.getWord(); // {
        comStateParse(retType); // 复合语句: 复合语句中会有局部变量和局部常量的声明
        if (type == "void") { // 在void最后加一条返回语句
            FourComExpr fuckExpr;
            fuckExpr.type = Return;
            fuckExpr.valueType = Void;
            fuckExpr.varScope = varScope;
            tmpCodeVector.push_back(fuckExpr);
        }
        token = lexicalParser.getWord(); // }
        symbolTabel.dropSymTabel(level); // 清空这层函数的局部变量
        level--;
    }
}

vector<SymbolItem> SyntaxParser::paraParse() {
    string token;
    string type;
    vector<SymbolItem> paraVector;
    char c;
    int paraOrder = 0; // 记录参数的顺序
    while (true) {
        c = lexicalParser.peekTop();
        if (c == ')') {
            break; // 参数表为空的情况
        }
        type = lexicalParser.getWord(); // 类型标识符
        token = lexicalParser.getWord(); // 标识符
        vector<SymbolItem> tmp;
        string value = "19981111"; // 参数的value不能设为0
        if (type == "int") {
            SymbolItem item(Parament, token, level, Integer, value, paraOrder++, varScope, tmp);
            paraVector.push_back(item);
            localVarSpace.insert(pair<string, SymbolItem>(token, item));
        } else if (type == "char") {
            SymbolItem item(Parament, token, level, Character, value, paraOrder++, varScope, tmp);
            paraVector.push_back(item);
            localVarSpace.insert(pair<string, SymbolItem>(token, item));
        } else {
            cout << "parameter parser enter a impossible area " << endl;
        }
        c = lexicalParser.peekTop(); // 正常情况下应为 ) ,
        if (c == ')') {
            break; // 正常退出参数表
        } else if (c == ',') {
            token = lexicalParser.getWord();
        }
    }
    return paraVector;
}

bool SyntaxParser::stateColParse(RetType retType) {
    string token;
    bool ownsRet = false;
    char c = lexicalParser.peekTop();
    while (true) {
        if (c == '}') {
            break; // 语句列的分析结束（也标志着复合语句的分析结束）
        }
        stateParse(retType);
        c = lexicalParser.peekTop();
    }
    return ownsRet;
}

void SyntaxParser::stateParse(RetType retType) {
    string token;
    token = lexicalParser.getWord();
    if (token == "if") {
        // IF
        ifParse(retType);
        return;
    } else if (token == "while") {
        // while
        whileParse(retType);
        return;
    } else if (token == "for") {
        forParse(retType);
        return;
    } else if (token == "do") {
        // do-while
        doWhileParse(retType);
        return;
    } else if (token == "scanf") {
        //scanf
        scanfParse();
    } else if (token == "printf") {
        // printf
        printfParse();
    } else if (token == "return") {
        //return
        // haveReturn = true; // 把return的标志设置为true
        returnParse();
        // 以下为函数返回值类型相关的判断
    } else if (token == ";") {
        return;
    } else if (token == "{") {
        // {语句}
        stateColParse(retType);
        token = lexicalParser.getWord(); // }
        return;
    } else {
        // 赋值语句 有返回值的函数调用 无返回值的函数调用
        // 赋值语句不能在函数表中找到标识符
        if (lexicalParser.isVoidFunc(token)) {
            // 无返回值函数调用
            funcRefParse(0, token);
        } else if (lexicalParser.isRetFunc(token)) {
            // 有返回值的函数调用
            funcRefParse(1, token);
        } else {
            // 赋值语句
            assignParse(token);
        }
    }
    char c = lexicalParser.peekTop();
    if (c == ';') {
        token = lexicalParser.getWord(); // 吃掉分号
    }
}

void SyntaxParser::cmpBranch(const string &condRes, const string &labelA) {
    FourComExpr fourComExpr;
    string left = condRes;
    // 需要多加一条表达式语句的情况
    if (condRes.at(0) != '$') {
        // 说明这不是一个临时寄存器 --- 可能是一个数值/变量/常量...
        condRelation = BGZ;
        fourComExpr.type = Expression;
        fourComExpr.target = genVar();
        fourComExpr.left = condRes;
        fourComExpr.right = "0";
        fourComExpr.valueType = Integer;
        fourComExpr.op = "-";
        left = fourComExpr.target;
        tmpCodeVector.push_back(fourComExpr);
    }
    switch (condRelation) {
        // 不满足if条件时时才会跳转
        // cnodRelation:: 需要满足的条件
        case BEZ:  /// ==
            fourComExpr.type = BNZ; // 不等于0跳转
            fourComExpr.left = left; // 左边等于cmp结果
            fourComExpr.target = labelA; // 跳到else语句
            fourComExpr.varScope = varScope;
            fourComExpr.right = "bnez";
            tmpCodeVector.push_back(fourComExpr);
            break;
        case BNZ: // !=
            fourComExpr.type = BEZ; // 等于0时跳转
            fourComExpr.left = left;
            fourComExpr.target = labelA;
            fourComExpr.varScope = varScope;
            fourComExpr.right = "beqz";
            tmpCodeVector.push_back(fourComExpr);
            break;
        case BLEZ:  // <=
            fourComExpr.type = BGZ; // 大于0时跳转
            fourComExpr.left = left;
            fourComExpr.target = labelA;
            fourComExpr.varScope = varScope;
            fourComExpr.right = "bgtz";
            tmpCodeVector.push_back(fourComExpr);
            break;
        case BLZ: // <
            fourComExpr.type = BGEZ; // 大于等于0 时跳转
            fourComExpr.left = left;
            fourComExpr.target = labelA;
            fourComExpr.varScope = varScope;
            fourComExpr.right = "bgez";
            tmpCodeVector.push_back(fourComExpr);
            break;
        case BGZ: // >
            fourComExpr.type = BLEZ; // 小于等于0时跳转
            fourComExpr.left = left;
            fourComExpr.target = labelA;
            fourComExpr.varScope = varScope;
            fourComExpr.right = "blez";
            tmpCodeVector.push_back(fourComExpr);
            break;
        case BGEZ: // >=
            fourComExpr.type = BLZ; // 小于0时跳转
            fourComExpr.left = left;
            fourComExpr.target = labelA;
            fourComExpr.varScope = varScope;
            fourComExpr.right = "bltz";
            tmpCodeVector.push_back(fourComExpr);
            break;
        default:
            break;
    }
}

string SyntaxParser::condParse() {
    char c;
    string token;
    ExpRet exprLeft, exprRight;
    exprLeft = exprParse(); // 左表达式解析
    c = lexicalParser.peekTop();
    if (c != '!' && c != '<' && c != '>' && c != '=') {
        // 如果不是关系运算符，则直接结束
        // 说明只有单个表达式
        return exprLeft.name; // 直接返回左表达式的值
    }
    token = lexicalParser.getWord(); // 关系运算符
    condRelation = relationMap.find(token)->second; //将关系运算符赋值
    exprRight = exprParse();
    // 生成比较的中间代码
    FourComExpr fourComExpr;
    // mips的比较方式是将两个表达式的值相减
    fourComExpr.target = genVar();
    fourComExpr.type = Expression;
    fourComExpr.left = exprLeft.name; // 左边
    fourComExpr.op = "-"; // 减
    fourComExpr.right = exprRight.name; //右边
    fourComExpr.varScope = varScope;
    tmpCodeVector.push_back(fourComExpr);
    return fourComExpr.target; // 返回相减结果的值
}

void SyntaxParser::ifParse(RetType retType) {
    string token;
    string labelA, labelB;
    labelA = generateLabel(); // 跳到else语句开始
    labelB = generateLabel(); // 跳到条件语句结束
    FourComExpr fourComExpr;
    fourComExpr.varScope = varScope;
    token = lexicalParser.getWord(); // (
    string condRes = condParse(); // 条件分析 -- 并返回比较的结果
    cmpBranch(condRes, labelA); // 不满足condRes 会发生跳转
    token = lexicalParser.getWord(); // )
    stateParse(retType); // 处理if的语句
    // 判断有无else
    token = lexicalParser.getWord();
    int flag = 0;
    if (token == "else") {
        flag = 1;
        // If 的语句如果结束了那么需要jump到else语句末尾
        fourComExpr.type = Jump; // if条件成立则跳转离开
        fourComExpr.target = labelB; // 直接跳到条件语句结束
        tmpCodeVector.push_back(fourComExpr); // 跳转语句
        // 设置label1
        fourComExpr.type = Label;
        fourComExpr.target = labelA;
        tmpCodeVector.push_back(fourComExpr);
        stateParse(retType);
    } else {
        // 退回一个单词
        lexicalParser.retract();
    }
    if (flag == 0) {
        // 设置label1
        fourComExpr.type = Label;
        fourComExpr.target = labelA; // 如果没有else 就设置为labelA
        tmpCodeVector.push_back(fourComExpr);
    } else {
        fourComExpr.type = Label;
        fourComExpr.target = labelB; // 这里是条件语句的末尾 --- 设置label2
        tmpCodeVector.push_back(fourComExpr);
    }
}

void SyntaxParser::whileParse(RetType retType) {
    string token;
    FourComExpr fourComExpr; // 四元式中间代码
    fourComExpr.varScope = varScope;
    string startLoop = generateLabel(); // 设置循环开始
    string endOfWhile = generateLabel(); // while跳出循环体
    // 设置label
    fourComExpr.type = Label;
    fourComExpr.target = startLoop;
    tmpCodeVector.push_back(fourComExpr);
    token = lexicalParser.getWord(); // (
    string condRet = condParse();
    cmpBranch(condRet, endOfWhile);
    token = lexicalParser.getWord(); // )
    stateParse(retType);
    // 跳到循环体开头
    fourComExpr.target = startLoop;
    fourComExpr.type = Jump;
    tmpCodeVector.push_back(fourComExpr);
    // 设置结束label
    fourComExpr.type = Label;
    fourComExpr.target = endOfWhile;
    tmpCodeVector.push_back(fourComExpr);
}

void SyntaxParser::forParse(RetType retType) {
    // retType 表示当前所在函数的类型，为return语句服务的
    string token;
    string idenName, idenName1;
    string step, op; // 步长
    string startLoop = generateLabel(); // 循环语句的开始
    string endFor = generateLabel(); // 循环语句的结束标签
    FourComExpr fourComExpr;
    fourComExpr.varScope = varScope;
    token = lexicalParser.getWord(); // (
    idenName = lexicalParser.getWord(); // 标识符
    token = lexicalParser.getWord(); // =
    ExpRet exprRet = exprParse(); // 表达式
    // 循环变量赋初值
    fourComExpr.type = AssignState;
    fourComExpr.arrayOrVar = 1; // 对变量赋值
    fourComExpr.target = idenName; // 标识符名称
    fourComExpr.right = exprRet.name; // 表达式的返回值作为赋值项
    fourComExpr.op = "=";
    tmpCodeVector.push_back(fourComExpr);
    // 设置循环开始的标签
    fourComExpr.target = startLoop;
    fourComExpr.type = Label;
    tmpCodeVector.push_back(fourComExpr);
    // 循环条件判断
    token = lexicalParser.getWord(); // ;
    string condRet = condParse();
    cmpBranch(condRet, endFor);
    token = lexicalParser.getWord(); //;
    // 循环变量自增
    idenName = lexicalParser.getWord(); // 循环变量标识符
    token = lexicalParser.getWord(); // =
    idenName1 = lexicalParser.getWord(); // 循环变量递增时的标识符
    op = lexicalParser.getWord(); // +/-
    step = (lexicalParser.getWord()); // 步长
    token = lexicalParser.getWord(); // )
    // 循环体解析
    stateParse(retType);
    // 循环变量增值
    fourComExpr.type = Expression; // 计算循环变量右边表达式
    fourComExpr.target = idenName;
    fourComExpr.left = idenName1;
    fourComExpr.right = step;
    fourComExpr.op = op;
    tmpCodeVector.push_back(fourComExpr);

    /*fourComExpr.type = AssignState; // 对循环变量进行增值
    fourComExpr.arrayOrVar = 1; // 对循环变量赋值
    fourComExpr.right = fourComExpr.target; // 从全局变量处取值
    fourComExpr.target = idenName;
    fourComExpr.op = "=";
    tmpCodeVector.push_back(fourComExpr);*/
    // 跳到循环体开头
    fourComExpr.target = startLoop;
    fourComExpr.type = Jump;
    tmpCodeVector.push_back(fourComExpr);
    // 循环体结束
    fourComExpr.target = endFor;
    fourComExpr.type = Label;
    tmpCodeVector.push_back(fourComExpr);
}

void SyntaxParser::doWhileParse(RetType retType) {
    string startLoop = generateLabel();
    string endDoWhile = generateLabel();
    FourComExpr fourComExpr;
    fourComExpr.varScope = varScope;
    // 设置开始标签
    fourComExpr.target = startLoop;
    fourComExpr.type = Label;
    tmpCodeVector.push_back(fourComExpr);
    stateParse(retType);
    string token;
    token = lexicalParser.getWord(); // while
    token = lexicalParser.getWord(); // (
    string condRet = condParse(); // 条件
    cmpBranch(condRet, endDoWhile); // 不满足条件的话就跳出循环
    token = lexicalParser.getWord(); // )
    // 跳转回循环体的开始
    fourComExpr.type = Jump;
    fourComExpr.target = startLoop;
    tmpCodeVector.push_back(fourComExpr);
    // 设置循环终止条件
    fourComExpr.type = Label;
    fourComExpr.target = endDoWhile;
    tmpCodeVector.push_back(fourComExpr);
}

void SyntaxParser::scanfParse() {
    string token;
    token = lexicalParser.getWord(); // (
    while (true) {
        FourComExpr fourComExpr;
        fourComExpr.varScope = varScope;
        token = lexicalParser.getWord(); // 标识符
        RetType varType;
        if (localVarSpace.find(token) != localVarSpace.end()) {
            varType = localVarSpace.find(token)->second.getRetType(); // 局部优先原则
        } else {
            varType = symbolTabel.getRetTypeByName(token); // 从全局符号表中寻找
        }
        if (varType == Integer) {
            fourComExpr.type = ReadInt; // 输入整数
        } else {
            fourComExpr.type = ReadChar; // 输入字符
        }
        fourComExpr.target = token; // 接收输入值的标识符
        tmpCodeVector.push_back(fourComExpr);
        char c = lexicalParser.peekTop();
        if (c == ')') {
            // 正常退出
            break;
        }
        token = lexicalParser.getWord(); // ,
    }
    token = lexicalParser.getWord(); // )

}

void SyntaxParser::printfParse() {
    string token;
    char c;
    FourComExpr fourComExpr;
    fourComExpr.varScope = varScope;
    token = lexicalParser.getWord(); // (
    c = lexicalParser.peekTop();
    if (c == '\"') {
        // 有字符串
        token = lexicalParser.getWord(); // 读取字符串
        fourComExpr.target = genString(); // 产生字符串标号
        fourComExpr.left = token;
        fourComExpr.type = PrintStr;  // 输出字符串
        tmpCodeVector.push_back(fourComExpr);
        c = lexicalParser.peekTop();
        if (c == ',') {
            // 有表达式 --- int or char
            token = lexicalParser.getWord(); // ,
            ExpRet expRet = exprParse();
            if (expRet.type == Integer) {
                fourComExpr.type = PrintInt;
                fourComExpr.target = expRet.name;
                tmpCodeVector.push_back(fourComExpr);
            } else {
                fourComExpr.type = PrintChar;
                fourComExpr.target = expRet.name;
                tmpCodeVector.push_back(fourComExpr);
            }
        }
    } else {
        // 没有字符串
        ExpRet expRet = exprParse();
        if (expRet.type == Integer) {
            fourComExpr.type = PrintInt;
            fourComExpr.target = expRet.name;
            tmpCodeVector.push_back(fourComExpr);
        } else {
            fourComExpr.type = PrintChar;
            fourComExpr.target = expRet.name;
            tmpCodeVector.push_back(fourComExpr);
        }
    }
    token = lexicalParser.getWord(); // )
    fourComExpr.type = PrintNewLine; // 每次printf结束之后，需要输出一个换行
    tmpCodeVector.push_back(fourComExpr);
}

RetType SyntaxParser::returnParse() {
    string token;
    char c;
    c = lexicalParser.peekTop();
    if (c == ';') {
        FourComExpr fourComExpr;
        fourComExpr.varScope = varScope;
        fourComExpr.type = Return;
        fourComExpr.valueType = Void;
        tmpCodeVector.push_back(fourComExpr);
        return Void;
    } else if (c == '(') {
        token = lexicalParser.getWord(); // (
        ExpRet expRet = exprParse();
        token = lexicalParser.getWord();// )
        FourComExpr fourComExpr;
        fourComExpr.varScope = varScope;
        fourComExpr.target = expRet.name; // 表达式返回值的临时变量
        if (expRet.name.find('[') != std::__cxx11::string::npos) {
            // 要把下标和名称分开
            int bPos1 = expRet.name.find('[');
            int bPos2 = expRet.name.find(']');
            string array = expRet.name.substr(0, bPos1); // 数组名称
            string index = expRet.name.substr(bPos1 + 1, bPos2 - bPos1 - 1); // 数组下标
            fourComExpr.type = Expression;
            fourComExpr.target = genVar();
            fourComExpr.left = array; // 数组名
            fourComExpr.index1 = index; // 数组下标
            fourComExpr.right = "0";
            fourComExpr.op = "+";
            tmpCodeVector.push_back(fourComExpr); // 压入中间代码中
        }
        // fourComExpr.target = fourComExpr.target;  临时寄存器名称不变
        fourComExpr.valueType = expRet.type; // 返回语句中 :: 四元式的返回值类型与表达式返回值类型相同
        fourComExpr.type = Return; // 四元式的类型
        tmpCodeVector.push_back(fourComExpr); // 加入全局符号表
        return fourComExpr.valueType;
    }
    return Void; // 默认情况
}

void SyntaxParser::assignParse(const string &idenName) {
    char c;
    string token;
    c = lexicalParser.peekTop();
    ExpRet arrayIndex, srcExpr; // 数组下标和赋值语句的右边
    FourComExpr fourComExpr; // 新建四元式项
    fourComExpr.type = AssignState; // 赋值语句
    if (c == '[') {
        token = lexicalParser.getWord(); // [
        arrayIndex = exprParse(); // 获取数组元素
        // 下标解析完成之后，马上保护寄存器的下标
        FourComExpr keepIndex; // 为了保护数组下标
        keepIndex.arrayOrVar = 1;
        keepIndex.type = AssignState;
        keepIndex.target = "$v1";
        keepIndex.op = "=";
        keepIndex.right = arrayIndex.name;
        tmpCodeVector.push_back(keepIndex);
        token = lexicalParser.getWord(); // ]
        fourComExpr.target = idenName; // 赋值语句的左侧
        fourComExpr.index = "$v1"; // 保存数组的下标
        fourComExpr.arrayOrVar = 0; // 表示对数组赋值
    } else {
        fourComExpr.target = idenName; // 变量名或者常量名
        fourComExpr.arrayOrVar = 1; // 表示对变量赋值
    }
    token = lexicalParser.getWord(); // =
    srcExpr = exprParse(); // 获取数组元素的右边
    fourComExpr.op = "="; // 赋值语句使用等号
    fourComExpr.right = srcExpr.name; // 赋值语句右边的表达式
    fourComExpr.varScope = varScope;
    tmpCodeVector.push_back(fourComExpr); // 加入全局符号表
}

string SyntaxParser::funcRefParse(int flag, const string &funcName) {
    // 函数调用
    string token;
    token = lexicalParser.getWord(); // (
    // 函数名称未定义时，跳过值参数表的解析
    vector<ExpRet> valueParaTab = valueTabParse(funcName); // 值参数表解析
    token = lexicalParser.getWord(); // )
    FourComExpr fourComExpr;
    fourComExpr.varScope = varScope;
    fourComExpr.target = funcName; // 四元式的名称等于函数名称
    fourComExpr.type = FunctionCall; // 函数调用类型
    fourComExpr.valueParaTab = valueParaTab; // 函数的值参数表
    fourComExpr.left = genVar(); // 保存函数的返回值
    tmpCodeVector.push_back(fourComExpr); // 加入中间代码
    return fourComExpr.left; // 返回一个新名字标识函数的返回值
}

// 值参数表解析
vector<ExpRet> SyntaxParser::valueTabParse(const string &funcName) {
    string token;
    vector<ExpRet> valueTabPara; // 记录值参数表的参数
    char c = lexicalParser.peekTop();
    if (c == ')') {
        // 值参数表为空
        return valueTabPara; // 值参数表为空的情况
    }
    ExpRet expRet;
    while (true) {
        // 解析：{[+-]expr}
        expRet = exprParse();
        valueTabPara.push_back(expRet);
        // 判断值参数表是否结束
        c = lexicalParser.peekTop();
        if (c == ')') {
            break; // 正常退出
        } else if (c == ',') {
            token = lexicalParser.getWord(); // , 继续解析值参数
            // 逗号后面只能是标识符
        }
    }
    return valueTabPara;
}

vector<ExprCompose> SyntaxParser::revPolTransfer(vector<ExprCompose> &infixExpr) {
    // 中缀表达式转后缀表达式
    stack<ExprCompose> opStack; // 用来装操作符的栈
    vector<ExprCompose> revPoliExpr; // 中缀表达式的结果
    for (auto &it : infixExpr) {
        if (it.isOperator) {
            if (it.op == "+" || it.op == "-") {
                while (!opStack.empty()) {
                    revPoliExpr.push_back(opStack.top()); // 将栈顶元素加入后缀表达式
                    opStack.pop(); // 弹出栈顶元素
                } // 因为没有比加法运算符优先级更低的 :: 一旦遇到加法运算符就可以将之前的加法运算符输出
                opStack.push(it); // 加法运算符入栈
            } else {
                while (!opStack.empty() && (opStack.top().op == "*" || opStack.top().op == "/")) { // 弹出乘法运算符
                    revPoliExpr.push_back(opStack.top()); // 将乘法运算符输出
                    opStack.pop(); // 弹出栈顶元素
                } // 乘法运算符:: 输出前先将所有乘法运算符输出
                opStack.push(it);
            }
        } else {
            revPoliExpr.push_back(it);
        }
    }
    while (!opStack.empty()) {
        revPoliExpr.push_back(opStack.top()); // 将栈顶元素加入后缀表达式
        opStack.pop(); // 弹出栈顶元素
    }
    return revPoliExpr;
}

// 逆波兰表达式的计算 --- 可以产生四元式
string SyntaxParser::revPolCal(vector<ExprCompose> &revPolExpr) {
    stack<ExprCompose> calResStack; // 存储运算结果
    for (auto &it : revPolExpr) {
        if (!it.isOperator) {
            // 如果是运算数，直接推入栈中
            calResStack.push(it);
        } else {
            // 如果是运算符，从栈顶取出两个元素计算后将结果压入栈中
            ExprCompose exprCompose1, exprCompose2;
            exprCompose1 = calResStack.top(); // 观察用
            calResStack.pop();
            exprCompose2 = calResStack.top(); // 观察用
            calResStack.pop();
            ExprCompose exprComposeNew;
            FourComExpr fourComExpr;
            exprComposeNew.isOperator = false; //存储中间运算结果
            exprComposeNew.target = genVar(); // 产生保存结果的运算符
            exprComposeNew.retType = Integer;
            calResStack.push(exprComposeNew); // 将运算结果返回到运算结果存储栈中
            // 产生四元式
            fourComExpr.target = exprComposeNew.target; // 运算结果
            fourComExpr.op = it.op; // 运算符
            fourComExpr.left = exprCompose2.target; // 第一个操作数
            fourComExpr.right = exprCompose1.target; // 第二个操作数
            // 如果是数组的话需要设置数组下标
            if (exprCompose2.target.find('[') != std::__cxx11::string::npos) {
                int bPos1 = exprCompose2.target.find('[');
                int bPos2 = exprCompose2.target.find(']');
                fourComExpr.left = exprCompose2.target.substr(0, bPos1);
                fourComExpr.index1 = exprCompose2.target.substr(bPos1 + 1, bPos2 - bPos1 - 1);
            }
            if (exprCompose1.target.find('[') != std::__cxx11::string::npos) {
                int bPos1 = exprCompose1.target.find('[');
                int bPos2 = exprCompose1.target.find(']');
                fourComExpr.right = exprCompose1.target.substr(0, bPos1);
                fourComExpr.index2 = exprCompose1.target.substr(bPos1 + 1, bPos2 - bPos1 - 1);
            }
            fourComExpr.type = Expression; // 设置四元式的类型
            fourComExpr.varScope = varScope;
            tmpCodeVector.push_back(fourComExpr); // 将新的四元式压入栈中
        }
    }
    // 如果是单独的数组元素，将其下标提出来
    string exprRes = calResStack.top().target;
    if (exprRes.find('[') != std::__cxx11::string::npos) {
        // 要把下标和名称分开
        int bPos1 = exprRes.find('[');
        int bPos2 = exprRes.find(']');
        string array = exprRes.substr(0, bPos1); // 数组名称
        string index = exprRes.substr(bPos1 + 1, bPos2 - bPos1 - 1); // 数组下标
        FourComExpr fourComExpr;
        fourComExpr.type = Expression;
        fourComExpr.target = genVar();
        fourComExpr.left = array; // 数组名
        fourComExpr.index1 = index; // 数组下标
        fourComExpr.right = "0";
        fourComExpr.op = "+";
        fourComExpr.varScope = varScope;
        tmpCodeVector.push_back(fourComExpr); // 压入中间代码中
        return fourComExpr.target;
    } else {
        return calResStack.top().target; // 栈顶元素就是最终的结果
    }
}

ExpRet SyntaxParser::exprParse() {
    char c;
    string token;
    ExpRet expRet;
    c = lexicalParser.peekTop();
    vector<ExprCompose> exprComposeAll; // 一个表达式的所有组成成分
    if (c == '+' || c == '-') {
        expRet.type = Integer; // 如果有加减号，就直接判定为整型
        token = lexicalParser.getWord(); // 获取加号
        ExprCompose exprCompose;
        exprCompose.op = token;
        exprCompose.isOperator = true;
        exprComposeAll.push_back(exprCompose);
    }
    while (true) {
        // 表达式内必须有一个因子，不能为空
        vector<ExprCompose> termCompose;
        ExpRet termRet = termParse(); // 表达式的组成应该合并项的内容
        termCompose = termRet.termFactor;
        if (termRet.type == Integer) {
            expRet.type = Integer;
        }
        exprComposeAll.insert(exprComposeAll.end(), termCompose.begin(), termCompose.end()); // 将项返回的内容合并到表达式中
        c = lexicalParser.peekTop();
        if (c != '+' && c != '-') {
            break;
        }
        token = lexicalParser.getWord(); // + / -
        expRet.type = Integer; // 只要有加减号,表达式就为整型
        // 新建表达式的组成成分保存符号
        ExprCompose exprCompose;
        exprCompose.op = token;
        exprCompose.isOperator = true;
        exprComposeAll.push_back(exprCompose);
    }
    // 逆波兰表达式转换
    if (exprComposeAll[0].isOperator) {
        ExprCompose exprCompose;
        exprCompose.retType = Integer;
        exprCompose.target = "0";
        exprCompose.isOperator = false;
        exprComposeAll.insert(exprComposeAll.begin(), exprCompose);
    }
    vector<ExprCompose> revPolExpr = revPolTransfer(exprComposeAll);
    // 逆波兰表达式计算 --- 产生四元表达式
    expRet.name = revPolCal(revPolExpr);
    return expRet;
}

ExpRet SyntaxParser::termParse() {
    char c;
    string token;
    ExpRet termRet = ExpRet(); // 表达式包含项，所以可以用一个面向对象父类返回值来处理项。
    while (true) {
        ExprCompose exprCompose;
        exprCompose = factorParse(); // 返回一个因子
        // 设置项的类型
        if (exprCompose.retType == Integer) {
            termRet.type = Integer;
        }
        termRet.termFactor.push_back(exprCompose);// 将因子加入项中
        c = lexicalParser.peekTop();
        if (c != '*' && c != '/') {
            return termRet;
        } // 如果不是乘法运算符则输出
        token = lexicalParser.getWord(); // 乘法运算符
        ExprCompose exprCompose1;
        exprCompose1.op = token; // 获得运算符
        exprCompose1.isOperator = true; // 设置运算符标志为true
        termRet.termFactor.push_back(exprCompose1); // 将符号加入项中
    }
}

ExprCompose SyntaxParser::factorParse() {
    char c;
    string token;
    ExprCompose exprCompose; // 因子的返回值
    c = lexicalParser.peekTop();
    exprCompose.isOperator = false; // 运算符不能成为单独的因子
    if (c == '(') {
        exprCompose.retType = Integer; // 只要有括号就是整型
        ExpRet expRet; // 表达式的返回值
        token = lexicalParser.getWord(); // (
        expRet = exprParse(); //
        token = lexicalParser.getWord(); // )
        exprCompose.factorType = ExprFactor; // 表达式因子
        exprCompose.target = expRet.name; // 保存表达式最终结果的临时变量
    } else if (isdigit(c)) {
        token = lexicalParser.getWord();// 数字
        exprCompose.factorType = IntFactor; // 整数类型
        exprCompose.target = token;
        exprCompose.retType = Integer; // 数字当然是整型
    } else if (c == '+' || c == '-') {
        token = lexicalParser.getWord(); // +-
        exprCompose.retType = Integer; // 有加减号肯定为整型
        token.append(lexicalParser.getWord());// 数字
        exprCompose.factorType = IntFactor; // 带符号的整数类型
        exprCompose.target = token;
    } else if (c == '\'') {
        token = "\'";
        token.append(lexicalParser.getWord()); // 获得一个字符
        token += '\'';
        exprCompose.factorType = CharFactor; // 字符类型
        exprCompose.retType = Character;
        exprCompose.target = token;
    } else {
        string idenName = lexicalParser.getWord(); // 标识符
        // 查符号表：返回该标识符的类型 :: 局部优先原则
        if (localVarSpace.find(idenName) != localVarSpace.end()) {
            exprCompose.retType = localVarSpace.find(idenName)->second.getRetType(); // 从局部空间中查找
        } else {
            exprCompose.retType = symbolTabel.getRetTypeByName(idenName); // 从全局空间中查找
        }
        c = lexicalParser.peekTop();
        if (c == '[') {
            // 数组定义
            ExpRet expRet;
            token = lexicalParser.getWord(); // [
            expRet = exprParse(); // 解析数组下标
            token = lexicalParser.getWord(); // ]
            exprCompose.factorType = ArrayFactor; // 数组因子
            exprCompose.target = idenName + "[" + expRet.name + "]"; // 数组的名称
        } else if (c == '(') {
            // 函数调用
            string funcRet;
            funcRet = funcRefParse(1, idenName); // 有返回值的函数调用
            exprCompose.factorType = FuncRefFactor; // 函数调用因子
            exprCompose.target = funcRet; // 函数的名字
        } else {
            exprCompose.factorType = IdenFactor; // 标识符因子
            exprCompose.target = idenName; // 标识符的名字
        }
        exprCompose.isOperator = false;
    }
    return exprCompose; // 返回继承属性--是否为整型因子
}

void SyntaxParser::mainParse() {
    string token;
    varScope = "main"; // 改变函数的作用域
    localVarSpace.clear(); // 将之前的局部变量空间清除
    order = 0; // 一个函数定义完之后order需要清零 -- 记录函数局部变量的序号
    token = lexicalParser.getWord(); // void
    token = lexicalParser.getWord(); // main
    FourComExpr fourComExpr;
    fourComExpr.varScope = varScope;
    fourComExpr.type = FunctionDef;
    fourComExpr.target = "main";
    fourComExpr.varScope = varScope;
    fourComExpr.valueType = Void; // main函数是void类型的
    tmpCodeVector.push_back(fourComExpr); // 把main函数作为一个函数定义放入中间代码生成中
    token = lexicalParser.getWord(); // (
    token = lexicalParser.getWord(); // )
    token = lexicalParser.getWord(); // {
    char c = lexicalParser.peekTop();
    if (c == '}') {
        token = lexicalParser.getWord(); // }
        return;
    }
    comStateParse();
    token = lexicalParser.getWord(); // }
}

void SyntaxParser::comStateParse(RetType retType) {
    constState(); // 常量说明
    varState(); // 变量说明
    stateColParse(retType); // 语句列
}

bool SyntaxParser::isVarState() {
    long record = lexicalParser.getFilepos();
    long llPos = lexicalParser.getLastPos();
    string token1 = lexicalParser.getWord(false); // int
    string token2 = lexicalParser.getWord(false); // change
    string token3 = lexicalParser.getWord(false); // 如果变量定义结束就是：(
    lexicalParser.setFilepos(record);
    lexicalParser.setLastPos(llPos);
    return (token1 == "int" || token1 == "char") && token3 != "(";
}

bool SyntaxParser::isConstState() {
    long record = lexicalParser.getFilepos();
    string token;
    token = lexicalParser.getWord(false);
    {
        lexicalParser.setFilepos(record);
        return token == "const";
    }
}

bool SyntaxParser::isMain() {
    long record = lexicalParser.getFilepos();
    string type;
    string name;
    type = lexicalParser.getWord(false); // void
    name = lexicalParser.getWord(false); // main
    {
        lexicalParser.setFilepos(record);
        return type == "void" && name == "main";
    }
}

void SyntaxParser::genTmpCode() {
    // 生成中间代码
    string output; // 输出内容
    for (auto &it : tmpCodeVector) {
        switch (it.type) {
            case AssignState: // 赋值语句
                if (it.arrayOrVar == 0) {
                    output.append(it.target + "[" + it.index + "]"); // 标识符
                } else {
                    output.append(it.target);
                }
                output.append(it.op + " "); // =
                output.append(it.right); // 表达式的值
                tmpCode << output << endl;
                break;
            case Label: // 标签
                output.append(it.target);// 标签输出
                output.append(":"); // 添加冒号
                tmpCode << output << endl;
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
                tmpCode << output << endl;
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
                tmpCode << output << endl;
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
                tmpCode << output << endl;
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
                tmpCode << output << endl;
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
                    tmpCode << output << endl;
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
                tmpCode << output << endl;
                break;
            case Jump: // 无条件跳转
                output.append("Goto ");
                output.append(it.target); // Label
                tmpCode << output << endl;
                break;
            case BEZ: // 等于
                output.append("BEZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                tmpCode << output << endl;
                break;
            case BNZ: // 不等于
                output.append("BNZ "); // Conditon
                output.append(it.left + ", ");
                output.append(it.target); // Label
                tmpCode << output << endl;
                break;
            case BLZ: // 小于
                output.append("BLZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                tmpCode << output << endl;
                break;
            case BLEZ: // 小于等于
                output.append("BLEZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                tmpCode << output << endl;
                break;
            case BGZ: // 大于
                output.append("BGZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                tmpCode << output << endl;
                break;
            case BGEZ: // 大于等于
                output.append("BGEZ "); // Conditon
                output.append(it.left + ", "); // 接受判断的寄存器
                output.append(it.target); // Label
                tmpCode << output << endl;
                break;
            case ReadChar: // 读入一个字符
                output.append("Rear Char ");
                output.append(it.target);
                tmpCode << output << endl;
                break;
            case ReadInt: // 读入一个整数
                output.append("Read Int ");
                output.append(it.target);
                tmpCode << output << endl;
                break;
            case PrintStr: // 打印字符串
                output.append("Print String ");
                output.append(it.target);
                tmpCode << output << endl;
                break;
            case PrintInt: // 打印整型
                output.append("Print Int ");
                output.append(it.target);
                tmpCode << output << endl;
                break;
            case PrintChar: // 打印字符
                output.append("Print Char ");
                output.append(it.target);
                tmpCode << output << endl;
                break;
            case Return: // 返回语句
                if (it.valueType == Void || it.valueType == NullType) {
                    tmpCode << "RET" << endl; // 直接返回
                    break;
                } else {
                    output.append("RET ");
                    output.append(it.target);
                    tmpCode << output << endl;
                }
                break;
            case FunctionCall:
                // 函数调用
                // 函数值保存在left中
                for (auto &it1 : it.valueParaTab) {
                    // Push x
                    tmpCode << "Push " + it1.name << endl;
                }
                tmpCode << "Call " + it.target << endl; // 函数调用语句
                tmpCode << it.left + " = RET" << endl; // 返回值
                break;
            default:
                break;
        }
        // 清空输出字符串
        output.clear();
    }
}






