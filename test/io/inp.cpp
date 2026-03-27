#include "../../src/bitio.h"
#include <iostream>

int main(){

    try{
        BitReader reader("../../data/file");
        while(!reader.EndOfFile()){
        std::cout << reader.read_byte_as_string() << std::endl;
    }
    }
    catch(const std::runtime_error& e){
        std::cout << "Error";
    }
    std::cout << std::endl;
    
    try{
        BitReader reader("../../data/file");
        reader.set_last_sign_bits(4);
        int count = 1;
        while(!reader.EndOfFile()){
        std::cout << reader.read();
        if(count == 8){
            std::cout << std::endl;
            count = 0;
        }
        count++;
    }
    }
    catch(const std::runtime_error& e){
        std::cout << "Error";
    }
    
}
