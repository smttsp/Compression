//
//  FileInformation.h
//  Compression
//
//  Created by Samet Taspinar on 2/15/20.
//  Copyright © 2020 Samet Taspinar. All rights reserved.
//

#ifndef FileInformation_h
#define FileInformation_h


#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Functions.h"

using namespace std;

class FileInformation{
private:
    string          _filename;
    int             _mode;
    size_t          _filesize;
    int             _unq;
    vector<int>     _freqs;
    
    size_t          _binary_size = sizeof(size_t);
    size_t          _sparse_size = sizeof(size_t);
    size_t          _string_size = sizeof(int);

    // 1) count continuous bits. ceil(log2(longest_cont_bit)) will be the size of each bit
    // then we will keep the first bit. So (15,1)(10,0)(5,1) can be represented as
    // 1(FIRST BIT), 15, 10, 5 which indicates first 15 bits of 1, then 10 bits 0, then 5 bits 1
    // THE PROBLEM IS UNLESS I ITERATE MULTIPLE TIMES, THIS WILL PROBABLY INCREASE THE SIZE.
    size_t          _bitset_size = 0;

    int             _comp_type;
    
    
    
public:
    
    FileInformation(const string filename, const int mode){
        _filename = filename;
        _mode = mode;
        if(mode == 0){ //compression mode
            _freqs.resize(SIZE);
            collect_info();
        }
        else{           // decompression mode
            collect_dec_info();
        }
    }

    string  get_filename()    {return _filename;     }
    int     get_mode()        {return _mode;         }
    int     get_unq()         {return _unq;          }
    vector<int> get_freqs()   {return _freqs;        }
    size_t  get_filesize()    {return _filesize;     }
    size_t  get_binary_size() {return _binary_size;  }
    size_t  get_sparse_size() {return _sparse_size;  }
    size_t  get_string_size() {return _string_size;  }
    int     get_comp_type()   {return _comp_type;    }
    
    
    void count_unq_chars(){
        _unq = 0;
        for(auto f: _freqs){
            if(f>0) _unq++;
        }
    }
    
    /* THIS FUNCTION COLLECTS A BUNCH OF INFORMATION ABOUT THE FILE
     * IN THE FIRST PASS. THE REASON I CHOOSE TO HAVE TWO PASSES IS THAT THIS
     * WAY, I CAN DECIDE ON MANY THINGS ABOUT THE ENCODING METHOD. FOR EXAMPLE, IF NOT ALL
     * CHARS ARE USED (LETS SAY A FEW CHARS), I CAN MAP THEM INTO SOME BITS AND USE SOME
     * PARTS OF A BYTE (E.G. INSTEAD OF 8-BIT CHAR, 4 BITS IF < 16 DIFFERENT CHARS EXIST)
     */

    // COLLECTED INFO: 1) DISTRIBUTION, 2) NUM UNIQUE CHARS
    // 3) HOW MUCH BINARY/STRING/SPARSE/BIT ENCODING WILL SAVE, WHICH ONE IS BEST
    // 3a) SIZE OF BINARY, 3B) SIZE OF STRING ENCODING IF POSSIBLE 3C) SIZE OF BITS
    // 4) IS INTEGERS USED, IF NOT STRING ENCODING MAY BE BETTER THAN BINARY
    // 5) INPUT FILESIZE

    void collect_info(){
        Functions fn; 
        _filesize = fn.get_filesize(_filename);
        
        std::ifstream fp(_filename, ios::binary);

        vector<uchar> vec;
        vector<int> lengthy;

        size_t pos = 0;
        auto read_size = min(buf_length, _filesize);
        
        char * buffer = new char [buf_length];
        fp.read(buffer, read_size);

        char prev = buffer[0];
        int cnt = 0;
        
        size_t sparse_cnt = 0;
        
        while(pos < _filesize){
            pos+=read_size;
            for(int i=0; i < read_size; ++i){
                _freqs[(int)((uchar)buffer[i])]++;
                if(prev == buffer[i]) cnt++;
                else{ //different character
                    uchar val = (cnt >= SIZE)? 0:(uchar)(cnt&0xFF);

                    //if more than 256 times occurs. 6 bytes are used, otherwise only two bytes
                    _binary_size += ((val==0)?sizeof(uint32_t):0+2);
                    
                    
                    //this requires us to have at most 245/6 unique chars being used in the text
                    // if the unique chars < 245 and string_size < binary_size, we can do mapping
                    // and create string text. We also skip 1s. For example, aaab = 3ab.
                    // we also need to store the mapping params which is unique(_freqs)*2 bytes.
                    _string_size += ((cnt>=2)?std::to_string(cnt).length():0)+1;
                    
                    
                    // sparse size is nice if number of unique chars in the string is low.
                    // for example, if there are 15 different chars, we can represent each char with 4bits.
                    // I don't want to dive into Huffman due to time limitations. But it may improve the compression ratio.
                    
                    _sparse_size += std::to_string(cnt).length();
                    sparse_cnt++;   //this one indicates how many times characters were switched.
                                    //abcba and aabbccbbaa will be 5. (each of a,b,c is 2bits)
                                    //so we will need 10bits (2bytes for chars and 5 bytes for their compressed versions)
                    
                    // bitset_size: this should be promising but I couldn't see how I can make use of it with one iteration. Maybe multiple iterations can compress more. But I need to do some analysis.
                    // I will try if I have time
                    // (UPDATE: zigzag bitset will probably make it better and more compressed)
                    
                    prev = buffer[i];
                    cnt=1;
                }
            }
            read_size = min(buf_length, _filesize - pos);
            fp.read(buffer, read_size);
        }
        
        count_unq_chars();
//        cout << "unique chars" << _unq << endl;
        _sparse_size += (size_t)ceil(log2(_unq)*sparse_cnt/_BYTE);

        // if there are more than 245 unique chars, string encoding cannot work.
        _string_size += (_unq < SIZE-10) ? 2*_unq : max(_sparse_size, _binary_size)+1;
    }
    
    /*
     This part is for decompression mode
     */
    
    void collect_dec_info(){
        _comp_type = -1;

        Functions fn;
        _filesize = fn.get_filesize(_filename);
        
        if(_filesize < 4){
            return;
        }
        
        std::ifstream fp(_filename, ios::binary);
        auto read_size = 4;

        char * buffer = new char [buf_length];
        fp.read(buffer, read_size);
        string header = string(&buffer[0], &buffer[read_size]);
        
        for(auto i = 0; i < compression_headers.size(); i++){
            if(header.compare(compression_headers[i])==0){
                _comp_type=i;
            }
        }
        fp.close();
    }
};

#endif /* Functions_h */
