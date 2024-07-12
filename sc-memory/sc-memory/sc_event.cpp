/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.hpp"

#include <utility>
#include "sc_utils.hpp"
#include "sc_addr.hpp"
#include "sc_memory.hpp"

#include "utils/sc_log.hpp"

extern "C"
{
#include "sc-core/sc-store/sc-base/sc_monitor.h"
}
// Should be equal to C values

ScEvent::Type constexpr ScEvent::Type::Unknown(sc_event_unknown);
ScEvent::Type constexpr ScEvent::Type::AddInputEdge(sc_event_add_input_arc);
ScEvent::Type constexpr ScEvent::Type::AddOutputEdge(sc_event_add_output_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveInputEdge(sc_event_remove_input_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveOutputEdge(sc_event_remove_output_arc);
ScEvent::Type constexpr ScEvent::Type::RemoveElement(sc_event_remove_element);
ScEvent::Type constexpr ScEvent::Type::ChangeContent(sc_event_change_content);
