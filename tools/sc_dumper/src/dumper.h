//
// Created by alexander on 23.05.20.
//

#ifndef SC_MACHINE_DUMPER_H
#define SC_MACHINE_DUMPER_H

#define NREL_FORMAT_STR     "nrel_format"
#define DUMP_FOLDER "/home/alexander/Desktop/KnowledgeDump.scs"
#define DUMP_CONTENT_FOLDER "/home/alexander/Desktop/content/"

#include <cstdio>
#include <sc-core/sc-store/sc_types.h>
#include <string>
#include "sc-memory/sc_memory.hpp"
#include "model/Node.h"

using namespace std;

bool printEl(ScAddr element, string *strBuilder);
bool printEl2(ScAddr element, string *strBuilder);
void printEdge(ScAddr element, string connector, Node* node, string *strBuilder);
string printContent(ScAddr element);
bool checkLinkFormat(ScAddr element, string *format);
sc_char *saveContentFile(ScAddr element, string data, string format);
bool isAddrExist(ScAddr addr);
int getElementIdByAddr(ScAddr addr);

#endif //SC_MACHINE_DUMPER_H
