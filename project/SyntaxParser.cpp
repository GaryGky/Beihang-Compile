//
// Created by dell on 2019/10/10.
//

#include "SyntaxParser.h"
#include <string>


SyntaxParser::SyntaxParser(LexicalParser &lex) : lexicalParser(lex) {
    // curString = ""; // 初始化token
}

void SyntaxParser::start() {
    constState();
    varState();
    funcState();
    mainParse();
    lexicalParser.output("<程序>");
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
        token = lexicalParser.getWord(); // ; 分号
    }
    lexicalParser.output("<常量说明>");
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
        lexicalParser.output("ERROR IN CONSTDEF");
        exit(-1); // 报错
    }
    lexicalParser.output("<常量定义>");
}

void SyntaxParser::constIntDef() {
    string token;
    while (true) {
        if (lexicalParser.peekTop() == ';') {
            break;
        }
        token = lexicalParser.getWord(); // 标识符
        token = lexicalParser.getWord(); // =
        char c = lexicalParser.peekTop();
        if (c == '+' || c == '-') {
            token = lexicalParser.getWord(); // +/-
        }
        token = lexicalParser.getWord(); // 整数
        lexicalParser.output("<无符号整数>");
        lexicalParser.output("<整数>");
        if (lexicalParser.peekTop() == ';') {
            break;
        }
        token = lexicalParser.getWord(); // , 逗号
    }
}

void SyntaxParser::constCharDef() {
    string token;
    while (true) {
        if (lexicalParser.peekTop() == ';') {
            break;
        }
        token = lexicalParser.getWord(); // 标识符或 ;
        token = lexicalParser.getWord(); // =
        token = lexicalParser.getWord(); // 字符 : 词法分析自动去掉单引号
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
    lexicalParser.output("<变量说明>");
}

void SyntaxParser::varDef() {
    string token = lexicalParser.getWord(); // 类型标识符 int 或// char;
    char c;
    while (true) {
        token = lexicalParser.getWord(); // 标识符
        c = lexicalParser.peekTop();
        if (c == ',') {
            token = lexicalParser.getWord();
        } else if (c == ';') {
            break;
        } else if (c == '[') {
            token = lexicalParser.getWord();
            token = lexicalParser.getWord(); // 无符号整数
            lexicalParser.output("<无符号整数>");
            token = lexicalParser.getWord(); // 右括号
        }
        c = lexicalParser.peekTop();
        if (c == ';') {
            break;
        }
    }
    lexicalParser.output("<变量定义>");
    token = lexicalParser.getWord(); // 输出分号
}

bool SyntaxParser::isMain() {
    long record = lexicalParser.getFilepos();
    string type;
    string name;
    type = lexicalParser.getWord(false);
    name = lexicalParser.getWord(false);
    {
        lexicalParser.setFilepos(record);
        return type == "void" && name == "main";
    }
}

void SyntaxParser::funcState() {
    // 函数说明
    string token;
    string tmp;
    string name;
    while (true) {
        // 不断的定义函数
        if (isMain()) {
            break;
        }
        tmp = lexicalParser.getWord(); // 函数类型
        name = lexicalParser.getWord(); // 函数名称
        if (tmp != "void") lexicalParser.output("<声明头部>");
        token = lexicalParser.getWord(); // (
        paraParse(); // 参数表解析
        // token = lexicalParser.getWord(); // )
        token = lexicalParser.getWord(); // {
        comStateParse(); // 复合语句
        token = lexicalParser.getWord(); // }
        if (tmp == "void") {
            lexicalParser.output("<无返回值函数定义>");
            lexicalParser.addFunc(name, 0);
        } else {
            lexicalParser.output("<有返回值函数定义>");
            lexicalParser.addFunc(name, 1);
        }
    }
}

void SyntaxParser::paraParse() {
    string token;
    while (true) {
        token = lexicalParser.getWord(false); // 类型标识符
        if (token == ")") {
            break; // 参数表有可能为空
        }
        lexicalParser.output(token);
        token = lexicalParser.getWord(); // 标识符
        token = lexicalParser.getWord(false); // , 或 )
        if (token == ")") {
            break;
        }
        lexicalParser.output(token);
    }
    lexicalParser.output("<参数表>");
    lexicalParser.output(token);
}

void SyntaxParser::stateColParse() {
    string token;
    char c = lexicalParser.peekTop();
    while (true) {
        if (c == '}') {
            lexicalParser.output("<语句列>");
            return; // 语句列的分析结束（也标志着复合语句的分析结束）
        }
        stateParse();
        c = lexicalParser.peekTop();
    }
}

void SyntaxParser::stateParse() {
    string token;
    token = lexicalParser.getWord();
    if (token == "if") {
        // IF
        ifParse();
        lexicalParser.output("<语句>");
        return;
    } else if (token == "while") {
        // while
        whileParse();
        lexicalParser.output("<语句>");
        return;
    } else if (token == "for") {
        forParse();
        lexicalParser.output("<语句>");
        return;
    } else if (token == "do") {
        // do-while
        doWhileParse();
        lexicalParser.output("<语句>");
        return;
    } else if (token == "scanf") {
        //scanf
        scanfParse();
    } else if (token == "printf") {
        // printf
        printfParse();
    } else if (token == "return") {
        //return
        returnParse();
    } else if (token == ";") {
        lexicalParser.output("<语句>");
        return;
    } else if (token == "{") {
        // {语句}
        stateColParse();
        token = lexicalParser.getWord(); // }
        lexicalParser.output("<语句>");
        return;
    } else {
        // 赋值语句 有返回值的函数调用 无返回值的函数调用
        // 赋值语句不能在函数表中找到标识符
        if (lexicalParser.isVoidFunc(token)) {
            // 无返回值函数调用
            funcRefParse(0);
        } else if (lexicalParser.isRetFunc(token)) {
            // 有返回值的函数调用
            funcRefParse(1);
        } else {
            // 赋值语句
            assignParse();
        }
    }
    token = lexicalParser.getWord(); // 吃掉分号
    lexicalParser.output("<语句>");
}

void SyntaxParser::condParse() {
    char c;
    string token;
    exprParse();
    c = lexicalParser.peekTop();
    if (c == ';' || c == ')') {
        lexicalParser.output("<条件>");
        return;
    }
    token = lexicalParser.getWord(); // 关系运算符
    exprParse();
    lexicalParser.output("<条件>");
}

void SyntaxParser::ifParse() {
    string token;
    token = lexicalParser.getWord(); // (
    condParse();
    token = lexicalParser.getWord(); // )
    stateParse();
    // 判断有无else
    token.clear();
    for (int i = 0; i < 5; i++) {
        token += lexicalParser.peekTop(i);
    }
    if (token.substr(0, 4) == "else" && !isalnum(token[5]) && token[5] != '_') {
        token = lexicalParser.getWord(); // else
        stateParse();
    }
    lexicalParser.output("<条件语句>");
}

void SyntaxParser::whileParse() {
    string token;
    token = lexicalParser.getWord(); // (
    condParse();
    token = lexicalParser.getWord(); // )
    stateParse();
    lexicalParser.output("<循环语句>");
}

void SyntaxParser::forParse() {
    string token;
    token = lexicalParser.getWord(); // (
    token = lexicalParser.getWord(); // 标识符
    token = lexicalParser.getWord(); // =
    exprParse(); // 表达式
    token = lexicalParser.getWord(); // ;
    condParse();
    token = lexicalParser.getWord(); // ;
    token = lexicalParser.getWord(); // 标识符
    token = lexicalParser.getWord(); // =
    token = lexicalParser.getWord(); // 标识符
    token = lexicalParser.getWord(); // +/-
    token = lexicalParser.getWord();// 数字
    lexicalParser.output("<无符号整数>");
    lexicalParser.output("<步长>");
    token = lexicalParser.getWord(); // )
    stateParse();
    lexicalParser.output("<循环语句>");
}

void SyntaxParser::doWhileParse() {
    stateParse();
    string token;
    token = lexicalParser.getWord(); // while
    token = lexicalParser.getWord(); // (
    condParse(); // 条件
    token = lexicalParser.getWord(); // )
    lexicalParser.output("<循环语句>");
}

void SyntaxParser::scanfParse() {
    string token;
    token = lexicalParser.getWord(); // (
    while (true) {
        token = lexicalParser.getWord();
        if (lexicalParser.peekTop() == ')') {
            break;
        }
        token = lexicalParser.getWord(); // ,
    }
    token = lexicalParser.getWord(); // )
    lexicalParser.output("<读语句>");
}

void SyntaxParser::printfParse() {
    string token;
    char c;
    token = lexicalParser.getWord(); // (
    c = lexicalParser.peekTop();
    if (c == '\"') {
        token = lexicalParser.getWord(); // 读取字符串
        c = lexicalParser.peekTop();
        if (c == ',') {
            token = lexicalParser.getWord(); // ,
            exprParse();
        }
    } else {
        exprParse();
    }
    token = lexicalParser.getWord(); // )
    lexicalParser.output("<写语句>");
}

void SyntaxParser::returnParse() {
    string token;
    char c;
    c = lexicalParser.peekTop();
    if (c == ';') {
        lexicalParser.output("<返回语句>");
        return;
    }
    token = lexicalParser.getWord(); // (
    exprParse();
    token = lexicalParser.getWord();// )
    lexicalParser.output("<返回语句>");
}

void SyntaxParser::assignParse() {
    char c;
    string token;
    c = lexicalParser.peekTop();
    if (c == '[') {
        token = lexicalParser.getWord(); // [
        exprParse();
        token = lexicalParser.getWord(); // ]
    }
    token = lexicalParser.getWord(); // =
    exprParse();
    lexicalParser.output("<赋值语句>");
}

void SyntaxParser::funcRefParse(int flag) {
    string token;
    token = lexicalParser.getWord(); // (
    valueTabParse(); // 值参数表解析
    token = lexicalParser.getWord(); // )
    if (flag == 0) {
        lexicalParser.output("<无返回值函数调用语句>");
    } else {
        lexicalParser.output("<有返回值函数调用语句>");
    }
}

void SyntaxParser::valueTabParse() {
    string token;
    if (lexicalParser.peekTop() == ')') {
        lexicalParser.output("<值参数表>");
        return; // 值参数表为空的情况
    }
    while (true) {
        // 解析：{[+-]expr}
        exprParse();
        // 判断值参数表是否结束
        if (lexicalParser.peekTop() == ')') {
            break;
        }
        token = lexicalParser.getWord(); // ,
    }
    lexicalParser.output("<值参数表>");
}

void SyntaxParser::exprParse() {
    char c;
    string token;
    c = lexicalParser.peekTop();
    if (c == '+' || c == '-') {
        token = lexicalParser.getWord(); // + / -
    }
    while (true) {
        termParse();
        c = lexicalParser.peekTop();
        if (c != '+' && c != '-') {
            break;
        }
        token = lexicalParser.getWord(); // + / -
    }
    lexicalParser.output("<表达式>");
}

void SyntaxParser::termParse() {
    char c;
    string token;
    c = lexicalParser.peekTop();
    while (true) {
        factorParse();
        c = lexicalParser.peekTop();
        if (c != '*' && c != '/') {
            break;
        } // 如果不是乘法运算符则输出
        token = lexicalParser.getWord();
    }
    lexicalParser.output("<项>");
}

void SyntaxParser::factorParse() {
    char c;
    string token;
    c = lexicalParser.peekTop();
    if (c == '(') {
        token = lexicalParser.getWord();
        exprParse();
        token = lexicalParser.getWord(); // ]
    } else if (isdigit(c)) {
        token = lexicalParser.getWord();// 数字
        lexicalParser.output("<无符号整数>");
        lexicalParser.output("<整数>");
    } else if (c == '+' || c == '-') {
        token = lexicalParser.getWord();
        token = lexicalParser.getWord();// 数字
        lexicalParser.output("<无符号整数>");
        lexicalParser.output("<整数>");
    } else if (c == '\'') {
        token = lexicalParser.getWord(); // 字符
    } else {
        token = lexicalParser.getWord(); // 标识符
        c = lexicalParser.peekTop();
        if (c == '[') {
            token = lexicalParser.getWord();
            exprParse();
            token = lexicalParser.getWord(); // ]
        } else if (c == '(') {
            funcRefParse(1);
        } else {
            // 单独的标识符因子，不做处理
        }
    }
    lexicalParser.output("<因子>");
}

void SyntaxParser::mainParse() {
    string token;
    token = lexicalParser.getWord(); // void
    token = lexicalParser.getWord(); // main
    token = lexicalParser.getWord(); // (
    token = lexicalParser.getWord(); // )
    token = lexicalParser.getWord(); // {
    char c = lexicalParser.peekTop();
    if (c == '}') {
        token = lexicalParser.getWord(); // }
        lexicalParser.output("<主函数>");
    }
    comStateParse();
    token = lexicalParser.getWord(); // }
    lexicalParser.output("<主函数>");
}

void SyntaxParser::comStateParse() {
    constState(); // 常量说明
    varState(); // 变量说明
    stateColParse(); // 语句列
    lexicalParser.output("<复合语句>");
}

bool SyntaxParser::isVarState() {
    long record = lexicalParser.getFilepos();
    string token1 = lexicalParser.getWord(false); // int
    string token2 = lexicalParser.getWord(false); // change
    string token3 = lexicalParser.getWord(false); // 如果变量定义结束就是：(
    lexicalParser.setFilepos(record);
    return (token1 == "int" || token1 == "char") && token3 != "(";
}

bool SyntaxParser::isConstState() {
    long record = lexicalParser.getFilepos();
    string token;
    token = lexicalParser.getWord(false);
    if (token == "const") {
        lexicalParser.setFilepos(record);
        return true;
    } else {
        lexicalParser.setFilepos(record);
        return false;
    }
}


