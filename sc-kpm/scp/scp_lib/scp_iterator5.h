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

#ifndef SCP_ITERATOR5_H
#define SCP_ITERATOR5_H
#include "scp_types.h"
#include "sc_memory_headers.h"

typedef sc_iterator5 scp_iterator5;

scp_iterator5 *scp_iterator5_new(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5);
scp_result scp_iterator5_next(sc_memory_context *context, scp_iterator5 *iter, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5);
void scp_iterator5_free(scp_iterator5 *iter);

#endif // SCP_ITERATOR5_H
