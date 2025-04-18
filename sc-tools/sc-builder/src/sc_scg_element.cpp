/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scg_element.hpp"

// SCgElement
SCgElement::SCgElement(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag)
  : m_id(id)
  , m_parent(parent)
  , m_identifier(identifier)
  , m_type(type)
  , m_tag(tag)
{
}

std::string const & SCgElement::GetId() const
{
  return m_id;
}

std::string const & SCgElement::GetIdentifier() const
{
  return m_identifier;
}

std::string const & SCgElement::GetType() const
{
  return m_type;
}

std::string const & SCgElement::GetTag() const
{
  return m_tag;
}

// SCgNode
SCgNode::SCgNode(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag)
  : SCgElement(id, parent, identifier, type, tag)
{
}

// SCgLink
SCgLink::SCgLink(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag,
    std::string const & contentType,
    std::string const & mimeType,
    std::string const & fileName,
    std::string const & contentData)
  : SCgNode(id, parent, identifier, type, tag)
  , m_contentType(contentType)
  , m_mimeType(mimeType)
  , m_fileName(fileName)
  , m_contentData(contentData)
{
}

std::string const & SCgLink::GetContentType() const
{
  return m_contentType;
}

std::string const & SCgLink::GetFileName() const
{
  return m_fileName;
}

std::string const & SCgLink::GetContentData() const
{
  return m_contentData;
}

// SCgBus
SCgBus::SCgBus(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag,
    std::string const & nodeId)
  : SCgNode(id, parent, identifier, type, tag)
  , m_nodeId(nodeId)
{
}

std::string const & SCgBus::GetNodeId() const
{
  return m_nodeId;
}

// SCgContour
SCgContour::SCgContour(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag)
  : SCgNode(id, parent, identifier, type, tag)
{
}

void SCgContour::SetElements(SCgElements const & elements)
{
  m_elements = elements;
}

SCgElements & SCgContour::GetElements()
{
  return m_elements;
}

// SCgConnector
SCgConnector::SCgConnector(
    std::string const & id,
    std::string const & parent,
    std::string const & identifier,
    std::string const & type,
    std::string const & tag,
    SCgElementPtr source,
    SCgElementPtr target)
  : SCgElement(id, parent, identifier, type, tag)
  , m_source(source)
  , m_target(target)
{
}

SCgElementPtr SCgConnector::GetSource() const
{
  return m_source;
}

SCgElementPtr SCgConnector::GetTarget() const
{
  return m_target;
}

void SCgConnector::SetSource(SCgElementPtr const & sourceEl)
{
  m_source = sourceEl;
}

void SCgConnector::SetTarget(SCgElementPtr const & targetEl)
{
  m_target = targetEl;
}
