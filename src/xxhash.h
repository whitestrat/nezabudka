#include <iostream>
#include <string>
#include "/home/sq/workspace/nezabudka/src/bitio.h"

constexpr uint64_t PRIME1 = 0x9E3779B185EBCA87ULL;
constexpr uint64_t PRIME2 = 0xC2B2AE3D27D4EB4FULL;
constexpr uint64_t PRIME3 = 0x165667B19E3779F9ULL;
constexpr uint64_t PRIME4 = 0x85EBCA77C2B2AE63ULL;
constexpr uint64_t PRIME5 = 0x27D4EB2F165667C5ULL;

constexpr uint64_t rotl64(uint64_t x, int r) {
    return (x << r) | (x >> (64 - r));
}

class xxHash{
    uint64_t acc1;
    uint64_t acc2;
    uint64_t acc3;
    uint64_t acc4;
    uint64_t hash;
    size_t total_len;
    public:
    xxHash(size_t _total_len):total_len(_total_len), hash(0){
        acc1 = PRIME1 + PRIME2;
        acc2 = PRIME2;
        acc3 = 0;
        acc4 = - PRIME1;
    
    }
    
    xxHash(const std::string filename){
        acc1 = PRIME1 + PRIME2;
        acc2 = PRIME2;
        acc3 = 0;
        acc4 = - PRIME1;
        BitReader reader(filename);
        total_len = reader.file_length();
        size_t len = 0;
        uint8_t* data;
        if(!reader.EndOfFile()){
            data = reader.read_buffer();
            len = reader.buffer_size();
        }
        while(!reader.EndOfFile()){
            process(data, len);
            data = reader.read_buffer();
            len = reader.buffer_size();
        }
        finalize(data, len);
    }

    uint64_t get_hash() const {return hash;}
    uint64_t round(uint64_t acc, uint64_t input){
        acc += input * PRIME2;
        acc = rotl64(acc, 31);
        acc *= PRIME1;
        return acc;
    }

    void process(uint8_t* data, size_t len){
        while(len >= 32){
            acc1 = round(acc1, *reinterpret_cast<const uint64_t*>(data));
            acc2 = round(acc2, *reinterpret_cast<const uint64_t*>(data + 8));
            acc3 = round(acc3, *reinterpret_cast<const uint64_t*>(data + 16));
            acc4 = round(acc4, *reinterpret_cast<const uint64_t*>(data + 24));
            len -= 32;
            data = data + 32;
        }
    }

    void avalanche(){
        hash ^= hash >> 33;
        hash *= PRIME2;
        hash ^= hash >> 29;
        hash *= PRIME3;
        hash ^= hash >> 32;
    }

    void finalize(uint8_t* data, size_t len){
        while(len >= 32){
            acc1 = round(acc1, *reinterpret_cast<const uint64_t*>(data));
            acc2 = round(acc2, *reinterpret_cast<const uint64_t*>(data + 8));
            acc3 = round(acc3, *reinterpret_cast<const uint64_t*>(data + 16));
            acc4 = round(acc4, *reinterpret_cast<const uint64_t*>(data + 24));
            len -= 32;
            data = data + 32;
        }

        hash = rotl64(acc1, 1) + rotl64(acc2, 7) + rotl64(acc3, 12) + rotl64(acc4, 18);
    
        hash = (hash ^ acc1) * PRIME1;
        hash = (hash ^ acc2) * PRIME2;
        hash = (hash ^ acc3) * PRIME3;
        hash = (hash ^ acc4) * PRIME4;
    
        hash += total_len;

        while(len >= 8){
            hash ^= *reinterpret_cast<const uint64_t*>(data) * PRIME1;
            hash = rotl64(hash, 27) * PRIME2 + PRIME3;
            data += 8;
            len -= 8;
        }
        if (len >= 4){
            hash ^= *reinterpret_cast<const uint32_t*>(data) * PRIME1;
            hash =  rotl64(hash, 23) * PRIME2 + PRIME3;
            data += 4;
            len -= 4;
        }

        while(len > 0){
            hash ^= (*data) * PRIME5;
            hash = rotl64(hash, 11) * PRIME1;
            data++;
            len--;
        }
        
        avalanche();

    }
};

