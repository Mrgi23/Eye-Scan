/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "drivers/DMADriver.hpp"
#include "utility/Enums.hpp"
#include "utility/Macros.hpp"
#include "utility/Types.hpp"

#include <memory>

/**
 * @namespace hardware
 * @brief Hardware interfaces for low‑level device operations (Camera, DMA, LED, GPU).
 */
namespace hardware
{
  /**
   * @struct VirtualMemory
   * @brief Representation of a virtual memory block.
   *
   * @param int32_t File descriptor associated with the memory block.
   * @param types::Size The size of the memory block.
   * @param size_t The length of the memory block.
   * @param size_t The stride of the memory block.
   * @param uint64_t The offset within the memory block.
   */
  struct VirtualMemory
  {
    types::Size size;
    size_t length = 0;
    size_t stride = 0;
    uint64_t offset = 0;
    int fd = -1;
  };

	/**
   * @class DMA
   * @brief High-level DMA interface for managing virtual memory and mapping it to CPU memory space.
   */
	class DMA
	{
		public:
			/**
       * @brief Construct new DMA object.
       */
		  DMA() noexcept;

			/**
       * @brief Destroy the DMA object.
       *
       */
			~DMA() noexcept;

			DISABLE_COPY_AND_MOVE(DMA)

      /**
       * @brief Connect DMA to a virtual memory with the given file descriptor.
       *
       * @param types::VirtualMemory The virtual memory object to connect to the DMA.
       * @return Status::SUCCESS on success,
       *         Status::FAIL if the descriptor is invalid or duplication fails.
       */
			types::Status connect(const VirtualMemory&);

      /**
       * @brief Get the mapped virtual memory to the CPU address space.
       *
       * @param uint16_t* Pointer to the memory where the virtual memory is mapped.
       * @return Status::SUCCESS on success,
       *         Status::FAIL if mapping fails.
       */
			types::Status cpu(uint16_t*&) const;

      /**
       * @brief Get the virtual memory associated with the DMA.
       *
       * @return Reference to the virtual memory object.
       */
      const VirtualMemory& memory() const noexcept;

    protected:
      /**
       * @brief Set the DMA driver for the DMA object.
       *
       * @param std::unique_ptr<drivers::IDMADriver> Unique pointer to the DMA driver.
       */
      void setDriver(std::unique_ptr<drivers::IDMADriver>);

		private:
      std::unique_ptr<drivers::IDMADriver> m_driver;
	};
} // namespace hardware
