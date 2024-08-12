/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-core/sc_memory.h"

#ifndef _search_functions_h_
#  define _search_functions_h_

/*! Creates new result node. It automaticaly appends into system elements set
 * @returns Returns sc-addr of created node
 */
sc_addr create_result_node();

/*! Connect result with action by specified releation
 * @param action sc-addr of action node
 * @param result sc-addr of result node
 */
void connect_result_to_action(sc_addr action, sc_addr result);

/*! Append element into result. It mark accessory arc from result node to
 * appended element as system. Also it provides uniques inclusion of element into result set
 * @param result sc-addr of result node
 * @param el sc-addr of sc-element to append into asnwer
 */
void appendIntoResult(sc_addr result, sc_addr el);

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
