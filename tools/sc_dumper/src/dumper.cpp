//
// Created by alexander Yurkov on 23.05.20.
//

#include "dumper.h"
#include <cstdio>
#include <unistd.h>
#include "model/Node.h"
#include <vector>
#include <sc-core/sc_helper.h>

using namespace std;

std::unique_ptr<ScMemoryContext> m_ctx;

ScAddr graph;

vector<Node> nodeVector;
vector<string> linkVector;
int uniqId;

string printContent(ScAddr element)
{
    string data;
    ScStreamPtr stream = m_ctx->GetLinkContent(element);
    string str;
    if (ScStreamConverter::StreamToString(stream, str)) {
        stringstream streamString(str);
        data = streamString.str();
    }
    string format;
    if (checkLinkFormat(element, &format)) {
        return saveContentFile(element, data, format);
    }
    return data;
}

bool checkLinkFormat(ScAddr element, string *format) {
    bool result = false;
    ScAddr nrel_format;
    nrel_format = m_ctx->HelperFindBySystemIdtf(NREL_FORMAT_STR);
    if (!nrel_format.IsValid()) {
        //todo make normal exeption
        printf("ERROR");
        exit(0);
    }
    ScIterator5Ptr it = m_ctx->Iterator5(element,
                                         ScType::EdgeDCommonConst,
                                         ScType::NodeConst,
                                         ScType::EdgeAccessConstPosPerm,
                                         nrel_format);
    if (it->Next())
    {
        ScAddr t_node =it->Get(2);
        string temp_string = m_ctx -> HelperGetSystemIdtf(t_node);
        size_t n = temp_string.find_last_of('_');
        if (n != string::npos)
            *format = temp_string.substr(n + 1);
        result=true;
    }
    return result;
}

sc_char *saveContentFile(ScAddr element, string data, string format) {
    string filename = "content_";
    string answer = "file://content/";
    filename.append(to_string(element.Hash()));
    filename.append(".");
    filename.append(format);
    FILE *f;
    f = fopen((DUMP_CONTENT_FOLDER + filename).c_str(), "w");
    //todo rework this method
    for (int i = 0; i < data.size(); i++) {
        fprintf(f, "%c", data[i]);
    }
    fclose(f);
    answer.append(filename);
    char *cstr = new char[answer.length() + 1];
    strcpy(cstr, answer.c_str());
    return cstr;
}

void run_test()
{
    FILE *f;
    f = fopen("/home/alexander/Desktop/KnowledgeDump.scs", "w");
    string gr = "concertedKB_hash_iF95K2";
    graph = m_ctx->HelperFindBySystemIdtf(gr);

//    ScIterator3Ptr it = m_ctx->Iterator3(graph,
//                                         ScType::EdgeAccessConstPosPerm,
//                                         ScType::NodeConst);
    ScIterator3Ptr it = m_ctx->Iterator3(graph,
                                         ScType::EdgeAccessConstPosPerm,
                                         ScType::Const);
    auto x = new std::string("");
    while (it->Next()) {
        ScAddr t_arc =it->Get(2);
        x->clear();
        if (printEl(t_arc, x)) {
            string x2 = x->substr(1, x->size()-2);
            x2.append(";;\n");
            size_t found = x2.find("nrel_system_identifier");
            if (found == std::string::npos) {
                fprintf(f, "%s", x2.c_str());
            }
        }
    }
    fprintf(f, "\n");
    //print all node atributes
    for (auto & node : nodeVector) {
        x->clear();
        if (printEl2(node.getAddr(), x)) {
            if (!node.getTypes().empty()) {
                fprintf(f, "%s", x->c_str());
            }
            for (int j = 0; j < node.getTypes().size(); j++) {
                fprintf(f, "<-%s;", node.getTypes().at(j).c_str());
            }
            if (!node.getTypes().empty()) {
                fprintf(f, ";\n");
            }
        }
    }
    nodeVector.clear();
    fclose(f);
    f = fopen("/home/alexander/Desktop/KnowledgeDumpLinks.scs", "w");
    for (auto & item : linkVector) {
        fprintf(f, "%s;;\n", item.c_str());
    }
    fclose(f);
}

bool printEl2(ScAddr element, string *strBuilder) {
    bool isPrinted = false;
    ScType type = m_ctx->GetElementType(element);
    if ((type == ScType::NodeConst) | (type == ScType::NodeConstStruct)) {
        try {
            string idtf = m_ctx -> HelperGetSystemIdtf(element);
            if (!idtf.empty()) {
                strBuilder->append(idtf);
            } else {
                strBuilder->append("..").append(to_string(getElementIdByAddr(element)));
            }
        }
        catch (...) {
            strBuilder->append("fail");
        }
        isPrinted = true;
    }
    if (type == ScType::LinkConst) {
        strBuilder->append("..").append(to_string(getElementIdByAddr(element)));
        isPrinted = true;
    }
    return isPrinted;
}

void printEdge(ScAddr element, string connector, Node *node, string *strBuilder) {
    ScAddr elem1 = m_ctx -> GetEdgeSource(element);
    ScAddr elem2 = m_ctx -> GetEdgeTarget(element);
    strBuilder->append("(");

//    if (isAddrExist(elem1)) {
//        int x = getElementIdByAddr(elem1);
//        //if 0, than element have system ID (no ..)
//        if (x!=0) {
//            strBuilder->append("..").append(to_string(x));
//        }
//        else {
//            printEl2(elem1, strBuilder);
//        }
//    }
//    strBuilder->append(connector);
//    if (isAddrExist(elem2)) {
//        int x = getElementIdByAddr(elem2);
//        if (x!=0) {
//            strBuilder->append("..").append(to_string(x));
//        }
//        else {
//            printEl2(elem2, strBuilder);
//        }
//    }
    //todo все полная ересь, переписать
    if (!printEl(elem1, strBuilder)) {
        int x = getElementIdByAddr(elem1);
        if (x!=0) {
            strBuilder->append("..").append(to_string(x));
        }
        else {
            printEl2(elem1, strBuilder);
        }
    }
    strBuilder->append(connector);
    if (!printEl(elem2, strBuilder)) {
        int x = getElementIdByAddr(elem2);
        if (x!=0) {
            strBuilder->append("..").append(to_string(x));
        }
        else {
            printEl2(elem2, strBuilder);
        }
    }
    strBuilder->append(")");
    node = new Node(element, uniqId);
    nodeVector.push_back(*node);
    uniqId++;
}

bool printEl(ScAddr element, string* strBuilder)
{
    bool answer = false;
    if (isAddrExist(element)) {
        return answer;
    }
    Node *node = NULL;
    ScType type = m_ctx->GetElementType(element);
    if ((type == ScType::NodeConst) | (type == ScType::NodeConstStruct)) {
        try {
            string idtf = m_ctx -> HelperGetSystemIdtf(element);
            if (!idtf.empty()) {
                strBuilder->append(idtf);
                node = new Node(element, 0);
                nodeVector.push_back(*node);
                answer = true;
            }
            else
            {
                node = new Node(element, uniqId);
                nodeVector.push_back(*node);
                strBuilder->append("..").append(to_string(uniqId));
                uniqId++;
                answer = true;
            }
        }
        catch (...) {
            strBuilder->append("fail");
        }
    }
    if (type == ScType::LinkConst) {
        strBuilder->append("..").append(to_string(uniqId));
        string newLink;
        newLink.append("..").append(to_string(uniqId)).append(" = [").append(printContent(element)).append("]");
        linkVector.push_back(newLink);
        node = new Node(element, uniqId);
        nodeVector.push_back(*node);
        uniqId++;
        answer = true;
    }
    string connector = "";
    if (type == ScType::EdgeDCommonConst) connector = "=>";
    if (type == ScType::EdgeDCommonVar) connector = "_=>";
    if (type == ScType::EdgeUCommonConst) connector = "<=>";
    if (type == ScType::EdgeUCommonVar) connector = "_<=>";
    if (type == ScType::EdgeAccess) connector = "..>";
    if (type == ScType::EdgeAccessConstPosPerm) connector = "->";
    if (type == ScType::EdgeAccessVarPosPerm) connector = "_->";
    if (type == ScType::EdgeAccessConstNegPerm) connector = "-|>";
    if (type == ScType::EdgeAccessVarNegPerm) connector = "_-|>";
    if (type == ScType::EdgeAccessConstFuzPerm) connector = "-/>";
    if (type == ScType::EdgeAccessVarFuzPerm) connector = "_-/>";
    if (type == ScType::EdgeAccessConstPosTemp) connector = "~>";
    if (type == ScType::EdgeAccessVarPosTemp) connector = "_~>";
    if (type == ScType::EdgeAccessConstNegTemp) connector = "~|>";
    if (type == ScType::EdgeAccessVarNegTemp) connector = "_~|>";
    if (type == ScType::EdgeAccessConstFuzTemp) connector = "~/>";
    if (type == ScType::EdgeAccessVarFuzTemp) connector = "_~/>";
    if (connector != "") {
        printEdge(element, connector, node, strBuilder);
        answer = true;
        return answer;
    }
//    if (node!= NULL) {
//        if (type == ScType::NodeConstStruct) nodeVector.at(nodeVector.size()-1).addType("sc_node_struct");
//        if (type == ScType::NodeConstTuple) nodeVector.at(nodeVector.size()-1).addType("sc_node_not_binary_tuple");
//        if (type == ScType::NodeConstRole) nodeVector.at(nodeVector.size()-1).addType("sc_node_role_relation");
//        if (type == ScType::NodeConstNoRole) nodeVector.at(nodeVector.size()-1).addType("sc_node_norole_relation");
//        if (type == ScType::NodeConstClass) nodeVector.at(nodeVector.size()-1).addType("sc_node_not_relation");
//        if (type == ScType::NodeConstAbstract) nodeVector.at(nodeVector.size()-1).addType("sc_node_abstract");
//        if (type == ScType::NodeConstMaterial) nodeVector.at(nodeVector.size()-1).addType("sc_node_material");
//    }
    return answer;
}

bool isAddrExist(ScAddr addr) {
    bool answer = false;
    for (auto & node : nodeVector) {
        if (addr == node.getAddr()) {
            answer = true;
            return answer;
        }
    }
    return answer;
}

int getElementIdByAddr(ScAddr addr) {
    for (auto & node : nodeVector) {
        if (addr == node.getAddr()) {
            return node.getId();
        }
    }
    return -1;
}

int main()
{
    sc_memory_params params;
    sc_memory_params_clear(&params);
    params.repo_path = "/home/alexander/work/0.6.0/ostis/kb.bin";
    params.config_file = "/home/alexander/work/0.6.0/ostis/config/sc-web.ini";
    params.ext_path = "/home/alexander/work/0.6.0/ostis/sc-machine/bin/extensions";
    params.clear = SC_FALSE;
    ScMemory::Initialize(params);
    m_ctx.reset(new ScMemoryContext(sc_access_lvl_make_max, "sc_dumper"));

    uniqId = 1;
    run_test();
    m_ctx.reset();
    ScMemory::Shutdown(true);

    return 0;
}

