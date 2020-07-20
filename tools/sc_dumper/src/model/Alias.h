//
// Created by alexander on 20.07.20.
//

#ifndef SC_MACHINE_ALIAS_H
#define SC_MACHINE_ALIAS_H


#include <sc-memory/sc_addr.hpp>

class Alias {
    ScAddr addr;
    int id;


public:
    Alias(ScAddr addr, int id);

    int getId();

    ScAddr getAddr();
};


#endif //SC_MACHINE_ALIAS_H
