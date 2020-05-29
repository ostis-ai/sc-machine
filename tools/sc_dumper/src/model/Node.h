//
// Created by alexander on 23.05.20.
//

#ifndef SC_MACHINE_NODE_H
#define SC_MACHINE_NODE_H

#include <sc-core/sc-store/sc_types.h>
#include <vector>
#include <string>
#include <sc-memory/sc_addr.hpp>

class Node {
private:
    ScAddr addr;
    int id;
    std::vector<std::string> types;


public:

    Node(ScAddr addr, int id);
    ~Node();
    ScAddr getAddr();
    int getId();
    std::vector<std::string> getTypes();
    void addType(std::string type);

};

#endif //SC_MACHINE_NODE_H
