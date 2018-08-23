/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpOperand.hpp"
#include "scpParameter.hpp"
#include "scpOperatorStr.hpp"

#include <vector>

namespace scp
{

class SCPOperatorCall: public SCPOperatorElStr3
{
private:
    struct param
    {
        bool isIn;
        bool isOut;
        int order;
    };
    std::vector<SCPOperand*> params;
    std::vector<SCPParameter*> expected_params;

public:
    SCPOperatorCall(ScMemoryContext &ctx, ScAddr addr);
    std::string GetTypeName();
    sc_result Parse();
    sc_result Execute();
    ~SCPOperatorCall();
};

}

