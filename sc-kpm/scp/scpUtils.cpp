/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpUtils.hpp"
#include "scpKeynodes.hpp"

#include <string>
#include <iostream>

using namespace std;

namespace scp
{
namespace Utils
{

bool addToSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
{
    if (ctx.helperCheckArc(setAddr, elAddr, sc_type_arc_pos_const_perm))
        return false;

    ScAddr arcAddr = ctx.createEdge(sc_type_arc_pos_const_perm, setAddr, elAddr);
    assert(arcAddr.isValid());
    return true;
}

bool removeFromSet(ScMemoryContext & ctx, ScAddr const & setAddr, ScAddr const & elAddr)
{
    ScIterator3Ptr it = ctx.iterator3(setAddr, sc_type_arc_pos_const_perm, elAddr);
    bool result = false;
    while (it->next())
        ctx.eraseElement(it->value(1));

    return result;
}

bool resolveOrderRoleRelation(ScMemoryContext & ctx, ScAddr const &arcAddr, ScAddr &relationAddr)
{
    ScIterator3Ptr it = ctx.iterator3(SC_TYPE(sc_type_node | sc_type_const), sc_type_arc_access, arcAddr);
    while (it->next())
    {
        if (ctx.helperCheckArc(Keynodes::order_role_relation, it->value(0), sc_type_arc_pos_const_perm))
        {
            relationAddr = it->value(0);
            return true;
        }
    }

    return false;
}

void printSystemIdentifier(ScMemoryContext & ctx, ScAddr const & elemAddr)
{
    if (ctx.getElementType(elemAddr).isNode() || ctx.getElementType(elemAddr).isLink())
    {
        string s = ctx.helperGetSystemIdtf(elemAddr);
        if (s == "")
            cout << elemAddr.getRealAddr().seg << "|" << elemAddr.getRealAddr().offset;
        else
            cout << s;
    }
    else
    {
        cout << "(";
        printSystemIdentifier(ctx, ctx.getArcBegin(elemAddr));
        cout << "->";
        printSystemIdentifier(ctx, ctx.getArcBegin(elemAddr));
        cout << ")";
    }
}

void printInfo(ScMemoryContext & ctx, ScAddr const & elemAddr)
{
    int c_in = 0, c_out = 0;
    assert(ctx.isElement(elemAddr));
    printSystemIdentifier(ctx, elemAddr);
    cout << endl;;

    cout << "Input arcs:\n";
    ScIterator3Ptr it = ctx.iterator3(SC_TYPE(0), SC_TYPE(0), elemAddr);
    while (it->next())
    {
        c_in++;
        cout << "\t" << it->value(1).getRealAddr().seg << "|" << it->value(1).getRealAddr().offset;
        if (ctx.getElementType(it->value(1)).bitAnd(sc_type_arc_access))
            cout << " <- ";
        else
            cout << " <= ";
        printSystemIdentifier(ctx, it->value(0));
        cout << endl;
    }
    cout << "Total input arcs: " << c_in << endl;

    cout << "Output arcs:\n";
    it = ctx.iterator3(elemAddr, SC_TYPE(0), SC_TYPE(0));
    while (it->next())
    {
        c_out++;
        cout << "\t" << it->value(1).getRealAddr().seg << "|" << it->value(1).getRealAddr().offset;
        if (ctx.getElementType(it->value(1)).bitAnd(sc_type_arc_access))
            cout << " -> ";
        else
            cout << " => ";
        printSystemIdentifier(ctx, it->value(2));
        cout << endl;
    }
    cout << "Total output arcs: " << c_out << endl;
}

void logMissedParameterError(ScMemoryContext & ctx, ScAddr &order)
{
    cout << "Missed scp-program parameter: ";
    printSystemIdentifier(ctx, order);
    cout << endl;
}

}
}
