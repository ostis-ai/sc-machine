/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-core/sc_memory.h"

#ifndef _search_functions_h_
#  define _search_functions_h_

/*! Creates new answer node. It automaticaly appends into system elements set
 * @returns Returns sc-addr of created node
 */
sc_addr create_answer_node();

/*! Connect answer with action by specified releation
 * @param action sc-addr of action node
 * @param answer sc-addr of answer node
 */
void connect_answer_to_action(sc_addr action, sc_addr answer);

/*! Append element into answer. It mark accessory arc from answer node to
 * appended element as system. Also it provides uniques inclusion of element into answer set
 * @param answer sc-addr of answer node
 * @param el sc-addr of sc-element to append into asnwer
 */
void appendIntoAnswer(sc_addr answer, sc_addr el);

/*!
 * Remove action from action_initiated set and append it into
 * action_finished set.
 * @param action sc-add of action node
 */
void finish_action(sc_addr action);

/*!
 * Append action into action_finished_successfully set.
 * @param action sc-add of action node
 */
void finish_action_successfully(sc_memory_context * ctx, sc_addr action);

/*!
 * Append action into action_finished_unsuccessfully set.
 * @param action sc-add of action node
 */
void finish_action_unsuccessfully(sc_memory_context * ctx, sc_addr action);

#endif
