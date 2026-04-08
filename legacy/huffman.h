#include <iostream>
#include <unordered_map>
#include <string>
#include <queue>
#include <vector>
#include <bitset>
//#include <../src/bitio.h>

struct Node{ //virtual structure of Node for Huffman tree
    size_t weight;
    Node(size_t _weight):weight(_weight){};
    virtual ~Node() = default;
    virtual bool isLeaf() const {
        return false;
    }
    virtual void build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, std::vector<uint8_t> code) = 0;
    virtual void writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) = 0;
};
std::ostream& operator <<(std::ostream& os, const Node* node){
    os <<  node -> weight;
    return os;
}

struct LeafNode : public Node{ //Leaf node jf Huffman tree
                               //contains symbol
    uint8_t sym;
    LeafNode(size_t _weight, uint8_t _sym):Node(_weight), sym(_sym){};
    bool isLeaf() const override{
        return true;
    }
    void build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, std::vector<uint8_t> code)override{
        enc[sym] = code;
    }
    void writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) override {
        writer.write(true);
        //std::cout << "1";
        alphabet.push_back(sym);
    }
};

struct InternalNode : public Node{ //Internal node of huffman tree
                                   //contains links to other Nodes
    Node* left;
    Node* right;
    InternalNode(size_t _weight, Node* _left, Node* _right):
                Node(_weight), left(_left), right(_right){};
    ~InternalNode(){
        delete left;
        delete right;
    }
    void build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, std::vector<uint8_t> code)override{
        if (left){
            code.push_back(0);
            left->build_map(enc, code);
            code.pop_back();
        }
        if (right){
            code.push_back(1);
            right->build_map(enc, code);
        }
    }

    void writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) override {
        writer.write(false);
        //std::cout << "0";
        if (left){
            left->writing_tree(writer, alphabet);
        }
        if (right){
            right->writing_tree(writer, alphabet);
        }
    }
};

struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const {
        return a->weight > b->weight;
    }
};

struct HuffmanTree{
    Node* root;
    uint8_t alphabet_size;
    HuffmanTree(const std::string& filename){
        std::unordered_map<uint8_t, size_t> alphabet;
        BitReader reader(filename);
        while(!reader.EndOfFile()){
            uint8_t byte = reader.read_byte();
            alphabet[byte]++;
           // std::cout << byte << std::endl;
        }
        alphabet_size = alphabet.empty() ? 0 : static_cast<uint8_t>(alphabet.size() - 1);
        //std::cout << (int) alphabet_size << std::endl;
        //for (const auto& pair  : alphabet){
          //   std::cout << (int)pair.first << " - " << pair.second << std::endl;
        //}

    //Create priority_queue with min heap
        std::priority_queue<
            Node*,              // Type: pointer to node
            std::vector<Node*>, // Container
            CompareNodes        // Comparator
        > pq;

        //Fill the queue
        for (const auto& pair : alphabet) {
            pq.push(new LeafNode(pair.second, pair.first));
        }

        //Create the tree
        while(pq.size() > 1){
            Node* tmp1 = pq.top(); pq.pop();
            Node* tmp2 = pq.top(); pq.pop();
            //std::cout << tmp1 << " with " << tmp2 << std::endl;
            Node* tmp = new InternalNode(tmp1->weight + tmp2->weight, tmp1, tmp2);
            pq.push(tmp);
        }
        if (!pq.empty()) {
                root = pq.top(); pq.pop();
            } else {
                root = nullptr;
        }
    }
    ~HuffmanTree(){
        delete root;
    }

    std::unordered_map<uint8_t, std::vector<uint8_t>> build_encoding_map(){
        std::unordered_map<uint8_t, std::vector<uint8_t>> enc;
        if(root){
            if (root->isLeaf()) { 
                std::vector<uint8_t> code{1};
                root->build_map(enc, code);  // Single character gets code "0"
            }else{
                std::vector<uint8_t> code;
                root->build_map(enc, code); 
            }
        }
        return enc;
    }
    void write_tree(BitWriter& writer){
        writer.write_byte(0);
        std::vector<uint8_t> alphabet;
        alphabet.reserve(alphabet_size);
        root->writing_tree(writer, alphabet);
        //std::cout << std::endl;
        for(auto it = alphabet.begin(); it != alphabet.end(); ++it){
            writer.write_byte(*it);
            //std::cout << *it << std::endl;
        }
    }
};

void HuffmanCode(const std::string& input){
    if(input.empty()){return;}
    HuffmanTree tree(input);
    auto encoding_map = tree.build_encoding_map();
    //std::cout << "Huffman codes:" << std::endl;
    //for(const auto& pair : encoding_map){
        //std::cout << pair.first << " - ";
        //for(auto& c : pair.second){
            //std::cout << (int)c << " ";
        //}
        //std::cout << std::endl;
    //}
    BitWriter writer(input+".bin");
    tree.write_tree(writer);
    BitReader reader(input);
    uint8_t byte; 
   // std::cout << "written symbols" << std::endl;
    while(!reader.EndOfFile()){
        byte = reader.read_byte();
        //std::cout << byte << std::endl;
        const uint8_t* data = encoding_map[byte].data();
        const size_t size = encoding_map[byte].size();
        for (size_t i = 0; i < size; ++i) {
            writer.write(data[i]);
            //std::cout << (int) data[i];
        }
        //std::cout << std::endl;
    }
    writer.write_at_the_beginning(writer.significant_bits());

}

struct DecodeNode{
    virtual bool is_leaf() const = 0;
    DecodeNode* left;
    DecodeNode* right;
    uint8_t symbol;
    DecodeNode(DecodeNode* _left, DecodeNode* _right, uint8_t _symbol):
                left(_left), right(_right), symbol(_symbol){}
    virtual void build_tree(BitReader& reader) = 0;
    virtual void fill_tree(BitReader& reader) = 0;
    virtual ~DecodeNode() = default;
};
struct LeafDecodeNode : DecodeNode{
    LeafDecodeNode(uint8_t _symbol):DecodeNode(nullptr, nullptr, _symbol){};
    LeafDecodeNode():DecodeNode(nullptr, nullptr, 0){};
    bool is_leaf() const override{return true;}
    void fill_tree(BitReader& reader) override {
        symbol = reader.read_byte();
    }
    void build_tree(BitReader& reader)  override {}
   
};
struct InternalDecodeNode : DecodeNode{
    InternalDecodeNode():DecodeNode(nullptr, nullptr, 0){}    
    InternalDecodeNode(DecodeNode* _left, DecodeNode* _right):DecodeNode(_left, _right, 0){}
    bool is_leaf() const override{return false;}
    void build_tree(BitReader& reader) override {
        if(reader.read()){
            left = new LeafDecodeNode;
        }
        else{
            left = new InternalDecodeNode;
            left -> build_tree(reader);
        }
        if(reader.read()){
            right = new LeafDecodeNode;
        }
        else{
            right = new InternalDecodeNode;
            right -> build_tree(reader);
        }
    }
    void fill_tree(BitReader& reader) override {
        if(left) {left -> fill_tree(reader);}
        if (right) {right -> fill_tree(reader);}
    }
};

struct DecodeTree{
    DecodeNode* root;
    DecodeTree(BitReader& reader){
        if(reader.read()){
            root = new LeafDecodeNode(reader.read_byte());
        }
        else{
            root = new InternalDecodeNode();
            root->build_tree(reader);
            root->fill_tree(reader);
        }
    }
};

void HuffmanDecode(const std::string& input){
    if(input.empty()){return;}
    BitReader reader(input);
    reader.set_last_sign_bits(reader.read_byte());
    BitWriter writer(input + ".decoded");
    DecodeTree tree(reader);
    DecodeNode* cur_node;
    //std::cout << "out symbols:" << std::endl;
    while(!reader.EndOfFile()){
        cur_node = tree.root;
        while(!cur_node->is_leaf()){
            if(reader.read()){
                cur_node = cur_node->right;
            }
            else{
                cur_node = cur_node->left;
            }
        }
        writer.write_byte(cur_node->symbol);
        
        //std::cout << cur_node->symbol << std::endl;
    }
}
