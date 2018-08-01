/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <deque>
#include <boost/thread/mutex.hpp>

namespace scp
{

template<typename Data>
class concurrent_deque
{
private:
    std::deque<Data> the_queue;
    mutable boost::mutex the_mutex;

public:
    void push(const Data& data)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        the_queue.push_back(data);
    }

    bool empty() const
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return the_queue.empty();
    }

    Data& front()
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return the_queue.front();
    }

    Data const& front() const
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return the_queue.front();
    }

    void pop()
    {
        boost::mutex::scoped_lock lock(the_mutex);
        the_queue.pop_front();
    }

    bool extract(std::function<bool (Data&)> checker, Data& result)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        for (auto it = the_queue.begin(); it != the_queue.end(); )
        {
            if (checker(*it))
            {
                result = *it;
                the_queue.erase(it);
                return true;
            }
            ++it;
        }
        return false;
    }
};

}

