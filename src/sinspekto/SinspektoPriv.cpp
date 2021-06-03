#include <iostream>
#include <cinttypes>
#include <sstream>
#include <random>
#include <string>

#include "sinspekto/sinspekto.hpp"

namespace sinspekto
{

  uint32_t random_char()
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
  }

  std::string generate_hex(const uint32_t len)
  {
    std::stringstream ss;
    for (uint32_t i = 0; i < len; i++)
    {
      const auto rc = random_char();
      std::stringstream hexstream;
      hexstream << std::hex << rc;
      auto hex = hexstream.str();
      ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
  }

}
