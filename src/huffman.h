#include <iostream>
#include <unordered_map>
#include <string>
#include <queue>
#include <vector>
struct Node{ 
    size_t weight;
    Node(size_t _weight):weight(_weight){};
    ~Node() = default;
    virtual bool isLeaf() const {
        return false;
    }
    virtual void build_map(std::unordered_map<char, std::string>& enc, std::string code) = 0;
};
std::ostream& operator <<(std::ostream& os, const Node* node){
    os <<  node -> weight;
    return os;
}

struct LeafNode : public Node{
    char sym;
    LeafNode(size_t _weight, char _sym):Node(_weight), sym(_sym){};
    bool isLeaf() const override{
        return true;
    }
    void build_map(std::unordered_map<char, std::string>& enc, std::string code)override{
        enc[sym] = code;
    }
};

struct InternalNode : public Node{
    Node* left;
    Node* right;
    InternalNode(size_t _weight, Node* _left, Node* _right):
                Node(_weight), left(_left), right(_right){};
    ~InternalNode(){
        delete left;
        delete right;
    }
    void build_map(std::unordered_map<char, std::string>& enc, std::string code)override{
        if (left){
            left->build_map(enc, code + "0");
        }
        if (right){
            right->build_map(enc, code + "1");
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
    HuffmanTree(const std::string& input){
        std::unordered_map<char, size_t> alphabet;
    for (char ch : input){
        alphabet[ch] ++;
    }

    // for (const auto& pair  : alphabet){
    //     std::cout << pair.first << " - " << pair.second << std::endl;
    // }

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

    std::unordered_map<char, std::string> build_encoding_map(){
        std::unordered_map<char, std::string> enc;
        if(root){
            if (root->isLeaf()) {
                    root->build_map(enc, "0");  // Single character gets code "0"
                } else {
                    root->build_map(enc, "");
            }
        }
        return enc;
    }
};

void HuffmanCode(const std::string& input){
    if(input.empty()){return;}
    HuffmanTree tree(input);
    auto encoding_map = tree.build_encoding_map();
    std::string code;
    for (char ch : input){
        code += encoding_map[ch];
    }
    std::cout << code << std::endl;
    // for (const auto& pair  : encoding_map){
    //     std::cout << pair.first << " - " << pair.second << std::endl;
    // }
}


