#include <memory>

#include "sc-memory/sc_memory.hpp"


#include "sc-memory/sc_event.hpp"




#define coreKeynodes_hpp_20_init  bool _InitInternal() override \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "CoreKeynodes::_InitInternal"); \
    bool result = true; \
    return result; \
}


#define coreKeynodes_hpp_20_initStatic static bool _InitStaticInternal()  \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "CoreKeynodes::_InitStaticInternal"); \
    bool result = true; \
	question = ctx.HelperResolveSystemIdtf("question", ScType::Node); result = result && question.IsValid(); \
	question_initiated = ctx.HelperResolveSystemIdtf("question_initiated", ScType::Node); result = result && question_initiated.IsValid(); \
	question_finished = ctx.HelperResolveSystemIdtf("question_finished", ScType::Node); result = result && question_finished.IsValid(); \
	question_finished_successfully = ctx.HelperResolveSystemIdtf("question_finished_successfully", ScType::Node); result = result && question_finished_successfully.IsValid(); \
	question_finished_unsuccessfully = ctx.HelperResolveSystemIdtf("question_finished_unsuccessfully", ScType::Node); result = result && question_finished_unsuccessfully.IsValid(); \
	question_use_logic_rule = ctx.HelperResolveSystemIdtf("question_use_logic_rule", ScType::Node); result = result && question_use_logic_rule.IsValid(); \
	question_reverse_inference = ctx.HelperResolveSystemIdtf("question_reverse_inference", ScType::Node); result = result && question_reverse_inference.IsValid(); \
	question_numbers_comparison = ctx.HelperResolveSystemIdtf("question_numbers_comparison", ScType::Node); result = result && question_numbers_comparison.IsValid(); \
	nrel_answer = ctx.HelperResolveSystemIdtf("nrel_answer", ScType::Node); result = result && nrel_answer.IsValid(); \
	nrel_inclusion = ctx.HelperResolveSystemIdtf("nrel_inclusion", ScType::Node); result = result && nrel_inclusion.IsValid(); \
	nrel_idtf = ctx.HelperResolveSystemIdtf("nrel_idtf", ScType::Node); result = result && nrel_idtf.IsValid(); \
	nrel_main_idtf = ctx.HelperResolveSystemIdtf("nrel_main_idtf", ScType::Node); result = result && nrel_main_idtf.IsValid(); \
	nrel_lexicographically_more = ctx.HelperResolveSystemIdtf("nrel_lexicographically_more", ScType::Node); result = result && nrel_lexicographically_more.IsValid(); \
	nrel_lexicographically_less = ctx.HelperResolveSystemIdtf("nrel_lexicographically_less", ScType::Node); result = result && nrel_lexicographically_less.IsValid(); \
	nrel_equal_strings = ctx.HelperResolveSystemIdtf("nrel_equal_strings", ScType::Node); result = result && nrel_equal_strings.IsValid(); \
	rrel_1 = ctx.HelperResolveSystemIdtf("rrel_1", ScType::Node); result = result && rrel_1.IsValid(); \
	rrel_2 = ctx.HelperResolveSystemIdtf("rrel_2", ScType::Node); result = result && rrel_2.IsValid(); \
	rrel_last_added_sc_element = ctx.HelperResolveSystemIdtf("rrel_last_added_sc_element", ScType::Node); result = result && rrel_last_added_sc_element.IsValid(); \
	number = ctx.HelperResolveSystemIdtf("number", ScType::Node); result = result && number.IsValid(); \
	nrel_equal_numbers = ctx.HelperResolveSystemIdtf("nrel_equal_numbers", ScType::Node); result = result && nrel_equal_numbers.IsValid(); \
	nrel_be_more = ctx.HelperResolveSystemIdtf("nrel_be_more", ScType::Node); result = result && nrel_be_more.IsValid(); \
	statement = ctx.HelperResolveSystemIdtf("statement", ScType::Node); result = result && statement.IsValid(); \
	rrel_key_sc_element = ctx.HelperResolveSystemIdtf("rrel_key_sc_element", ScType::Node); result = result && rrel_key_sc_element.IsValid(); \
	nrel_sc_text_translation = ctx.HelperResolveSystemIdtf("nrel_sc_text_translation", ScType::Node); result = result && nrel_sc_text_translation.IsValid(); \
	rrel_main_key_sc_element = ctx.HelperResolveSystemIdtf("rrel_main_key_sc_element", ScType::Node); result = result && rrel_main_key_sc_element.IsValid(); \
	rrel_solver_key_sc_element = ctx.HelperResolveSystemIdtf("rrel_solver_key_sc_element", ScType::Node); result = result && rrel_solver_key_sc_element.IsValid(); \
	nrel_implication = ctx.HelperResolveSystemIdtf("nrel_implication", ScType::Node); result = result && nrel_implication.IsValid(); \
	atomic_logical_formula = ctx.HelperResolveSystemIdtf("atomic_logical_formula", ScType::Node); result = result && atomic_logical_formula.IsValid(); \
	cardinality = ctx.HelperResolveSystemIdtf("cardinality", ScType::Node); result = result && cardinality.IsValid(); \
	lang_ru = ctx.HelperResolveSystemIdtf("lang_ru", ScType::Node); result = result && lang_ru.IsValid(); \
	nrel_basic_sequence = ctx.HelperResolveSystemIdtf("nrel_basic_sequence", ScType::Node); result = result && nrel_basic_sequence.IsValid(); \
    return result; \
}


#define coreKeynodes_hpp_20_decl 

#define coreKeynodes_hpp_CoreKeynodes_impl 

#undef ScFileID
#define ScFileID coreKeynodes_hpp

