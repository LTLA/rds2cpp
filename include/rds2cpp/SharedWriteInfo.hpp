#ifndef RDS2CPP_SHARED_WRITE_HPP
#define RDS2CPP_SHARED_WRITE_HPP

#include <vector>
#include <memory>
#include <string>

#include "Environment.hpp"
#include "Symbol.hpp"
#include "RObject.hpp"
#include "SEXPType.hpp"

#include "write_single_string.hpp"
#include "utils_write.hpp"

namespace rds2cpp {

struct SharedWriteInfo {
    size_t symbol_count;

    std::vector<std::unordered_map<std::string, size_t> > symbol_mappings;

public:
    SharedWriteInfo() : symbol_count(1), symbol_mappings(static_cast<int>(StringEncoding::ASCII) + 1) {}

    template<class Writer>
    void write_symbol(const std::string& value, StringEncoding encoding, Writer& writer, std::vector<unsigned char>& buffer) {
        auto& host = symbol_mappings[static_cast<int>(encoding)];

        auto it = host.find(value);
        if (it != host.end()) {
            auto ref = it->second;
            buffer.resize(4);
            buffer[2] = ref & 255;
            ref >>= 8;
            buffer[1] = ref & 255;
            ref >>= 8;
            buffer[0] = ref & 255;
            buffer[3] = static_cast<unsigned char>(SEXPType::REF);
            writer.write(buffer.data(), buffer.size());
            return;
        } else {
            buffer.clear();
            buffer.push_back(0);
            buffer.push_back(0);
            buffer.push_back(0);
            buffer.push_back(static_cast<unsigned char>(SEXPType::SYM));

            writer.write(buffer.data(), buffer.size());
            write_single_string(value, encoding, false, writer, buffer);

            host[value] = symbol_count;
            ++symbol_count;
        }
    }
};

}

#endif
