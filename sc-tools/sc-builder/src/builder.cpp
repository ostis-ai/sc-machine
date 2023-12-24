/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder.hpp"
#include "scs_translator.hpp"
#include "gwf_translator.hpp"

#include <memory>

#include <fstream>

Builder::Builder() = default;

bool Builder::Run(BuilderParams const & params, sc_memory_params const & memoryParams)
{
  m_params = params;

  if (!ScMemory::Initialize(memoryParams))
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Error while sc-memory initialize");

  ScRepoPathCollector::Sources excludedSources;
  ScRepoPathCollector::Sources checkSources;
  if (m_collector.IsRepoPathFile(m_params.m_inputPath))
  {
    ScConsole::PrintLine() << ScConsole::Color::Blue << "Parse repo path file... ";
    m_collector.ParseRepoPath(m_params.m_inputPath, excludedSources, checkSources);
  }
  ScRepoPathCollector::Sources buildSources;
  ScConsole::PrintLine() << ScConsole::Color::Blue << "Collect all sources... ";
  m_collector.CollectBuildSources(m_params.m_inputPath, excludedSources, checkSources, buildSources);

  m_ctx = std::make_unique<ScMemoryContext>("test");
  ScAddr const & outputStructure = m_params.m_resultStructureUpload ? ResolveOutputStructure() : ScAddr::Empty;

  ScConsole::PrintLine() << ScConsole::Color::Blue << "Build knowledge base from sources... ";
  bool const status = BuildSources(buildSources, outputStructure);

  m_ctx.reset();
  ScMemory::Shutdown(true);

  return status;
}

bool Builder::BuildSources(ScRepoPathCollector::Sources const & buildSources, ScAddr const & outputStructure)
{
  m_translators = {{"scs", std::make_shared<SCsTranslator>(*m_ctx)}, {"gwf", std::make_shared<GWFTranslator>(*m_ctx)}};

  // process founded files
  bool status = true;

  size_t done = 0;
  for (auto const & fileName : buildSources)
  {
    ScConsole::Print() << ScConsole::Color::LightBlue << "[" << (++done) << "/" << buildSources.size() << "]: ";
    ScConsole::Print() << ScConsole::Color::Grey << fileName << " - ";

    try
    {
      ProcessFile(fileName, outputStructure);

      ScConsole::PrintLine() << ScConsole::Color::Green << "ok";
    }
    catch (utils::ScException const & e)
    {
      ScConsole::PrintLine() << ScConsole::Color::Red << "failed";
      ScConsole::PrintLine() << ScConsole::Color::Red << e.Message();
      status = false;
      break;
    }
  }

  ScConsole::PrintLine() << ScConsole::Color::Green << "Clean state...";
  Translator::Clean(*m_ctx);

  if (status)
    DumpStatistics();

  return status;
}

ScAddr Builder::ResolveOutputStructure()
{
  ScSystemIdentifierQuintuple fiver;
  m_ctx->HelperResolveSystemIdtf(m_params.m_resultStructureSystemIdtf, ScType::NodeConstStruct, fiver);
  ScAddr const & outputStructure = fiver.addr1;

  auto const & AddElementToStructure = [this, &outputStructure](ScAddr const & addr)
  {
    if (!m_ctx->HelperCheckEdge(outputStructure, addr, ScType::EdgeAccessConstPosPerm))
    {
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, outputStructure, addr);
    }
  };

  AddElementToStructure(fiver.addr2);
  AddElementToStructure(fiver.addr3);
  AddElementToStructure(fiver.addr4);
  AddElementToStructure(fiver.addr5);

  return outputStructure;
}

bool Builder::ProcessFile(std::string const & fileName, ScAddr const & outputStructure)
{
  Translator::Params translateParams;
  translateParams.m_fileName = fileName;
  translateParams.m_autoFormatInfo = m_params.m_autoFormatInfo;
  translateParams.m_outputStructure = outputStructure;

  std::string const & fileExt = m_collector.GetFileExtension(fileName);
  auto const & it = m_translators.find(fileExt);
  if (it == m_translators.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Not found translators for sources with extension \"" << fileExt << "\"");

  return it->second->Translate(translateParams);
}

void Builder::DumpStatistics()
{
  // print statistics
  ScMemoryContext::ScMemoryStatistics const stats = m_ctx->CalculateStat();

  auto const allCount = stats.GetAllNum();

  auto const printLine = [](std::string const & name, uint32_t num, float percent)
  {
    ScConsole::PrintLine() << ScConsole::Color::LightBlue << name << ": " << ScConsole::Color::White << num << "("
                           << percent << "%)";
  };

  ScConsole::PrintLine() << ScConsole::Color::White << "Statistics";
  printLine("Nodes", stats.m_nodesNum, float(stats.m_nodesNum) / float(allCount) * 100);
  printLine("Edges", stats.m_edgesNum, float(stats.m_edgesNum) / float(allCount) * 100);
  printLine("Links", stats.m_linksNum, float(stats.m_linksNum) / float(allCount) * 100);
  ScConsole::PrintLine() << ScConsole::Color::LightBlue << "Total: " << ScConsole::Color::White << stats.GetAllNum();
}
