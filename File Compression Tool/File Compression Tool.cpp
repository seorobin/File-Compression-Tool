#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <bitset>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left, * right;

    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

void generateHuffmanCodes(Node* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (root == nullptr) return;

    if (!root->left && !root->right) {
        huffmanCodes[root->ch] = code;
    }

    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

unordered_map<char, string> buildHuffmanTree(const string& text) {
    unordered_map<char, int> freq;
    for (char ch : text) freq[ch]++;

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        int sum = left->freq + right->freq;
        Node* newNode = new Node('\0', sum);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    Node* root = pq.top();
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    return huffmanCodes;
}

void compressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Could not open files." << endl;
        return;
    }

 
    string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    unordered_map<char, string> huffmanCodes = buildHuffmanTree(text);

   
    string encodedText = "";
    for (char ch : text) {
        encodedText += huffmanCodes[ch];
    }

    outFile << huffmanCodes.size() << '\n';
    for (auto pair : huffmanCodes) {
        outFile << pair.first << " " << pair.second << '\n';
    }

    outFile << encodedText.size() << '\n';

    for (size_t i = 0; i < encodedText.size(); i += 8) {
        bitset<8> byte(encodedText.substr(i, 8));
        outFile.put(byte.to_ulong());
    }

    outFile.close();
    cout << "File compressed successfully!" << endl;
}

void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Could not open files." << endl;
        return;
    }

    int mapSize;
    inFile >> mapSize;
    unordered_map<string, char> reverseHuffmanCodes;
    for (int i = 0; i < mapSize; ++i) {
        char ch;
        string code;
        inFile >> ch >> code;
        reverseHuffmanCodes[code] = ch;
    }

    int encodedTextLength;
    inFile >> encodedTextLength;

    string encodedText = "";
    inFile.get(); 
    while (!inFile.eof()) {
        bitset<8> byte(inFile.get());
        encodedText += byte.to_string();
    }
    encodedText = encodedText.substr(0, encodedTextLength);

 
    string currentCode = "";
    for (char bit : encodedText) {
        currentCode += bit;
        if (reverseHuffmanCodes.find(currentCode) != reverseHuffmanCodes.end()) {
            outFile.put(reverseHuffmanCodes[currentCode]);
            currentCode = "";
        }
    }

    inFile.close();
    outFile.close();
    cout << "File decompressed successfully!" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <command> <input_file> <output_file>\n";
        cerr << "Commands:\n";
        cerr << "  compress   Compress the file\n";
        cerr << "  decompress Decompress the file\n";
        return 1;
    }

    string command = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];

    if (command == "compress") {
        compressFile(inputFile, outputFile);
    }
    else if (command == "decompress") {
        decompressFile(inputFile, outputFile);
    }
    else {
        cerr << "Invalid command. Use 'compress' or 'decompress'.\n";
    }

    return 0;
}
