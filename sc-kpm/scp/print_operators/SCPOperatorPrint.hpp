/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "scpKeynodes.hpp"
#include "scpOperatorStr.hpp"

namespace scp
{

class SCPOperatorPrint: public SCPOperatorElStr1
{
private:
    sc_bool newline;

public:
    SCPOperatorPrint(ScMemoryContext &ctx, ScAddr addr, sc_bool newline_);
    std::string GetTypeName();
    sc_result Parse();
    sc_result Execute();
};

}

