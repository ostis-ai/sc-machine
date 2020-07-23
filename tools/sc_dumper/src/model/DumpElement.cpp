//
// Created by alexander on 23.05.20.
//

#include "DumpElement.h"
#include <vector>

ScAddr DumpElement::getAddr() {
    return this->addr;
}

DumpElement::DumpElement(ScAddr addr, int id) {
    this->addr = addr;
    this->id = id;
}

int DumpElement::getId() {
    return this->id;
}

std::vector<std::string> DumpElement::getTypes() {
    return this->types;
}

DumpElement::~DumpElement() {
    this->types.clear();

}

void DumpElement::addType(std::string type) {
    this->types.push_back(type);
}
