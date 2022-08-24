/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_event.hpp"

#include "algorithm"

class ScMemoryJsonEventsManager
{
public:
  static ScMemoryJsonEventsManager * GetInstance()
  {
    if (m_instance == nullptr)
      m_instance = new ScMemoryJsonEventsManager();

    return m_instance;
  }

  size_t Add(ScEvent * event)
  {
    m_events.insert({counter, event});
    return counter++;
  }

  size_t Next() const
  {
    return counter;
  }

  ScEvent * Remove(size_t index)
  {
    auto const & it = m_events.find(index);
    if (it != m_events.end())
    {
      auto const & pair = m_events.find(index);
      auto * e = pair->second;
      pair->second = nullptr;
      return e;
    }

    return nullptr;
  }

  ~ScMemoryJsonEventsManager()
  {
    for (auto const & pair : m_events)
      delete pair.second;
    m_events.clear();

    delete m_instance;
  }

private:
  static ScMemoryJsonEventsManager * m_instance;
  std::unordered_map<size_t, ScEvent *> m_events;
  size_t counter = 0;

  ScMemoryJsonEventsManager() = default;
};
