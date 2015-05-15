/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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
