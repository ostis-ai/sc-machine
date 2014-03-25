/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/
#include "gwf_translator.h"
#include "tinyxml/tinyxml2.h"

#include <fstream>
#include <iostream>
#include <assert.h>


GwfTranslator::GwfTranslator()
{

}

GwfTranslator::~GwfTranslator()
{

}

bool GwfTranslator::translateImpl()
{
    // open file and read data
    bool result = true;
    std::ifstream ifs(mParams.fileName.c_str());
    if (ifs.is_open())
    {
        String data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        result = processString(data);
    } else
        return false;

    ifs.close();

    return result;
}

const String& GwfTranslator::getFileExt() const
{
    return GwfTranslatorFactory::EXTENSION;
}


bool GwfTranslator::processString(const String &data)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.Parse(data.c_str());

    if (error != tinyxml2::XML_SUCCESS)
    {
        THROW_EXCEPT(Exception::ERR_PARSE,
                    doc.GetErrorStr2(),
                    mParams.fileName,
                    -1);
    }

    tinyxml2::XMLElement *root = doc.FirstChildElement("GWF");
    if (!root)
    {
        THROW_EXCEPT(Exception::ERR_PARSE,
                     "Can't find root element",
                     mParams.fileName,
                     -1);
    }
    root = root->FirstChildElement("staticSector");
    if (!root)
    {
        THROW_EXCEPT(Exception::ERR_PARSE,
                     "Cna't find static sector",
                     mParams.fileName,
                     -1);
    }

    // collect elements
    std::vector<tinyxml2::XMLElement*> nodes;
    std::vector<tinyxml2::XMLElement*> edges;

    static std::string s_arc = "arc";
    static std::string s_pair = "pair";

    tinyxml2::XMLElement *el = root->FirstChildElement();
    while (el)
    {
        if (el->Name() == s_arc || el->Name() == s_pair)
            edges.push_back(el);
        else
            nodes.push_back(el);

        el = el->NextSiblingElement();
    }

    static std::string s_node = "node";
    static std::string s_contour = "contour";

    tStringAddrMap id_map;
    tStringAddrMap::iterator itId;

    // create all nodes
    std::vector<tinyxml2::XMLElement*>::iterator it, itEnd = nodes.end();
    for (it = nodes.begin(); it != itEnd; ++it)
    {
        el = *it;

        String idtf = el->Attribute("idtf");
        String id = el->Attribute("id");
        sc_addr addr;

        if (idtf.size() > 0)
        {
            if (getScAddr(idtf, addr))
                continue;    // skip elements that already exists
        } else
        {
            itId = id_map.find(id);
            if (itId != id_map.end())
                continue;
        }

        if (el->Name() == s_contour)
        {
            addr = sc_memory_node_new(sc_type_const);
            appendScAddr(addr, idtf);
        } else
        {
            tinyxml2::XMLElement *content = el->FirstChildElement("content");
            if (!content)
            {
                THROW_EXCEPT(Exception::ERR_PARSE,
                            "There are no child content for node with id=" + id,
                             mParams.fileName,
                             -1);
            }

            if (content->IntAttribute("type") == 0)
            {

            } else
            {
                // need to create link
            }
        }
    }

    return false;
}

bool GwfTranslator::getScAddr(const String &idtf, sc_addr &addr)
{
    tStringAddrMap::iterator it = mLocalIdtfAddrs.find(idtf);
    if (it != mLocalIdtfAddrs.end())
    {
        addr = it->second;
        return true;
    }

    it = mSysIdtfAddrs.find(idtf);
    if (it != mSysIdtfAddrs.end())
    {
        addr = it->second;
        return true;
    }

    it = msGlobalIdtfAddrs.find(idtf);
    if (it != msGlobalIdtfAddrs.end())
    {
        addr = it->second;
        return true;
    }

    return false;
}

sc_type GwfTranslator::convertType(const String &type)
{
    if (type == "node/const/general_node" || type == "node/const/general")
        return sc_type_node | sc_type_const;

    if (type == "node/var/general_node" || type == "node/const/var")
        return sc_type_node | sc_type_var;

    if (type == "node/const/relation")
        return sc_type_node | sc_type_node_tuple | sc_type_const;

    if (type == "node/var/relation")
        return sc_type_node | sc_type_node_tuple | sc_type_var;

    if (type == "node/const/attribute")
        return sc_type_node | sc_type_node_norole | sc_type_const;

    if (type == "node/var/attribute")
        return sc_type_node | sc_type_node_norole | sc_type_var;

    if (type == "node/const/nopredmet")
        return sc_type_node | sc_type_node_struct | sc_type_const;

    if (type == "node/var/nopredmet")
        return sc_type_node | sc_type_node_struct | sc_type_var;

    return sc_type_node;
}

// -------------------
const String GwfTranslatorFactory::EXTENSION = "gwf";

GwfTranslatorFactory::GwfTranslatorFactory()
{
}

GwfTranslatorFactory::~GwfTranslatorFactory()
{
}

iTranslator* GwfTranslatorFactory::createInstance()
{
    return new GwfTranslator();
}

const String& GwfTranslatorFactory::getFileExt() const
{
    return EXTENSION;
}
