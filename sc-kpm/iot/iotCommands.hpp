/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotEnergy.hpp"

namespace sc
{
	class MemoryContext;
}

namespace iot
{
	class Commands
	{
	public:
		static bool initialize();
		static bool shutdown();

	private:
		static sc::MemoryContext * memory_ctx;
	};

}
