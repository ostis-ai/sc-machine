/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <keynodes.generated.hpp>
#include "utils.hpp"

class ScsLoader {
public:
    void loadScsFile(ScMemoryContext & context, const std::string & filename);
};
