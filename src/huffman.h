#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <unordered_map>
#include <string>
#include <queue>
#include <vector>
#include "bitio.h"

struct Node {
    size_t weight;
    Node(size_t _weight) : weight(_weight) {}
    virtual ~Node() = default;
    virtual bool isLeaf() const;
    virtual void build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, 
                          std::vector<uint8_t> code) = 0;
    virtual void writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) = 0;
};

std::ostream& operator<<(std::ostream& os, const Node* node);

struct LeafNode : public Node {
    uint8_t sym;
    LeafNode(size_t _weight, uint8_t _sym);
    bool isLeaf() const override;
    void build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, 
                  std::vector<uint8_t> code) override;
    void writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) override;
};

struct InternalNode : public Node {
    Node* left;
    Node* right;
    InternalNode(size_t _weight, Node* _left, Node* _right);
    ~InternalNode();
    void build_map(std::unordered_map<uint8_t, std::vector<uint8_t>>& enc, 
                  std::vector<uint8_t> code) override;
    void writing_tree(BitWriter& writer, std::vector<uint8_t>& alphabet) override;
};

struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const;
};

struct HuffmanTree {
    Node* root;
    uint8_t alphabet_size;
    HuffmanTree(const std::string& filename);
    ~HuffmanTree();
    std::unordered_map<uint8_t, std::vector<uint8_t>> build_encoding_map();
    void write_tree(BitWriter& writer);
};

void HuffmanCode(const std::string& input);

struct DecodeNode {
    virtual bool is_leaf() const = 0;
    DecodeNode* left;
    DecodeNode* right;
    uint8_t symbol;
    DecodeNode(DecodeNode* _left, DecodeNode* _right, uint8_t _symbol);
    virtual void build_tree(BitReader& reader) = 0;
    virtual void fill_tree(BitReader& reader) = 0;
    virtual ~DecodeNode() = default;
};

struct LeafDecodeNode : DecodeNode {
    LeafDecodeNode(uint8_t _symbol);
    LeafDecodeNode();
    bool is_leaf() const override;
    void fill_tree(BitReader& reader) override;
    void build_tree(BitReader& reader) override;
};

struct InternalDecodeNode : DecodeNode {
    InternalDecodeNode();
    InternalDecodeNode(DecodeNode* _left, DecodeNode* _right);
    bool is_leaf() const override;
    void build_tree(BitReader& reader) override;
    void fill_tree(BitReader& reader) override;
};

struct DecodeTree {
    DecodeNode* root;
    DecodeTree(BitReader& reader);
};

void HuffmanDecode(const std::string& input);

#endif // HUFFMAN_H
