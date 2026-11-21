/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "hardware/DMA.hpp"
#include "utility/Logger.hpp"

#include <sys/mman.h>
#include <unistd.h>
#include <utility>

// LCOV_EXCL_START
namespace
{
  long pageSize() noexcept
  {
    static const long pageSize = []() noexcept
    {
      long ps = sysconf(_SC_PAGESIZE);
      return ps > 0 ? ps : 4096L;
    }();
    return pageSize;
  }
}

namespace drivers
{
  class DMADriver : public IDMADriver
  {
    public:
      DMADriver() noexcept = default;
      ~DMADriver() noexcept override;

      DISABLE_COPY_AND_MOVE(DMADriver)

      bool connect(const types::VirtualMemory& memory) override;
      bool cpu(uint16_t*& data) const override;
      const types::VirtualMemory& memory() const noexcept override;

    private:
      bool map() noexcept;
      void unmap() noexcept;

    private:
      types::VirtualMemory m_memory;
      void* m_addr = nullptr;
      size_t m_length = 0;
      uint64_t m_offset = 0;
  };

  DMADriver::~DMADriver() noexcept
  {
    if (m_memory.fd >= 0)
      close(m_memory.fd);
    unmap();
  }

  bool DMADriver::connect(const types::VirtualMemory& memory)
  {
    if (memory.fd < 0)
    {
      logger::Warning("DMA: Invalid source file descriptor.");
      return false;
    }

    if (m_memory.fd >= 0)
      close(m_memory.fd);

    m_memory.fd = dup(memory.fd);
    if (m_memory.fd < 0)
    {
      logger::Warning("DMA: Failed to duplicate file descriptor.");
      return false;
    }

    m_memory.size = memory.size;
    m_memory.length = memory.length;
    m_memory.stride = memory.stride;
    m_memory.offset = memory.offset;
    if (!map())
      return false;
    return true;
  }

  bool DMADriver::cpu(uint16_t*& data) const
  {
    if (m_memory.fd < 0 || m_addr == nullptr)
    {
      logger::Warning("DMA: Memory not mapped.");
      return false;
    }

    data = static_cast<uint16_t*>(m_addr) + m_offset / sizeof(uint16_t);
    return true;
  }

  const types::VirtualMemory& DMADriver::memory() const noexcept
  {
    return m_memory;
  }

  bool DMADriver::map() noexcept
  {
    unmap();

    const uint64_t pageStart = m_offset & ~(static_cast<uint64_t>(pageSize()) - 1ULL);
    m_offset -= pageStart;
    m_length += static_cast<size_t>(m_offset);

    m_addr = mmap(
      nullptr, m_length, PROT_READ, MAP_SHARED, m_memory.fd, static_cast<off_t>(pageStart)
    );
    if (m_addr == MAP_FAILED)
      m_addr = nullptr;
    return m_addr != nullptr;
  }

  void DMADriver::unmap() noexcept
  {
    if (m_addr != nullptr && m_length > 0)
      munmap(m_addr, m_length);

    m_addr = nullptr;
    m_length = m_memory.length;
    m_offset = m_memory.offset;
  }
}
// LCOV_EXCL_STOP
namespace hardware
{
  DMA::DMA() noexcept : m_driver(std::make_unique<drivers::DMADriver>())
  {
  }

  DMA::~DMA() noexcept = default;

  types::Status DMA::connect(const types::VirtualMemory& memory)
  {
    if (!m_driver->connect(memory))
      return types::Status::FAIL;
    return types::Status::SUCCESS;
  }

  types::Status DMA::cpu(uint16_t*& data) const
  {
    if (!m_driver->cpu(data))
      return types::Status::FAIL;
    return types::Status::SUCCESS;
  }

  const types::VirtualMemory& DMA::memory() const noexcept
  {
    return m_driver->memory();
  }

  void DMA::setDriver(std::unique_ptr<drivers::IDMADriver> driver)
  {
    m_driver = std::move(driver);
  }
} // namespace hardware
