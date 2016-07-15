/* ----------------------------------------------------------------------------
** © 201x Austin Brunkhorst, All Rights Reserved.
**
** Precompiled.h
** --------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <functional>

#include <clang-c/Index.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "MetaUtils.hpp"
#include "MetaDataConfig.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;
