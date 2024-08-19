#include <iostream>
#include <string>
// #include <sstream>
#include <cstdint>  // For uint32_t
// NOTE: uin32_t is better option than unsigned integer 
// since in a 16bit processor unsigned int is of 16bits

// Design: We have an extra root node and an end node
// Root node is the entry point for all insertions
// End node is to represent the end of IP address
// And in between root and end nodes we have the actual
// IP address
// One of the major advantage here in binary trie over
// bst is that the depth is always constant. Whereas in
// bst it's O(n) i.e the worst case, where "n" is the 
// number of trees. It happens when the tree is skewed

using namespace std;

class TrieNode {
public:
    TrieNode* left;  // Represents binary 0
    TrieNode* right; // Represents binary 1
    bool isEnd;      // Indicates if the node is the end of an IP address

    TrieNode() : left(nullptr), right(nullptr), isEnd(false) {}
};

class BinaryTrie {
private:
    TrieNode* root;

    // Helper function to recursively delete the Trie
    void deleteTrie(TrieNode* node) {
        if (!node) return;
        deleteTrie(node->left);
        deleteTrie(node->right);
        delete node;
    }

    // Helper function to recursively insert a number into the Trie
    void insertHelper(TrieNode* node, uint32_t key, int index) {
        if (index < 0) {
            node->isEnd = true;
            return;
        }

        bool bit = (key >> index) & 1;
        if (bit == 0) {
            if (!node->left) node->left = new TrieNode();
            insertHelper(node->left, key, index - 1);
        }
        else {
            if (!node->right) node->right = new TrieNode();
            insertHelper(node->right, key, index - 1);
        }
    }

    // Helper function to recursively search for a number in the Trie
    bool searchHelper(TrieNode* node, uint32_t key, int index) {
        if (!node) return false;
        if (index < 0) return node->isEnd;

        bool bit = (key >> index) & 1;
        if (bit == 0) {
            return searchHelper(node->left, key, index - 1);
        }
        else {
            return searchHelper(node->right, key, index - 1);
        }
    }

    // Helper function to recursively delete a number from the Trie
    bool deleteHelper(TrieNode* node, uint32_t key, int index) {
        if (!node) return false;
        if (index < 0) {
            if (node->isEnd) {
                node->isEnd = false;
                return node->left == nullptr && node->right == nullptr;
            }
            return false;
        }

        bool bit = (key >> index) & 1;
        bool shouldDeleteChild;
        if (bit == 0) {
            shouldDeleteChild = deleteHelper(node->left, key, index - 1);
            if (shouldDeleteChild) {
                delete node->left;
                node->left = nullptr;
            }
        }
        else {
            shouldDeleteChild = deleteHelper(node->right, key, index - 1);
            if (shouldDeleteChild) {
                delete node->right;
                node->right = nullptr;
            }
        }

        return !node->isEnd && node->left == nullptr && node->right == nullptr;
    }

    // Helper function to perform in-order traversal
    void inOrderHelper(TrieNode* node, uint32_t path, int index) {
        if (!node) return;

        // Traverse the left subtree (binary 0)
        inOrderHelper(node->left, path, index - 1);

        // Process the current node
        if (node->isEnd) {
            cout << binaryToIp(path) << ", ";
        }

        // Traverse the right subtree (binary 1)
        inOrderHelper(node->right, path | (1 << index), index - 1);
    }

public:
    BinaryTrie() : root(new TrieNode()) {}

    // Destructor
    ~BinaryTrie() {
        deleteTrie(root);
    }

    // Convert IP address string to a 32-bit unsigned integer
    uint32_t ipToBinary(const string& ip) {
        // uint32_t result = 0;
        // stringstream ss(ip);
        // string octet;
        // while (getline(ss, octet, '.')) {
        //     result = (result << 8) | stoi(octet);
        // }
        // return result;

        // Initialized to 0.0.0.0
        uint8_t parts[4] = {0};

        uint16_t num = 0, index = 0;
        if(!(ip[0] >= '0' && ip[0] <= '9')) {
            return -1;
        }

        for (char ch : ip) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
            } else if (ch == '.') {
                if (index >= 4 || num > 255) 
                return -1;  // More than 4 parts or segment > 255
                parts[index++] = num;
                num = 0;
            } else {
                return -1;  // Invalid character
            }
        }

        if (index >= 4 || num > 255) return -1;
        // last num is not added within the loop 
        // since we have null instead of "."
        parts[index] = num;

        return (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | 
        parts[3];
    }

    // Convert a 32-bit unsigned integer to an IP address string
    string binaryToIp(uint32_t binary) {
        // stringstream ss;
        // ss << ((binary >> 24) & 0xFF) << '.'
        //     << ((binary >> 16) & 0xFF) << '.'
        //     << ((binary >> 8) & 0xFF) << '.'
        //     << (binary & 0xFF);
        // return ss.str();
        return to_string((binary >> 24) & 0xFF) + "." +
            to_string((binary >> 16) & 0xFF) + "." +
            to_string((binary >> 8) & 0xFF) + "." +
            to_string(binary & 0xFF);
    }

    void insert(const string& ip) {
        uint32_t key = ipToBinary(ip);
        
        cout << ((key == -1) ? "Invalid IP: " : "Valid IP: ") << ip << endl;
        
        if(key == -1)
        return;

        insertHelper(root, key, 31);  // Start from the highest bit
    }

    bool search(const string& ip) {
        uint32_t key = ipToBinary(ip);
        return searchHelper(root, key, 31);
    }

    void remove(const string& ip) {
        uint32_t key = ipToBinary(ip);
        deleteHelper(root, key, 31);
    }

    // Method to start in-order traversal from the root
    void inOrderTraversal() {
        inOrderHelper(root, 0, 31);  // Start with an empty path and highest bit index
        cout << endl;
    }
};

int main() {
    BinaryTrie trie;

    cout << "Trie Insertion: " << endl;
    trie.insert("192.168.1.1");
    trie.insert("10.0.0.1");
    trie.insert("256.0.0.1");

    cout << endl << "Trie Search: " << endl;
    cout << "Search 192.168.1.1: " << (trie.search("192.168.1.1") ? "Found" : "Not Found") << endl;
    cout << "Search 10.0.0.1: " << (trie.search("10.0.0.1") ? "Found" : "Not Found") << endl;
    cout << "Search 172.16.0.1: " << (trie.search("172.16.0.1") ? "Found" : "Not Found") << endl;
    cout << "Search 256.0.0.1: " << (trie.search("256.0.0.1") ? "Found" : "Not Found") << endl;

    trie.remove("192.168.1.1");

    cout << "Search 192.168.1.1 after removal: " << (trie.search("192.168.1.1") ? "Found" : "Not Found") << endl;

    cout << endl << "Trie Insertion: " << endl;
    trie.insert("10.8.2.5");
    trie.insert("10.0.0.1");
    trie.insert("140.0.0.1");
    trie.insert("5.80.35.6");

    cout << endl << "In-Order Traversal:" << endl;
    trie.inOrderTraversal();  // Print all IPs in the trie

    return 0;
}