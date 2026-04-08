#include "../../src/xxhash.h"
using namespace std;

int main(){

    xxHash hash("data_to_hash");
    cout << hash.get_hash() << endl;

    return 0;
}