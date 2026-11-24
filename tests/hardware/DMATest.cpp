/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "hardware/DMA.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockDMADriver : public drivers::IDMADriver
{
  public:
    MOCK_METHOD(bool, connect, (const types::VirtualMemory&), (override));
    MOCK_METHOD(bool, cpu, (uint16_t*&), (const, override));
    MOCK_METHOD(const types::VirtualMemory&, memory, (), (const, override));
};

class DMATest : public hardware::DMA
{
  public:
    void setDriver(std::unique_ptr<MockDMADriver> driver)
    {
      hardware::DMA::setDriver(std::move(driver));
    }
};

TEST(DMA, Connect)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockDMADriver>();
    EXPECT_CALL(*driver, connect).WillOnce(Return(false));

    DMATest dma;
    dma.setDriver(std::move(driver));

    types::VirtualMemory vmem{.fd = -1};
    EXPECT_EQ(dma.connect(vmem), types::Status::FAIL) << "DMA did not fail to connect";
  }

  {
    using ::testing::ReturnRef;

    types::VirtualMemory vmem{
      .size = {.width = 1024, .height = 1024},
      .length = 1024,
      .stride = 2,
      .offset = 0,
      .fd = 5
    };
    auto driver = std::make_unique<MockDMADriver>();
    EXPECT_CALL(*driver, connect).WillOnce(Return(true));
    EXPECT_CALL(*driver, memory).WillOnce(ReturnRef(vmem));

    DMATest dma;
    dma.setDriver(std::move(driver));
    ASSERT_EQ(dma.connect(vmem), types::Status::SUCCESS);

    types::VirtualMemory dmaVmem = dma.memory();
    EXPECT_EQ(dmaVmem.size.width, vmem.size.width) << "DMA memory width does not match";
    EXPECT_EQ(dmaVmem.size.height, vmem.size.height) << "DMA memory height does not match";
    EXPECT_EQ(dmaVmem.length, vmem.length) << "DMA memory length does not match";
    EXPECT_EQ(dmaVmem.stride, vmem.stride) << "DMA memory stride does not match";
    EXPECT_EQ(dmaVmem.offset, vmem.offset) << "DMA memory offset does not match";
    EXPECT_EQ(dmaVmem.fd, vmem.fd) << "DMA memory fd does not match";
  }
}

TEST(DMA, CPU)
{
  using ::testing::Return;
  {
    auto driver = std::make_unique<MockDMADriver>();
    EXPECT_CALL(*driver, cpu).WillOnce(Return(false));

    DMATest dma;
    dma.setDriver(std::move(driver));

    uint16_t* ptr = nullptr;
    EXPECT_EQ(dma.cpu(ptr), types::Status::FAIL) << "DMA did not fail to get CPU pointer";
  }

  {
    auto driver = std::make_unique<MockDMADriver>();
    EXPECT_CALL(*driver, cpu(::testing::_)).WillOnce(
      [](uint16_t*& out)
      {
        static uint16_t data[10];
        out = data;
        return true;
      }
    );

    DMATest dma;
    dma.setDriver(std::move(driver));

    uint16_t* ptr = nullptr;
    EXPECT_EQ(dma.cpu(ptr), types::Status::SUCCESS) << "DMA did not successfully get CPU pointer";
    EXPECT_NE(ptr, nullptr) << "DMA CPU pointer is null";
  }
}
