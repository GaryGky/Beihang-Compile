//
// Created by dell on 2019/10/10.
//

#include "LexicalParser.h"
#include <iostream>
#include <map>
#include <set>
#include <cstdio>
#include <fstream>
#include <sstream>

LexicalParser::LexicalParser() = default;

void LexicalParser::start() {
    initial();
    setWordMap();
    setOpMap();
    setSinDiv();
}

void LexicalParser::initial() {
    outfile.open("output.txt", ios::out);
    input.open("testfile.txt", ios::in);
    stringstream buf;
    // 将文件转为字符串
    buf << input.rdbuf();
    file = buf.str();
    // 获取文件从长度
    filelength = file.length();
    cout << filelength << endl;
    filepos = 0;
    input.close();                    // close file handle
}

void LexicalParser::output(const string &token) {
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
        }
        filepos++;
    }
}

bool LexicalParser::isAlpha(char c) {
    return isalpha(c) || c == '_';
}

char LexicalParser::peekTop(int n) {
    trim();
    return file[filepos + n];
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

string LexicalParser::getWord(bool on_off) {
    char c;
    string token;
    trim(); // 进入是去掉空格
    c = file[filepos++];
    if (c == '\'') {
        // 字符
        c = file[filepos++];
        token += c;
        c = file[filepos++]; // 吃掉单引号
        string res = "CHARCON "; // 输出字符
        res.append(token);
        if (on_off) outfile << res << endl;
        return token;
    } else if (c == '\"') {
        // 字符串
        // 双引号已经被吃掉
        string res;
        c = file[filepos]; // 字符串第一个字符
        res = "STRCON ";
        while (file[filepos] != '\"') {
            token += file[filepos++];
        }
        res.append(token);
        if (on_off) {
            output(res);
            outfile << "<字符串>" << endl;
        }
        // 输出字符串
        c = file[filepos++]; // 双引号
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
        while (isAlpha(file[filepos]) || isdigit(file[filepos])) {
            token += file[filepos++];
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
            } else {// 异常退出
                output("ERROR IN !-Parse");
                exit(0); // ！后面没有 =
            }
        } else {
            output("ERROR IN OP-Parse");
            exit(-1);
        }
        if (on_off) output(token);
        return token;
    }
}