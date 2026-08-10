/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scs_writer.hpp"

#include <regex>
#include <string>
#include <filesystem>
#include <fstream>

#include <sc-memory/sc_utils.hpp>

#include "sc_scg_element.hpp"
#include "sc_scs_element.hpp"
#include "sc_scg_to_scs_types_converter.hpp"

using namespace Constants;

std::string SCsWriter::MakeAlias(std::string const & prefix, std::string const & elementId)
{
  return ALIAS_PREFIX + prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(elementId, DASH, UNDERSCORE);
}

bool SCsWriter::IsVariable(std::string const & elementType)
{
  return elementType.find(VAR) != std::string::npos;
}

bool SCsWriter::SCgIdentifierCorrector::IsRussianIdentifier(std::string const & identifier)
{
  std::regex identifierPatternRus(R"(^[0-9a-zA-Z_\xD0\x80-\xD1\x8F\xD1\x90-\xD1\x8F\xD1\x91\xD0\x81*' ]*$)");
  return std::regex_match(identifier, identifierPatternRus);
}

bool SCsWriter::SCgIdentifierCorrector::IsEnglishIdentifier(std::string const & identifier)
{
  std::regex identifierPatternEng("^[0-9a-zA-Z_]*$");
  return std::regex_match(identifier, identifierPatternEng);
}

std::string SCsWriter::SCgIdentifierCorrector::GenerateCorrectedIdentifier(
    std::string & systemIdentifier,
    std::string const & elementId,
    bool isVar)
{
  if (systemIdentifier.empty())
    return GenerateIdentifierForUnresolvedCharacters(systemIdentifier, elementId, isVar);

  if (isVar && systemIdentifier[0] != UNDERSCORE[0])
    return GenerateSCsIdentifierForVariable(systemIdentifier);

  return systemIdentifier;
}

std::string SCsWriter::SCgIdentifierCorrector::GenerateIdentifierForUnresolvedCharacters(
    std::string & systemIdentifier,
    std::string const & elementId,
    bool isVar)
{
  std::string const & prefix = isVar ? EL_VAR_PREFIX : EL_PREFIX;
  systemIdentifier = prefix + UNDERSCORE + utils::StringUtils::ReplaceAll(elementId, DASH, UNDERSCORE);
  return systemIdentifier;
}

std::string SCsWriter::SCgIdentifierCorrector::GenerateSCsIdentifierForVariable(std::string & systemIdentifier)
{
  return UNDERSCORE + systemIdentifier;
}

void SCsWriter::SCgIdentifierCorrector::GenerateSCsIdentifier(
    SCgElementPtr const & scgElement,
    SCsElementPtr & scsElement)
{
  scsElement->SetIdentifierForSCs(scgElement->GetIdentifier());
  bool isVar = SCsWriter::IsVariable(scgElement->GetType());

  std::string const & systemIdentifier = scsElement->GetIdentifierForSCs();
  if (!IsEnglishIdentifier(systemIdentifier))
  {
    if (IsRussianIdentifier(systemIdentifier))
      scsElement->SetMainIdentifier(systemIdentifier);

    scsElement->SetIdentifierForSCs("");
  }

  std::string id = scgElement->GetId();
  std::string correctedIdentifier = scsElement->GetIdentifierForSCs();
  correctedIdentifier = GenerateCorrectedIdentifier(correctedIdentifier, id, isVar);
  scsElement->SetIdentifierForSCs(correctedIdentifier);

  auto const & tag = scgElement->GetTag();
  if (tag == PAIR || tag == ARC)
    scsElement->SetIdentifierForSCs(SCsWriter::MakeAlias(CONNECTOR, id));
}

// Вспомогательная функция для рекурсивного сбора узлов
void SCsWriter::CollectNodes(
    SCgElements const & elements,
    std::unordered_set<SCgElementPtr> & nodes,
    std::unordered_set<SCgElementPtr> & visitedContours)
{
  for (auto const & [id, element] : elements)
  {
    if (element->GetTag() == NODE)
    {
      nodes.insert(element);
    }
    else if (element->GetTag() == CONTOUR && !visitedContours.count(element))
    {
      visitedContours.insert(element);
      auto contour = std::dynamic_pointer_cast<SCgContour>(element);
      CollectNodes(contour->GetElements(), nodes, visitedContours);
    }
  }
}

void SCsWriter::Write(
    SCgElements const & elements,
    std::string const & filePath,
    Buffer & buffer,
    size_t depth,
    std::unordered_set<SCgElementPtr> & writtenElements)
{
  // Шаг 1: Сбор всех узлов, включая вложенные в контуры
  std::unordered_set<SCgElementPtr> allNodes;
  std::unordered_set<SCgElementPtr> visitedContours;
  CollectNodes(elements, allNodes, visitedContours);

  // Запись типов всех узлов
  for (auto const & node : allNodes)
  {
    if (writtenElements.count(node))
      continue;
    writtenElements.insert(node);
    std::string identifier = node->GetIdentifier();
    if (identifier.empty())
      identifier = NODE + UNDERSCORE + node->GetId();
    buffer.AddTabs(depth) << identifier << NEWLINE;

    // Запись типа узла
    std::string elementTypeStr;
    SCgToSCsTypesConverter::ConvertSCgNodeTypeToSCsNodeType(node->GetType(), elementTypeStr);
    if (elementTypeStr.empty())
      elementTypeStr = NODE + UNDERSCORE;
    buffer.AddTabs(depth + 1) << SC_CONNECTOR_MAIN_L << elementTypeStr << ELEMENT_END << NEWLINE;

    // Проверка, является ли узел SCgLink, и запись содержимого
    auto link = std::dynamic_pointer_cast<SCgLink>(node);
    if (link && link->GetContentType() != NO_CONTENT)
    {
      std::string contentTypeStr = link->GetContentType();
      int contentType = std::stoi(contentTypeStr);

      if (contentType == 1 || contentType == 2 || contentType == 3)  // STRING, INTEGER, FLOAT
      {
        std::string content = link->GetContentData();
        if (!content.empty())
        {
          if (contentType == 2)  // INTEGER
            content = DOUBLE_QUOTE + INT64 + content + DOUBLE_QUOTE;
          else if (contentType == 3)  // FLOAT
            content = DOUBLE_QUOTE + FLOAT + content + DOUBLE_QUOTE;
          buffer.AddTabs(depth + 1) << SPACE << EQUAL << SPACE << OPEN_BRACKET << content
                                    << CLOSE_BRACKET + ELEMENT_END + NEWLINE;
        }
      }
      else if (contentType == 4)  // IMAGE
      {
        std::string fileName = link->GetFileName();
        std::string content = link->GetContentData();
        if (!fileName.empty() && !content.empty())
        {
          // Формирование пути к файлу
          std::filesystem::path basePath = std::filesystem::path(filePath).parent_path();
          std::filesystem::path fullPath = basePath / fileName;

          // Сохранение содержимого в файл
          std::ofstream file(fullPath, std::ios::binary);
          if (!file)
            SC_THROW_EXCEPTION(
                utils::ExceptionInvalidParams,
                "SCsWriter::Write: Failed to open file for writing `" << fullPath.string() << "`.");

          file.write(content.data(), content.size());
          file.close();

          // Определение формата изображения
          std::string fileExtension = fullPath.extension().string();
          auto it = IMAGE_FORMATS.find(fileExtension);
          if (it == IMAGE_FORMATS.end())
            SC_THROW_EXCEPTION(
                utils::ExceptionItemNotFound,
                "SCsWriter::Write: File extension `" << fileExtension << "` is not supported.");

          std::string imageFormat = it->second;
          std::string fileContent = FILE_PREFIX + fullPath.filename().string();

          // Вывод SCs-кода для изображения
          buffer.AddTabs(depth + 1) << EQUAL + SPACE + DOUBLE_QUOTE << fileContent
                                    << DOUBLE_QUOTE + ELEMENT_END + NEWLINE;
          buffer.AddTabs(depth + 1) << FORMAT_ARC + SPACE + EQUAL + SPACE + OPEN_PARENTHESIS << identifier
                                    << SPACE + SC_CONNECTOR_DCOMMON_R + SPACE << imageFormat
                                    << CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE;
          buffer.AddTabs(depth + 1) << NREL_FORMAT_ARC << SPACE + EQUAL + SPACE
                                    << OPEN_PARENTHESIS + NREL_FORMAT + SPACE << SC_CONNECTOR_MAIN_R + SPACE
                                    << FORMAT_ARC + CLOSE_PARENTHESIS + ELEMENT_END + NEWLINE;
        }
      }
    }
    buffer << NEWLINE;
  }

  // Шаг 2: Предварительная обработка для дуг
  std::unordered_set<SCgElementPtr> complexArcs;
  std::unordered_set<SCgElementPtr> attributeArcs;
  for (auto const & [id, element] : elements)
  {
    if (element->GetTag() == ARC || element->GetTag() == PAIR)
    {
      auto connector = std::dynamic_pointer_cast<SCgConnector>(element);
      auto target = connector->GetTarget();
      if (target->GetTag() == ARC || target->GetTag() == PAIR)
      {
        complexArcs.insert(target);
        attributeArcs.insert(element);
      }
    }
  }

  // Шаг 3: Запись дуг и пар
  for (auto const & [id, element] : elements)
  {
    if (element->GetTag() == ARC || element->GetTag() == PAIR)
    {
      if (writtenElements.count(element))
        continue;
      if (attributeArcs.count(element))
        continue;
      writtenElements.insert(element);

      auto connector = std::dynamic_pointer_cast<SCgConnector>(element);
      std::string sourceId = connector->GetSource()->GetIdentifier();
      std::string targetId = connector->GetTarget()->GetIdentifier();
      if (sourceId.empty())
        sourceId = NODE + UNDERSCORE + connector->GetSource()->GetId();
      if (targetId.empty())
        targetId = NODE + UNDERSCORE + connector->GetTarget()->GetId();

      if (complexArcs.count(element))
      {
        std::string attrSourceId;
        for (auto const & [incomingId, incomingElement] : elements)
        {
          if (incomingElement->GetTag() == ARC || incomingElement->GetTag() == PAIR)
          {
            auto incConnector = std::dynamic_pointer_cast<SCgConnector>(incomingElement);
            if (incConnector->GetTarget() == element)
            {
              attrSourceId = incConnector->GetSource()->GetIdentifier();
              if (attrSourceId.empty())
                attrSourceId = NODE + UNDERSCORE + incConnector->GetSource()->GetId();
              break;
            }
          }
        }

        if (!attrSourceId.empty())
        {
          std::string connectorSymbol;
          SCgToSCsTypesConverter::ConvertSCgConnectorTypeToSCsConnectorDesignation(
              connector->GetType(), connectorSymbol);
          if (connectorSymbol.empty())
            connectorSymbol = SC_CONNECTOR_MAIN_R;
          buffer.AddTabs(depth) << sourceId << SPACE << connectorSymbol << SPACE << attrSourceId << COLON << SPACE
                                << targetId << ELEMENT_END << NEWLINE << NEWLINE;
        }
        else
        {
          std::string connectorSymbol;
          SCgToSCsTypesConverter::ConvertSCgConnectorTypeToSCsConnectorDesignation(
              connector->GetType(), connectorSymbol);
          if (connectorSymbol.empty())
            connectorSymbol = SC_CONNECTOR_MAIN_R;
          buffer.AddTabs(depth) << sourceId << SPACE << connectorSymbol << SPACE << targetId << ELEMENT_END << NEWLINE
                                << NEWLINE;
        }
      }
      else
      {
        std::string connectorSymbol;
        SCgToSCsTypesConverter::ConvertSCgConnectorTypeToSCsConnectorDesignation(connector->GetType(), connectorSymbol);
        if (connectorSymbol.empty())
          connectorSymbol = SC_CONNECTOR_MAIN_R;
        buffer.AddTabs(depth) << sourceId << SPACE << connectorSymbol << SPACE << targetId << ELEMENT_END << NEWLINE
                              << NEWLINE;
      }
    }
  }

  // Шаг 4: Запись контуров
  for (auto const & [id, element] : elements)
  {
    if (element->GetTag() == CONTOUR)
    {
      if (writtenElements.count(element))
        continue;
      writtenElements.insert(element);
      auto contour = std::dynamic_pointer_cast<SCgContour>(element);
      std::string identifier = element->GetIdentifier();
      if (identifier.empty())
        identifier = CONTOUR + UNDERSCORE + element->GetId();
      buffer.AddTabs(depth) << identifier << SPACE + EQUAL << SPACE << OPEN_CONTOUR + NEWLINE;

      // Сбор узлов, принадлежащих текущему контуру
      std::unordered_set<SCgElementPtr> contourNodes;
      std::unordered_set<SCgElementPtr> contourVisitedContours;
      CollectNodes(contour->GetElements(), contourNodes, contourVisitedContours);

      // Сбор узлов, участвующих в дугах внутри контура
      std::unordered_set<SCgElementPtr> nodesInArcs;
      for (auto const & [elemId, elem] : contour->GetElements())
      {
        if (elem->GetTag() == ARC || elem->GetTag() == PAIR)
        {
          auto connector = std::dynamic_pointer_cast<SCgConnector>(elem);
          if (connector->GetSource()->GetTag() == NODE)
            nodesInArcs.insert(connector->GetSource());
          if (connector->GetTarget()->GetTag() == NODE)
            nodesInArcs.insert(connector->GetTarget());
        }
      }

      // Запись идентификаторов узлов, не участвующих в дугах
      for (auto const & node : contourNodes)
      {
        if (nodesInArcs.count(node))
          continue;  // Пропускаем узлы, которые будут в дугах
        std::string nodeIdentifier = node->GetIdentifier();
        if (nodeIdentifier.empty())
          nodeIdentifier = NODE + UNDERSCORE + node->GetId();
        buffer.AddTabs(depth + 1) << nodeIdentifier << ELEMENT_END << NEWLINE;
      }

      // Рекурсивная запись элементов контура (дуги, вложенные контуры)
      Write(contour->GetElements(), filePath, buffer, depth + 1, writtenElements);

      buffer.AddTabs(depth) << CONTOUR_END << NEWLINE << NEWLINE;
    }
  }
}

void SCsWriter::WriteMainIdentifier(
    Buffer & buffer,
    size_t depth,
    std::string const & systemIdentifier,
    std::string const & mainIdentifier)
{
  buffer << NEWLINE;
  buffer.AddTabs(depth) << systemIdentifier << NEWLINE;
  buffer.AddTabs(depth) << SPACE << SC_CONNECTOR_DCOMMON_R << SPACE << NREL_MAIN_IDTF << COLON << SPACE << OPEN_BRACKET
                        << mainIdentifier << CLOSE_BRACKET << ELEMENT_END << NEWLINE;
}