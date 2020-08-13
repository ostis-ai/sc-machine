//
// Created by alexander on 20.07.20.
//

#include "Alias.h"

Alias::Alias(ScAddr addr, int id) {
    this->addr = addr;
    this->id = id;
}

ScAddr Alias::getAddr() {
    return this->addr;
}

int Alias::getId() {
    return this->id;
}

std::string Alias::getEdge() {
    return this->edge;
}

void Alias::setEdge(std::string edge) {
    this->edge = edge;
}
