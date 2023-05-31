/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_event.hpp"

class ScMemoryJsonEventsManager
{
public:
  static ScMemoryJsonEventsManager * GetInstance();

  size_t Add(ScEvent * event);

  size_t Next() const;

  ScEvent * Remove(size_t index);

  ~ScMemoryJsonEventsManager();

private:
  static ScMemoryJsonEventsManager * m_instance;
  std::unordered_map<size_t, ScEvent *> m_events;
  size_t counter = 0;

  ScMemoryJsonEventsManager() = default;
};
