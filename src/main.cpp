#include <iostream>
#include <string>
//#include "../src/huffman.h"
#include "../src/bitio.h"
using namespace std;

int main(int argc, char* argv[]){
	// if (argc > 1){
	// 	HuffmanCode(argv[1]);
	// }
	// else{
	// 	cout << "Empty entry" << endl;
	// }
	try {
		BitWriter writer("./data/writing_bits.bin");
		// writer.write(true);
		// writer.write(true);
		// writer.write(false);
		// writer.write(true);
		// writer.write_byte(125);
		writer.write_byte(0);
		for(uint8_t i = 1; i != 255; i++){
			writer.write_byte(i);
		}
		writer.write_byte(255);
	}
	catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }
	return 0;
}
