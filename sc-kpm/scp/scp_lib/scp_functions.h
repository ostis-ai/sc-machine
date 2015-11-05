/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/
#ifndef SCP_FUNCTIONS_H
#define SCP_FUNCTIONS_H

#include "scp_types.h"

scp_result scp_lib_init();
scp_result scp_lib_shutdown();

scp_result genEl(sc_memory_context *context, scp_operand *param);
scp_result genElStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result genElStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5);

scp_result searchElStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result searchElStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5);

scp_result searchSetStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *sets);
scp_result searchSetStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets);

scp_result eraseEl(sc_memory_context *context, scp_operand *param);
scp_result eraseElStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result eraseElStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5);
scp_result eraseSetStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result eraseSetStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5);

scp_result ifVarAssign(sc_memory_context *context, scp_operand *param);
scp_result ifFormCont(sc_memory_context *context, scp_operand *param);
scp_result ifCoin(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result ifType(sc_memory_context *context, scp_operand *param);

scp_result varAssign(sc_memory_context *context, scp_operand *param1, scp_operand *param2);

#ifdef SCP_MATH
scp_result ifEq(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result ifGr(sc_memory_context *context, scp_operand *param1, scp_operand *param2);

scp_result contAdd(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result contDiv(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result contDivInt(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result contDivRem(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result contMult(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result contSub(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result contPow(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);

scp_result contLn(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result contSin(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result contCos(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result contTg(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result contASin(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result contACos(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result contATg(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
#endif

#ifdef SCP_STRING
scp_result contStringConcat(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result stringIfEq(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result stringIfGr(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result stringSplit(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result stringLen(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result stringSub(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result stringSlice(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4);
scp_result stringStartsWith(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result stringEndsWith(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result stringReplace(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4);
scp_result stringToUpperCase(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result stringToLowerCase(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
#endif

scp_result contAssign(sc_memory_context *context, scp_operand *param1, scp_operand *param2);
scp_result contErase(sc_memory_context *context, scp_operand *param);

//scp_result idtfSearch(scp_operand *param, const char *idtf);
//scp_result idtfAssign(scp_operand *param, const char *idtf);

scp_result print(sc_memory_context *context, scp_operand *param);
scp_result printNl(sc_memory_context *context, scp_operand *param);
scp_result printEl(sc_memory_context *context, scp_operand *param);

scp_result scp_lib_resolve_system_identifier(sc_memory_context *context, const char *idtf, scp_operand *param);

#endif // SCP_FUNCTIONS_H
