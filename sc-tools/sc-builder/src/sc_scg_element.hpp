/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <list>

class SCgElement
{
public:
  SCgElement(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag);

  virtual ~SCgElement() = default;

  std::string const & GetId() const;
  std::string const & GetParent() const;
  std::string const & GetIdentifier() const;
  std::string const & GetType() const;
  std::string const & GetTag() const;

  void SetIdentifier(std::string const & newIdentifier);

private:
  std::string m_id;
  std::string m_parent;
  std::string m_identifier;
  std::string m_type;
  std::string m_tag;
};

class SCgNode : public SCgElement
{
public:
  SCgNode(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag);
};

class SCgLink : public SCgNode
{
public:
  SCgLink(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag,
      std::string const & contentType,
      std::string const & mimeType,
      std::string const & fileName,
      std::string const & contentData);

  std::string const & GetContentType() const;
  std::string const & GetMimeType() const;
  std::string const & GetFileName() const;
  std::string const & GetContentData() const;

private:
  std::string m_contentType;
  std::string m_mimeType;
  std::string m_fileName;
  std::string m_contentData;
};

class SCgBus : public SCgNode
{
public:
  SCgBus(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag,
      std::string const & nodeId);

  std::string const & GetNodeId() const;

private:
  std::string m_nodeId;
};

class SCgContour : public SCgNode
{
public:
  SCgContour(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag);

  void AddElement(std::shared_ptr<SCgElement> const & element);
  std::list<std::shared_ptr<SCgElement>> & GetElements();

private:
  std::list<std::shared_ptr<SCgElement>> m_elements;
};

class SCgConnector : public SCgElement
{
public:
  SCgConnector(
      std::string const & id,
      std::string const & parent,
      std::string const & identifier,
      std::string const & type,
      std::string const & tag,
      std::shared_ptr<SCgElement> sourceEl,
      std::shared_ptr<SCgElement> targetEl);

  std::shared_ptr<SCgElement> GetSource() const;
  std::shared_ptr<SCgElement> GetTarget() const;

  void SetSource(std::shared_ptr<SCgElement> const & sourceEl);
  void SetTarget(std::shared_ptr<SCgElement> const & targetEl);

private:
  std::shared_ptr<SCgElement> m_source;
  std::shared_ptr<SCgElement> m_target;
};
