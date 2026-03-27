#include <iostream>
#include <string>
#include "../src/bitio.h"
#include "../src/huffman.h"
using namespace std;

int main(int argc, char* argv[]){
	// if (argc > 1){
	// 	HuffmanCode(argv[1]);
	// }
	// else{
	// 	cout << "Empty entry" << endl;
	// }
	try {
		HuffmanCode("data/file");
	}
	catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }
	try {
		HuffmanDecode("data/file.bin");
	}
	catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }
	return 0;
}
