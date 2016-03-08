/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotEnergy.hpp"

class ScMemoryContext;


namespace iot
{
	class Commands
	{
	public:
		static bool initialize();
		static bool shutdown();

	private:
        static ScMemoryContext * memory_ctx;
	};

}
