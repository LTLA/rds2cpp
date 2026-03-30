#ifndef RDS2CPP_VERSION_HPP
#define RDS2CPP_VERSION_HPP

#include <cstdint>

#include "sanisizer/sanisizer.hpp"

#include "utils_parse.hpp"
#include "utils_write.hpp"

/**
 * @file Version.hpp
 *
 * @brief R version information.
 */

namespace rds2cpp {

/**
 * @brief Major-minor-patch version number, usually of R itself.
 */
struct Version {
    /**
     * Major version.
     */
    std::int16_t major = 3;

    /**
     * Minor version.
     */
    std::uint8_t minor = 5;

    /**
     * Patch version.
     */
    std::uint8_t patch = 0;
};

/**
 * @cond
 */
template<typename Source_>
Version parse_version(Source_& src) {
    // Using int32_t for the versions as the R_Version macro operates with signed integers AFIACT.
    auto writer_version = quick_integer<std::int32_t>(src);
    Version output;
    output.major = (writer_version >> 16);
    output.minor = (writer_version >> 8) & 255;
    output.patch = writer_version & 255;
    return output;
}

template<class BufferedWriter_>
void write_version(const Version& version, BufferedWriter_& bufwriter) {
    // Mimic the behavior of the R_Version macro.
    inject_integer<std::int32_t, std::int32_t>(
        sanisizer::product_unsafe<std::int32_t>(version.major, 65536) +
        sanisizer::product_unsafe<std::int32_t>(version.minor, 256) +
        static_cast<std::int32_t>(version.patch),
        bufwriter
    );
}
/**
 * @endcond
 */

}

#endif
