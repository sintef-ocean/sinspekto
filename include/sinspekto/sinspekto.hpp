#pragma once

#include <cinttypes>
#include <string>

/**
   @brief Helper classes and functions to enable DDS and QML interconnectivity.

*/
namespace sinspekto
{
  /**
     @brief Random char generator uses std::uniform_int_distribution
     @return a uint8_t in a uint32_t.
  */
  uint32_t random_char();

  /**
     @brief Generate a hex string of indicated length.

     This function makes use of random_char() to output a random string.

     @param[in] len Number of characters in string.
     @return random hex string.
  */
  std::string generate_hex(const uint32_t len);
}
