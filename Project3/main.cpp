#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <bitset>
#include <sstream>
#include <cmath>
#include <stdlib.h>
#define REG "$"
#define ERROR -1
using namespace std;
int zeroExtension(string str);
int getInstMemIndex(int cur);
int getDataMemIndex(int cur);
string stringToBinaryStr(string str);
void printMem(string str);
void printReg(string rs, string rt, string rd);
void charToHex(void *ptr, int buflen);
int binaryStrToDec(string str);
string strToDec(string str);
string decToHex(int dec);
string classifyStr(string str);
void runBinaryStr(string str);
void print(string str);
void printRegisters();
void init();
void caching(int mem);
void printCache();
int instNum = 0, hitCnt=0, missCnt=0, total=0, temp=0;
int registers[33];
int dataMem[16385], instMem[16385];
bool flag = false;
struct block{
    int LRU, Tag;
};
block cache[8][2];

void charToHex(void *ptr, int buflen){
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
    str = stringToBinaryStr(str);
    instMem[instNum++] = binaryStrToDec(str);
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
    init();
    ifstream in;
    in.open(argv[1], ios::in | ios::binary);
    if(in.is_open()){
        streampos st = in.tellg();
        in.seekg(0, std::ios::end);
        streampos en = in.tellg();
        in.seekg(0, std::ios::beg); 
        std::vector<char> contents;
        contents.resize(static_cast<size_t>(en - st));
        in.read(&contents[0], contents.size());
        charToHex(contents.data(), contents.size());
    }
    for(int i=0; i<stoi(argv[2]); i++){
        if(!flag) runBinaryStr(bitset<32>(instMem[getInstMemIndex(registers[32])]).to_string());
        else break;
    }
    cout << "Instructions: " << temp << endl;
    cout << "Hits: " << hitCnt << endl;
    cout << "Misses: " << missCnt << endl;
    cout << "Total: " << total << endl;
    if(!strcmp(argv[3], "reg")) printRegisters();
    if(!strcmp(argv[3], "mem")) printMem(argv[4]);
}

void runBinaryStr(string str){   
    temp++;
    // cout << classifyStr(str) << endl;
    registers[32] += 4;
    string opCode = str.substr(0, 6);
    string rs = str.substr(6, 5);
    string rt = str.substr(11, 5);
    int rsInt = binaryStrToDec(rs);
    int rtInt = binaryStrToDec(rt);
    if(opCode.compare("000000") == 0) { // R-Type
        string rd = str.substr(16, 5);
        string sa = str.substr(21, 5);
        string func = str.substr(26, 6);
        int rdInt = binaryStrToDec(rd);
        int saInt = binaryStrToDec(sa);
        int funcInt = binaryStrToDec(func);
        // cout << opCode << " " << rs << " " << rt << " " << rd << " " << sa << " " << func << endl;
        if(func.compare("100000") == 0) { //add
            registers[rdInt] = registers[rsInt] + registers[rtInt];
        } else if(func.compare("100001") == 0) { //addu
            registers[rdInt] = registers[rsInt] + registers[rtInt];
        } else if(func.compare("100100") == 0) { //and
            registers[rdInt] = registers[rsInt] & registers[rtInt];
        } else if(func.compare("100101") == 0) { //or
            registers[rdInt] = registers[rsInt] | registers[rtInt];
        } else if(func.compare("000000") == 0) { //sll
            registers[rdInt] = registers[rtInt] << saInt;
        } else if(func.compare("101010") == 0) { //slt
            if(registers[rsInt] < registers[rtInt]) registers[rdInt] = 1;
            else registers[rdInt] = 0;
        } else if(func.compare("001000") == 0) { //jr
            registers[32] = registers[rsInt];
        } else if(func.compare("101011") == 0) { //sltu
            if(registers[rsInt] < registers[rtInt]) registers[rdInt] = 1;
            else registers[rdInt] = 0;
        } else if(func.compare("000010") == 0) { //srl
            string tmp = bitset<32>(registers[rtInt]).to_string();
            for(int i=0; i<saInt; i++) tmp = "0" + tmp;
            tmp = tmp.substr(0, 32);
            registers[rdInt] = binaryStrToDec(tmp);
        } else if(func.compare("100010") == 0) { //sub
            registers[rdInt] = registers[rsInt] - registers[rtInt];
        } else { //unknown instruction
            cout << "unknown instruction\n";
            flag = true;
        }
    } else if(opCode.compare("000010") == 0) { // J-type j
        string lb = str.substr(6, 26);
        lb = lb + "00";
        registers[32] += 4;
        int tmp = registers[32] & 0xF0000000;
        tmp = tmp >> 28;
        int nextAddr = instMem[getInstMemIndex(binaryStrToDec(bitset<4>(tmp).to_string() + lb))];
        registers[32] = getInstMemIndex(binaryStrToDec(bitset<4>(tmp).to_string() + lb)) * 4;
    } else if(opCode.compare("000011") == 0) { // J-type jal
        string lb = str.substr(6, 26);
        lb = lb + "00";
        registers[31] = registers[32];
        registers[32] += 4;
        int tmp = registers[32] & 0xF0000000;
        tmp = tmp >> 28;
        int nextAddr = instMem[getInstMemIndex(binaryStrToDec(bitset<4>(tmp).to_string() + lb))];
        registers[32] = getInstMemIndex(binaryStrToDec(bitset<4>(tmp).to_string() + lb)) * 4;
    } else { // I-type
        string im = str.substr(16, 16);
        int imInt = binaryStrToDec(im);
        if(opCode.compare("001000") == 0){ // addi
            registers[rtInt] = registers[rsInt] + imInt;
        } else if(opCode.compare("001001") == 0){ // addiu
            registers[rtInt] = registers[rsInt] + imInt;
        } else if(opCode.compare("001100") == 0){ // andi
            imInt = zeroExtension(im);
            registers[rtInt] = registers[rsInt] & imInt;
        } else if(opCode.compare("000100") == 0){ // beq
            if(registers[rsInt] == registers[rtInt]) {
                int nextAddr = registers[32] + imInt*4;
                registers[32] = nextAddr;
            }
        } else if(opCode.compare("000101") == 0){ // bne
            if(registers[rsInt] != registers[rtInt]) {
                int nextAddr = registers[32] + imInt*4;
                registers[32] = nextAddr;
            }
        } else if(opCode.compare("100000") == 0){ // lb
            int dest = imInt + registers[rsInt];
            caching(dest);
            if(dest%4 == 0){
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0xFF000000) >> 24;
            } else if(dest%4 == 1){
                dest -= 1;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x00FF0000) >> 16;
            } else if(dest%4 == 2) {
                dest -= 2;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x0000FF00) >> 8;
            } else if(dest %4 == 3) {
                dest -= 3;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x000000FF);
            }
            if((registers[rtInt] & 0x00000080) == 128) registers[rtInt] = registers[rtInt] | 0xFFFFFF00;
            else registers[rtInt] = registers[rtInt] & 0x000000FF;
        } else if(opCode.compare("100100") == 0){ // lbu
            int dest = imInt + registers[rsInt];
            if(dest%4 == 0){
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0xFF000000) >> 24;
            } else if(dest%4 == 1){
                dest -= 1;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x00FF0000) >> 16;
            } else if(dest%4 == 2) {
                dest -= 2;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x0000FF00) >> 8;
            } else if(dest %4 == 3) {
                dest -= 3;
                registers[rtInt] = dataMem[getDataMemIndex(dest)];
            }
            registers[rtInt] = registers[rtInt] & 0x000000FF;
        } else if(opCode.compare("100001") == 0){ // lh
            int dest = imInt + registers[rsInt];
            caching(dest);
            if(dest%4 == 0){
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0xFFFF0000) >> 16;
            } else if(dest%4 == 1){
                dest -= 1;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x00FFFF00) >> 8;
            } else if(dest%4 == 2) {
                dest -= 2;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x0000FFFF);
            } else if(dest%4 == 3){
                dest -= 3;
                registers[rtInt] = ((dataMem[getDataMemIndex(dest+4)] & 0xFF000000) >> 24 & 0x000000FF);
                registers[rtInt] = registers[rtInt] | (dataMem[getDataMemIndex(dest)] & 0x000000FF) << 8;
            }
            if((registers[rtInt] & 0x00008000) == 32768) registers[rtInt] = registers[rtInt] | 0xFFFF0000;
            else registers[rtInt] = registers[rtInt] & 0x0000FFFF;
        } else if(opCode.compare("100101") == 0){ // lhu
            int dest = imInt + registers[rsInt];
            if(dest%4 == 0){
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0xFFFF0000) >> 16;
            } else if(dest%4 == 1){
                dest -= 1;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x00FFFF00) >> 8;
            } else if(dest%4 == 2) {
                dest -= 2;
                registers[rtInt] = (dataMem[getDataMemIndex(dest)] & 0x0000FFFF);
            } else if(dest%4 == 3){
                dest -= 3;
                registers[rtInt] = ((dataMem[getDataMemIndex(dest+4)] & 0xFF000000) >> 24 & 0x000000FF);
                registers[rtInt] = registers[rtInt] | (dataMem[getDataMemIndex(dest)] & 0x000000FF) << 8;
            }
            registers[rtInt] = registers[rtInt] & 0x0000FFFF;
        } else if(opCode.compare("001111") == 0){ // lui
            registers[rtInt] = registers[rtInt] & (0x00000000);
            registers[rtInt] = registers[rtInt] | (imInt << 16);
        } else if(opCode.compare("100011") == 0){ // lw
            int dest = imInt + registers[rsInt];
            caching(dest);
            registers[rtInt] = dataMem[getDataMemIndex(dest)];
        } else if(opCode.compare("001101") == 0){ // ori
            int imInt = zeroExtension(im);
            registers[rtInt] = registers[rsInt] | (imInt);
            // res = "ori " + string(REG) + strToDec(rt) + ", " + string(REG) +  strToDec(rs) + ", " + strToDec(im);
        } else if(opCode.compare("101000") == 0){ // sb
            int dest = imInt + registers[rsInt];
            caching(dest);
            if(dest%4 == 0){
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0x00FFFFFF);
                dataMem[getDataMemIndex(dest)] |= ((registers[rtInt] & 0x000000FF) << 24);
            } else if(dest%4 == 1){
                dest -= 1;
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0xFF00FFFF);
                dataMem[getDataMemIndex(dest)] |= ((registers[rtInt] & 0x000000FF) << 16);
            } else if(dest%4 == 2) {
                dest -= 2;
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0xFFFF00FF);
                dataMem[getDataMemIndex(dest)] |= ((registers[rtInt] & 0x000000FF) << 8);
            } else if(dest %4 == 3) {
                dest -= 3;
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0xFFFFFF00);
                dataMem[getDataMemIndex(dest)] |= (registers[rtInt] & 0x000000FF);
            }
        } else if(opCode.compare("001010") == 0){ // slti
            if(registers[rsInt] < imInt) registers[rtInt] = 1;
            else registers[rtInt] = 0;
        } else if(opCode.compare("101001") == 0){ // sh
            int dest = imInt + registers[rsInt];
            caching(dest);
            if(dest%4 == 0){
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0x0000FFFF);
                dataMem[getDataMemIndex(dest)] |= ((registers[rtInt] & 0x0000FFFF) << 16);
            } else if(dest%4 == 1){
                dest -= 1;
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0xFF0000FF);
                dataMem[getDataMemIndex(dest)] |= ((registers[rtInt] & 0x0000FFFF) << 8);
            } else if(dest%4 == 2) {
                dest -= 2;
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0xFFFF0000);
                dataMem[getDataMemIndex(dest)] |= (registers[rtInt] & 0x0000FFFF);
            } else if(dest %4 == 3) {
                dest -= 3;
                dataMem[getDataMemIndex(dest)] = (dataMem[getDataMemIndex(dest)] & 0xFFFFFF00);
                dataMem[getDataMemIndex(dest)] |= (registers[rtInt] & 0x000000FF);

                dataMem[getDataMemIndex(dest+4)] = (dataMem[getDataMemIndex(dest+4)] & 0x00FFFFFF);
                dataMem[getDataMemIndex(dest+4)] |= ((registers[rtInt] & 0xFF000000) << 24);
            }
        } else if(opCode.compare("101011") == 0){ // sw
            int dest = imInt + registers[rsInt];
            caching(dest);
            dataMem[getDataMemIndex(dest)] = registers[rtInt];
        } else { //unknown instruction
            cout << "unknown instruction\n";
            flag = true;
        }
    } 
    return;
}

void caching(int mem){
    string tag, index, offset;
    // cout << "tag : " << binaryStrToDec(tag) << endl;
    // cout << "index : " << binaryStrToDec(index) << endl;
    // cout << "offset : " << binaryStrToDec(offset) << endl;
    string tmp = bitset<32>(mem).to_string();
    tag = tmp.substr(0, 23);
    index = tmp.substr(23, 3);
    offset = tmp.substr(26, 6);
    // cout << "tag: " << binaryStrToDec("0"+tag) << endl;
    // cout << "index: " << binaryStrToDec("0"+index) << endl;
    // cout << "offset: " << binaryStrToDec("0"+offset) << endl;
    int tagInt = binaryStrToDec("0"+tag);
    int indexInt = binaryStrToDec("0"+index);
    int offsetInt = binaryStrToDec("0"+offset);
    if(cache[indexInt][0].Tag == tagInt) {
        hitCnt++;
        cache[indexInt][0].LRU = 1;
        cache[indexInt][1].LRU = 1;
    } else if(cache[indexInt][1].Tag == tagInt) {
        hitCnt++;
        cache[indexInt][0].LRU = 0;
        cache[indexInt][1].LRU = 0;
    } else {
        cache[indexInt][cache[indexInt][0].LRU].Tag = tagInt;
        if(cache[indexInt][0].LRU == 0 || cache[indexInt][1].LRU == 0) {
            cache[indexInt][0].LRU = 1;
            cache[indexInt][1].LRU = 1;
        } else if(cache[indexInt][0].LRU == 1 || cache[indexInt][1].LRU == 1) {
            cache[indexInt][0].LRU = 0;
            cache[indexInt][1].LRU = 0;
        }
        missCnt++;
    }
    total++;
    // printCache();
    return;
}


int getInstMemIndex(int cur){
    if(cur > 65536) return ERROR;
    else if(cur % 4 == 0) return cur/4;
    else return ERROR;
}

void printCache(){
    for(int i=0; i<2; i++){
        cout << "Cache " << i << "  : " << cache[0][i].Tag << endl;
    }
    return;
}

int getDataMemIndex(int cur){
    // if(cur < 0x10000000 || cur > 268500976) {
    //     cout << "memory dump address range error\n";
    //     exit(0);
    // }
    int next = (cur - 0x10000000);
    return next/4;
    // else {
    //     cout << "Memory address alignment error\n";
    //     exit(0);
    // }
}

int zeroExtension(string str){
    return stoi("0000000000000000" + str, nullptr, 2);
}


string classifyStr(string str) {
    string res = "";
    string opCode = str.substr(0, 6);
    string rs = str.substr(6, 5);
    string rt = str.substr(11, 5);
    if(opCode.compare("000000") == 0) { // R-Type
        string rd = str.substr(16, 5);
        string sa = str.substr(21, 5);
        string func = str.substr(26, 6);
        // cout << opCode << " " << rs << " " << rt << " " << rd << " " << sa << " " << func << endl;
        if(func.compare("100000") == 0) { //add
            res = "add " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("100001") == 0) { //addu
            res = "addu " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("100100") == 0) { //and
            res = "and " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("011010") == 0) { //div
            res = "div " + string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("011011") == 0) { //divu
            res = "divu " + string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("001001") == 0) { //jalr
            res = "jalr " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs);
        } else if(func.compare("001000") == 0) { //jr
            res = "jr " + string(REG) + strToDec(rs);
        } else if(func.compare("010000") == 0) { //mfhi
            res = "mfhi " + string(REG) + strToDec(rd);
        } else if(func.compare("010010") == 0) { //mflo
            res = "mflo " + string(REG) + strToDec(rd);
        } else if(func.compare("010001") == 0) { //mthi
            res = "mthi " + string(REG) + strToDec(rs);
        } else if(func.compare("010011") == 0) { //mtlo
            res = "mtlo " + string(REG) + strToDec(rs);
        } else if(func.compare("011000") == 0) { //mult
            res = "mult " + string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("011001") == 0) { //multu
            res = "mult " + string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("100111") == 0) { //nor
            res = "nor " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("100101") == 0) { //or
            res = "or " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("000000") == 0) { //sll
            res = "sll " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rt) + ", " + strToDec(sa);
        } else if(func.compare("000100") == 0) { //sllv
            res = "sllv " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rt) + ", " +  string(REG) + strToDec(rs);
        } else if(func.compare("101010") == 0) { //slt
            res = "slt " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("101011") == 0) { //sltu
            res = "sltu " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("000011") == 0) { //sra
            res = "sra " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rt) + ", " +  strToDec(sa);
        } else if(func.compare("000111") == 0) { //srav
            res = "srav " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rt) + ", " +  string(REG) + strToDec(rs);
        } else if(func.compare("000010") == 0) { //srl
            res = "srl " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rt) + ", " +  strToDec(sa);
        } else if(func.compare("000110") == 0) { //srlv
            res = "srlv " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rt) + ", " +  string(REG) + strToDec(rs);
        } else if(func.compare("100010") == 0) { //sub
            res = "sub " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("100011") == 0) { //subu
            res = "subu " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else if(func.compare("001100") == 0) { //syscall
            res = "syscall";
        } else if(func.compare("100110") == 0) { //xor
            res = "xor " + string(REG) + strToDec(rd) + ", " +  string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt);
        } else { //unknown instruction
            res = "unknown instruction";
        }
    } else if(opCode.compare("000010") == 0) { // J-type j
        string lb = str.substr(6, 26);
        res = "j " + strToDec(lb);
    } else if(opCode.compare("000011") == 0) { // J-type jal
        string lb = str.substr(6, 26);
        res = "jal " + strToDec(lb);
    } else { // I-type
        string im = str.substr(16, 16);
        if(opCode.compare("001000") == 0){ // addi
            res = "addi " + string(REG) + strToDec(rt) + ", " +  string(REG) + strToDec(rs) + ", " + strToDec(im);
        } else if(opCode.compare("001001") == 0){ // addiu
            res = "addiu " + string(REG) + strToDec(rt) + ", " +  string(REG) + strToDec(rs) + ", " + strToDec(im);
        } else if(opCode.compare("001100") == 0){ // andi
            res = "andi " + string(REG) + strToDec(rt) + ", " +  string(REG) + strToDec(rs) + ", " + strToDec(im);
        } else if(opCode.compare("000100") == 0){ // beq
            res = "beq " + string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt) + ", " + strToDec(im);
        } else if(opCode.compare("000101") == 0){ // bne
            res = "bne " + string(REG) + strToDec(rs) + ", " +  string(REG) + strToDec(rt) + ", " + strToDec(im);
        } else if(opCode.compare("100000") == 0){ // lb
            res = "lb " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("100100") == 0){ // lbu
            res = "lbu " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("100001") == 0){ // lh
            res = "lh " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("100101") == 0){ // lhu
            res = "lhu " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("001111") == 0){ // lui
            res = "lui " + string(REG) + strToDec(rt) + ", " +  strToDec(im);
        } else if(opCode.compare("100011") == 0){ // lw
            res = "lw " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("001101") == 0){ // ori
            res = "ori " + string(REG) + strToDec(rt) + ", " + string(REG) +  strToDec(rs) + ", " + strToDec(im);
        } else if(opCode.compare("101000") == 0){ // sb
            res = "sb " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("001010") == 0){ // slti
            res = "slti " + string(REG) + strToDec(rt) + ", " + string(REG) + strToDec(rs) + ", " + strToDec(im);
        } else if(opCode.compare("001011") == 0){ // sltiu
            res = "sltiu " + string(REG) + strToDec(rt) + ", " + string(REG) + strToDec(rs) + ", " + strToDec(im);
        } else if(opCode.compare("101001") == 0){ // sh
            res = "sh " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("101011") == 0){ // sw
            res = "sw " + string(REG) + strToDec(rt) + ", " +  strToDec(im) + "(" + string(REG) + strToDec(rs) + ")";
        } else if(opCode.compare("001110") == 0){ // xori
            res = "xori " + string(REG) + strToDec(rt) + ", " +  strToDec(rs) + ", " + strToDec(im);
        } else { //unknown instruction
            res = "unknown instruction";
        }
    } 
    return res;
}
int binaryStrToDec(string str){
    int res = 0;
    for(int i=str.size()-1; i>=0 ;i--){
        if(i == 0 && str[i] == '1' && str.size() != 5) {
            res += pow(2, str.size()-i-1) * -1;
        } else if(str[i] == '1') {
            res += pow(2, str.size()-i-1);
        }
    }
    return res;
}

string strToDec(string str){
    int res = 0;
    for(int i=str.size()-1; i>=0 ;i--){
        if(i == 0 && str[i] == '1' && str.size() != 5) {
            res += pow(2, str.size()-i-1) * -1;
        } else if(str[i] == '1') {
            res += pow(2, str.size()-i-1);
        }
    }
    return to_string(res);
}

void init(){
    for(int i=0; i<33; i++) registers[i] = 0;
    for(int i=0; i<16385; i++) {
        instMem[i] = -1;
        dataMem[i] = -1;
    }
    for(int i=0; i<8; i++){
        for(int j=0; j<2; j++){
            cache[i][j].LRU = 0;
            cache[i][j].Tag = 0;
        }
    }
    return;
}

void printRegisters(){
    for(int i=0; i<32; i++){
        cout << "$" << i << ": ";
        if(registers[i] == 0) cout << "0x00000000\n";
        else printf("%#010x\n", registers[i]);
    }
    cout << "PC: " ;
    if(registers[32] == 0) cout << "0x00000000\n";
    else printf("%#010x\n", registers[32]);
    return;
}

void printMem(string str){
    int number = (int)strtol(str.c_str(), NULL, 0);
    int idx = getDataMemIndex(number);
    for(int i=idx; i<idx+4; i++){
        if(dataMem[i] == 0) cout << "0x00000000\n";
        else printf("%#010x\n", dataMem[i]);
    }
    return;
}

string decToHex(int dec){
    string tmp = bitset<8>(dec).to_string();
    return tmp;
}