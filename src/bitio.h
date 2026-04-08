#ifndef BITIO_H
#define BITIO_H

#include <fstream>
#include <string>
#include <cstdint>
#include <filesystem>

#define byte_start_pos 7 

class BitWriter {
    static constexpr size_t BUFFER_SIZE = 64;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t cur_byte;
    int byte_pos;
    int buffer_pos;
    std::ofstream file;
    
public:
    explicit BitWriter(const std::string& filename);
    ~BitWriter();
    
    void write(bool bit);
    void clear_buffer();
    void write_buffer();
    void write_byte(uint8_t my_byte);
    uint8_t significant_bits();
    void write_at_the_beginning(uint8_t byte);
};

class BitReader {
    static constexpr size_t BUFFER_SIZE = 64;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t cur_byte;
    int byte_pos;
    int buffer_pos;
    std::ifstream file;
    std::uintmax_t bytes_to_process;
    size_t bytes_in_buffer;
    bool file_ended;
    int last_sign_bits;
    
public:
    explicit BitReader(const std::string& filename);
    ~BitReader();
    
    void set_last_sign_bits(int _last_sign_bits);
    bool EndOfFile();
    bool read();
    void read_to_buffer();
    uint8_t read_byte();
    uint8_t* read_buffer();

    size_t file_length();

    std::string read_byte_as_string();

    size_t buffer_size() const {return bytes_in_buffer;}
};

#endif // BITIO_H
