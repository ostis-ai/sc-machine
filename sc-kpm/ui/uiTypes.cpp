/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
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

#include "uiPrecompiled.h"
#include "uiTypes.h"

bool operator < (const sc_addr &addr1, const sc_addr &addr2)
{
    if (addr1.seg != addr2.seg)
        return addr1.seg < addr2.seg;

    return addr1.offset < addr2.offset;
}

bool operator == (const sc_addr &addr1, const sc_addr &addr2)
{
    return (addr1.seg == addr2.seg) && (addr1.offset == addr2.offset);
}

bool operator != (const sc_addr &addr1, const sc_addr &addr2)
{
    return (addr1.seg != addr2.seg) || (addr1.offset != addr2.offset);
}
