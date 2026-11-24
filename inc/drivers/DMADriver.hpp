/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include <cstdint>

/**
 * @namespace hardware
 * @brief Hardware interfaces for low‑level device operations (Camera, DMA, LED, GPU).
 */
namespace hardware
{
  struct VirtualMemory; // Forward declaration of VirtualMemory structure.

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

        virtual bool connect(const VirtualMemory&) = 0;
        virtual bool cpu(uint16_t*&) const = 0;
        virtual const VirtualMemory& memory() const = 0;
    };
  } // namespace drivers
} // namespace hardware
