/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <list>
#include <unordered_map>
#include <memory>

class SCgElement;

class SCsElement
{
protected:
  std::string m_mainIdentifier;
  std::string m_systemIdentifier;

public:
  virtual void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) = 0;
  virtual std::string Dump(std::string const & filepath) const = 0;
  virtual ~SCsElement() = default;

  void SetSystemIdentifier(std::string const & identifier);
  void SetMainIdentifier(std::string const & identifier);
  std::string GetSystemIdentifier() const;
  std::string GetMainIdentifier() const;
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
  bool m_isUrl{false};
  std::string m_type;
  std::string m_fileName;
  std::string m_content;
  std::string m_urlContent;

public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;
  std::string Dump(std::string const & filepath) const override;
};

class SCsConnector : public SCsElement
{
private:
  std::string m_symbol;
  bool m_isUnsupported;
  std::string m_alias;
  std::string m_sourceIdentifier;
  std::string m_targetIdentifier;

public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;
  std::string Dump(std::string const & filepath) const override;
  std::string GetSourceTargetIdentifier(std::shared_ptr<SCgElement> const & element) const;
};

class SCsContour : public SCsElement
{
private:
  std::list<std::shared_ptr<SCsElement>> m_scsElements;
  std::list<std::shared_ptr<SCgElement>> m_scgElements;

public:
  std::list<std::shared_ptr<SCsElement>> & GetSCsElements();
  std::list<std::shared_ptr<SCgElement>> & GetSCgElements();
  void SetSCgElements(std::list<std::shared_ptr<SCgElement>> elements);
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
      std::string const & contourIdentifier);

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override = 0;
  std::string Dump(std::string const & filepath) const override = 0;

  std::list<std::shared_ptr<SCgElement>> GetSCgElements();
  void SetMultipleArcCounter(size_t counter);
  std::string GetMultipleArcCounter() const;
  void SetNodeId(std::string const & id);
  std::string const & GetNodeId() const;
  void SetNodeIdentifier(std::string const & identifier);
  std::string const & GetNodeIdentifier() const;
  void SetContourId(std::string const & id);
  std::string const & GetContourId() const;
  void SetContourIdentifier(std::string const & identifier);
  std::string const & GetContourIdentifier() const;
  void AddWrittenPair(std::string const & contourId, std::string const & nodeId);
  bool IsPairWritten(std::string const & contourId, std::string const & nodeId) const;
  void CalculateMultipleArcs(size_t & multipleArcCounter);

private:
  std::unordered_map<std::string, std::string> m_writtenPairs;
  std::list<std::shared_ptr<SCgElement>> m_scgElements;
  std::string m_nodeId;
  std::string m_nodeIdentifier;
  std::string m_contourId;
  std::string m_contourIdentifier;
  size_t m_multipleArcCounter;
};

class SCsNodeInContour : public SCsMultipleElement
{
public:
  SCsNodeInContour(
      std::list<std::shared_ptr<SCgElement>> const & scgElements,
      std::string const & nodeId,
      std::string const & contourId,
      std::string const & contourIdentifier);

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
      std::string const & contourIdentifier);

  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;
  std::string Dump(std::string const & filepath) const override;
};
