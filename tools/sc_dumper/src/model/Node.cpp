//
// Created by alexander on 23.05.20.
//

#include "Node.h"
#include <vector>

ScAddr Node::getAddr() {
    return this->addr;
}

Node::Node(ScAddr addr, int id) {
    this->addr = addr;
    this->id = id;
}

int Node::getId() {
    return this->id;
}

std::vector<std::string> Node::getTypes() {
    return this->types;
}

Node::~Node() {
    this->types.clear();

}

void Node::addType(std::string type) {
    this->types.push_back(type);
}
