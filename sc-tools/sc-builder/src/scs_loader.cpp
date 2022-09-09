/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "scs_loader.hpp"

#include "sc-memory/sc_memory.hpp"
#include "scs_translator.hpp"

void ScsLoader::loadScsFile(ScMemoryContext &context, const std::string &filename)
{
    SCsTranslator translator = SCsTranslator(context);

    Translator::Params translateParams;
    translateParams.m_fileName = filename;
    translateParams.m_autoFormatInfo = true;

    translator.Translate(translateParams);
}
