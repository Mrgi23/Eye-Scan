/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Enums.hpp"
#include "utility/Macros.hpp"

#include <array>
#include <cstddef>
#include <mutex>
#include <type_traits>
#include <utility>

/**
 * @namespace types
 * @brief Lightweight fundamental enums and utility objects used across the system.
 */
namespace types
{
  /**
   * @class Queue
   * @brief Fixed‑capacity, thread‑safe FIFO queue for move‑constructible types.
   *
   * @tparam T Type stored in the queue. Must be default‑constructible,
   * move‑constructible, and move‑assignable.
   * @tparam Capacity Maximum number of elements the queue can hold. Must be > 0.
   */
  template <typename T, size_t Capacity> class Queue
  {
  public:
    static_assert(Capacity > 0, "Capacity must be > 0.");
    static_assert(
      std::is_default_constructible_v<T>, "T must be default constructible."
    );
    static_assert(std::is_move_constructible_v<T>, "T must be move constructible.");
    static_assert(std::is_move_assignable_v<T>, "T must be move assignable.");

    /**
     * @brief Construct new Queue object.
     */
    Queue() noexcept = default;

    /**
     * @brief Destroy the Queue object.
     */
    ~Queue() noexcept = default;

    DISABLE_COPY_AND_MOVE(Queue)

    /**
     * @brief Pop the next item from the queue (thread‑safe).
     *
     * @param T& Reference receiving the popped value.
     * @return Status::SUCCESS if an item was popped,
     *         Status::FAIL if the queue was empty.
     */
    Status pop(T& item)
    {
      std::scoped_lock lock(m_mutex);
      if (m_count == 0)
        return Status::FAIL;

      item = std::move(m_buffer[m_read]);
      m_read = (m_read + 1) % Capacity;
      --m_count;
      return Status::SUCCESS;
    }

    /**
     * @brief Push a new item into the queue (thread‑safe).
     *
     * @param T Value to be moved into the queue.
     * @return Status::SUCCESS if the item was pushed,
     *         Status::FAIL if the queue was full.
     */
    Status push(T item)
    {
      std::scoped_lock lock(m_mutex);
      if (m_count == Capacity)
        return Status::FAIL;

      m_buffer[m_write] = std::move(item);
      m_write = (m_write + 1) % Capacity;
      ++m_count;
      return Status::SUCCESS;
    }

    /**
     * @brief Clear all items from the queue (thread‑safe).
     */
    void clear() noexcept
    {
      std::scoped_lock lock(m_mutex);
      m_buffer = {};
      m_write = m_read = m_count = 0;
    }

  private:
    std::array<T, Capacity> m_buffer;
    size_t m_write = 0;
    size_t m_read = 0;
    size_t m_count = 0;
    mutable std::mutex m_mutex;
  };
} // namespace types
