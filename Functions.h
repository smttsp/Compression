//
//  Functions.h
//  Compression
//
//  Created by Samet Taspinar on 2/15/20.
//  Copyright © 2020 Samet Taspinar. All rights reserved.
//

#ifndef Functions_h
#define Functions_h

#include "md5.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;


#define _BYTE 8
#define uchar unsigned char

const size_t buf_length = 1024;
const int SIZE = 256;

// This is the number of compression methods we have
// This will be at the beginning of the file header
const vector<string> compression_headers = {
    "ZED1", "ZED2", "ZED3"
};

class Functions{
private:
    
    
public:
    Functions(){}
    size_t get_filesize(const string filename){
        ifstream file(filename, ios::binary | ios::ate);
        if(file)
            return file.tellg();
        else    cout << "File is not opened\n";
        return 0;
    }
    

    inline bool file_exists(const std::string& name) {
        ifstream f(name.c_str());
        return f.good();
    }
    
    string to_hex(int my_int){
        std::stringstream sstream;
        sstream << std::hex << my_int;
        std::string result = sstream.str();
        return "0x"+result;
    }
    
    bool compare_md5s(const string file1, const string file2){
        MD5 md5;
        std::ifstream fp1(file1);
        std::ifstream fp2(file2);
        
        std::string str, str2;
        while (std::getline(fp1, str)){
            std::getline(fp2, str2);
            
            auto m1 = std::string(md5.digestString(str.c_str()));
            auto m2 = std::string(md5.digestString(str2.c_str()));

            if(m1.compare(m2)!=0) return false;
        }
        return !std::getline(fp2, str2);
    }
    
    
    bool isInt(char ch){
        return ch >= '0' && ch <= '9';
    }
    
};



#endif /* Functions_h */
