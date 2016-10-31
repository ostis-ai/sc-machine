/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_object.hpp"


ScObject::ScObject()
    : mIsInitialized(false)
    , mInitResult(false)
{
}

ScObject::~ScObject()
{

}

ScObject::ScObject(ScObject const & other)
{

}

ScObject & ScObject::operator=(ScObject const & other)
{
    return *this;
}

bool ScObject::init()
{
    if (!mIsInitialized)
    {
        mIsInitialized = true;
        mInitResult = _initInternal();
    }
    
    return mInitResult;
}