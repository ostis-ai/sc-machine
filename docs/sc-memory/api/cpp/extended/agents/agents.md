# **C++ Agents API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement sc-agents on C++.

## **What is the agent-based model?**

The sc-machine implements the agent-based model to process information. In the agent-based model, agents exchange messages only through shared memory, adding a new agent or eliminating one or more existing agents does not result in changes to other agents, agent initiation is decentralised and most often independent of each other. 

All agents within the OSTIS Technology are divided into two classes: platform-independent, i.e. implemented only by means of SC-code, and platform-dependent, implemented by means of sc-machine API. This sc-machine presents a powerful, but simple API for developing and maintaining platform-dependent agents in C++.

All agents react to the occurrence of events in sc-memory (sc-events). That is, an agent is called implicitly when an sc-event occurs, for which type this agent is already subscribed. Knowledge about which sc-event will cause this agent to be called (awakening of this agent) is called primary initiation condition. Upon awakening, the agent checks for the presence of its full initiation condition. If the full initiation condition is successfully checked, the agent initiates an action of some class and starts interpreting it with a agent program. After executing its program, the agent can check if there is a result.

## **What does agent specification represent?

All knowledge about an agent: primary initiation condition, class of actions it can interpret, initiation condition, and result condition, are part of specification of this agent. This specification can be represented either in a knowledge base, using SC-code, or programmarly, using sc-machine API.

Let's describe specification for abstract sc-agent of counting power of specified set in SCs-code. An abstract sc-agent is a class of functionally equivalent sc-agents, different instances of which can be implemented in different ways. Each abstract sc-agent has a specification corresponding to it.

```scs
// Abstract sc-agent
agent_for_calculating_set_power
<- abstract_sc_agent;
=> nrel_primary_initiation_condition: 
    // Class of sc-event and listen (subscription) sc-element
    (sc_event_add_output_arc => action_initiated); 
=> nrel_sc_agent_action_class:
    // Class of actions to be interpreted by agent
    action_for_сalculating_set_power; 
=> nrel_initiation_condition_and_result: 
    (..agent_for_calculating_set_power_initiation_condition 
        => ..agent_for_calculating_set_power_result_condition);
<= nrel_sc_agent_key_sc_elements:
// Set of key sc-elements used by this agent
{
    action_initiated;
    action;
    action_for_сalculating_set_power;
    concept_set;
    nrel_set_power;
};
=> nrel_inclusion: 
    // Instance of abstract sc-agent; concrete implementation of agent in C++
    agent_for_calculating_set_power_implementation 
    (*
        <- platform_dependent_abstract_sc_agent;;
        // Set of links with paths to sources of agent programs
        <= nrel_sc_agent_program: 
        {
            [github.com/path/to/agent/sources] 
            (* => nrel_format: format_github_source_link;; *)
        };; 
    *);;

// Full initiation condition of agent
..agent_for_calculating_set_power_initiation_condition
= [*
    action_for_сalculating_set_power _-> .._action;;
    action_initiated _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
*];; 
// Agent should check by this template that initiated action is instance of 
// class `action_for_сalculating_set_power` and that it has argument.

// Full result condition of agent
..agent_for_calculating_set_power_result_condition
= [*
    action_for_сalculating_set_power _-> .._action;;
    action_initiated _-> .._action;;
    action_finished_successfully _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
    .._action _=> nrel_answer:: .._answer;;
*];;
// Agent should check by this template that initiated action is finished 
// and that it has answer.
```

## **ScAgentAbstract**

### **GetAbstractAgent**

### **GetEventClass**

### **GetEventSubscriptionElement**

### **GetActionClass**

### **CheckInitiationCondition**

### **GetInitiationCondition**

### **GetInitiationConditionTemplate**

### **DoProgram**

### **CheckResultCondition**

### **GetResultCondition**

### **GetResultConditionTemplate**

## **ScAgent**



## **ScActionAgent**
