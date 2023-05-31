/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_json_events_manager.hpp"

#include <algorithm>

ScMemoryJsonEventsManager * ScMemoryJsonEventsManager::m_instance = nullptr;

ScMemoryJsonEventsManager * ScMemoryJsonEventsManager::GetInstance()
{
  if (m_instance == nullptr)
    m_instance = new ScMemoryJsonEventsManager();

  return m_instance;
}

size_t ScMemoryJsonEventsManager::Add(ScEvent * event)
{
  m_events.insert({counter, event});
  return counter++;
}

size_t ScMemoryJsonEventsManager::Next() const
{
  return counter;
}

ScEvent * ScMemoryJsonEventsManager::Remove(size_t index)
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

ScMemoryJsonEventsManager::~ScMemoryJsonEventsManager()
{
  for (auto const & pair : m_events)
    delete pair.second;
  m_events.clear();

  delete m_instance;
}
