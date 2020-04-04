/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_debug.hpp"

class ExceptionFileNotFound final : public utils::ScException
{
public:
  ExceptionFileNotFound(std::string const & description, std::string const & msg) : utils::ScException("Assert: " + description, msg) {}
};