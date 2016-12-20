/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <atomic>

namespace utils
{
    // Simple lock without mutex
    class ScLock
    {
    public:
        ScLock();

        void lock();
        void unlock();

    private:
        std::atomic_bool mLocked;
    };
}