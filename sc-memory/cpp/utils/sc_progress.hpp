/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "../sc_types.hpp"

namespace utils
{
    class ScProgress final
    {
    public:
        _SC_EXTERN explicit ScProgress(std::string const & title, size_t stepsNum, size_t width = 50);

        _SC_EXTERN void PrintStatus(size_t passedSteps);

    private:
        std::string mTitle;
        size_t mWidth;
        size_t mStepsNum;
        size_t mPassedSteps;
        bool mIsComplete : 1;
        bool mIsFirst : 1;
        size_t mPrevPercent;
    };
}