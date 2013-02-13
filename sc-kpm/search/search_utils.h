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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */

#include "sc_memory.h"

#ifndef _search_functions_h_
#define _search_functions_h_

/*! Search full semantic neighbourhood of given element
 * @param addr sc-addr of element
 * @param answer sc-addr of set, which will contain all the elements of answer.
 * @return Returns SC_FALSE, if system error appeared. Otherwise returns SC_TRUE.
 */
sc_bool search_full_semantic_neighbourhood(sc_addr node, sc_addr answer);


/*! Connect answer with question by specified releation
 * @param question sc-addr of question node
 * @param answer sc-addr of answer node
 */
void connect_answer_to_question(sc_addr question, sc_addr answer);

/*!
 * Remove question from question_initiated set and append it into
 * question_finished set.
 * @param question sc-add of question node
 */
void finish_question(sc_addr question);

#endif
