// https://github.com/kourain/AOV-Tool/blob/main/mses_0007.h
// msesid07.cpp
// bmx.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

struct msesid07 {
    uint8_t len[4];
    uint8_t namebyid[5];
    uint8_t lenname[4];
    std::string name;
    uint8_t dirname[41];
    uint8_t lendata[4];
    std::string data;
};

static uint32_t read_u32(std::ifstream &in) {
    uint8_t b[4];
    in.read((char*)b, 4);
    return (b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24));
}

static std::string read_cstr(std::ifstream &in) {
    std::string s;
    char c;
    while (in.get(c)) {
        if (c == '\0') break;
        s.push_back(c);
    }
    return s;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: msesid07 <input> <output>\n";
        return 0;
    }

    std::ifstream in(argv[1], std::ios::binary);
    std::ofstream out(argv[2]);

    if (!in) {
        std::cout << "Cannot open input file\n";
        return 0;
    }

    // skip header 140 bytes
    in.ignore(140);

    int index = 0;

    while (in.peek() != EOF) {
        msesid07 obj;

        // len[4]
        in.read((char*)obj.len, 4);

        // namebyid[5]
        in.read((char*)obj.namebyid, 5);

        // lenname[4]
        in.read((char*)obj.lenname, 4);

        // name string
        obj.name = read_cstr(in);

        // dirname[41]
        in.read((char*)obj.dirname, 41);

        // lendata[4]
        in.read((char*)obj.lendata, 4);

        // data string
        obj.data = read_cstr(in);

        out << "=== ITEM " << index++ << " ===\n";
        out << "name: " << obj.name << "\n";
        out << "data: " << obj.data << "\n\n";
 aov_unpa  std::cout << "Done. Output -> " << argv[2] << "\n";
    return 0;cpp
