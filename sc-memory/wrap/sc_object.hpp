/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_types.hpp"

/**
 * Base class for all objects that has meta data.
 * If you override it, then call any constructor of ScObject
 * in your custom constructors
 */
class _SC_EXTERN ScObject
{

protected:
    explicit ScObject();
    virtual ~ScObject();

    ScObject(ScObject const & other);
    ScObject & operator = (ScObject const & other);

    /// TODO: Need mechanism to call that function automaticaly after object construction
    bool init();

private:
    /** This method override genarates by code generator, and initialize all
      * meta data for this object. It calls from ScObject constructors
      */
    virtual bool _initInternal() = 0;

private:
    bool mIsInitialized : 1;
    bool mInitResult : 1;
};
