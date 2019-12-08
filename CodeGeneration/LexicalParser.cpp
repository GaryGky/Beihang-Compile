//
// Created by dell on 2019/10/10.
//

#include "LexicalParser.h"
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <fstream>

int lineNo = 1;


void LexicalParser::start() {
    initial();
    setWordMap();
    setOpMap();
    setSinDiv();
}


void LexicalParser::initial() {
    input.open("testfile.txt", ios::in);
    stringstream buf;
    // 将文件转为字符串
    buf << input.rdbuf();
    file = buf.str();
    // 获取文件从长度
    filelength = file.length();
    cout << "filelength: " << filelength << endl;
    filepos = 0;
    cout << "filepos: " << filepos << endl;
    input.close();                    // close file handle
}

void LexicalParser::output(const string &token) {
    if (!outputSwitch) {
        return;
    }
    string res1;
    res1 = queryOp(token);
    string res2;
    res2 = queryWord(token);
    if (res1.length() > 0) {
        res1.append(" ");
        res1 += token;
        outfile << res1 << endl;
    } else if (res2.length() > 0) {
        res2 = queryWord(token);
        res2 += ' ';
        res2.append(token);
        outfile << res2 << endl;
    } else {
        outfile << token << endl;
    }
}

bool LexicalParser::isSingleDiv(const string &c) {
    return singleDiv.find(c) != singleDiv.end();
}

bool LexicalParser::isDoubleDiv(char c) {
    return (c == '<' || c == '>' || c == '=');
}

void LexicalParser::trim() {
    while (filepos != filelength) {
        if (!isspace(file[filepos])) {
            return;
        } else if (file[filepos] == '\n') {
            lineNo++; // 如果读到换行符，那么行数记录器加一
        }
        filepos++;
    }
}

void LexicalParser::toNextSemi() {
    while (filepos != filelength) {
        if (file[filepos] == ',' || file[filepos] == ';') {
            return;
        }
        filepos++;
    }
}

bool LexicalParser::isAlpha(char c) {
    return isalpha(c) || c == '_';
}

char LexicalParser::peekTop(int n) {
    long tmp1 = lineNo;
    long tmp2 = filepos;
    long tmp3 = lastFilePos;
    trim();
    char ret = file[filepos + n];
    lineNo = tmp1;
    filepos = tmp2;
    lastFilePos = tmp3;
    return ret;
}

void LexicalParser::setOpMap() {
    opMap.insert(pair<string, string>("+", "PLUS"));
    opMap.insert(pair<string, string>("-", "MINU"));
    opMap.insert(pair<string, string>("*", "MULT"));
    opMap.insert(pair<string, string>("/", "DIV"));
    opMap.insert(pair<string, string>("<", "LSS"));
    opMap.insert(pair<string, string>("<=", "LEQ"));
    opMap.insert(pair<string, string>(">", "GRE"));
    opMap.insert(pair<string, string>(">=", "GEQ"));
    opMap.insert(pair<string, string>("==", "EQL"));
    opMap.insert(pair<string, string>("!=", "NEQ"));
    opMap.insert(pair<string, string>("=", "ASSIGN"));
    opMap.insert(pair<string, string>(";", "SEMICN"));
    opMap.insert(pair<string, string>(",", "COMMA"));
    opMap.insert(pair<string, string>("(", "LPARENT"));
    opMap.insert(pair<string, string>(")", "RPARENT"));
    opMap.insert(pair<string, string>("[", "LBRACK"));
    opMap.insert(pair<string, string>("]", "RBRACK"));
    opMap.insert(pair<string, string>("{", "LBRACE"));
    opMap.insert(pair<string, string>("}", "RBRACE"));
}

void LexicalParser::setSinDiv() {
    singleDiv.insert("+");
    singleDiv.insert("-");
    singleDiv.insert("*");
    singleDiv.insert("/");
    singleDiv.insert(";");
    singleDiv.insert(",");
    singleDiv.insert("(");
    singleDiv.insert(")");
    singleDiv.insert("[");
    singleDiv.insert("]");
    singleDiv.insert("{");
    singleDiv.insert("}");
}

string LexicalParser::queryWord(const string &token) {
    if (keyWordMap.find(token) != keyWordMap.end()) {
        return keyWordMap.find(token)->second;
    } else {
        return "";
    }
}

string LexicalParser::queryOp(const string &token) {
    // cout << opMap.find(token)->second << endl;
    if (opMap.find(token) != opMap.end()) {
        return opMap.find(token)->second;
    } else {
        return "";
    }
}

void LexicalParser::setWordMap() {
    keyWordMap.insert(pair<string, string>("const", "CONSTTK"));
    keyWordMap.insert(pair<string, string>("int", "INTTK"));
    keyWordMap.insert(pair<string, string>("char", "CHARTK"));
    keyWordMap.insert(pair<string, string>("main", "MAINTK"));
    keyWordMap.insert(pair<string, string>("void", "VOIDTK"));
    keyWordMap.insert(pair<string, string>("if", "IFTK"));
    keyWordMap.insert(pair<string, string>("else", "ELSETK"));
    keyWordMap.insert(pair<string, string>("do", "DOTK"));
    keyWordMap.insert(pair<string, string>("while", "WHILETK"));
    keyWordMap.insert(pair<string, string>("for", "FORTK"));
    keyWordMap.insert(pair<string, string>("scanf", "SCANFTK"));
    keyWordMap.insert(pair<string, string>("printf", "PRINTFTK"));
    keyWordMap.insert(pair<string, string>("return", "RETURNTK"));
}

bool LexicalParser::isChar(char c) {
    return isAlpha(file[filepos]) || isdigit(file[filepos]);
}

string LexicalParser::getWord(bool on_off) {
    lastFilePos = filepos; // 记录在读单词前的位置
    char c;
    string token;
    trim(); // 进入是去掉空格
    if (filepos == filelength) {
        exit(0);
    }
    c = file[filepos++];
    if (c == '\'') {
        // 字符
        c = file[filepos++];
        token += c; // 得到返回的字符
        c = file[filepos++]; // 吃掉单引号
        string res = "CHARCON "; // 输出字符
        res.append(token);
        if (on_off) output(res);
        return token;
    } else if (c == '\"') {
        // 字符串
        // 双引号已经被吃掉
        c = file[filepos]; // 字符串第一个字符
        while (filepos != filelength) {
            if (c == '\"') {
                break;
            }
            token += file[filepos++];
            c = file[filepos];
            if (c == '\"') {
                break;
            }
        }
        c = file[filepos++]; // 吃掉右边的双引号
        return token; // 返回字符串
    } else if (isdigit(c)) {
        // 组成数字
        token += c;
        while (filepos != filelength) {
            if (!isdigit(file[filepos])) {
                break;
            }
            c = file[filepos++];
            token += c;
        }
        string ret = token;
        token.insert(0, "INTCON ");
        if (on_off) output(token);
        return ret;
    } else if (isAlpha(c)) {
        // 组成字母
        token += c;
        while (isChar(file[filepos])) {
            token += file[filepos++];
            // 如果在标识符定义中出现了非法字符怎么办？？？
        }
        string res;
        res = queryWord(token); // 查看是否为保留字
        if (res.length() > 0) {
            // 是保留字
            if (on_off) output(token);
            return token;
        } else {
            // 是标识符
            res = "IDENFR ";
            res.append(token);
            if (on_off) output(res);
            return token;
        }
    } else {
        // 符号
        string res;
        token += c;
        if (token == "'" || token == "\"") {
            return token;
        } // 如果是单引号或者双引号，直接输出
        if (isSingleDiv(token)) {

        } else if (isDoubleDiv(c)) {
            // 双分界符
            if (file[filepos] == '=') {
                token += file[filepos++];
            }
        } else if (c == '!') {
            if (file[filepos] == '=') {
                token += file[filepos++];
            } else {
                output("error in != condition parse");
            }
        } else {
            output("can not identify character");
        }
        if (on_off) output(token);
        return token;
    }
}

void LexicalParser::retract() {
    filepos = lastFilePos; // 退回到上一个单词的位置
}


void LexicalParser::toNextChar(char chDst) {
    while (filepos != filelength) {
        if (file[filepos] == chDst) {
            return;
        }
        filepos++;
    }
}

LexicalParser::LexicalParser() {}


