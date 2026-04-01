#ifndef RDS2CPP_UTILS_WRITE_HPP
#define RDS2CPP_UTILS_WRITE_HPP

#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <type_traits>

#include "utils_parse.hpp"
#include "RObject.hpp"

namespace rds2cpp {

template<typename Target_, typename Type_, class BufferedWriter_>
void inject_integer(Type_ value, BufferedWriter_& bufwriter) {
    static_assert(std::is_same<Target_, Type_>::value); // force users to set the type to avoid implicit typing.
    auto ptr = reinterpret_cast<unsigned char*>(&value);
    constexpr std::size_t width = sizeof(Target_);
    if (little_endian()) {
        std::reverse(ptr, ptr + width);
    }
    bufwriter.write(ptr, width);
}

template<class BufferedWriter_>
void inject_length(std::size_t value, BufferedWriter_& bufwriter) {
    if (value <= 2147483647) {
        inject_integer<std::int32_t, std::int32_t>(value, bufwriter);
    } else {
        // See get_length() for the inverse logic.
        inject_integer<std::int32_t, std::int32_t>(-1, bufwriter);
        inject_integer<std::uint32_t>(sanisizer::cast<std::uint32_t>(value >> 32), bufwriter);
        inject_integer<std::uint32_t>(static_cast<std::uint32_t>(value & 0xFFFFFFFF), bufwriter); // must fit in a uint32 as we're taking the lowest 32 bits.
    }
}

template<class Object, typename = int>
struct has_attributes_for_writing {
    static constexpr bool value = false;
};

template<class Object>
struct has_attributes_for_writing<Object, decltype((void) std::declval<Object>().attributes, 0)> {
    static constexpr bool value = true;
};

template<class Shared_>
unsigned char inject_attribute_header(const std::vector<Attribute>& attributes, const Shared_& shared) {
    unsigned char bit = 0;

    if (!attributes.empty()) {
        bit |= 0x2;
        for (const auto& x : attributes) {
            if (shared.known_symbols[x.name.index].name == "class") {
                bit |= 0x1;
                break;
            }
        }
    }

    return bit;
}

template<class Object_, class BufferedWriter_, class Shared_>
void inject_header(Object_& vec, BufferedWriter_& bufwriter, const Shared_& shared) {
    Header details;
    details[0] = 0;
    details[1] = 0;

    if constexpr(has_attributes_for_writing<Object_>::value) {
        details[2] = inject_attribute_header(vec.attributes, shared);
    } else {
        details[2] = 0;
    }

    details[3] = static_cast<unsigned char>(vec.type()); // Cast from enum should be safe, as SEXPTypes are also unsigned chars.
    bufwriter.write(details.data(), details.size());
}

template<class BufferedWriter_>
void inject_header(SEXPType type, BufferedWriter_& bufwriter) {
    Header details;
    details[0] = 0;
    details[1] = 0;
    details[2] = 0;
    details[3] = static_cast<unsigned char>(type);
    bufwriter.write(details.data(), details.size());
}

template<class BufferedWriter_, class Shared_>
void inject_header(SEXPType type, const std::vector<Attribute>& attributes, BufferedWriter_& bufwriter, const Shared_& shared) {
    Header details;
    details[0] = 0;
    details[1] = 0;
    details[2] = inject_attribute_header(attributes, shared);
    details[3] = static_cast<unsigned char>(type);
    bufwriter.write(details.data(), details.size());
}

template<class BufferedWriter_>
void inject_next_pairlist_header(bool tagged, BufferedWriter_& bufwriter) {
    Header details;
    details[0] = 0;
    details[1] = 0;
    details[2] = (tagged ? 0x4 : 0); // has tag.
    details[3] = static_cast<unsigned char>(SEXPType::LIST);
    bufwriter.write(details.data(), details.size());
}

}

#endif
