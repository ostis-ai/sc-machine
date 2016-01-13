/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"
#include "iotAgent.hpp"

namespace iot
{

	sc_result handler_generate_text_command(sc_event const * event, sc_addr arg);

}
