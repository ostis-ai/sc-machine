/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include "gwf_translator_const.hpp"

class SCgElement;
class Buffer;

class SCsElement
{
public:
  virtual void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) = 0;
  virtual void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const = 0;
  virtual ~SCsElement() = default;

  void SetSystemIdentifier(std::string const & identifier);
  void SetMainIdentifier(std::string const & identifier);
  std::string GetSystemIdentifier() const;
  std::string GetMainIdentifier() const;

protected:
  std::string m_mainIdentifier;
  std::string m_systemIdentifier;
};

class SCsNode : public SCsElement
{
public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const override;

private:
  std::string type;
};

class SCsLink : public SCsElement
{
public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const override;

private:
  bool m_isUrl{false};
  std::string m_type;
  std::string m_fileName;
  std::string m_content;
  std::string m_urlContent;
};

class SCsConnector : public SCsElement
{
public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const override;
  std::string GetIncidentElementIdentifier(std::shared_ptr<SCgElement> const & element) const;

private:
  std::string m_type;
  bool m_isUnsupported;
  std::string m_alias;
  std::string m_sourceIdentifier;
  std::string m_targetIdentifier;

  static inline size_t multiple_arc_counter = 0;
};

class SCsContour : public SCsElement
{
public:
  void ConvertFromSCgElement(std::shared_ptr<SCgElement> const & element) override;
  void Dump(
      std::string const & filePath,
      Buffer & buffer,
      size_t depth,
      std::unordered_set<std::shared_ptr<SCgElement>> & writtenElements) const override;

private:
  SCgElements m_scgElements;
};
