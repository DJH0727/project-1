#include <iostream>  
#include <fstream>  
#include <vector>  
#include <bitset>  
#include"hamming_code.h"
using namespace std;
int main() {
    ifstream inputFile("D:\\test\\test1.bin", ios::binary);
    ofstream outputFile("D:\\test\\test2.bin", ios::binary);
    code(inputFile, outputFile);
    inputFile.close();
    outputFile.close();
    ifstream inputFile1("D:\\test\\test2.bin", ios::binary);
    ofstream outputFile1("D:\\test\\test3.bin", ios::binary);
    decode(inputFile1, outputFile1);
    inputFile1.close();
    outputFile1.close();
    return 0;
}