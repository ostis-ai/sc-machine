/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>
#include <unordered_set>
#include <filesystem>

#include <sc-memory/sc_utils.hpp>

#include "scg_to_scs_el.hpp"
#include "gwf_parser.hpp"

class Buffer
{
public:
  Buffer();

  void Write(std::string const & s);
  void AddTabs(std::size_t const & count);

  std::string GetValue() const;

private:
  std::string value;
};

class SCsElement
{
protected:
  std::string mainIdtf;
  std::string systemIdtf;

public:
  virtual void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) = 0;

  virtual std::string Dump(std::string const & filepath) const = 0;

  virtual ~SCsElement() = default;

  void SetSystemIdtf(std::string const & idtf)
  {
    systemIdtf = idtf;
  }

  void SetMainIdtf(std::string const & idtf)
  {
    mainIdtf = idtf;
  }

  std::string GetSystemIdtf() const
  {
    return systemIdtf;
  }

  std::string GetMainIdtf() const
  {
    return mainIdtf;
  }
};

class SCsNode : public SCsElement
{
private:
  std::string type;

public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  std::string Dump(std::string const & filepath) const override;
};

class SCsLink : public SCsElement
{
private:
  bool isUrl{false};
  std::string type;
  std::string fileName;
  std::string content;
  std::string urlContent;

public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  std::string Dump(std::string const & filepath) const override;
};

class SCsConnector : public SCsElement
{
private:
  std::string symbol;
  bool isUnsupported;

  std::string alias;

  std::string sourceIdtf;
  std::string targetIdtf;

public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  std::string Dump(std::string const & filepath) const override;

  std::string GetSourceTargetIdtf(std::shared_ptr<SCgElement> const & element) const;
};

class SCsContour : public SCsElement
{
private:
  std::list<std::shared_ptr<SCsElement>> scsElements;

  std::list<std::shared_ptr<SCgElement>> scgElements;

public:
  std::list<std::shared_ptr<SCsElement>> & GetContourElements()
  {
    return scsElements;
  }

  std::list<std::shared_ptr<SCgElement>> GetSCgElements()
  {
    return scgElements;
  }

  void SetSCgElements(std::list<std::shared_ptr<SCgElement>> elements)
  {
    scgElements = elements;
  }

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  void AddElement(std::shared_ptr<SCsElement> const & element);

  std::string Dump(std::string const & filepath) const override;
};

class SCsMultipleElement : public SCsElement
{
public:
  SCsMultipleElement(
      std::list<std::shared_ptr<SCgElement>> const & scgElements,
      std::string const & nodeId,
      std::string const & contourId,
      std::string const & contourIdtf)
    : scgElements(scgElements)
    , nodeId(nodeId)
    , contourId(contourId)
    , contourIdtf(contourIdtf)
    , multipleArcCounter(0)
  {
  }

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override = 0;

  std::string Dump(std::string const & filepath) const override = 0;

  std::list<std::shared_ptr<SCgElement>> GetSCgElements()
  {
    return scgElements;
  }

  void SetMultipleArcCounter(size_t counter)
  {
    multipleArcCounter = counter;
  }

  std::string GetMultipleArcCounter() const
  {
    return std::to_string(multipleArcCounter);
  }

  void SetNodeId(std::string const & id)
  {
    nodeId = id;
  }

  std::string const & GetNodeId() const
  {
    return nodeId;
  }

  void SetNodeIdtf(std::string const & idtf)
  {
    nodeIdtf = idtf;
  }

  std::string const & GetNodeIdtf() const
  {
    return nodeIdtf;
  }

  void SetContourId(std::string const & id)
  {
    contourId = id;
  }

  std::string const & GetContourId() const
  {
    return contourId;
  }

  void SetContourIdtf(std::string const & idtf)
  {
    contourIdtf = idtf;
  }

  std::string const & GetContourIdtf() const
  {
    return contourIdtf;
  }

  void AddWrittenPair(std::string const & contourId, std::string const & nodeId)
  {
    writtenPairs[contourId] = nodeId;
  }

  bool IsPairWritten(std::string const & contourId, std::string const & nodeId) const
  {
    auto it = writtenPairs.find(contourId);
    return it != writtenPairs.end() && it->second == nodeId;
  }

  void CalculateMultipleArcs(size_t & multipleArcCounter);

private:
  std::unordered_map<std::string, std::string> writtenPairs;
  std::list<std::shared_ptr<SCgElement>> scgElements;

  std::string nodeId;
  std::string nodeIdtf;
  std::string contourId;
  std::string contourIdtf;
  size_t multipleArcCounter;
};

class SCsNodeInContour : public SCsMultipleElement
{
public:
  SCsNodeInContour(
      std::list<std::shared_ptr<SCgElement>> const & scgElements,
      std::string const & nodeId,
      std::string const & contourId,
      std::string const & contourIdtf)
    : SCsMultipleElement(scgElements, nodeId, contourId, contourIdtf)
  {
  }

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  std::string Dump(std::string const & filepath) const override;
};

class SCsEdgeFromContourToNode : public SCsMultipleElement
{
public:
  SCsEdgeFromContourToNode(
      std::list<std::shared_ptr<SCgElement>> const & scgElements,
      std::string const & nodeId,
      std::string const & contourId,
      std::string const & contourIdtf)
    : SCsMultipleElement(scgElements, nodeId, contourId, contourIdtf)
  {
  }

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  std::string Dump(std::string const & filepath) const override;
};

class SCsElementFactory
{
public:
  static std::shared_ptr<SCsElement> CreateAndConvertElementFromSCgElement(
      std::shared_ptr<SCgElement> const & scgElement);

  static std::shared_ptr<SCsElement> CreateAndConvertElementFromSCgElement(
      std::shared_ptr<SCgElement> const & scgElement,
      std::list<std::shared_ptr<SCgElement>> const & scgElements,
      std::shared_ptr<SCgContour> const & scgContour,
      std::shared_ptr<SCsContour> const & scsContour);
};

class SCsWriter
{
public:
  std::string Write(
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elementsList,
      std::string const & filePath) const;

  // Utils

  class CorrectorOfSCgIdtf

  {
  public:
    static void CorrectIdtf(std::shared_ptr<SCgElement> const & scgElement, std::shared_ptr<SCsElement> & scsElement);

  private:
    static bool IsRussianIdtf(std::string const & idtf);
    static bool IsEnglishIdtf(std::string const & idtf);

    static std::string GenerateIdtfForUnresolvedCharacters(
        std::string & systemIdtf,
        std::string const & elementId,
        bool isVar);
    static std::string GenerateCorrectedIdtf(std::string & systemIdtf, std::string const & elementId, bool isVar);
    static std::string CorrectIdtfForVariable(std::string & systemIdtf);
    static std::string CorrectIdtfForNonVariable(std::string & systemIdtf);
  };

  static std::list<std::shared_ptr<SCgElement>> ConvertMapToList(
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & scgElements);
  static void WriteMainIdtf(Buffer & buffer, std::string const & systemIdtf, std::string const & mainIdtf);
  static std::string MakeAlias(std::string const & prefix, std::string const & element_id);
  static bool IsVariable(std::string const & el_type);
};
