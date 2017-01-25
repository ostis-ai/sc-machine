#include "sc_progress.hpp"

#include "sc_log.hpp"
#include "sc_console.hpp"

#include <iostream>

namespace utils
{
// ---------------
ScProgress::ScProgress(std::string const & title, size_t stepsNum, size_t width)
  : mTitle(title)
  , mWidth(width)
  , mStepsNum(stepsNum)
  , mPassedSteps(0)
  , mIsComplete(false)
  , mIsFirst(false)
  , mPrevPercent(0)
{
  std::cout << mTitle << "..." << std::endl;
}

void ScProgress::PrintStatus(size_t passedStep)
{
  mPassedSteps = passedStep + 1; // to correctly work with for (0; N);
  // calculate status
  float progress = (float)(mPassedSteps + 1) / (float)mStepsNum;
  mIsComplete = (progress >= 1.f);
  progress = std::min(std::max(0.f, progress), 1.f);

  size_t const filledNum = (size_t)((float)mWidth * progress);
  size_t const percentInt = (size_t)((float)100 * progress);

  if (mIsFirst || (mPrevPercent != percentInt))
  {
    mIsFirst = false;
    mPrevPercent = percentInt;

    // prepare string
    std::string const filledStr(filledNum, '#');
    std::string const notFilledStr(mWidth - filledNum, '-');

    ScConsole::SetString("[" + filledStr + notFilledStr + "] " + std::to_string(percentInt) + "%");
  }
  if (mIsComplete)
    std::cout << std::endl;
}

} // namespace utils
