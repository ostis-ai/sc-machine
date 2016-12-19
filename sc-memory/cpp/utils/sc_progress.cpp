#include "sc_progress.hpp"

#include "sc_log.hpp"
#include "sc_console.hpp"

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
    {
        std::cout << m_title << "..." << std::endl;
    }

    void ScProgress::PrintStatus(size_t passedSteps)
    {
        m_passedSteps = passedSteps;
        
        // calculate status
        float progress = (float)(m_passedSteps + 1) / (float)m_stepsNum;
        m_isComplete = (progress >= 1.f);
        progress = std::min(std::max(0.f, progress), 1.f);
       
        size_t const filledNum = (size_t)((float)m_width * progress);
        size_t const percentInt = (size_t)((float)100 * progress);

        // prepare string
        std::string const filledStr(filledNum, '#');
        std::string const notFilledStr(m_width - filledNum, '-');
     
        ScConsole::SetString("[" + filledStr + notFilledStr + "] " + std::to_string(percentInt) + "%");
        if (m_isComplete)
            std::cout << std::endl;
    }
}