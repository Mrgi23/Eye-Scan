/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Types.hpp"

/**
 * @namespace drivers
 * @brief Low-level hardware driver interfaces.
 */
namespace drivers
{
  /**
   * @class IDMADriver
   * @brief Virtual interface for DMA hardware drivers.
   */
  class IDMADriver
  {
    public:
      virtual ~IDMADriver() = default;

      virtual bool connect(const types::VirtualMemory&) = 0;
      virtual bool cpu(uint16_t*&) const = 0;
      virtual const types::VirtualMemory& memory() const = 0;
  };
} // namespace drivers
