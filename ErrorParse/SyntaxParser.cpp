//
// Created by dell on 2019/10/10.
//

#include "SyntaxParser.h"

#include <utility>
#include <iostream>


SyntaxParser::SyntaxParser(LexicalParser &lex, ErrorParse &parse, SymbolTabel &symbolTabel1) :
        lexicalParser(lex), errorParse(parse), symbolTabel(symbolTabel1) {
    level = 0; // 初始化为0层 最外层的全局变量
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
        if (lexicalParser.peekTop() == ';') {
            token = lexicalParser.getWord(); // ; 分号
        }
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
    vector<SymbolItem> tmp; // 构造一个空引用放到符号表中占位置
    string token;
    string iden;
    while (true) {
        iden = lexicalParser.getWord(); // 标识符
        token = lexicalParser.getWord(); // =
        char c = lexicalParser.peekTop();
        if (c == '+' || c == '-') {
            token = lexicalParser.getWord(); // +/-
            c = lexicalParser.peekTop();
            if (isdigit(c)) {
                token.append(lexicalParser.getWord()); // 得到一个带正负号的整数
                SymbolItem item(Constant, iden, level, Integer, token, tmp); // 新建一个符号项
                symbolTabel.pushSymTabel(item); // 将符号压入符号表
            } else {
                // 考虑到只有一个加号的错误
                errorParse.syntaxParseError(lineNo, ConstDefError);
            }
        } else if (isdigit(c)) {
            token = lexicalParser.getWord(); // 得到一个无符号整数
            SymbolItem item(Constant, iden, level, Integer, token, tmp); // 新建一个符号项
            symbolTabel.pushSymTabel(item); // 将符号压入符号表
        } else {
            lexicalParser.toNextSemi();
            errorParse.syntaxParseError(lineNo, ConstDefError); // 只能是整型常量
        }
        lexicalParser.output("<无符号整数>");
        lexicalParser.output("<整数>");
        long tmpLine = lineNo;
        if (lexicalParser.peekTop() == ';') {
            break;
        } else {
            token = lexicalParser.getWord(); // 取一个单词进行判断
            if (token == "const" || token == "int" || token == "char" || token == "void") {
                errorParse.syntaxParseError(tmpLine, LackSemicn);
                lexicalParser.retract();
                break;
            }
            lexicalParser.retract(); // 退回一个单词
        }
        token = lexicalParser.getWord(); // , 逗号
    }
}

bool isConstChar(char ch) {
    return isalnum(ch) || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_';
}

void SyntaxParser::constCharDef() {
    string token;
    string iden;
    vector<SymbolItem> tmp;
    while (true) {
        iden = lexicalParser.getWord(); // 标识符
        token = lexicalParser.getWord(); // =
        // 提前判断三个字符
        char char1, char2, char0;
        char0 = lexicalParser.peekTop(0);
        char1 = lexicalParser.peekTop(1);
        char2 = lexicalParser.peekTop(2);
        if (char0 == '\'' && char2 == '\'' && isConstChar(char1)) {
            token = lexicalParser.getWord(); // 字符 : 词法分析自动去掉单引号
            SymbolItem item(Constant, iden, level, Character, token, tmp); // 新建一个符号项
            symbolTabel.pushSymTabel(item); // 将符号压入符号表, 填表前自动查表
        } else if (char0 == '\'' && char2 == '\'' && !isConstChar(char1)) {
            lexicalParser.toNextSemi();
            errorParse.syntaxParseError(lineNo, LexError);
        } else {
            lexicalParser.toNextSemi();
            errorParse.syntaxParseError(lineNo, ConstDefError);
        }
        long tmpLine = lineNo; // 记录一下 trim 前的位置
        if (lexicalParser.peekTop() == ';') {
            break;
        } else {
            token = lexicalParser.getWord();
            if (token == "const" || token == "int" || token == "char" || token == "void") {
                errorParse.syntaxParseError(tmpLine, LackSemicn);
                lexicalParser.retract();
                break;
            }
            lexicalParser.retract(); // 退回一个单词
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
    string token; // 类型标识符 int 或// char;
    string type = lexicalParser.getWord(); // 类型标识符
    char c;
    vector<SymbolItem> tmp;
    while (true) {
        token = lexicalParser.getWord(); // 标识符
        string value;
        long tmpLine = lineNo;
        if (type == "int") {
            SymbolItem item(Variable, token, level, Integer, value, tmp);
            symbolTabel.pushSymTabel(item);
        } else if (type == "char") {
            SymbolItem item(Variable, token, level, Character, value, tmp);
            symbolTabel.pushSymTabel(item);
        }
        c = lexicalParser.peekTop();
        if (c == ',') {
            token = lexicalParser.getWord();
        } else if (c == ';') {
            break;
        } else if (c == '[') {
            token = lexicalParser.getWord();
            token = lexicalParser.getWord(); // 无符号整数
            lexicalParser.output("<无符号整数>");
            c = lexicalParser.peekTop();
            if (c == ']') {
                token = lexicalParser.getWord(); // ]
            } else {
                // 错误处理，缺少右括号
                errorParse.syntaxParseError(lineNo, LackRbrack);
            }
            c = lexicalParser.peekTop();
            if (c == ';') {
                break;
            } else if (c == ',') {
                token = lexicalParser.getWord(); // 吃掉逗号才行
            }
            // 以下判断为错误处理
        } else {
            long pos = lexicalParser.getFilepos();
            token = lexicalParser.getWord(); // 会吃掉一个单词
            pos = lexicalParser.getFilepos();
            if (token == "int" || token == "char" || !isVarState()) {
                // 缺少分号会直接读到int,char
                lexicalParser.retract();
                pos = lexicalParser.getFilepos();
                errorParse.syntaxParseError(tmpLine, LackSemicn);
                return;
            } else {
                cout << "impossible" << token << endl;
            }
        }
    }
    lexicalParser.output("<变量定义>");
    if (lexicalParser.peekTop() == ';') {
        token = lexicalParser.getWord(); // 输出分号
    }
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
            break;
        }
        type = lexicalParser.getWord(); // 函数类型
        name = lexicalParser.getWord(); // 函数名称
        if (symbolTabel.checkIdenDef(name) != NullType) {
            // 函数名称已经定义过
            errorParse.semanticError(lineNo, Redeclare);
        }
        if (type != "void") lexicalParser.output("<声明头部>");
        token = lexicalParser.getWord(); // (
        level++;
        paraVector = paraParse(); // 参数表解析 : 要吃掉右小括号
        level--; // 此处level的变化是为了将参数与函数层次分开
        string value1;
        if (type == "int") {
            retType = Integer;
            value1 = to_string(paraVector.size());
            SymbolItem item(Function, name, level, Integer, value1, paraVector);
            symbolTabel.pushSymTabel(item);
        } else if (type == "char") {
            retType = Character;
            value1 = to_string(paraVector.size());
            SymbolItem item(Function, name, level, Character, value1, paraVector);
            symbolTabel.pushSymTabel(item);
        } else {
            value1 = to_string(paraVector.size());
            retType = Void;
            SymbolItem item(Function, name, level, Void, value1, paraVector);
            symbolTabel.pushSymTabel(item);
        }
        for (const auto &it : paraVector) {
            symbolTabel.pushSymTabel(it);
        }
        level++; // level 变成1
        token = lexicalParser.getWord(); // {
        comStateParse(retType); // 复合语句: 复合语句中会有局部变量和局部常量的声明
        token = lexicalParser.getWord(); // }
        symbolTabel.dropSymTabel(level); // 清空这层函数的局部变量
        level--;
        if (type == "void") {
            lexicalParser.output("<无返回值函数定义>");
            lexicalParser.addFunc(name, 0);
        } else {
            lexicalParser.output("<有返回值函数定义>");
            lexicalParser.addFunc(name, 1);
        }
    }
}

vector<SymbolItem> SyntaxParser::paraParse() {
    string token;
    string type;
    vector<SymbolItem> paraVector;
    char c;
    while (true) {
        c = lexicalParser.peekTop();
        if (c == '{') {
            errorParse.syntaxParseError(lineNo, LackRparent); // 缺少 )
            break;

        } else if (c == ')') {
            token = lexicalParser.getWord();
            break; // 小括号：参数表为空，是正常情况
        } else {
            // 剩余情况默认为标识符
            type = lexicalParser.getWord(false); // 类型标识符
        }
        // lexicalParser.output(token);
        token = lexicalParser.getWord(); // 标识符
        vector<SymbolItem> tmp;
        string value = to_string(lineNo); // 把行数信息存到value中
        if (type == "int") {
            SymbolItem item(Parament, token, level, Integer, value, tmp);
            paraVector.push_back(item);
        } else if (type == "char") {
            SymbolItem item(Parament, token, level, Character, value, tmp);
            paraVector.push_back(item);
        } else {
            lexicalParser.output("参数表解析时参数类型错误");
            exit(-1);
        }
        c = lexicalParser.peekTop(); // 正常情况下应为 ) ,
        if (c == ')') {
            token = lexicalParser.getWord();
            break; // 正常退出参数表
        } else if (c == ',') {
            token = lexicalParser.getWord();
        } else {
            // 错误处理: 缺少右括号
            errorParse.syntaxParseError(lineNo, LackRparent);
        }
    }
    lexicalParser.output("<参数表>");
    lexicalParser.output(token);
    return paraVector;
}

void SyntaxParser::stateColParse(RetType retType) {
    string token;
    bool ownsRet = false;
    char c = lexicalParser.peekTop();
    while (true) {
        if (c == '}') {
            lexicalParser.output("<语句列>");
            break; // 语句列的分析结束（也标志着复合语句的分析结束）
        }
        stateParse((bool &) ownsRet, retType);
        c = lexicalParser.peekTop();
    }
    if (!ownsRet && retType != Void) {
        // 有返回值的函数定义中没有出现return
        errorParse.returnError(lineNo, lackReturn);
    }
}

void SyntaxParser::stateParse(bool &ownsRet, RetType retType) {
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
        RetType rpRet;
        rpRet = returnParse();

        // 以下为函数返回值类型相关的判断
        if (retType == Void && rpRet != Void) {
            // 无返回值函数存在不匹配的return
            errorParse.returnError(lineNo, voidReturn);
        } else if (retType == Integer) {
            // 有返回值函数存在不匹配的return
            if (rpRet != Integer) {
                errorParse.returnError(lineNo, retTypeError);
            } else {
                ownsRet = true;
            }
        } else if (retType == Character) {
            // 有返回值函数存在return类型不匹配的情况
            if (rpRet != Character) {
                errorParse.returnError(lineNo, retTypeError);
            } else {
                ownsRet = true;
            }
        }
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
        RetType ridenDef = symbolTabel.checkIdenDef(token); // 检查标识符是否定义
        if (ridenDef == NullType) {
            errorParse.semanticError(lineNo, Nodeclare); // 输出了未定义之后还必须往下走
            char c = lexicalParser.peekTop();
            if (c == '(') {
                long look = lineNo;
                // 函数调用语句
                funcRefParse(-1, token);
                long tmpLine = lineNo; // peek 有可能导致换行，所以暂时记录行号
                c = lexicalParser.peekTop();
                if (c != ';') {
                    errorParse.syntaxParseError(tmpLine, LackSemicn); // 缺少分号
                } else {
                    token = lexicalParser.getWord(); // 吃掉分号
                    lexicalParser.output("<语句>");
                }
                return;
            }
        }
        if (lexicalParser.isVoidFunc(token)) {
            // 无返回值函数调用
            funcRefParse(0, token);
        } else if (lexicalParser.isRetFunc(token)) {
            // 有返回值的函数调用
            funcRefParse(1, token);
        } else {
            // 赋值语句
            if (ridenDef != NullType) {
                // 如果标识符未定义则跳过查表
                SymbolItem assignObj = symbolTabel.getSymbol(token);
                if (assignObj.getItemType() == Constant) {
                    // 如果标识符为常量，语义错误
                    errorParse.semanticError(lineNo, ConstAssError); // 对常量赋值
                }
            }
            assignParse();
        }
    }
    long tmpLine = lexicalParser.getLineNo();
    char c = lexicalParser.peekTop();
    if (c != ';') {
        errorParse.syntaxParseError(tmpLine, LackSemicn); // 缺少分号
    } else {
        token = lexicalParser.getWord(); // 吃掉分号
        lexicalParser.output("<语句>");
    }
}

void SyntaxParser::condParse() {
    char c;
    string token;
    bool isIntExpr;
    isIntExpr = exprParse();
    if (!isIntExpr) {
        // 条件判断非法
        errorParse.semanticError(lineNo, CondParseError);
    }
    c = lexicalParser.peekTop();
    if (c != '!' && c != '<' && c != '>' && c != '=') {
        // 如果不是关系运算符，则直接结束
        lexicalParser.output("<条件>");
        return;
    }
    token = lexicalParser.getWord(); // 关系运算符
    isIntExpr = exprParse();
    if (!isIntExpr) {
        // 条件判断非法
        errorParse.semanticError(lineNo, CondParseError);
    }
    lexicalParser.output("<条件>");
}

void SyntaxParser::ifParse() {
    string token;
    char c;
    token = lexicalParser.getWord(); // (
    condParse();
    c = lexicalParser.peekTop();
    if (c == ')') {
        // 正常情况
        token = lexicalParser.getWord(); // )
    } else {
        // 错误处理：缺少小括号
        errorParse.syntaxParseError(lineNo, LackRparent);
    }
    bool tmp;
    stateParse(tmp);
    // 判断有无else
    token.clear();
    for (int i = 0; i < 5; i++) {
        token += lexicalParser.peekTop(i);
    }
    if (token.substr(0, 4) == "else" && !isalnum(token[5]) && token[5] != '_') {
        token = lexicalParser.getWord(); // else
        stateParse(tmp);
    }
    lexicalParser.output("<条件语句>");
}

void SyntaxParser::whileParse() {
    string token;
    bool tmp;
    token = lexicalParser.getWord(); // (
    condParse();
    char c = lexicalParser.peekTop();
    if (c == ')') {
        token = lexicalParser.getWord(); // )
    } else {
        // 错误处理：缺少小括号
        errorParse.syntaxParseError(lineNo, LackRparent);
    }
    stateParse(tmp);
    lexicalParser.output("<循环语句>");
}

void SyntaxParser::forParse() {
    string token;
    RetType retType;
    char c;
    token = lexicalParser.getWord(); // (
    token = lexicalParser.getWord(); // 标识符
    retType = symbolTabel.checkIdenDef(token); // 检查标识符是否定义
    if (retType == NullType) {
        errorParse.semanticError(lineNo, Nodeclare);
    }
    token = lexicalParser.getWord(); // =
    exprParse(); // 表达式
    if (lexicalParser.peekTop() != ';') {
        errorParse.syntaxParseError(lineNo, LackSemicn);
    } else {
        token = lexicalParser.getWord(); // ;
    } // 错误处理：可能缺少分号
    condParse();
    if (lexicalParser.peekTop() != ';') {
        errorParse.syntaxParseError(lineNo, LackSemicn);
    } else {
        token = lexicalParser.getWord(); // ;
    } // 错误处理：可能缺少分号
    token = lexicalParser.getWord(); // 标识符
    retType = symbolTabel.checkIdenDef(token); // 检查标识符是否定义
    if (retType == NullType) {
        errorParse.semanticError(lineNo, Nodeclare);
    } // 检查标识符是否定义
    token = lexicalParser.getWord(); // =
    token = lexicalParser.getWord(); // 标识符
    retType = symbolTabel.checkIdenDef(token); // 检查标识符是否定义
    if (retType == NullType) {
        errorParse.semanticError(lineNo, Nodeclare);
    } // 检查标识符是否定义
    token = lexicalParser.getWord(); // +/-
    token = lexicalParser.getWord();// 数字
    lexicalParser.output("<无符号整数>");
    lexicalParser.output("<步长>");
    if (lexicalParser.peekTop() == ')') {
        // 正常情况：以括号结尾
        token = lexicalParser.getWord(); // )
    } else {
        // 错误处理：缺少小括号
        errorParse.syntaxParseError(lineNo, LackRparent);
    }
    bool tmp;
    stateParse(tmp);
    lexicalParser.output("<循环语句>");
}

void SyntaxParser::doWhileParse() {
    bool tmp;
    stateParse(tmp);
    string token;
    token = lexicalParser.getWord(); // while
    if (token != "while") {
        // 语法分析器要回退
        lexicalParser.retract();
        errorParse.syntaxParseError(lineNo, LackWhile);
    } else {
        // 如果等于while才能进行后续的分析
        token = lexicalParser.getWord(); // (
        condParse(); // 条件
        char c = lexicalParser.peekTop();
        if (c == ')') {
            token = lexicalParser.getWord(); // )
        } else {
            // 错误处理，缺少小括号
            errorParse.syntaxParseError(lineNo, LackRparent);
        }
    }
    lexicalParser.output("<循环语句>");
}

void SyntaxParser::scanfParse() {
    string token;
    RetType retType;
    token = lexicalParser.getWord(); // (
    while (true) {
        token = lexicalParser.getWord(); // 标识符
        retType = symbolTabel.checkIdenDef(token); // 检查标识符是否定义
        if (retType == NullType) {
            errorParse.semanticError(lineNo, Nodeclare);
        } // 检查标识符是否定义
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

RetType SyntaxParser::returnParse() {
    string token;
    char c;
    c = lexicalParser.peekTop();
    if (c == ';') {
        lexicalParser.output("<返回语句>");
        return Void;
    } else if (c == '(') {
        token = lexicalParser.getWord(); // (
        bool isInt = exprParse();
        c = lexicalParser.peekTop();
        if (c == ')') {
            token = lexicalParser.getWord();// )
        } else {
            // 错误处理，缺少右小括号
            errorParse.syntaxParseError(lineNo, LackRparent);
        }
        lexicalParser.output("<返回语句>");
        if (isInt) {
            return Integer;
        } else {
            return Character;
        }
    } else {
        // 缺少分号的情况
        return Void; // 默认缺省为void
    }
}

void SyntaxParser::assignParse() {
    char c;
    string token;
    c = lexicalParser.peekTop();
    if (c == '[') {
        token = lexicalParser.getWord(); // [
        bool isInt = exprParse();
        if (!isInt) {
            errorParse.syntaxParseError(lineNo, arrayDefError); // 数组下标只能为整型
        }
        c = lexicalParser.peekTop();
        if (c == ']') {
            token = lexicalParser.getWord(); // ]
        } else {
            // 错误处理, 缺少右括号
            errorParse.syntaxParseError(lineNo, LackRbrack);
        }
    }
    token = lexicalParser.getWord(); // =
    exprParse();
    lexicalParser.output("<赋值语句>");
}

void SyntaxParser::funcRefParse(int flag, string funcName) {
    // flag = -1: 表示函数名称未定义
    string token;
    token = lexicalParser.getWord(); // (
    if (flag != -1) {
        // 函数名称未定义时，跳过值参数表的解析
        valueTabParse(std::move(funcName)); // 值参数表解析
    } else {
        // 函数未定义时的参数表解析
        excValueTabParse();
    }
    if (flag == 0) {
        lexicalParser.output("<无返回值函数调用语句>");
    } else {
        lexicalParser.output("<有返回值函数调用语句>");
    }
}

void SyntaxParser::excValueTabParse() {
    // 异常参数表解析
    string token;
    bool lackRparnt = false;
    if (lexicalParser.peekTop() == ')') {
        lexicalParser.output("<值参数表>");
        return; // 值参数表为空的情况
    }
    while (true) {
        // 解析：{[+-]expr}
        exprParse();
        // 判断值参数表是否结束
        char c = lexicalParser.peekTop();
        if (c == ')') {
            break; // 正常退出
        } else if (c == ',') {
            token = lexicalParser.getWord(); // , 继续解析值参数
        } else {
            lackRparnt = true;
            errorParse.syntaxParseError(lineNo, LackRparent);
            break;
        }
    }
    lexicalParser.output("<值参数表>");
    if (!lackRparnt) {
        token = lexicalParser.getWord(); // 吃掉括号
    }
}

void SyntaxParser::valueTabParse(string funcName) {
    string token;
    int paraNum = 0; // 记录参数的个数
    bool isIntExpr; // 表示是否为整型表达式
    vector<SymbolItem> paraVector = symbolTabel.getFuncPara(funcName); // 获取该函数的参数表
    if (lexicalParser.peekTop() == ')') {
        // 值参数表为空
        lexicalParser.output("<值参数表>");
        token = lexicalParser.getWord(); // 吃掉括号
        if (paraNum != paraVector.size()) {
            errorParse.semanticError(lineNo, ParaNumError); // 函数参数个数不对
        }
        return; // 值参数表为空的情况
    }

    vector<RetType> valueParaVec; // 记录值参数表中参数的类型
    bool lackRparnt = false; // 标识一下是否缺少右小括号
    while (true) {
        // 解析：{[+-]expr}
        isIntExpr = exprParse();
        if (isIntExpr) {
            valueParaVec.push_back(Integer); // 整型表达式
        } else {
            valueParaVec.push_back(Character); // 非整型表达式
        }
        paraNum++;
        // 判断值参数表是否结束
        char c = lexicalParser.peekTop();
        if (c == ')') {
            break; // 正常退出
        } else if (c == ',') {
            token = lexicalParser.getWord(); // , 继续解析值参数
        } else {
            lackRparnt = true;
            errorParse.syntaxParseError(lineNo, LackRparent);
            break;
        }
    }

    if (paraNum != paraVector.size()) {
        errorParse.semanticError(lineNo, ParaNumError); // 函数参数个数不对
    } else {
        for (int i = 0; i < paraNum; i++) {
            if (valueParaVec[i] != paraVector[i].getRetType()) {
                errorParse.semanticError(lineNo, ParaTypeError); // 函数参数类型不符
            }
        }
    }
    lexicalParser.output("<值参数表>");
    if (!lackRparnt) {
        token = lexicalParser.getWord(); // )
    }
}

bool SyntaxParser::exprParse() {
    bool isIntExpr = false; // 是否为整型表达式
    bool isIntTerm; // 看项是不是整型的
    char c;
    string token;
    c = lexicalParser.peekTop();
    if (c == '+' || c == '-') {
        token = lexicalParser.getWord(); // + / -
    }
    while (true) {
        isIntTerm = termParse();
        if (isIntTerm && !isIntExpr) {
            isIntExpr = true;
        }
        c = lexicalParser.peekTop();
        if (c != '+' && c != '-') {
            break;
        }
        token = lexicalParser.getWord(); // + / -
        isIntExpr = true; // 只要char型参与了运算，则自动向整型转换
    }
    lexicalParser.output("<表达式>");
    return isIntExpr;
}

bool SyntaxParser::termParse() {
    char c;
    bool isIntTerm = false; // 是否为整型项
    bool isIntFactor;
    string token;
    c = lexicalParser.peekTop();
    while (true) {
        isIntFactor = factorParse();
        if (isIntFactor && !isIntTerm) {
            isIntTerm = true; // 一旦设置为true之后就不能变成false了
        }
        c = lexicalParser.peekTop();
        if (c != '*' && c != '/') {
            break;
        } // 如果不是乘法运算符则输出
        token = lexicalParser.getWord(); // 乘法运算符
        isIntTerm = true;// 因为只要参与了运算，就是int类型
    }
    lexicalParser.output("<项>");
    return isIntTerm;
}

bool SyntaxParser::factorParse() {
    char c;
    string token;
    bool isIntFac = true; // 是否为整型因子,默认为整型表达式,因为整型表达式的情况比较多
    bool tmp;
    c = lexicalParser.peekTop();
    if (c == '(') {
        token = lexicalParser.getWord(); // (
        tmp = exprParse(); // 观测一下
        isIntFac = true; // 表达式因子必为整型
        if (lexicalParser.peekTop() == ')') {
            // 正常情况，表达式因子以括号结尾
            token = lexicalParser.getWord(); // )
        } else {
            // 错误处理：表达式因子缺少右括号
            errorParse.syntaxParseError(lineNo, LackRparent);
        }
    } else if (isdigit(c)) {
        token = lexicalParser.getWord();// 数字
        lexicalParser.output("<无符号整数>");
        lexicalParser.output("<整数>");
    } else if (c == '+' || c == '-') {
        token = lexicalParser.getWord(); // +-
        token = lexicalParser.getWord();// 数字
        lexicalParser.output("<无符号整数>");
        lexicalParser.output("<整数>");
    } else if (c == '\'') {
        token = lexicalParser.getWord(); // 字符
        isIntFac = false; // 第二种情况：字符字面量
    } else {
        token = lexicalParser.getWord(); // 标识符
        // 查符号表：返回该标识符的类型
        RetType retType = symbolTabel.checkIdenDef(token); // 检查标识符是否定义
        if (retType == NullType) {
            errorParse.semanticError(lineNo, Nodeclare); // 使用了未定义的标识符
        } else if (retType == Character) {
            isIntFac = false; // 标识符如果是char类型的不管是函数还是其他，都是char型
        }
        c = lexicalParser.peekTop();
        if (c == '[') {
            bool isIntExpr;
            token = lexicalParser.getWord();
            isIntExpr = exprParse();
            if (!isIntExpr) {
                errorParse.syntaxParseError(lineNo, arrayDefError); // 数组下标只能是整型表达式
            }
            c = lexicalParser.peekTop();
            if (c == ']') {
                token = lexicalParser.getWord(); // ]
            } else {
                // 错误处理，缺少右中括号
                errorParse.syntaxParseError(lineNo, LackRbrack);
            }
        } else if (c == '(' && retType != NullType) {
            funcRefParse(1, token); // 有返回值的函数调用
        } else if (c == '(') {
            // 未定义的名称
            excValueTabParse();
        } else {
            // 如果是普通的标识符，就不做处理了
        }
    }
    lexicalParser.output("<因子>");
    return isIntFac; // 返回继承属性--是否为整型因子
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
        return;
    }
    comStateParse();
    token = lexicalParser.getWord(); // }
    lexicalParser.output("<主函数>");
}

void SyntaxParser::comStateParse(RetType retType) {
    constState(); // 常量说明
    varState(); // 变量说明
    stateColParse(retType); // 语句列
    lexicalParser.output("<复合语句>");
}

bool SyntaxParser::isVarState() {
    long record = lexicalParser.getFilepos();
    long lastLineNo = lexicalParser.getLineNo();
    long llPos = lexicalParser.getLastPos();
    string token1 = lexicalParser.getWord(false); // int
    string token2 = lexicalParser.getWord(false); // change
    string token3 = lexicalParser.getWord(false); // 如果变量定义结束就是：(
    lexicalParser.setFilepos(record);
    lexicalParser.setLastPos(llPos);
    lexicalParser.setLineNo(lastLineNo);
    return (token1 == "int" || token1 == "char") && token3 != "(";
}

bool SyntaxParser::isConstState() {
    long record = lexicalParser.getFilepos();
    long lastLineNo = lexicalParser.getLineNo();
    string token;
    token = lexicalParser.getWord(false);
    {
        lexicalParser.setFilepos(record);
        lexicalParser.setLineNo(lastLineNo);
        return token == "const";
    }
}

bool SyntaxParser::isMain() {
    long record = lexicalParser.getFilepos();
    long lastLineNo = lexicalParser.getLineNo();
    string type;
    string name;
    type = lexicalParser.getWord(false); // void
    name = lexicalParser.getWord(false); // main
    {
        lexicalParser.setFilepos(record);
        lexicalParser.setLineNo(lastLineNo);
        return type == "void" && name == "main";
    }
}


