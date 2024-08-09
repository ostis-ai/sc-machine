/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace scAgentsCommon
{

class CoreKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const nrel_inclusion{"nrel_inclusion", ScType::NodeConstNoRole};
  static inline ScKeynode const nrel_idtf{"nrel_idtf", ScType::NodeConstNoRole};
  static inline ScKeynode const nrel_main_idtf{"nrel_main_idtf", ScType::NodeConstNoRole};
  static inline ScKeynode const nrel_lexicographically_more{"nrel_lexicographically_more", ScType::NodeConstNoRole};
  static inline ScKeynode const nrel_lexicographically_less{"nrel_lexicographically_less", ScType::NodeConstNoRole};
  static inline ScKeynode const nrel_equal_strings{"nrel_equal_strings", ScType::NodeConstNoRole};
  static inline ScKeynode const rrel_1{"rrel_1", ScType::NodeConstRole};
  static inline ScKeynode const rrel_2{"rrel_2", ScType::NodeConstRole};
  static inline ScKeynode const rrel_3{"rrel_3", ScType::NodeConstRole};
  static inline ScKeynode const rrel_last_added_sc_element{"rrel_last_added_sc_element", ScType::NodeConstRole};
  static inline ScKeynode const number{"number", ScType::NodeConstClass};
  static inline ScKeynode const nrel_equal_numbers{"nrel_equal_numbers", ScType::NodeConstNoRole};
  static inline ScKeynode const nrel_be_more{"nrel_be_more", ScType::NodeConstNoRole};
  static inline ScKeynode const rrel_key_sc_element{"rrel_key_sc_element", ScType::NodeConstRole};
  static inline ScKeynode const nrel_sc_text_translation{"nrel_sc_text_translation", ScType::NodeConstNoRole};
  static inline ScKeynode const rrel_main_key_sc_element{"rrel_main_key_sc_element", ScType::NodeConstRole};
  static inline ScKeynode const cardinality{"cardinality", ScType::NodeConstClass};
  static inline ScKeynode const lang_ru{"lang_ru", ScType::NodeConstClass};
  static inline ScKeynode const nrel_basic_sequence{"nrel_basic_sequence", ScType::NodeConstNoRole};
};

}  // namespace scAgentsCommon
