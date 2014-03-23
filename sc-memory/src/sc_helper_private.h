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
#ifndef _sc_helper_private_h_
#define _sc_helper_private_h_

#include "sc-store/sc_types.h"

/*! Initialize helper.
 * @remarks Need to be called once at the beginning of sc-helper usage
 * @return If sc-helper initialized without any errors, then return SC_OK;
 * otherwise returns SC_ERROR
 */
sc_result sc_helper_init();

/*! Shuts down sc-helper.
 * @remarks This function need to be called once at the end of sc-helper usage
 */
void sc_helper_shutdown();


#endif
