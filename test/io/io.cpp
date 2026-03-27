#include "../../src/bitio.h"
#include <iostream>

int main(){
    BitWriter writer("./out");
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);

    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);
    writer.write(true);

    writer.write(true);

    writer.write_byte(6);
    writer.~BitWriter();

    try{
        BitReader reader("./out");
        while(!reader.EndOfFile()){
        std::cout << reader.read() << std::endl;
    }
    }
    catch(const std::runtime_error& e){
        std::cout << "Error";
    }
    
}
