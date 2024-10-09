/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <list>
#include <unordered_set>

#include "buffer.hpp"
#include "gwf_translator_constants.hpp"
#include "sc_scg_element.hpp"

class SCsElement
{
public:
  virtual void ConvertFromSCgElement(SCgElementPtr const & element) = 0;
  virtual void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<SCgElementPtr> & writtenElements) const = 0;
  virtual ~SCsElement() = default;

  void SetIdentifierForSCs(std::string const & identifier);
  void SetMainIdentifier(std::string const & identifier);
  std::string GetIdentifierForSCs() const;
  std::string GetMainIdentifier() const;

protected:
  std::string m_mainIdentifier;
  std::string m_identifierForSCs;
  std::string m_type;
};

class SCsNode : public SCsElement
{
public:
  void ConvertFromSCgElement(SCgElementPtr const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<SCgElementPtr> & writtenElements) const override;
};

class SCsLink : public SCsElement
{
public:
  void ConvertFromSCgElement(SCgElementPtr const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<SCgElementPtr> & writtenElements) const override;

private:
  bool m_isUrl{false};
  std::string m_fileName;
  std::string m_content;
  std::string m_urlContent;
};

class SCsConnector : public SCsElement
{
public:
  void ConvertFromSCgElement(SCgElementPtr const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<SCgElementPtr> & writtenElements) const override;
  static std::string GetIncidentElementIdentifier(SCgElementPtr const & element);

private:
  bool m_isUnsupported;
  std::string m_sourceIdentifier;
  std::string m_targetIdentifier;
};

class SCsContour : public SCsElement
{
public:
  void ConvertFromSCgElement(SCgElementPtr const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<SCgElementPtr> & writtenElements) const override;

private:
  SCgElements m_scgElements;
};

class SCsElementFactory
{
public:
  static SCsElementPtr CreateSCsElementForSCgElement(SCgElementPtr const & scgElement);
};
