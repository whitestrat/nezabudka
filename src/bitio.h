#include <fstream>
#include <string>
#include <iostream>
#include <cstdint>
#include <filesystem>

#define byte_start_pos 7 

class BitWriter{
    static constexpr size_t BUFFER_SIZE = 64;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t cur_byte;
    int byte_pos;
    int buffer_pos;
    std::ofstream file;
    public:
    explicit BitWriter(const std::string& filename):cur_byte(0), byte_pos(byte_start_pos), buffer_pos(0){
        clear_buffer();
        file.open(filename, std::ios::binary);
        if(!file.is_open()){
            throw std::runtime_error("couldn't open " + filename);
        }
        if(file.fail()){
            throw std::runtime_error("error with file " + filename);
        }
    }

    ~BitWriter(){
        write_buffer();
        file.close();
    }

    void write(bool bit){
        if(bit){
            cur_byte |= (1 << byte_pos);
        }
        byte_pos--;
        if(byte_pos < 0){
            write_byte(cur_byte);
            byte_pos = byte_start_pos;
            cur_byte = 0;
        }
    }

    void clear_buffer(){
        for(size_t i = 0; i < BUFFER_SIZE; i++){
            buffer[i] = 0;
        }
    }

    void write_buffer(){
        if (buffer_pos > 0) {
            file.write(reinterpret_cast<const char*>(buffer), buffer_pos);
            buffer_pos = 0;
        }
    }

    void write_byte(uint8_t my_byte){
        if(byte_pos < byte_start_pos){
            byte_pos = byte_start_pos;
            write_byte(cur_byte);
        }
        buffer[buffer_pos] = my_byte;
        buffer_pos += 1;
        if(buffer_pos == BUFFER_SIZE){
                write_buffer();
        }
    }
};

class BitReader{
    static constexpr size_t BUFFER_SIZE = 64;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t cur_byte;
    int byte_pos;
    int buffer_pos;
    std::ifstream file;
    std::uintmax_t bytes_to_process;
    size_t bytes_in_buffer;
    bool file_ended;

    public:
    
    explicit BitReader(const std::string& filename):cur_byte(0){
        file.open(filename, std::ios::binary);
        if(!file.is_open()){
            throw std::runtime_error("couldn't open " + filename);
        }
        if(file.fail()){
            throw std::runtime_error("error with file " + filename);
        }
        bytes_to_process = std::filesystem::file_size(filename);
        read_to_buffer();
    }

    ~BitReader(){
        file.close();
    }

    bool read(){
        if(buffer_pos == BUFFER_SIZE-1){read_to_buffer();}
        else{
            if(byte_pos == 0){
                buffer_pos++;
                //std::cout << " buffer_pos = " << buffer_pos << std::endl;
                cur_byte = buffer[buffer_pos];
                byte_pos = byte_start_pos+1;
            }
        }
        byte_pos--;
        return (cur_byte >> byte_pos) & 1;
    }

    void read_to_buffer(){
        if(bytes_to_process == 0){
            file_ended = true;
            return;
        }
        bytes_in_buffer = (bytes_to_process >= BUFFER_SIZE) ? BUFFER_SIZE : bytes_to_process;
        bytes_to_process -= bytes_in_buffer;
        file.read(reinterpret_cast<char*>(buffer), bytes_in_buffer);
        buffer_pos = -1;
        byte_pos = 0;
    }

    uint8_t read_byte(){
        if(buffer_pos == BUFFER_SIZE-1){read_to_buffer();}
        byte_pos = 0;
        buffer_pos++;
        return buffer[buffer_pos];
    }

    std::string read_byte_as_string(){
        if(buffer_pos == BUFFER_SIZE-1){read_to_buffer();}
        byte_pos = 0;
        buffer_pos++;
        //std::cout << " buffer_pos = " << buffer_pos << std::endl;
        uint8_t byte = buffer[buffer_pos];
        std::string ans;
        for(int i = 7; i >= 0; i--){
            if((byte >> i) & 1){
                ans += "1";
            }
            else{
                ans += "0";
            }
        }
        
        return ans;
        
    }

    bool end(){return file_ended;}
};
