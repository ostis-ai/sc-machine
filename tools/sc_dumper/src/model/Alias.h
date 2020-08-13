//
// Created by alexander on 20.07.20.
//

#ifndef SC_MACHINE_ALIAS_H
#define SC_MACHINE_ALIAS_H


#include <sc-memory/sc_addr.hpp>
#include <string>

class Alias {
    ScAddr addr;
    int id;
    std::string edge;


public:
    Alias(ScAddr addr, int id);
    int getId();
    ScAddr getAddr();
    void setEdge(std::string edge);
    std::string getEdge();
};


#endif //SC_MACHINE_ALIAS_H
