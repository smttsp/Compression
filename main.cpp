//
//  main.cpp
//  Compression
//
//  Created by Samet Taspinar on 2/15/20.
//  Copyright © 2020 Samet Taspinar. All rights reserved.

#include <iostream>
#include <string>
#include <stdio.h>

#include "Compression.h"
#include "Functions.h"
#include "md5.h"

using namespace std;

int main(int argc, const char * argv[]) {
    
    
    Functions fn;
    
    string file1 = string(argv[1]);
    string file2 = file1 + ".mzip";
    string file3 = string(argv[2]);

//    ofstream ofs (file1, ios::binary);
//    for (auto i = 0; i < 240; i++){
//        ofs << (char)(i) << (char)(i) << (char)(i);
//    }
//    ofs.close();
//
//    Compression cmp(file1,0);
//    Compression cmp2(file2,1);
//
//    if(fn.file_exists(file1) && fn.file_exists(file3)){
//        auto is_eq = fn.compare_md5s(file1, file3);
//        cout << "Files are " << (is_eq?"":" NOT ") << "equal\n";
//    }
    
    if(argc < 2){
        cout << "USAGE\n";
        cout << "For compression, select mode 0 or compress or leave it empty\n";
        cout << "There is additional compare method which compares two files.\n"
             << "Call it with the following arguments (file1, file2, "")\n\n";

        cout << "Error!!! No filename and mode arguments\n";
        return 0;
    }
    else if( argc <= 3){
        auto filename = argv[1];
        if (!fn.file_exists(filename)){
            cout << "The file you selected does not exist\n";
            return 0;
        }
        
        clock_t t1 = clock();
        std::string str1(argv[2]);
        if(argc == 2 || str1.compare("0")==0 || str1.compare("compress")==0){
            cout << "You selected compression mode\n";
            Compression cmp(filename,0);

            cout << "It took "<< 1000*(clock()-t1)/CLOCKS_PER_SEC << " ms for compression\n";
        }
        else{
            cout << "You selected decompression mode\n";
            Compression cmp(filename,1);
            
            cout << "It took "<< 1000*(clock()-t1)/CLOCKS_PER_SEC << " ms for decompression\n";
        }
        
    }
    else{ // this is compare two files whether they are equal
        string file1 = string(argv[1]);
        string file2 = string(argv[2]);

        if(fn.file_exists(file1) && fn.file_exists(file2)){
            auto is_eq = fn.compare_md5s(file1, file2);
            cout << "Files are " << (is_eq?"":" NOT ") << "equal\n";
        }
        else{
            cout << "At least one of the files cannot be opened\n";
        }
    }
    return 0;
}
