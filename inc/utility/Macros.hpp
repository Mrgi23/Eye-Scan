/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

/**
 * @def DISABLE_COPY
 * @brief Delete copy constructor and copy assignment operator for the given class.
 *
 * @param ClassName Name of the class for which copy operations are disabled.
 */
#define DISABLE_COPY(ClassName)                                                     \
  ClassName(const ClassName&) = delete;                                             \
  ClassName& operator=(const ClassName&) = delete;

/**
 * @def DISABLE_MOVE
 * @brief Delete move constructor and move assignment operator for the given class.
 *
 * @param ClassName Name of the class for which move operations are disabled.
 */
#define DISABLE_MOVE(ClassName)                                                     \
  ClassName(ClassName&&) = delete;                                                  \
  ClassName& operator=(ClassName&&) = delete;

/**
 * @def DISABLE_COPY_AND_MOVE
 * @brief Disable both copy and move operations for the given class.
 *
 * @param ClassName Name of the class for which copy and move operations are
 * disabled.
 */
#define DISABLE_COPY_AND_MOVE(ClassName)                                            \
  DISABLE_COPY(ClassName)                                                           \
  DISABLE_MOVE(ClassName)
