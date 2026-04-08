#include "bitio.h"
#include <iostream>
#include <stdexcept>

BitWriter::BitWriter(const std::string& filename) 
    : cur_byte(0), byte_pos(byte_start_pos), buffer_pos(0) {
    clear_buffer();
    file.open(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("couldn't open " + filename);
    }
    if (file.fail()) {
        throw std::runtime_error("error with file " + filename);
    }
}

BitWriter::~BitWriter() {
    write_buffer();
    file.close();
}

void BitWriter::write(bool bit) {
    if (bit) {
        cur_byte |= (1 << byte_pos);
    }
    byte_pos--;
    if (byte_pos < 0) {
        byte_pos = byte_start_pos;
        write_byte(cur_byte);
        cur_byte = 0;
    }
}

void BitWriter::clear_buffer() {
    for (size_t i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
}

void BitWriter::write_buffer() {
    if (byte_pos < byte_start_pos) {
        byte_pos = byte_start_pos;
        write_byte(cur_byte);
    }
    if (buffer_pos > 0) {
        file.write(reinterpret_cast<const char*>(buffer), buffer_pos);
        buffer_pos = 0;
    }
}

void BitWriter::write_byte(uint8_t my_byte) {
    if (byte_pos < byte_start_pos) {
        byte_pos = byte_start_pos;
        write_byte(cur_byte);
        cur_byte = 0;
    }
    buffer[buffer_pos] = my_byte;
    buffer_pos += 1;
    if (buffer_pos == BUFFER_SIZE) {
        write_buffer();
    }
}

uint8_t BitWriter::significant_bits() {
    return byte_start_pos - byte_pos;
}

void BitWriter::write_at_the_beginning(uint8_t byte) {
    write_buffer();
    std::streampos currentPos = file.tellp();
    file.seekp(0, std::ios::beg);
    file.put(byte);
    file.seekp(currentPos);
}

BitReader::BitReader(const std::string& filename)
    : cur_byte(0), byte_pos(0), last_sign_bits(byte_start_pos) {
    file.open(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("couldn't open " + filename);
    }
    if (file.fail()) {
        throw std::runtime_error("error with file " + filename);
    }
    bytes_to_process = std::filesystem::file_size(filename);
    file_ended = false;
    read_to_buffer();
}

void BitReader::set_last_sign_bits(int _last_sign_bits) {
    last_sign_bits = _last_sign_bits;
}

bool BitReader::EndOfFile() {
    return (file_ended && (byte_pos <= (byte_start_pos + 1 - last_sign_bits)));
}

BitReader::~BitReader() {
    file.close();
}

bool BitReader::read() {
    if (byte_pos == 0) {
        cur_byte = read_byte();
        byte_pos = byte_start_pos + 1;
    }
    byte_pos--;
    return (cur_byte >> byte_pos) & 1;
}

void BitReader::read_to_buffer() {
    if (bytes_to_process == 0) {
        file_ended = true;
        return;
    }
    bytes_in_buffer = (bytes_to_process >= BUFFER_SIZE) ? BUFFER_SIZE : bytes_to_process;
    bytes_to_process -= bytes_in_buffer;
    file.read(reinterpret_cast<char*>(buffer), bytes_in_buffer);
    buffer_pos = -1;
    byte_pos = 0;
}

uint8_t BitReader::read_byte() {
    if (buffer_pos == bytes_in_buffer - 2) {
        cur_byte = buffer[buffer_pos + 1];
        byte_pos = 0;
        read_to_buffer();
        return cur_byte;
    }
    byte_pos = 0;
    buffer_pos++;
    return buffer[buffer_pos];
}

uint8_t* BitReader::read_buffer(){
    read_to_buffer();
    return buffer;
}

size_t BitReader::file_length() {
    auto pos = file.tellg();
    file.seekg(0, std::ios::end);
    size_t len = file.tellg();
    file.seekg(pos);
    return len;
}

std::string BitReader::read_byte_as_string() {
    uint8_t byte;
    if (buffer_pos == bytes_in_buffer - 2) {
        byte = buffer[buffer_pos + 1];
        read_to_buffer();
    } else {
        buffer_pos++;
        byte = buffer[buffer_pos];
        byte_pos = 0;
    }
    std::string ans;
    for (int i = 7; i >= 0; i--) {
        if ((byte >> i) & 1) {
            ans += "1";
        } else {
            ans += "0";
        }
    }
    return ans;
}
