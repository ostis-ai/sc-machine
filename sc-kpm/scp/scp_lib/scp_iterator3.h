/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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

#ifndef SCP_ITERATOR3_H
#define SCP_ITERATOR3_H
#include "scp_types.h"

typedef sc_iterator3 scp_iterator3;

scp_iterator3 *scp_iterator3_new(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3);
scp_result scp_iterator3_next(sc_memory_context *context, scp_iterator3 *iter, scp_operand *param1, scp_operand *param2, scp_operand *param3);
void scp_iterator3_free(scp_iterator3 *iter);

#endif // SCP_ITERATOR3_H
