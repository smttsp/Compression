//
//  Compression_tmp.h
//  Compression
//
//  Created by Samet Taspinar on 2/17/20.
//  Copyright © 2020 Samet Taspinar. All rights reserved.
//

#ifndef Compression_tmp_h
#define Compression_tmp_h

// I STARTED working on this one but I didn't have time.
// If wanted, I can work on this
string char2bits(char ch){
    string str = "";
    for(auto i = _BYTE-1; i >= 0; --i){
        str+= (((int)ch>>i)&1)+'0';
//        cout << str << endl;
    }
    return str;
}

    
void binary_encode_once(const vector<int> lengths) {
    
}

void binary_encode(const string filename, vector<int> &dist){
    Functions fn;
    auto filesize = fn.get_filesize(filename);
    
    std::ifstream fp(filename, ios::binary);
    
    vector<uint32_t> bit_lengths;

    size_t pos = 0;
    auto read_size = min(buf_length, filesize);
    
    char * buffer = new char [buf_length];
    fp.read(buffer, read_size);

    
    
    char tmp = buffer[0];
    int prev_bit = tmp&1;
    int least_significant_bit = prev_bit;
    cout << (int)tmp << " - " << char2bits(tmp) <<  endl;
    int cnt = 0;
    
    size_t bitset_size = 0; //this is tricky. 1) count continuous bits. ceil(log2(longest_continuous_bit)) will be the value we need.
    // then we will keep the first bit. So (15,1)(10,0)(5,1) can be represented as
    // 1, 15, 10, 5 which indicates first 15 bits are 1, then 10 bits 0, then 5 bits 1 again...

    vector<int> lengths;
    int mx_continious=0;
    while(pos < filesize){
        pos+=read_size;
        for(int i=0; i < read_size; ++i){
            dist[buffer[i]]++;
            
            //we may have to reverse this one. check it
            auto cur_byte = buffer[i];
            for(auto p = 0; p <_BYTE; p++){
                if(prev_bit == (cur_byte&1)) cnt++;
                else{ //different character
                    mx_continious = max(mx_continious, cnt);
                    lengths.push_back(cnt);
                    prev_bit = (cur_byte&1);
                    cnt=1;
                }
                cur_byte >>= 1;
            }
        }
        auto read_size = min(buf_length, filesize - pos);
        fp.read(buffer, read_size);
    }
    lengths.push_back(cnt);
    
    bitset_size = (size_t)ceil(log2(mx_continious+1));
    
    size_t encoded_size = lengths.size()*bitset_size;
    vector<bool> encoded(encoded_size, false);
    
    
    
}



#endif /* Compression_tmp_h */
