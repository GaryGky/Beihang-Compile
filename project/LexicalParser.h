//
// Created by dell on 2019/10/10.
//

#ifndef OOP_LEXICALPARSER_H
#define OOP_LEXICALPARSER_H

#include <string>
#include <map>
#include <set>
#include <fstream>

using namespace std;

class LexicalParser {
private:
    map<string, string> keyWordMap; // 保留字
    map<string, string> opMap;      // 所有符号
    set<string> singleDiv;          // 单分界符
    ifstream input; // 输入流
    ofstream outfile; // 输出流
    string file; // 文件内容
    long filelength = 0; // 文件的最大长度
    long filepos = 0; // 用来指示文件内容
    set<string> retFunc; // 有返回值函数表
    set<string> voidFunc; // 无返回值函数表
public:
    LexicalParser();

    void initial(); // 初始化文件

    void start();
    void output(const string &token);

    void setWordMap();

    void setOpMap();

    string queryWord(const string &token);

    string queryOp(const string &token);

    string getWord(bool on_off = true); // 最重要，最核心的函数！

    void setSinDiv();

    char peekTop(int n = 0); // 查看最近的非空字符

    bool isAlpha(char c);

    void trim();

    bool isSingleDiv(const string &c);

    bool isDoubleDiv(char c);

    bool isRetFunc(const string &funcName) {
        return retFunc.find(funcName) != retFunc.end(); // 如果不等于end说明找到了
    }

    bool isVoidFunc(const string &funcName) {
        return voidFunc.find(funcName) != voidFunc.end();
    }

    bool addFunc(const string &funcName, int op) {
        if (op == 0) {
            voidFunc.insert(funcName); // 无返回值
            return true;
        } else if (op == 1) {
            retFunc.insert(funcName); // 有返回值
            return true;
        }
        return false;
    }

    long getFilepos() {
        return filepos;
    }

    void setFilepos(long pos) {
        filepos = pos;
    }
};


#endif //OOP_LEXICALPARSER_H