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

  std::shared_ptr<SCsElement> source;
  std::shared_ptr<SCsElement> target;

  std::string sourceIdtf;
  std::string targetIdtf;

public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  std::string Dump(std::string const & filepath) const override;
};

class SCsContour : public SCsElement
{
private:
  std::list<std::shared_ptr<SCsElement>> elements;

public:
  std::list<std::shared_ptr<SCsElement>> & GetContourElements()
  {
    return elements;
  }

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  void AddElement(std::shared_ptr<SCsElement> const & element);

  std::string Dump(std::string const & filepath) const override;
};

class SCsElementFactory
{
public:
  static std::shared_ptr<SCsElement> CreateElementFromSCgElement(std::shared_ptr<SCgElement> const & element);
};

class SCsNodeInContour : public SCsElement
{
private:
  std::list<std::shared_ptr<SCsElement>> & contourElements;
  size_t multipleArcCounter;
  std::string contourId;
  std::string nodeId;

  std::string contourIdtf;

public:
  SCsNodeInContour(std::list<std::shared_ptr<SCsElement>> & contourElements, std::string const & contourId, std::string const & contourIdtf)
    : contourElements(contourElements)
    , multipleArcCounter(0)
    , contourId(contourId)
    , contourIdtf(contourIdtf)
  {
  }

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;

  std::string Dump(std::string const & filepath) const override;
};

class SCsWriter
{
public:
  std::string Write(
      std::unordered_map<std::string, std::shared_ptr<SCgElement>> const & elementsList,
      std::string const & filePath);

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

  static void WriteMainIdtf(Buffer & buffer, std::string const & systemIdtf, std::string const & mainIdtf);
  static bool CheckForNode(
      std::shared_ptr<SCgElement> const refElement,
      std::vector<std::shared_ptr<SCgElement>> const & contourElements);
  static std::string MakeAlias(std::string const & prefix, std::string const & element_id);
  static bool IsVariable(std::string const & el_type);
};
