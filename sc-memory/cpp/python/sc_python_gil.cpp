#include "sc_python_gil.hpp"

namespace py
{
utils::ScLock ReleaseGILPolicy::m_lock;
std::unordered_map<std::thread::id, PyThreadState*> ReleaseGILPolicy::m_threadStates;
}