#include "huffman.h"
#include <iostream>

// Node implementation
bool Node::isLeaf() const {
    return false;
}

std::ostream& operator<<(std::ostream& os, const Node* node) {
    os << node->weight;
    return os;
}

// LeafNode implementation
LeafNode::LeafNode(size_t _weight, uint8_t _sym) 
    : Node(_weight), sym(_sym) {}

bool LeafNode::isLeaf() const {
    return true;
}

void LeafNode::build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, 
                         std::vector<uint8_t> code) {
    enc[sym] = code;
}

void LeafNode::writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) {
    writer.write(true);
    alphabet.push_back(sym);
}

// InternalNode implementation
InternalNode::InternalNode(size_t _weight, Node* _left, Node* _right)
    : Node(_weight), left(_left), right(_right) {}

InternalNode::~InternalNode() {
    delete left;
    delete right;
}

void InternalNode::build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, 
                             std::vector<uint8_t> code) {
    if (left) {
        code.push_back(0);
        left->build_map(enc, code);
        code.pop_back();
    }
    if (right) {
        code.push_back(1);
        right->build_map(enc, code);
    }
}

void InternalNode::writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) {
    writer.write(false);
    if (left) {
        left->writing_tree(writer, alphabet);
    }
    if (right) {
        right->writing_tree(writer, alphabet);
    }
}

// CompareNodes implementation
bool CompareNodes::operator()(const Node* a, const Node* b) const {
    return a->weight > b->weight;
}

// HuffmanTree implementation
HuffmanTree::HuffmanTree(const std::string& filename) {
    std::unordered_map<uint8_t, size_t> alphabet;
    BitReader reader(filename);
    while (!reader.EndOfFile()) {
        uint8_t byte = reader.read_byte();
        alphabet[byte]++;
    }
    alphabet_size = alphabet.empty() ? 0 : static_cast<uint8_t>(alphabet.size() - 1);

    std::priority_queue<Node*, std::vector<Node*>, CompareNodes> pq;

    for (const auto& pair : alphabet) {
        pq.push(new LeafNode(pair.second, pair.first));
    }

    while (pq.size() > 1) {
        Node* tmp1 = pq.top(); pq.pop();
        Node* tmp2 = pq.top(); pq.pop();
        Node* tmp = new InternalNode(tmp1->weight + tmp2->weight, tmp1, tmp2);
        pq.push(tmp);
    }
    
    if (!pq.empty()) {
        root = pq.top(); pq.pop();
    } else {
        root = nullptr;
    }
}

HuffmanTree::~HuffmanTree() {
    delete root;
}

std::unordered_map<uint8_t, std::vector<uint8_t>> HuffmanTree::build_encoding_map() {
    std::unordered_map<uint8_t, std::vector<uint8_t>> enc;
    if (root) {
        if (root->isLeaf()) {
            std::vector<uint8_t> code{1};
            root->build_map(enc, code);
        } else {
            std::vector<uint8_t> code;
            root->build_map(enc, code);
        }
    }
    return enc;
}

void HuffmanTree::write_tree(BitWriter& writer) {
    writer.write_byte(0);
    std::vector<uint8_t> alphabet;
    alphabet.reserve(alphabet_size);
    root->writing_tree(writer, alphabet);
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it) {
        writer.write_byte(*it);
    }
}

void HuffmanCode(const std::string& input) {
    if (input.empty()) {
        return;
    }
    HuffmanTree tree(input);
    auto encoding_map = tree.build_encoding_map();
    BitWriter writer(input + ".bin");
    tree.write_tree(writer);
    BitReader reader(input);
    uint8_t byte;
    
    while (!reader.EndOfFile()) {
        byte = reader.read_byte();
        const uint8_t* data = encoding_map[byte].data();
        const size_t size = encoding_map[byte].size();
        for (size_t i = 0; i < size; ++i) {
            writer.write(data[i]);
        }
    }
    writer.write_at_the_beginning(writer.significant_bits());
}

// DecodeNode implementation
DecodeNode::DecodeNode(DecodeNode* _left, DecodeNode* _right, uint8_t _symbol)
    : left(_left), right(_right), symbol(_symbol) {}

// LeafDecodeNode implementation
LeafDecodeNode::LeafDecodeNode(uint8_t _symbol) 
    : DecodeNode(nullptr, nullptr, _symbol) {}

LeafDecodeNode::LeafDecodeNode() 
    : DecodeNode(nullptr, nullptr, 0) {}

bool LeafDecodeNode::is_leaf() const {
    return true;
}

void LeafDecodeNode::fill_tree(BitReader& reader) {
    symbol = reader.read_byte();
}

void LeafDecodeNode::build_tree(BitReader& reader) {}

// InternalDecodeNode implementation
InternalDecodeNode::InternalDecodeNode() 
    : DecodeNode(nullptr, nullptr, 0) {}

InternalDecodeNode::InternalDecodeNode(DecodeNode* _left, DecodeNode* _right) 
    : DecodeNode(_left, _right, 0) {}

bool InternalDecodeNode::is_leaf() const {
    return false;
}

void InternalDecodeNode::build_tree(BitReader& reader) {
    if (reader.read()) {
        left = new LeafDecodeNode;
    } else {
        left = new InternalDecodeNode;
        left->build_tree(reader);
    }
    if (reader.read()) {
        right = new LeafDecodeNode;
    } else {
        right = new InternalDecodeNode;
        right->build_tree(reader);
    }
}

void InternalDecodeNode::fill_tree(BitReader& reader) {
    if (left) {
        left->fill_tree(reader);
    }
    if (right) {
        right->fill_tree(reader);
    }
}

// DecodeTree implementation
DecodeTree::DecodeTree(BitReader& reader) {
    if (reader.read()) {
        root = new LeafDecodeNode(reader.read_byte());
    } else {
        root = new InternalDecodeNode();
        root->build_tree(reader);
        root->fill_tree(reader);
    }
}

void HuffmanDecode(const std::string& input) {
    if (input.empty()) {
        return;
    }
    BitReader reader(input);
    reader.set_last_sign_bits(reader.read_byte());
    BitWriter writer(input + ".decoded");
    DecodeTree tree(reader);
    DecodeNode* cur_node;
    
    while (!reader.EndOfFile()) {
        cur_node = tree.root;
        while (!cur_node->is_leaf()) {
            if (reader.read()) {
                cur_node = cur_node->right;
            } else {
                cur_node = cur_node->left;
            }
        }
        writer.write_byte(cur_node->symbol);
    }
}
