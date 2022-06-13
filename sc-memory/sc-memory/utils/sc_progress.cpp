#include "sc_progress.hpp"

#include "sc_console.hpp"

#include <algorithm>
#include <iostream>

namespace utils
{
// ---------------
ScProgress::ScProgress(std::string const & title, size_t stepsNum, size_t width)
  : m_title(title)
  , m_width(width)
  , m_stepsNum(stepsNum)
  , m_passedSteps(0)
  , m_isComplete(false)
  , m_isFirst(false)
  , m_prevPercent(0)
{
  std::cout << m_title << "..." << std::endl;
}

void ScProgress::PrintStatus(size_t passedStep)
{
  m_passedSteps = passedStep + 1;  // to correctly work with for (0; N);
  // calculate status
  float progress = (float)(m_passedSteps + 1) / (float)m_stepsNum;
  m_isComplete = (progress >= 1.f);
  progress = std::min(std::max(0.f, progress), 1.f);

  auto const filledNum = (size_t)((float)m_width * progress);
  auto const percentInt = (size_t)((float)100 * progress);

  if (m_isFirst || (m_prevPercent != percentInt))
  {
    m_isFirst = false;
    m_prevPercent = percentInt;

    // prepare string
    std::string const filledStr(filledNum, '#');
    std::string const notFilledStr(m_width - filledNum, '-');

    ScConsole::SetString("[" + filledStr + notFilledStr + "] " + std::to_string(percentInt) + "%\n");
  }
  if (m_isComplete)
    std::cout << std::endl;
}

}  // namespace utils
