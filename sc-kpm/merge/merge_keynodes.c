/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */

#include "merge_keynodes.h"
#include "merge.h"

#include "../common/sc_keynodes.h"

#include "sc_helper.h"
#include <glib.h>

sc_addr keynode_question_set_cantorization;

sc_addr keynode_question;
sc_addr keynode_question_initiated;
sc_addr keynode_nrel_answer;
sc_addr keynode_question_finished;
sc_addr keynode_system_element;

const char keynode_question_set_cantorization_str[] = "question_set_cantorization";

const char keynode_question_initiated_str[] = "question_initiated";
const char keynode_question_str[] = "question";
const char keynode_nrel_answer_str[] = "nrel_answer";
const char keynode_question_finished_str[] = "question_finished";
const char keynode_system_element_str[] = "system_element";

sc_result merge_keynodes_initialize()
{

    RESOLVE_KEYNODE(s_default_ctx, keynode_question_set_cantorization);

    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_answer);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_initiated);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_finished);
    RESOLVE_KEYNODE(s_default_ctx, keynode_system_element);

    return SC_RESULT_OK;
}
