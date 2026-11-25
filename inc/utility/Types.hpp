/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include <bitset>
#include <cstddef>
#include <string>

#include <opencv2/core/mat.hpp>

constexpr size_t BITSET_SIZE = 2048;

/**
 * @namespace types
 * @brief Lightweight fundamental enums and utility objects used across the system.
 */
namespace types
{
  /**
   * @struct EyeCode
   * @brief Encoded representation of the eye features.
   *
   * @param std::bitset<BITSET_SIZE> The encoded data.
   * @param std::bitset<BITSET_SIZE> The mask indicating valid bits.
   */
  struct EyeCode
  {
    std::bitset<BITSET_SIZE> data;
    std::bitset<BITSET_SIZE> mask = std::bitset<BITSET_SIZE>().set();
  };

  /**
   * @struct PersonView
   * @brief Lightweight view of a person for search results.
   *
   * @param std::string The ID of the person.
   * @param std::string The first name of the person.
   * @param std::string The last name of the person.
   * @param std::string The timestamp of the last modification.
   * @param size_t The number of entries currently stored for the person.
   * @param size_t The index of the matched entry.
   * @param std::string The image path of the matched entry.
   * @param double The match score.
   */
  struct PersonView
  {
    std::string id;
    std::string firstName;
    std::string lastName;
    std::string modifiedAt;
    size_t count;
    size_t index;
    std::string image;
    double match;
  };

  /**
   * @struct Size
   * @brief Two‑dimensional extent.
   *
   * @param size_t Horizontal extent.
   * @param size_t Vertical extent.
   */
  struct Size
  {
    size_t width = 0;
    size_t height = 0;
  };
} // namespace types
