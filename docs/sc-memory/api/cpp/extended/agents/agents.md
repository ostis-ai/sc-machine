# **C++ Agents API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement sc-agents on C++.

## **What is the agent-based model?**

The sc-machine implements the agent-based model to process information. In the agent-based model, agents exchange messages only through shared memory, adding a new agent or eliminating one or more existing agents does not result in changes to other agents, agent initiation is decentralised and most often independent of each other. 

All agents within the OSTIS Technology are divided into two classes: platform-independent, i.e. implemented only by means of SC-code, and platform-dependent, implemented by means of sc-machine API. This sc-machine presents a powerful, but simple API for developing and maintaining platform-dependent agents in C++.

All agents react to the occurrence of events in sc-memory (sc-events). That is, an agent is called implicitly when an sc-event occurs, for which type this agent is already subscribed. Knowledge about which sc-event will cause this agent to be called (awakening of this agent) is called primary initiation condition. Upon awakening, the agent checks for the presence of its full initiation condition. If the full initiation condition is successfully checked, the agent initiates an action of some class and starts interpreting it with a agent program. After executing its program, the agent can check if there is a result.

All knowledge about an agent: primary initiation condition, class of actions it can interpret, initiation condition, and result condition, are part of specification of this agent. This specification can be represented either in a knowledge base, using SC-code, or programmarly, using sc-machine API.
