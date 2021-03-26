#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <bitset>
#include <sstream>
#include <cmath>
#define REG "$"
using namespace std;

string stringToBinaryStr(string str);
void printReg(string rs, string rt, string rd);

string strToUnsDec(string str);
string strToDec(string str);
void classifyStr(string str);
void print(string str);
int instNum = 0;

void hexdump(void *ptr, int buflen){
    unsigned char *buf = (unsigned char*)ptr;
    for (int i=0; i<buflen; i+=16) {
        string str = "";
        for (int j=0; (i+j < buflen) && j<16; j++) { 
            str += buf[i+j];
            if(j % 4 == 3) {
                print(str);
                str = "";
            }
        }
    }
}

void print(string str){
    cout << "inst " << instNum << ": ";
    for(int i=0; i<str.size(); i++){
        printf("%02x", str[i] & 0xff);
    }
    cout << " ";
    str = stringToBinaryStr(str);
    classifyStr(str);
    instNum++;
    return;
} 

string stringToBinaryStr(string str){
    string binStr = "";
    for (int i = 0; i < str.size(); i++){
        binStr += bitset<8>(str.c_str()[i]).to_string();
    }
    return binStr;
}

int main(int argc, char** argv){
    ifstream in;
    in.open(argv[1], ios::in | ios::binary);
    if(in.is_open()){
        // get the starting position
        streampos start = in.tellg();

        // go to the end
        in.seekg(0, std::ios::end);

        // get the ending position
        streampos end = in.tellg();

        // go back to the start
        in.seekg(0, std::ios::beg);

        // create a vector to hold the data that
        // is resized to the total size of the file    
        std::vector<char> contents;
        contents.resize(static_cast<size_t>(end - start));

        // read it in
        in.read(&contents[0], contents.size());

        // print it out (for clarity)
        hexdump(contents.data(), contents.size());
    }
}

void classifyStr(string str) {
    // cout << "str : " << str << endl;
    string opCode = str.substr(0, 6);
    string rs = str.substr(6, 5);
    string rt = str.substr(11, 5);
    if(opCode.compare("000000") == 0) { // R-Type
        string rd = str.substr(16, 5);
        string sa = str.substr(21, 5);
        string func = str.substr(26, 6);
        // cout << opCode << " " << rs << " " << rt << " " << rd << " " << sa << " " << func << endl;
        if(func.compare("100000") == 0) { //add
            cout << "add " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("100001") == 0) { //addu
            cout << "addu " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("100100") == 0) { //and
            cout << "and " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("011010") == 0) { //div
            cout << "div " << REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("011011") == 0) { //divu
            cout << "divu " << REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("001001") == 0) { //jalr
            cout << "jalr " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << endl;
        } else if(func.compare("001000") == 0) { //jr
            cout << "jr " << REG << strToUnsDec(rs) << endl;
        } else if(func.compare("010000") == 0) { //mfhi
            cout << "mfhi " << REG << strToUnsDec(rd) << endl;
        } else if(func.compare("010010") == 0) { //mflo
            cout << "mflo " << REG << strToUnsDec(rd) << endl;
        } else if(func.compare("010001") == 0) { //mthi
            cout << "mthi " << REG << strToUnsDec(rs) << endl;
        } else if(func.compare("010011") == 0) { //mtlo
            cout << "mtlo " << REG << strToUnsDec(rs) << endl;
        } else if(func.compare("011000") == 0) { //mult
            cout << "mult " << REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("011001") == 0) { //multu
            cout << "mult " << REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("100111") == 0) { //nor
            cout << "nor " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("100101") == 0) { //or
            cout << "or " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("000000") == 0) { //sll
            cout << "sll " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rt) << ", " << strToDec(sa) << endl;
        } else if(func.compare("000100") == 0) { //sllv
            cout << "sllv " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rt) << ", " <<  REG << strToUnsDec(rs) << endl;
        } else if(func.compare("101010") == 0) { //slt
            cout << "slt " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("101011") == 0) { //sltu
            cout << "sltu " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("000011") == 0) { //sra
            cout << "sra " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rt) << ", " <<  strToDec(sa) << endl;
        } else if(func.compare("000111") == 0) { //srav
            cout << "srav " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rt) << ", " <<  REG << strToUnsDec(rs) << endl;
        } else if(func.compare("000010") == 0) { //srl
            cout << "srl " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rt) << ", " <<  strToDec(sa) << endl;
        } else if(func.compare("000110") == 0) { //srlv
            cout << "srlv " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rt) << ", " <<  REG << strToUnsDec(rs) << endl;
        } else if(func.compare("100010") == 0) { //sub
            cout << "sub " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("100011") == 0) { //subu
            cout << "subu " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else if(func.compare("001100") == 0) { //syscall
            cout << "syscall" << endl;
        } else if(func.compare("100110") == 0) { //xor
            cout << "xor " << REG << strToUnsDec(rd) << ", " <<  REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << endl;
        } else { //unknown instruction
            cout << "unknown instruction" << endl;
        }
    } else if(opCode.compare("000010") == 0) { // J-type j
        string lb = str.substr(6, 26);
        cout << "j " << strToUnsDec(lb) << endl;
    } else if(opCode.compare("000011") == 0) { // J-type jal
        string lb = str.substr(6, 26);
        cout << "jal " << strToUnsDec(lb) << endl;
    } else { // I-type
        string im = str.substr(16, 16);
        if(opCode.compare("001000") == 0){ // addi
            cout << "addi " << REG << strToUnsDec(rt) << ", " <<  REG << strToUnsDec(rs) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("001001") == 0){ // addiu
            cout << "addiu " << REG << strToUnsDec(rt) << ", " <<  REG << strToUnsDec(rs) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("001100") == 0){ // andi
            cout << "andi " << REG << strToUnsDec(rt) << ", " <<  REG << strToUnsDec(rs) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("000100") == 0){ // beq
            cout << "beq " << REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("000101") == 0){ // bne
            cout << "bne " << REG << strToUnsDec(rs) << ", " <<  REG << strToUnsDec(rt) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("100000") == 0){ // lb
            cout << "lb " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("100100") == 0){ // lbu
            cout << "lbu " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("100001") == 0){ // lh
            cout << "lh " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("100101") == 0){ // lhu
            cout << "lhu " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("001111") == 0){ // lui
            cout << "lui " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << endl;
        } else if(opCode.compare("100011") == 0){ // lw
            cout << "lw " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("001101") == 0){ // ori
            cout << "ori " << REG << strToUnsDec(rt) << ", " <<  strToDec(rs) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("101000") == 0){ // sb
            cout << "sb " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("001010") == 0){ // slti
            cout << "slti " << REG << strToUnsDec(rt) << ", " << REG << strToUnsDec(rs) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("001011") == 0){ // sltiu
            cout << "sltiu " << REG << strToUnsDec(rt) << ", " << REG << strToUnsDec(rs) << ", " << strToDec(im) << endl;
        } else if(opCode.compare("101001") == 0){ // sh
            cout << "sh " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("101011") == 0){ // sw
            cout << "sw " << REG << strToUnsDec(rt) << ", " <<  strToDec(im) << "(" << REG << strToUnsDec(rs) << ")" << endl;
        } else if(opCode.compare("001110") == 0){ // xori
            cout << "xori " << REG << strToUnsDec(rt) << ", " <<  strToDec(rs) << ", " << strToDec(im) << endl;
        } else { //unknown instruction
            cout << "unknown instruction" << endl;
        }
    } 
    return;
}

string strToDec(string str){
    int res = 0;
    for(int i=str.size()-1; i>=0 ;i--){
        if(i == 0 && str[i] == '1') {
            res += pow(2, str.size()-i-1) * -1;
        } else if(str[i] == '1') {
            res += pow(2, str.size()-i-1);
        }
        
    }
    return to_string(res);
}

string strToUnsDec(string str){
    int res = 0;
    for(int i=str.size()-1; i>=0 ;i--){
        if(str[i] == '1') {
            res += pow(2, str.size()-i-1);
        }
        
    }
    return to_string(res);
}
