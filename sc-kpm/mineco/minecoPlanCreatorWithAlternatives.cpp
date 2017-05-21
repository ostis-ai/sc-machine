/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_wait.hpp"
#include <iostream>

#include "minecoPlanCreatorWithAlternatives.hpp"

namespace mineco
{
ScAddr AMinEcoPlanCreatorWithAlternatives::msAgentKeynode;
ScAddr AMinEcoPlanCreatorWithAlternatives::rrel_1;
ScAddr AMinEcoPlanCreatorWithAlternatives::rrel_2;
ScAddr AMinEcoPlanCreatorWithAlternatives::rrel_key_sc_element;
ScAddr AMinEcoPlanCreatorWithAlternatives::question_plan_creation;
ScAddr AMinEcoPlanCreatorWithAlternatives::question_initiated;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_answer;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_inclusion;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_goto;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_then;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_else;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_decomposition_of_action;
ScAddr AMinEcoPlanCreatorWithAlternatives::action_select_from_alternatives;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_transition_condition;
ScAddr AMinEcoPlanCreatorWithAlternatives::conditional_transfer;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_authors;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_result;
ScAddr AMinEcoPlanCreatorWithAlternatives::nrel_object;
ScAddr AMinEcoPlanCreatorWithAlternatives::action_select_one_from_alternatives;

void AMinEcoPlanCreatorWithAlternatives::RemoveSet(ScAddr set)
{
    ScIterator3Ptr iter = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Const);
    while (iter->Next())
    {
        ScAddr element = iter->Get(2);
        ms_context->EraseElement(element);
    }
}

void AMinEcoPlanCreatorWithAlternatives::RemoveFirstLevelELements(ScAddr set)
{
    ScIterator3Ptr iter = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    while (iter->Next())
    {
        ScAddr element = iter->Get(2);

        ScIterator5Ptr iter_elems = ms_context->Iterator5(element, ScType::EdgeAccessConstPosPerm, ScType::Const, ScType::EdgeAccessConstPosPerm, set);
        if (iter_elems->Next())
            continue;

        iter_elems = ms_context->Iterator5(ScType::Const, ScType::EdgeAccessConstPosPerm, element, ScType::EdgeAccessConstPosPerm, set);
        if (iter_elems->Next())
            continue;

        iter_elems = ms_context->Iterator5(ScType::Const, ScType::EdgeDCommonConst, element, ScType::EdgeAccessConstPosPerm, set);
        if (iter_elems->Next())
            continue;

        iter_elems = ms_context->Iterator5(element, ScType::EdgeDCommonConst, ScType::Const, ScType::EdgeAccessConstPosPerm, set);
        if (iter_elems->Next())
            continue;

        ScAddr arc = iter->Get(1);
        ms_context->EraseElement(arc);
    }
}

ScAddr AMinEcoPlanCreatorWithAlternatives::GetFirstSubaction(ScAddr action)
{
    ScAddr set_action, subaction;
    ScIterator5Ptr iter_action = ms_context->Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, action, ScType::EdgeAccessConstPosPerm, nrel_decomposition_of_action);
    if (iter_action->Next())
        set_action = iter_action->Get(0);

    if (set_action.IsValid())
    {
        iter_action = ms_context->Iterator5(set_action, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, rrel_1);
        if (iter_action->Next())
            subaction = iter_action->Get(2);
    }

    return subaction;
}

ScAddr AMinEcoPlanCreatorWithAlternatives::GetTransferCondition(ScAddr action_arc)
{
    ScAddr result;
    ScIterator3Ptr iter = ms_context->Iterator3(ScType::NodeConst, ScType::EdgeAccessConstPosPerm, action_arc);
    while (iter->Next())
    {
        if (ms_context->HelperCheckArc(conditional_transfer, iter->Get(0), ScType::EdgeAccessConstPosPerm))
        {
            ScIterator5Ptr iter_action = ms_context->Iterator5(iter->Get(0), ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_transition_condition);
            if (iter_action->Next())
                result = iter_action->Get(2);
        }
    }
    return result;
}

sc_bool AMinEcoPlanCreatorWithAlternatives::IsSelection(ScAddr action)
{
    ScIterator3Ptr iter = ms_context->Iterator3(ScType::NodeConst, ScType::EdgeAccessConstPosPerm, action);
    while (iter->Next())
    {
        ScIterator5Ptr iter_action = ms_context->Iterator5(action_select_from_alternatives, ScType::EdgeDCommonConst, iter->Get(0), ScType::EdgeAccessConstPosPerm, nrel_inclusion);
        if (iter_action->Next())
            return SC_TRUE;
    }
    return SC_FALSE;
}

void AMinEcoPlanCreatorWithAlternatives::RemoveBranch(ScAddr action, ScAddr set_action)
{
    ScAddr next_action;
    ScIterator5Ptr iter_action = ms_context->Iterator5(action, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_goto);
    while (iter_action->Next())
    {
        next_action = iter_action->Get(2);
        RemoveBranch(next_action, set_action);
    }

    iter_action = ms_context->Iterator5(action, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_then);
    while (iter_action->Next())
    {
        next_action = iter_action->Get(2);
        RemoveBranch(next_action, set_action);
    }

    iter_action = ms_context->Iterator5(action, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_else);
    while (iter_action->Next())
    {
        next_action = iter_action->Get(2);
        RemoveBranch(next_action, set_action);
    }

    next_action = GetFirstSubaction(action);
    if (next_action.IsValid())
        RemoveBranch(next_action, set_action);

    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, set_action, action);
}

ScAddr AMinEcoPlanCreatorWithAlternatives::SelectFromAlternative(ScAddr main_class, ScAddr user)
{
    ScAddr action = ms_context->CreateNode(ScType::NodeConst);
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, action_select_one_from_alternatives, action);
    ScAddr arc = ms_context->CreateArc(ScType::EdgeDCommonConst, action, user);
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, nrel_object, arc);
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, action, main_class);

    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, question_initiated, action);

    ScAddr result;
    /*auto check = [&](ScAddr const & listenAddr,
                     ScAddr const & edgeAddr,
                     ScAddr const & otherAddr)
    {
        if (ms_context->HelperCheckArc(nrel_result, edgeAddr, ScType::EdgeAccessConstPosPerm))
        {
            result = otherAddr;
            return true;
        }
        return false;
    };
    ScWaitCondition<ScEventAddOutputEdge> waiter((ScMemoryContext&)ms_context, action, SC_WAIT_CHECK(check));
    waiter.Wait();*/
    while (true)
    {
        ScWait<ScEventAddOutputEdge> waiter((ScMemoryContext&)ms_context, action);
        waiter.Wait();

        ScIterator5Ptr iter = ms_context->Iterator5(action, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_result);
        if (iter->Next())
        {
            result = iter->Get(2);
            break;
        }
    }

    return result;
}

SC_AGENT_IMPLEMENTATION(AMinEcoPlanCreatorWithAlternatives)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr action = ms_context->GetArcEnd(edgeAddr);

    if (!ms_context->HelperCheckArc(question_plan_creation, action, ScType::EdgeAccessConstPosPerm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr program;
    program.Reset();

    ScIterator3Ptr iter_param = ms_context->Iterator3(action, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    while (iter_param->Next())
    {
        if (!program.IsValid())
            program = iter_param->Get(2);
        else
            return SC_RESULT_ERROR_INVALID_PARAMS;
    }
    if (!program.IsValid())
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr user;
    ScIterator5Ptr iter_user = ms_context->Iterator5(action, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_authors);
    if (iter_user->Next())
        user = iter_user->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScTemplate program_templ;
    ms_context->HelperBuildTemplate(program_templ, program);
    ScTemplateGenParams gen_params;

    ScTemplateGenResult res;
    ms_context->HelperGenTemplate(program_templ, res);

    ScAddr answer = ms_context->CreateNode(ScType::Const);

    for (int i = 0; i < res.GetSize(); i++)
    {
        ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, answer, res[i]);
    }

    // search of actions
    ScAddr main_action_class, main_action;

    ScIterator5Ptr iter_action = ms_context->Iterator5(program, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, rrel_key_sc_element);
    if (iter_action->Next())
        main_action_class = iter_action->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    iter_action = ms_context->Iterator5(main_action_class, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, answer);
    if (iter_action->Next())
        main_action = iter_action->Get(2);
    else
    {
        RemoveSet(answer);
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }

    ScAddr curr_action = GetFirstSubaction(main_action);
    if (!curr_action.IsValid())
    {
        RemoveSet(answer);
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }

    ScAddr set_action = ms_context->CreateNode(ScType::Const);
    //RemoveBranch(curr_action, set_action);

    while (true)
    {
        if (!curr_action.IsValid())
            break;

        if (IsSelection(curr_action))
        {
            ScAddr param;
            ScIterator3Ptr iter = ms_context->Iterator3(curr_action, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
            if (iter->Next())
                param = iter->Get(2);
            ScAddr result = SelectFromAlternative(param, user);
            if (!result.IsValid())
            {
                curr_action.Reset();
                continue;
            }

            ScAddr next_action;
            iter_action = ms_context->Iterator5(curr_action, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_goto);
            curr_action.Reset();
            while (iter_action->Next())
            {
                next_action = iter_action->Get(2);
                ScAddr cond = GetTransferCondition(iter_action->Get(1));
                if (cond != result)
                    RemoveBranch(next_action, set_action);
                else
                    curr_action = next_action;
            }

            continue;
        }

        ScAddr subaction = GetFirstSubaction(curr_action);
        if (subaction.IsValid())
        {
            curr_action = subaction;
            continue;
        }

        iter_action = ms_context->Iterator5(main_action_class, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_goto);
        if (iter_action->Next())
        {
            curr_action = iter_action->Get(2);
            continue;
        }

        iter_action = ms_context->Iterator5(main_action_class, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nrel_then);
        if (iter_action->Next())
        {
            curr_action = iter_action->Get(2);
            continue;
        }

        curr_action.Reset();
    }

    //finish action
    RemoveSet(set_action);
    ms_context->EraseElement(set_action);
    RemoveFirstLevelELements(answer);
    ScAddr myarc = ms_context->CreateArc(ScType::EdgeDCommonConst, action, answer);
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, nrel_answer, myarc);

    return SC_RESULT_OK;
}

}
