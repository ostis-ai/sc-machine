/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils_garbage_deletion.h"
#include "utils.h"

sc_result agent_garbage_delete(const sc_event *event, sc_addr arg)
{
    //! TODO: when sc-memory will start event in many threads (not in one), then need to check if element exists
    /// because it can be already deleted as an input or outout arc of element deleted in other thread
    /// Also object can be deleted by another agent, for that moment just this agent can delete objects,
    /// all other agents must to add objects to the set sc_garbage

    sc_addr addr;
    sc_type t;

    if (sc_memory_get_arc_end(s_garbage_ctx, arg, &addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR_INVALID_STATE;

    if (sc_memory_get_element_type(s_garbage_ctx, addr, &t) != SC_RESULT_OK)
        return SC_RESULT_ERROR_INVALID_STATE;

    if (t & sc_type_node_struct)
        return sc_memory_element_free(s_garbage_ctx, addr);
    else
    {
        sc_uint32 count = 0;
        sc_iterator3 *it = sc_iterator3_f_a_a_new(s_garbage_ctx, addr, 0, 0);
        while (sc_iterator3_next(it) == SC_TRUE)
            ++count;
        sc_iterator3_free(it);

        it = sc_iterator3_a_a_f_new(s_garbage_ctx, 0, 0, addr);
        while (sc_iterator3_next(it) == SC_TRUE)
            ++count;
        sc_iterator3_free(it);

        if (count < 20)
            sc_memory_element_free(s_garbage_ctx, addr);
    }

    return SC_RESULT_OK;
}
