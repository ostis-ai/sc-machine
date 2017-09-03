/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"

namespace scp
{

class SCPOperand
{
private:
    enum SCPOperandType
    {
        SCP_CONST = 0,
        SCP_VAR = 1
    };
    enum SCPParamType
    {
        SCP_FIXED = 0,
        SCP_ASSIGN = 1
    };

    ScAddr addr;
    ScAddr arc_addr;
    ScAddr value_addr;
    ScType element_type = ScType(0);
    SCPParamType param_type = SCP_FIXED;
    SCPOperandType operand_type = SCP_CONST;
    bool isSet = false;
    bool isErase = false;
    void resolveModifiers();
    void resolveOrder(ScAddr modifier);
    uint8_t order = 0;
    ScMemoryContext &ms_context;

public:
    SCPOperand(ScMemoryContext &ctx_, ScAddr addr_);
    ScAddr GetAddr();
    ScAddr GetValue();
    ScType GetType();
    uint8_t GetOrder();
    void ResetValue();
    void SetValue(ScAddr value);
    ScAddr CreateNodeOrLink();
    bool IsFixed();
    bool IsAssign();
    bool IsSCPConst();
    bool IsSCPVar();
    bool IsErase();
    bool IsSet();
    bool HasSetModifier();
    bool HasEraseModifier();
};

}

