#pragma once

#include "sc-memory/sc_event.hpp"

#include "algorithm"

class ScJSEventsManager
{
public:
  static ScJSEventsManager * GetInstance()
  {
    if (m_instance == nullptr)
      m_instance = new ScJSEventsManager();

    return m_instance;
  }

  size_t Add(ScEvent * event)
  {
    m_events.insert({counter, event});
    return counter++;
  }

  ScEvent * Remove(size_t index)
  {
    auto const & it = m_events.find(index);
    if (it != m_events.end())
      return m_events.find(index)->second;

    return nullptr;
  }

  ~ScJSEventsManager()
  {
    for (auto const & pair : m_events)
      delete pair.second;
  }

private:
  static ScJSEventsManager * m_instance;
  std::unordered_map<size_t, ScEvent *> m_events;
  size_t counter = 0;

  ScJSEventsManager() = default;
};
