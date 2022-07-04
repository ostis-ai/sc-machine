/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>

#include "coreKeynodes.generated.hpp"

namespace scAgentsCommon
{
class CoreKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("question"), ForceCreate)
  static ScAddr question;

  SC_PROPERTY(Keynode("question_initiated"), ForceCreate)
  static ScAddr question_initiated;

  SC_PROPERTY(Keynode("question_finished"), ForceCreate)
  static ScAddr question_finished;

  SC_PROPERTY(Keynode("question_finished_successfully"), ForceCreate)
  static ScAddr question_finished_successfully;

  SC_PROPERTY(Keynode("question_finished_unsuccessfully"), ForceCreate)
  static ScAddr question_finished_unsuccessfully;

  SC_PROPERTY(Keynode("nrel_answer"), ForceCreate)
  static ScAddr nrel_answer;

  SC_PROPERTY(Keynode("nrel_inclusion"), ForceCreate)
  static ScAddr nrel_inclusion;

  SC_PROPERTY(Keynode("nrel_idtf"), ForceCreate)
  static ScAddr nrel_idtf;

  SC_PROPERTY(Keynode("nrel_main_idtf"), ForceCreate)
  static ScAddr nrel_main_idtf;

  SC_PROPERTY(Keynode("nrel_lexicographically_more"), ForceCreate)
  static ScAddr nrel_lexicographically_more;

  SC_PROPERTY(Keynode("nrel_lexicographically_less"), ForceCreate)
  static ScAddr nrel_lexicographically_less;

  SC_PROPERTY(Keynode("nrel_equal_strings"), ForceCreate)
  static ScAddr nrel_equal_strings;

  SC_PROPERTY(Keynode("rrel_1"), ForceCreate)
  static ScAddr rrel_1;

  SC_PROPERTY(Keynode("rrel_2"), ForceCreate)
  static ScAddr rrel_2;

  SC_PROPERTY(Keynode("rrel_3"), ForceCreate)
  static ScAddr rrel_3;

  SC_PROPERTY(Keynode("rrel_last_added_sc_element"), ForceCreate)
  static ScAddr rrel_last_added_sc_element;

  SC_PROPERTY(Keynode("number"), ForceCreate)
  static ScAddr number;

  SC_PROPERTY(Keynode("nrel_equal_numbers"), ForceCreate)
  static ScAddr nrel_equal_numbers;

  SC_PROPERTY(Keynode("nrel_be_more"), ForceCreate)
  static ScAddr nrel_be_more;

  SC_PROPERTY(Keynode("rrel_key_sc_element"), ForceCreate)
  static ScAddr rrel_key_sc_element;

  SC_PROPERTY(Keynode("nrel_sc_text_translation"), ForceCreate)
  static ScAddr nrel_sc_text_translation;

  SC_PROPERTY(Keynode("rrel_main_key_sc_element"), ForceCreate)
  static ScAddr rrel_main_key_sc_element;

  SC_PROPERTY(Keynode("cardinality"), ForceCreate)
  static ScAddr cardinality;

  SC_PROPERTY(Keynode("lang_ru"), ForceCreate)
  static ScAddr lang_ru;

  SC_PROPERTY(Keynode("nrel_basic_sequence"), ForceCreate)
  static ScAddr nrel_basic_sequence;
};

}  // namespace scAgentsCommon
