#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

struct Record {
    uint8_t len[4];
    uint8_t namebyid[5];
    uint8_t lenname_raw[4];
    std::string name;
    uint8_t dirname[41];
    uint8_t lendata_raw[4];
    std::string data;
};

static uint32_t le32(const uint8_t b[4]) {
    return (uint32_t)b[0]
        | ((uint32_t)b[1] << 8)
        | ((uint32_t)b[2] << 16)
        | ((uint32_t)b[3] << 24);
}

static bool read_exact(std::ifstream &in, void* buf, size_t n) {
    return (bool)in.read(reinterpret_cast<char*>(buf), n);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: msesv2 <input> <output>\n";
        return 0;
    }

    std::ifstream in(argv[1], std::ios::binary);
    std::ofstream out(argv[2]);

    if (!in) {
        std::cout << "Cannot open input\n";
        return 0;
    }

    // Skip header
    in.seekg(0, std::ios::end);
    size_t filesize = in.tellg();
    in.seekg(140, std::ios::beg);

    size_t pos = 140;
    int index = 0;

    while (pos < filesize) {
        Record r{};

        // --- fixed fields ---
        if (!read_exact(in, r.len, 4)) break;
        if (!read_exact(in, r.namebyid, 5)) break;
        if (!read_exact(in, r.lenname_raw, 4)) break;

        pos += 4 + 5 + 4;

        uint32_t lenname = le32(r.lenname_raw);

        // --- read name using lenname ---
        if (lenname > 0 && lenname < 0x10000) {
            std::vector<char> buf(lenname);
            if (!read_exact(in, buf.data(), lenname)) break;
            r.name.assign(buf.begin(), buf.end());

            pos += lenname;

            // skip null if present
            if (in.peek() == '\0') {
                in.get();
                pos += 1;
            }
        } else {
            // fallback if length err
            std::string tmp;
            char c;
            while (in.get(c)) {
                pos++;
                if (c == '\0') break;
                tmp.push_back(c);
            }
            r.name = tmp;
        }

        // --- dirname ---
        if (!read_exact(in, r.dirname, 41)) break;
        pos += 41;

        // --- lendata ---
        if (!read_exact(in, r.lendata_raw, 4)) break;
        pos += 4;

        uint32_t lendata = le32(r.lendata_raw);

        // --- read data using lendata ---
        if (lendata > 0 && lendata < 0x1000000) {
            std::vector<char> buf(lendata);
            if (!read_exact(in, buf.data(), lendata)) break;
            r.data.assign(buf.begin(), buf.end());

            pos += lendata;

            // skip null 
            if (in.peek() == '\0') {
                in.get();
                pos += 1;
            }
        } else {
            // fallback
            std::string tmp;
            char c;
            while (in.get(c)) {
                pos++;
                if (c == '\0') break;
                tmp.push_back(c);
            }
            r.data = tmp;
        }

        // --- output ---
        out << "=== ITEM " << index++ << " ===\n";
        out << "name: " << r.name << "\n";
        out << "data: " << r.data << "\n\n";
    }

    std::cout << "Done\n";
    return 0;
}
