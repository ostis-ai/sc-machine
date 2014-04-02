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

#include "sc_memory.h"

#ifndef _search_functions_h_
#define _search_functions_h_

/*! Creates new answer node. It automaticaly appends into system elements set
 * @returns Returns sc-addr of created node
 */
sc_addr create_answer_node();

/*! Connect answer with question by specified releation
 * @param question sc-addr of question node
 * @param answer sc-addr of answer node
 */
void connect_answer_to_question(sc_addr question, sc_addr answer);

/*! Append element into answer. It mark accessory arc from answer node to
 * appended element as system. Also it provides uniques inclusion of element into answer set
 * @param answer sc-addr of answer node
 * @param el sc-addr of sc-element to append into asnwer
 */
void appendIntoAnswer(sc_addr answer, sc_addr el);

/*!
 * Remove question from question_initiated set and append it into
 * question_finished set.
 * @param question sc-add of question node
 */
void finish_question(sc_addr question);

/*!
 * Prints in log information that sc-agent has started
 * @param agent_keynode system identifier of sc-element, which denotes given sc-agent
 */
void log_agent_started(const sc_char *agent_keynode_str);

/*!
 * Prints in log information that sc-agent has finished
 * @param agent_keynode_str system identifier of sc-element, which denotes given sc-agent
 * @param success if SC_TRUE then agents has finisehed successfully, otherwise unsuccessfully
 */
void log_agent_finished(const sc_char *agent_keynode_str, sc_bool success);

#endif
