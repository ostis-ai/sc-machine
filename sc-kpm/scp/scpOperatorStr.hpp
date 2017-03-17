/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpOperand.hpp"
#include "scpOperator.hpp"

#include <vector>

namespace scp
{

class SCPOperatorStr1: SCPOperator
{
protected:
    std::vector<SCPOperand*> operands = std::vector<SCPOperand*>(1);

public:
    SCPOperatorStr1(ScMemoryContext &ctx, ScAddr addr);
    sc_result Parse();
};

class SCPOperatorStr3: SCPOperator
{
protected:
    std::vector<SCPOperand*> operands = std::vector<SCPOperand*>(3);

public:
    SCPOperatorStr3(ScMemoryContext &ctx, ScAddr addr);
    sc_result Parse();
};

class SCPOperatorStr5: SCPOperator
{
protected:
    std::vector<SCPOperand*> operands = std::vector<SCPOperand*>(5);

public:
    SCPOperatorStr5(ScMemoryContext &ctx, ScAddr addr);
    sc_result Parse();
};

}

