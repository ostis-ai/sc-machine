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
#include "model/DumpElement.h"

using namespace std;

bool printEl(ScAddr element, string *strBuilder);
bool printEl3(ScAddr element, string *strBuilder);
bool printElFinal(ScAddr element, string *strBuilder);
void printEdge(ScAddr element, const string& connector, DumpElement* dumpElement, string *strBuilder);
string printContent(ScAddr element);
bool checkLinkFormat(ScAddr element, string *format);
sc_char *saveContentFile(ScAddr element, string data, const string& format);
bool isAddrExist(ScAddr addr);
int getElementIdByAddr(ScAddr addr);
bool isEdge(ScAddr addr);
string processAlias(string line);

#endif //SC_MACHINE_DUMPER_H
