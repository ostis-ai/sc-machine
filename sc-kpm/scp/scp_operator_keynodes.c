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

#include "sc_helper.h"
#include "sc_memory_headers.h"
#include "scp_operator_keynodes.h"
#include "scp_interpreter_utils.h"
#include "scp_keynodes.h"

scp_operand scp_operator_atomic_type;

scp_operand op_searchElStr3;
scp_operand op_searchElStr5;

scp_operand op_searchSetStr3;
scp_operand op_searchSetStr5;

scp_operand op_genEl;
scp_operand op_genElStr3;
scp_operand op_genElStr5;

scp_operand op_eraseEl;
scp_operand op_eraseElStr3;
scp_operand op_eraseElStr5;
scp_operand op_eraseSetStr3;
scp_operand op_eraseSetStr5;

scp_operand op_ifVarAssign;
scp_operand op_ifFormCont;
scp_operand op_ifCoin;
scp_operand op_ifType;

scp_operand op_varAssign;

#ifdef SCP_MATH
scp_operand op_ifEq;
scp_operand op_ifGr;

scp_operand op_contAdd;
scp_operand op_contDiv;
scp_operand op_contMult;
scp_operand op_contSub;
scp_operand op_contPow;

scp_operand op_contLn;
scp_operand op_contSin;
scp_operand op_contCos;
scp_operand op_contTg;
scp_operand op_contASin;
scp_operand op_contACos;
scp_operand op_contATg;

scp_operand op_contDivInt;
scp_operand op_contDivRem;
#endif
#ifdef SCP_STRING
scp_operand op_contStringConcat;
scp_operand op_stringIfEq;
scp_operand op_stringIfGr;
scp_operand op_stringSplit;
scp_operand op_stringLen;
scp_operand op_stringSub;
scp_operand op_stringSlice;
scp_operand op_stringStartsWith;
scp_operand op_stringEndsWith;
scp_operand op_stringReplace;
scp_operand op_stringToUpperCase;
scp_operand op_stringToLowerCase;
#endif

scp_operand op_contAssign;
scp_operand op_contErase;

scp_operand op_print;
scp_operand op_printNl;
scp_operand op_printEl;

scp_operand op_sys_search;
scp_operand op_sys_gen;

scp_operand op_call;
scp_operand op_waitReturn;
scp_operand op_waitReturnSet;
scp_operand op_return;

scp_operand op_sys_wait;

scp_operand op_syncronize;

#define resolve_keynode(keynode, keynode_str) \
    if (sc_helper_resolve_system_identifier(s_default_ctx, keynode_str, &(keynode)) == SC_FALSE) \
    {\
        g_warning("Can't find element with system identifier: %s", keynode_str); \
        keynode = sc_memory_node_new(s_default_ctx, 0); \
        if (sc_helper_set_system_identifier(s_default_ctx, keynode, keynode_str, (sc_uint32)strlen(keynode_str)) != SC_RESULT_OK) \
            return SCP_RESULT_ERROR; \
        g_message("Created element with system identifier: %s", keynode_str); \
    }

scp_result init_operator_keynodes()
{
    MAKE_DEFAULT_OPERAND_FIXED(scp_operator_atomic_type);
    MAKE_DEFAULT_OPERAND_FIXED(op_searchElStr3);
    MAKE_DEFAULT_OPERAND_FIXED(op_searchElStr5);
    MAKE_DEFAULT_OPERAND_FIXED(op_searchSetStr3);
    MAKE_DEFAULT_OPERAND_FIXED(op_searchSetStr5);
    MAKE_DEFAULT_OPERAND_FIXED(op_genEl);
    MAKE_DEFAULT_OPERAND_FIXED(op_genElStr3);
    MAKE_DEFAULT_OPERAND_FIXED(op_genElStr5);
    MAKE_DEFAULT_OPERAND_FIXED(op_eraseEl);
    MAKE_DEFAULT_OPERAND_FIXED(op_eraseElStr3);
    MAKE_DEFAULT_OPERAND_FIXED(op_eraseElStr5);
    MAKE_DEFAULT_OPERAND_FIXED(op_eraseSetStr3);
    MAKE_DEFAULT_OPERAND_FIXED(op_eraseSetStr5);
    MAKE_DEFAULT_OPERAND_FIXED(op_ifVarAssign);
    MAKE_DEFAULT_OPERAND_FIXED(op_ifFormCont);
    MAKE_DEFAULT_OPERAND_FIXED(op_ifCoin);
    MAKE_DEFAULT_OPERAND_FIXED(op_ifType);
    MAKE_DEFAULT_OPERAND_FIXED(op_varAssign);

#ifdef SCP_MATH
    MAKE_DEFAULT_OPERAND_FIXED(op_ifEq);
    MAKE_DEFAULT_OPERAND_FIXED(op_ifGr);
    MAKE_DEFAULT_OPERAND_FIXED(op_contAdd);
    MAKE_DEFAULT_OPERAND_FIXED(op_contDiv);
    MAKE_DEFAULT_OPERAND_FIXED(op_contMult);
    MAKE_DEFAULT_OPERAND_FIXED(op_contSub);
    MAKE_DEFAULT_OPERAND_FIXED(op_contPow);
    MAKE_DEFAULT_OPERAND_FIXED(op_contLn);
    MAKE_DEFAULT_OPERAND_FIXED(op_contSin);
    MAKE_DEFAULT_OPERAND_FIXED(op_contCos);
    MAKE_DEFAULT_OPERAND_FIXED(op_contTg);
    MAKE_DEFAULT_OPERAND_FIXED(op_contASin);
    MAKE_DEFAULT_OPERAND_FIXED(op_contACos);
    MAKE_DEFAULT_OPERAND_FIXED(op_contATg);
    MAKE_DEFAULT_OPERAND_FIXED(op_contDivInt);
    MAKE_DEFAULT_OPERAND_FIXED(op_contDivRem);
#endif

#ifdef SCP_STRING
    MAKE_DEFAULT_OPERAND_FIXED(op_contStringConcat);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringIfEq);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringIfGr);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringSplit);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringLen);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringSub);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringSlice);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringStartsWith);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringEndsWith);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringReplace);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringToUpperCase);
    MAKE_DEFAULT_OPERAND_FIXED(op_stringToLowerCase);
#endif

    MAKE_DEFAULT_OPERAND_FIXED(op_contAssign);
    MAKE_DEFAULT_OPERAND_FIXED(op_contErase);
    MAKE_DEFAULT_OPERAND_FIXED(op_print);
    MAKE_DEFAULT_OPERAND_FIXED(op_printNl);
    MAKE_DEFAULT_OPERAND_FIXED(op_printEl);
    MAKE_DEFAULT_OPERAND_FIXED(op_sys_search);
    MAKE_DEFAULT_OPERAND_FIXED(op_sys_gen);
    MAKE_DEFAULT_OPERAND_FIXED(op_call);
    MAKE_DEFAULT_OPERAND_FIXED(op_waitReturn);
    MAKE_DEFAULT_OPERAND_FIXED(op_waitReturnSet);
    MAKE_DEFAULT_OPERAND_FIXED(op_return);
    MAKE_DEFAULT_OPERAND_FIXED(op_sys_wait);
    MAKE_DEFAULT_OPERAND_FIXED(op_syncronize);
    resolve_keynode(scp_operator_atomic_type.addr, "scp_operator_atomic_type");
    resolve_keynode(op_searchElStr3.addr, "searchElStr3");
    resolve_keynode(op_searchElStr5.addr, "searchElStr5");
    resolve_keynode(op_searchSetStr3.addr, "searchSetStr3");
    resolve_keynode(op_searchSetStr5.addr, "searchSetStr5");
    resolve_keynode(op_genEl.addr, "genEl");
    resolve_keynode(op_genElStr3.addr, "genElStr3");
    resolve_keynode(op_genElStr5.addr, "genElStr5");
    resolve_keynode(op_eraseEl.addr, "eraseEl");
    resolve_keynode(op_eraseElStr3.addr, "eraseElStr3");
    resolve_keynode(op_eraseElStr5.addr, "eraseElStr5");
    resolve_keynode(op_eraseSetStr3.addr, "eraseSetStr3");
    resolve_keynode(op_eraseSetStr5.addr, "eraseSetStr5");
    resolve_keynode(op_ifVarAssign.addr, "ifVarAssign");
    resolve_keynode(op_ifFormCont.addr, "ifFormCont");
    resolve_keynode(op_ifCoin.addr, "ifCoin");
    resolve_keynode(op_ifType.addr, "ifType");
    resolve_keynode(op_varAssign.addr, "varAssign");

#ifdef SCP_MATH
    resolve_keynode(op_contAdd.addr, "contAdd");
    resolve_keynode(op_contDiv.addr, "contDiv");
    resolve_keynode(op_contMult.addr, "contMult");
    resolve_keynode(op_contSub.addr, "contSub");
    resolve_keynode(op_contPow.addr, "contPow");
    resolve_keynode(op_contLn.addr, "contLn");
    resolve_keynode(op_contSin.addr, "contSin");
    resolve_keynode(op_contCos.addr, "contCos");
    resolve_keynode(op_contTg.addr, "contTg");
    resolve_keynode(op_contASin.addr, "contASin");
    resolve_keynode(op_contACos.addr, "contACos");
    resolve_keynode(op_contATg.addr, "contATg");
    resolve_keynode(op_ifEq.addr, "ifEq");
    resolve_keynode(op_ifGr.addr, "ifGr");
    resolve_keynode(op_contDivInt.addr, "contDivInt");
    resolve_keynode(op_contDivRem.addr, "contDivRem");
#endif

#ifdef SCP_STRING
    resolve_keynode(op_contStringConcat.addr, "contStringConcat");
    resolve_keynode(op_stringIfEq.addr, "stringIfEq");
    resolve_keynode(op_stringIfGr.addr, "stringIfGr");
    resolve_keynode(op_stringSplit.addr, "stringSplit");
    resolve_keynode(op_stringLen.addr, "stringLen");
    resolve_keynode(op_stringSub.addr, "stringSub");
    resolve_keynode(op_stringSlice.addr, "stringSlice");
    resolve_keynode(op_stringStartsWith.addr, "stringStartsWith");
    resolve_keynode(op_stringEndsWith.addr, "stringEndsWith");
    resolve_keynode(op_stringReplace.addr, "stringReplace");
    resolve_keynode(op_stringToUpperCase.addr, "stringToUpperCase");
    resolve_keynode(op_stringToLowerCase.addr, "stringToLowerCase");
#endif

    resolve_keynode(op_contAssign.addr, "contAssign");
    resolve_keynode(op_contErase.addr, "contErase");
    resolve_keynode(op_print.addr, "print");
    resolve_keynode(op_printNl.addr, "printNl");
    resolve_keynode(op_printEl.addr, "printEl");
    resolve_keynode(op_sys_search.addr, "sys_search");
    resolve_keynode(op_sys_gen.addr, "sys_gen");
    resolve_keynode(op_call.addr, "call");
    resolve_keynode(op_waitReturn.addr, "waitReturn");
    resolve_keynode(op_waitReturnSet.addr, "waitReturnSet");
    resolve_keynode(op_return.addr, "return");
    resolve_keynode(op_sys_wait.addr, "sys_wait");
    resolve_keynode(op_syncronize.addr, "syncronize");

    return SCP_RESULT_TRUE;
}
