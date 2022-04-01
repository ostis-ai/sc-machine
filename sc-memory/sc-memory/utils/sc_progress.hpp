/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../sc_defines.hpp"

#include <string>

namespace utils
{
class ScProgress final
{
public:
  _SC_EXTERN explicit ScProgress(std::string const & title, size_t stepsNum, size_t width = 50);

  _SC_EXTERN void PrintStatus(size_t passedSteps);

private:
  std::string m_title;
  size_t m_width;
  size_t m_stepsNum;
  size_t m_passedSteps;
  bool m_isComplete : 1;
  bool m_isFirst : 1;
  size_t m_prevPercent;
};

}  // namespace utils
