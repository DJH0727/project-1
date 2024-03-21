#pragma once
#include <iostream>  
#include <fstream>  
#include <vector>  
#include <bitset>  
using namespace std;
bool* calculateHammingCode(bool data[4]) {
    bool* hammingCode = new bool[7];
    hammingCode[0] = data[0];
    hammingCode[1] = data[1];
    hammingCode[2] = data[2];
    hammingCode[4] = data[3];
    hammingCode[3] = data[0] ^ data[1] ^ data[2];
    hammingCode[5] = data[0] ^ data[1] ^ data[3];
    hammingCode[6] = data[0] ^ data[2] ^ data[3];
    return hammingCode;
}
void code(ifstream& inputFile,ofstream& outputFile)
{
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file." << endl;
        return ;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file." << endl;
        return ;
    }

    vector<bool> dataBits;
    vector<bool> hammingdataBits;
    char byte;

    // 读取输入文件中的数据位  
    while (inputFile.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            dataBits.push_back((byte >> i) & 1);
        }
    }
    for (int i = 0; i < dataBits.size(); i++)
    {
        if (dataBits[i] == true)
            cout << 1;
        else
            cout << 0;
    }
    cout << endl;
    bool data[4];
    int i;
    for (i = 0; i + 3 < dataBits.size(); i += 4)
    {
        data[0] = dataBits[i];
        data[1] = dataBits[i + 1];
        data[2] = dataBits[i + 2];
        data[3] = dataBits[i + 3];
        bool* hd = calculateHammingCode(data);
        for (int j = 0; j < 7; j++)
        {
            hammingdataBits.push_back(hd[j]);
        }
    }
    for (; i < dataBits.size(); i++)
    {
        hammingdataBits.push_back(dataBits[i]);
    }
    for (i = 0; i < hammingdataBits.size(); i++)
    {
        if (hammingdataBits[i] == true)
            cout << 1;
        else
            cout << 0;
    }
    cout << endl;

    // 将海明校验码写入输出文件  
    for (i = 0; i < hammingdataBits.size(); i += 8) {
        char bit = 0;
        for (int j = 0; j < 8; j++)
        {
            if (i + j < hammingdataBits.size())
            {
                if (hammingdataBits[i + j])
                    bit = bit | 1;
            }
            if (j < 7)
                bit = bit << 1;

        }
        // 将bool值转换为对应的字节位  
        outputFile.put(bit);
    }
}
bool* correctcode(bool data[7])
{
    bool* rightcode = new bool[4];
    rightcode[0] = data[0];
    rightcode[1] = data[1];
    rightcode[2] = data[2];
    rightcode[3] = data[4];
    int correct = 0;
    bool G[3];
    G[0] = data[0] ^ data[2] ^ data[4] ^ data[6];
    G[1] = data[0] ^ data[1] ^ data[4] ^ data[5];
    G[2] = data[0] ^ data[1] ^ data[2] ^ data[3];
    for (int i = 2; i >= 0; i--)
    {
        if (G[i])
            correct = correct | 1;
        correct = correct << 1;
    }
    correct = correct >> 1;
    if (correct != 0)
    {
        rightcode[4 - correct] = !rightcode[4 - correct];
    }
    return rightcode;
}
void decode(ifstream& inputFile, ofstream& outputFile)
{
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file." << endl;
        return ;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file." << endl;
        return ;
    }

    vector<bool> dataBits;
    vector<bool> rightdataBits;
    char byte;

    // 读取输入文件中的数据位  
    while (inputFile.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            dataBits.push_back((byte >> i) & 1);
        }
    }
    for (int i = 0; i < dataBits.size(); i++)
    {
        if (dataBits[i] == true)
            cout << 1;
        else
            cout << 0;
    }
    cout << endl;
    bool data[7];
    for (int i = 0; i + 6 < dataBits.size(); i += 7)
    {
        for (int j = 0; j < 7; j++)
        {
            data[j] = dataBits[i + j];
        }
        bool* rc = correctcode(data);
        for (int k = 0; k < 4; k++)
        {
            rightdataBits.push_back(rc[k]);
        }
    }
    for (int i = 0; i < rightdataBits.size(); i++)
    {
        if (rightdataBits[i] == true)
            cout << 1;
        else
            cout << 0;
    }
    cout << endl;
    for (int i = 0; i < rightdataBits.size(); i += 8) {
        char bit = 0;
        for (int j = 0; j < 8; j++)
        {
            if (i + j < rightdataBits.size())
            {
                if (rightdataBits[i + j])
                    bit = bit | 1;
            }
            if (j < 7)
                bit = bit << 1;

        }
        // 将bool值转换为对应的字节位  
        outputFile.put(bit);
    }
}
