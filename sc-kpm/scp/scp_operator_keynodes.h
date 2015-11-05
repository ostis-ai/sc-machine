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

#ifndef SCP_OPERATOR_KEYNODES_H
#define SCP_OPERATOR_KEYNODES_H

#include "scp_lib.h"

extern scp_operand scp_operator_atomic_type;

extern scp_operand op_searchElStr3;
extern scp_operand op_searchElStr5;

//extern scp_operand op_searchIterStr3;
//extern scp_operand op_searchIterStr5;

extern scp_operand op_searchSetStr3;
extern scp_operand op_searchSetStr5;

extern scp_operand op_genEl;
extern scp_operand op_genElStr3;
extern scp_operand op_genElStr5;

extern scp_operand op_eraseEl;
extern scp_operand op_eraseElStr3;
extern scp_operand op_eraseElStr5;
extern scp_operand op_eraseSetStr3;
extern scp_operand op_eraseSetStr5;

extern scp_operand op_ifVarAssign;
extern scp_operand op_ifFormCont;
extern scp_operand op_ifCoin;
extern scp_operand op_ifType;

extern scp_operand op_varAssign;

#ifdef SCP_MATH
extern scp_operand op_contAdd;
extern scp_operand op_contDiv;
extern scp_operand op_contMult;
extern scp_operand op_contSub;
extern scp_operand op_contPow;

extern scp_operand op_contLn;
extern scp_operand op_contSin;
extern scp_operand op_contCos;
extern scp_operand op_contTg;
extern scp_operand op_contASin;
extern scp_operand op_contACos;
extern scp_operand op_contATg;

extern scp_operand op_ifEq;
extern scp_operand op_ifGr;

extern scp_operand op_contDivInt;
extern scp_operand op_contDivRem;
#endif

#ifdef SCP_STRING
extern scp_operand op_contStringConcat;
extern scp_operand op_stringIfEq;
extern scp_operand op_stringIfGr;
extern scp_operand op_stringSplit;
extern scp_operand op_stringLen;
extern scp_operand op_stringSub;
extern scp_operand op_stringSlice;
extern scp_operand op_stringStartsWith;
extern scp_operand op_stringEndsWith;
extern scp_operand op_stringReplace;
extern scp_operand op_stringToUpperCase;
extern scp_operand op_stringToLowerCase;
#endif

extern scp_operand op_contAssign;
extern scp_operand op_contErase;

extern scp_operand op_print;
extern scp_operand op_printNl;
extern scp_operand op_printEl;

extern scp_operand op_sys_search;
extern scp_operand op_sys_gen;

extern scp_operand op_call;
extern scp_operand op_waitReturn;
extern scp_operand op_waitReturnSet;
extern scp_operand op_return;

extern scp_operand op_sys_wait;

extern scp_operand op_syncronize;

scp_result init_operator_keynodes();

#endif // SCP_OPERATOR_KEYNODES_H
