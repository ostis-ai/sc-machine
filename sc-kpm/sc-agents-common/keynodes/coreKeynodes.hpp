/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include "sc-memory/sc_keynodes.hpp"

namespace scAgentsCommon
{
class CoreKeynodes : public ScKeynodes
{
public:
  static ScKeynodeClass const action;
  static ScKeynodeClass const action_initiated;
  static ScKeynodeClass const action_finished;
  static ScKeynodeClass const action_finished_successfully;
  static ScKeynodeClass const action_finished_unsuccessfully;
  static ScKeynodeNoRole const nrel_answer;
  static ScKeynodeNoRole const nrel_inclusion;
  static ScKeynodeNoRole const nrel_idtf;
  static ScKeynodeNoRole const nrel_main_idtf;
  static ScKeynodeNoRole const nrel_lexicographically_more;
  static ScKeynodeNoRole const nrel_lexicographically_less;
  static ScKeynodeNoRole const nrel_equal_strings;
  static ScKeynodeRole const rrel_1;
  static ScKeynodeRole const rrel_2;
  static ScKeynodeRole const rrel_3;
  static ScKeynodeRole const rrel_last_added_sc_element;
  static ScKeynodeClass const number;
  static ScKeynodeNoRole const nrel_equal_numbers;
  static ScKeynodeNoRole const nrel_be_more;
  static ScKeynodeRole const rrel_key_sc_element;
  static ScKeynodeNoRole const nrel_sc_text_translation;
  static ScKeynodeRole const rrel_main_key_sc_element;
  static ScKeynodeClass const cardinality;
  static ScKeynodeClass const lang_ru;
  static ScKeynodeNoRole const nrel_basic_sequence;
};

}  // namespace scAgentsCommon