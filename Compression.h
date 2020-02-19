//
//  Compression.h
//  Leetcode
//
//  Created by Samet Taspinar on 2/15/20.
//  Copyright © 2020 Samet Taspinar. All rights reserved.
//

#ifndef Compression_h
#define Compression_h

#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>

#include "FileInformation.h"
#include "Functions.h"

using namespace std;



class Compression{
private:
    FileInformation fi;
    
    string          _filename;
    string          _zipfile;
    int             _mode;
    size_t          _filesize;
    int             _unq;
    vector<int>     _freqs;
    
    size_t          _binary_size = sizeof(size_t);
    size_t          _sparse_size = sizeof(size_t);
    size_t          _string_size = sizeof(int);
    
    int             _type;
    
public:
    Compression(const string filename, const int mode): fi(filename, mode){
        _filename       = fi.get_filename();
        _filesize       = fi.get_filesize();

        if(mode == 0){
            _zipfile        = _filename + ".mzip";
            _string_size    = fi.get_string_size();
            _sparse_size    = fi.get_sparse_size();
            _binary_size    = fi.get_binary_size();
            _freqs          = fi.get_freqs();
            _unq            = fi.get_unq();

            print_entropy();
            choose_encoding();
        }
        else{
            _zipfile        = _filename;
            choose_decoding();
        }
    }
    
    void print_entropy(){
        Functions fn;
        double k = 2.5;
        cout << "The entropy of the file\n";
        cout << "   (Characters with less than 1\% are discarded)\n";
        std::string str (50, '-');
        cout << str << endl;
        
        for(auto i = 0; i < _freqs.size(); i++){
            auto v = _freqs[i];
            double freq = v*100.0/_filesize;
            if(freq >= 1){
                int num_eq = (int)ceil(freq/k);
                std::string str (num_eq, '=');
                std::string str2 (int(100.0/k-num_eq), ' ');

                cout << (char)i<< " : " << fn.to_hex(i) << " [" << str << str2 << "]\n";
            }
        }
    }
    
    void choose_encoding(){
        //whichever has the highest compression rate, go for it
        if(_string_size <= _binary_size){
            encode_string();
        }
        else{
            encode_binary();
        }
        
        // TO EXTEND THE NUMBER OF METHODS, ADD THE METHODS HERE
//        if(_string_size <= min(_sparse_size, _binary_size))     {   }
//        else if(_binary_size <= min(_sparse_size, _string_size)){   }
//        else if(_sparse_size <= min(_binary_size, _string_size)){   }
    }
    
    void choose_decoding(){
        auto type = fi.get_comp_type();
        if(type == -1){
            cout << "The input file is not a compressed file\n";
            cout << "Please change the mode or update the filename\n";
            return;
        }
        else if(type == 0){
            decode_string();
        }
        else{
            decode_binary();
        }
    }
    
    /* ----------------------- ENCODE & DECODE STRING --------------------------*/
    //here we already know that the file contains at most 245 different ascii chars
    void encode_string(){
        const string method_header = compression_headers[0];
        
        if (_filesize==0) return;
        
        map<uchar, uchar> freq_map;
        
        char p = (char)1;
        for(auto i = 0; i < _freqs.size(); i++){
            if(_freqs[i]>=1){
                freq_map[i] = (p++&0xFF);
                while(p >= '0' && p <= '9') p++;
            }
        }
        
        ofstream ofp(_zipfile, ios::binary);
        
        ofp.write(reinterpret_cast<const char*>(&method_header[0]),method_header.length());

        //we will insert the map to the output file
        int tmp = _unq*2;
        ofp.write(reinterpret_cast<char*>(&tmp),sizeof(int));

        Functions fn;
        for(auto pr : freq_map){
//            cout << fn.to_hex(pr.first) << " " << fn.to_hex(pr.second)<< endl;
            
            ofp.write((char*)(&pr.first),sizeof(char));
            ofp.write((char*)(&pr.second),sizeof(char));
        }
        
        std::ifstream fp(_filename);
        auto read_size = min(buf_length, _filesize-1);
        
        size_t pos = 0;
        char * buffer = new char [buf_length];
        fp.read(buffer, read_size);

        char prev = buffer[0];
        int cnt = 0;
        
        while(pos < _filesize){
            pos+=read_size;
            for(int i=0; i < read_size; ++i){
                if(prev == buffer[i]) cnt++;
                else{
                    if(cnt > 1){
                        string str = to_string(cnt);
                        for(auto ch: str){
                            ofp.write(reinterpret_cast<char*>(&ch),sizeof(char));
                        }
                    }
                    ofp.write(reinterpret_cast<char*>(&freq_map[prev]),sizeof(char));
                    prev = buffer[i];
                    cnt=1;
                }
            }
            read_size = min(buf_length, _filesize - pos);
            fp.read(buffer, read_size);
        }
//        ofp.close();
        
        if(cnt > 1){
            string str = to_string(cnt);
            for(auto ch: str){
                ofp.write(reinterpret_cast<char*>(&ch),sizeof(char));
            }
        }
        ofp.write(reinterpret_cast<char*>(&freq_map[prev]),sizeof(char));
        
        ofp.close();
        fp.close();
        delete[] buffer;
    }
    
    void decode_string(){
        Functions fn;
        auto dec_filesize = fn.get_filesize(_filename);
        
        string new_encode_file = _filename + ".txt";
        
        ifstream file(_filename, ios::binary);
        
        //find out the map
        int unqs;
        string tmp;
        file.read(reinterpret_cast<char*>(&tmp), compression_headers[0].length());
        file.read(reinterpret_cast<char*>(&unqs), sizeof(int));

        vector<char> arr(unqs);
        file.read(reinterpret_cast<char*>(&arr[0]), arr.size());
        
        map<char, char> freq_map;
        
        for(auto i = 0; i < unqs; i+=2){
            freq_map[arr[i+1]] = arr[i];
        }
        
        char * buffer = new char [buf_length];
        auto pos = compression_headers[0].length() + sizeof(int) + unqs;
        
        ofstream new_file(new_encode_file);

        size_t cnt = 0;
        while(pos < dec_filesize){
            auto read_size = min(buf_length, dec_filesize-pos);

            file.read(buffer, read_size);
            
            for(auto i = 0; i < read_size; ){
                while(i < read_size && fn.isInt(buffer[i])){
                    cnt = cnt*10+(buffer[i]-'0');
                    i++;
                }
                if(i < read_size){
                    char ch = freq_map[buffer[i]];
                    if(cnt>0){
                        std::string str (cnt, ch);
                        new_file << str;
                    }
                    else new_file << ch;
                    i++;
                    cnt = 0;
                }
            }
            pos+=read_size;
        }
        new_file.close();
    }
        
    
    /* ----------------------- ENCODE & DECODE BINARY --------------------------
     */
    
    /*  THIS IS THE FUNCTION FOR ENCODING:
     * THE STRUCTURE OF CODING IS AS FOLLOWS:
     * SUPPOSE OUR ENCODED STRING IS THIS: 1A 1000B 1A 5000B
     * I WILL STORE THIS AS (1A 0B 1A 0B 1000 5000) AND
     * I WILL PUT A REFERENCE TO THE BEGINNING OF THE FILE. THAT REFERENCE WILL BE
     * 8B --> size_t. SO IN OUR CASE THE FILE WILL BE:
     * 12[INT] (1A 0B 1A 0B)[CHAR*] (1000 5000)[VECTOR<INT>]
     * --> THIS WAY THE OUTPUT WILL BE 20 BYTES
     **/
    
    //THIS IS METHOD 2: ZED2
    void encode_binary(){
        const string method_header = compression_headers[1];
        
        if (_filesize==0) return;

        //we already know the file is fine
        std::ifstream fp(_filename, ios::binary);

        vector<uchar> vec;
        vector<int> lengthy;

        // write the first 4 bytes of the files
        size_t beg_byte = sizeof(size_t)+method_header.length();
        ofstream fp2(_zipfile, ios::binary);
        
        fp2.write(reinterpret_cast<const char*>(&method_header[0]),method_header.length());
        fp2.write(reinterpret_cast<char*>(&beg_byte),sizeof(size_t));
        
        size_t pos = 0;
        auto read_size = min(buf_length, _filesize);
        
        char * buffer = new char [buf_length];
        fp.read(buffer, read_size);

        char prev = buffer[0];
        int cnt = 0;
        
        while(pos <= _filesize){
            pos+=read_size;
            for(int i=0; i < read_size; ++i){
                if(prev == buffer[i]) cnt++;
                else{
                    uchar val = (cnt >= SIZE)? 0:(uchar)(cnt&0xFF);
                    vec.push_back(val);
                    vec.push_back(prev);

                    fp2.write(reinterpret_cast<char *>(&val), sizeof(val));
                    fp2.write(reinterpret_cast<char *>(&prev), sizeof(prev));
                    
                    beg_byte += 2;
                    if(val==0) lengthy.push_back(cnt);
                    prev = buffer[i];
                    cnt=1;
                }
            }
            read_size = min(buf_length, _filesize - pos+1);
            fp.read(buffer, read_size);
        }
        
        if (!lengthy.empty())
            fp2.write(reinterpret_cast<char*>(&lengthy[0]), lengthy.size()*sizeof(int));
        
        // update the first 4 bytes of encoded file.
        // it is the location where integers will start
        
        fp2.seekp(method_header.length());
        fp2.write(reinterpret_cast<char*>(&beg_byte),sizeof(size_t));

        fp2.close();
        fp.close();
        delete[] buffer;
    }

    /*    DECODE FUNCTION    */
    //THIS IS METHOD 2: ZED2
    void decode_binary() {
        const string method_header = compression_headers[1];
        _zipfile = _filename;
        string new_encode_file = _zipfile + ".txt";
        
        
        ifstream file(_zipfile, ios::binary | ios::in);
        file.seekg(0);
        //find out the beginning of the integers
        size_t beg_byte;
        
        auto len = compression_headers[0].length();
        char * tmp2 = new char [len];
        file.read(tmp2, len);
        file.read(reinterpret_cast<char*>(&beg_byte), sizeof(size_t));

        //now we know how many integers we have by (filesize-beg_byte)/4.
        auto vec_sz = (_filesize-beg_byte)/4;
        vector<int> lengthy(vec_sz);
        
        file.seekg(beg_byte);
        
        // now we can start filling our vector.
        file.read(reinterpret_cast<char*>(&lengthy[0]),sizeof(int)*vec_sz);
        
        char * buffer = new char [buf_length];
        auto pos = sizeof(size_t)+len;
        int p = 0;
        
        file.seekg(pos);
        
        ofstream new_file(new_encode_file);
        
        while(pos < beg_byte){
            auto read_size = min(buf_length, beg_byte-pos);
            file.read(buffer, read_size);
            
            for(auto i = 0; i < read_size; i+=2){\
                auto num = ((int)buffer[i] != 0) ? (int)buffer[i] : lengthy[p++];
                std::string str (num, buffer[i+1]);
                new_file << str;
            }
            pos+=read_size;
        }
        new_file.close();
    }
    

    /*These are not implemented. However, I believe, in some cases, they might improve the compression rate a lot. Especially zigzag and sparse is very likely to compress in certains cases
     */
    // Another issue is each of these can be converted to a new child class of Compression.
    // i.e., class Compress_Sparse: public Compression { }
    // This way, the whoe implementation can be more clear etc.
    void encode_sparse(){ }
    void decode_sparse(){ }
    void encode_bitset(){ }
    void decode_bitset(){ }
    void encode_zigzag(){ }
    void decode_zigzag(){ }
};

#endif /* Compression_h */
