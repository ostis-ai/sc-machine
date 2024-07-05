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
  static inline ScKeynodeNoRole const nrel_inclusion{"nrel_inclusion"};
  static inline ScKeynodeNoRole const nrel_idtf{"nrel_idtf"};
  static inline ScKeynodeNoRole const nrel_main_idtf{"nrel_main_idtf"};
  static inline ScKeynodeNoRole const nrel_lexicographically_more{"nrel_lexicographically_more"};
  static inline ScKeynodeNoRole const nrel_lexicographically_less{"nrel_lexicographically_less"};
  static inline ScKeynodeNoRole const nrel_equal_strings{"nrel_equal_strings"};
  static inline ScKeynodeRole const rrel_1{"rrel_1"};
  static inline ScKeynodeRole const rrel_2{"rrel_2"};
  static inline ScKeynodeRole const rrel_3{"rrel_3"};
  static inline ScKeynodeRole const rrel_last_added_sc_element{"rrel_last_added_sc_element"};
  static inline ScKeynodeClass const number{"number"};
  static inline ScKeynodeNoRole const nrel_equal_numbers{"nrel_equal_numbers"};
  static inline ScKeynodeNoRole const nrel_be_more{"nrel_be_more"};
  static inline ScKeynodeRole const rrel_key_sc_element{"rrel_key_sc_element"};
  static inline ScKeynodeNoRole const nrel_sc_text_translation{"nrel_sc_text_translation"};
  static inline ScKeynodeRole const rrel_main_key_sc_element{"rrel_main_key_sc_element"};
  static inline ScKeynodeClass const cardinality{"cardinality"};
  static inline ScKeynodeClass const lang_ru{"lang_ru"};
  static inline ScKeynodeNoRole const nrel_basic_sequence{"nrel_basic_sequence"};
};

}  // namespace scAgentsCommon
