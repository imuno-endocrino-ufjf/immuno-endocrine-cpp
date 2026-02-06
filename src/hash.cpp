#include "hash.hpp"

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string hashJson(const nlohmann::json& j) {
    std::string dumped = j.dump(); // JSON normalizado

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(
        reinterpret_cast<const unsigned char*>(dumped.c_str()),
        dumped.size(),
        hash
    );

    std::stringstream ss;
    for (unsigned char c : hash) {
        ss << std::hex
           << std::setw(2)
           << std::setfill('0')
           << static_cast<int>(c);
    }

    return ss.str();
}
