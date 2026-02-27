#include <iostream>
#include <string>
#include "../src/huffman.h"
using namespace std;

int main(int argc, char* argv[]){
	if (argc > 1){
		HuffmanCode(argv[1]);
	}
	else{
		cout << "Empty entry" << endl;
	}
	return 0;
}
