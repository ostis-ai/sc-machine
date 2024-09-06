/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

/*!
 * @file sc_memory.hpp
 *
 * @brief This file contains the C++ API for managing the sc-memory.
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include "sc_iterator.hpp"
#include "sc_template.hpp"

#include "sc_stream.hpp"

class ScMemoryContext;

typedef struct
{
  ScAddr addr1;
  ScAddr addr2;
  ScAddr addr3;
  ScAddr addr4;
  ScAddr addr5;
} ScSystemIdentifierQuintuple;

class ScMemory
{
  friend class ScMemoryContext;

public:
  static std::string ms_configPath;

  /*!
   * @brief Initializes the sc-memory system with the specified parameters.
   *
   * This function initializes the sc-memory system with the provided parameters.
   * It sets up the global sc-memory context, initializes keynodes, and sets up logging.
   *
   * @param params The parameters for initializing the sc-memory system.
   * @return Returns true if the initialization is successful; otherwise, returns false.
   */
  _SC_EXTERN static bool Initialize(sc_memory_params const & params);

  /*!
   * @brief Checks if the sc-memory system is initialized.
   *
   * This function checks whether the sc-memory system is initialized.
   * It returns true if the system is initialized; otherwise, it returns false.
   *
   * @return Returns true if the sc-memory system is initialized; otherwise, returns false.
   */
  _SC_EXTERN static bool IsInitialized();

  /*!
   * @brief Shuts down the sc-memory system.
   *
   * This function shuts down the sc-memory system, performing necessary cleanup operations.
   * It optionally saves the state of the system before shutting down.
   *
   * @param saveState If true, the current state of the sc-memory system is saved before shutting down.
   * @return Returns true if the shutdown is successful; otherwise, returns false.
   */
  _SC_EXTERN static bool Shutdown(bool saveState = true);

  _SC_EXTERN static void LogMute();
  _SC_EXTERN static void LogUnmute();

  static ScMemoryContext * ms_globalContext;
};

//! Class used to work with memory. It provides functions to create/retrieve/erase sc-elements
class ScMemoryContext
{
  friend class ScMemory;
  friend class ScServerMessageAction;
  friend class ScAction;
  friend class ScTemplateKeynode;

public:
  struct ScMemoryStatistics
  {
    sc_uint64 m_nodesNum;
    sc_uint64 m_linksNum;
    sc_uint64 m_edgesNum;

    sc_uint64 GetAllNum() const
    {
      return m_nodesNum + m_linksNum + m_edgesNum;
    }
  };

public:
  _SC_EXTERN explicit ScMemoryContext() noexcept;
  _SC_EXTERN explicit ScMemoryContext(sc_memory_context * context) noexcept;
  _SC_EXTERN ~ScMemoryContext() noexcept;

  _SC_EXTERN ScMemoryContext(ScMemoryContext const & other) = delete;
  _SC_EXTERN ScMemoryContext & operator=(ScMemoryContext const & other) = delete;

  _SC_EXTERN ScMemoryContext(ScMemoryContext && other) noexcept;
  _SC_EXTERN ScMemoryContext & operator=(ScMemoryContext && other) noexcept;

  _SC_EXTERN ScAddr GetUser();
  _SC_EXTERN ScAddr GetContextStructure();

  _SC_EXTERN sc_memory_context * operator*() const;

  _SC_EXTERN sc_memory_context * GetRealContext() const;

  //! Call this function, when you request to destroy real memory context, before destructor calls for this object
  _SC_EXTERN void Destroy() noexcept;

  //! Begin events pending mode
  _SC_EXTERN void BeingEventsPending();

  //! End events pending mode
  _SC_EXTERN void EndEventsPending();

  //! Begin events blocking mode
  _SC_EXTERN void BeingEventsBlocking();

  //! End events blocking mode
  _SC_EXTERN void EndEventsBlocking();

  /*!
   * @brief Checks if the sc-memory context is valid.
   *
   * This method checks whether the sc-memory context is valid.
   *
   * @return Returns true if the sc-memory context is valid; otherwise, returns false.
   *
   * @code
   * ScMemoryContext context;
   * if (context.IsValid())
   * {
   *   // Perform operations using the valid context.
   * }
   * else
   * {
   *   // Handle invalid context.
   * }
   * @endcode
   */
  _SC_EXTERN bool IsValid() const;

  /*!
   * @brief Checks if an sc-element exists with the specified address.
   *
   * This method checks whether an sc-element with the specified sc-address exists in the sc-memory.
   *
   * @param addr A sc-address to check for existence.
   * @return Returns true if the sc-element exists; otherwise, returns false.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * if (context.IsElement(elementAddr))
   * {
   *   // Element exists.
   * }
   * else
   * {
   *   // Element does not exist.
   * }
   * @endcode
   */
  _SC_EXTERN bool IsElement(ScAddr const & elementAddr) const;

  /*!
   * @brief Returns the count of outgoing sc-arcs for a specified sc-element.
   *
   * This method retrieves the count of outgoing sc-arcs for the sc-element identified by the given sc-address.
   *
   * @param elementAddr A sc-address of the sc-element to query.
   * @return Returns the count of outgoing sc-arcs for the specified sc-element.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * size_t outgoingArcsCount = context.GetElementOutgoingArcsCount(elementAddr);
   * std::cout << "Outgoing arcs count: " << outgoingArcsCount << std::endl;
   * @endcode
   */
  _SC_EXTERN size_t GetElementOutgoingArcsCount(ScAddr const & elementAddr) const noexcept(false);

  /*!
   * @brief Returns the count of outgoing sc-arcs for a specified sc-element.
   *
   * This method retrieves the count of outgoing sc-arcs for the sc-element identified by the given sc-address.
   *
   * @param elementAddr A sc-address of the sc-element to query.
   * @return Returns the count of outgoing sc-arcs for the specified sc-element.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * size_t outgoingArcsCount = context.GetElementOutputArcsCount(elementAddr);
   * std::cout << "Outgoing arcs count: " << outgoingArcsCount << std::endl;
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `GetElementOutgoingArcsCount` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `GetElementOutgoingArcsCount` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN size_t GetElementOutputArcsCount(ScAddr const & elementAddr) const noexcept(false);

  /*!
   * @brief Returns the count of incoming sc-arcs for a specified sc-element.
   *
   * This method retrieves the count of incoming sc-arcs for the sc-element identified by the given sc-address.
   *
   * @param elementAddr A sc-address of the sc-element to query.
   * @return Returns the count of incoming sc-arcs for the specified sc-element.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * size_t incomingArcsCount = context.GetElementIncomingArcsCount(elementAddr);
   * std::cout << "Incoming arcs count: " << incomingArcsCount << std::endl;
   * @endcode
   */
  _SC_EXTERN size_t GetElementIncomingArcsCount(ScAddr const & elementAddr) const noexcept(false);

  /*!
   * @brief Returns the count of incoming sc-arcs for a specified sc-element.
   *
   * This method retrieves the count of incoming sc-arcs for the sc-element identified by the given sc-address.
   *
   * @param elementAddr A sc-address of the sc-element to query.
   * @return Returns the count of incoming sc-arcs for the specified sc-element.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * size_t incomingArcsCount = context.GetElementInputArcsCount(elementAddr);
   * std::cout << "Incoming arcs count: " << incomingArcsCount << std::endl;
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `GetElementIncomingArcsCount` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `GetElementIncomingArcsCount` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN size_t GetElementInputArcsCount(ScAddr const & elementAddr) const noexcept(false);

  /*!
   * @brief Erases an sc-element from the sc-memory.
   *
   * This method erases the sc-element identified by the given sc-address from the sc-memory.
   *
   * @param elementAddr A sc-address of the sc-element to erase.
   * @return Returns true if the sc-element was successfully erased; otherwise, returns false.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have erase permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * if (context.EraseElement(elementAddr))
   * {
   *   // Element successfully erased.
   * }
   * else
   * {
   *   // Erase operation failed.
   * }
   * @endcode
   */
  _SC_EXTERN bool EraseElement(ScAddr const & elementAddr) noexcept(false);

  /*!
   * @brief Generates a new sc-node with the specified type.
   *
   * This method creates a new sc-node with the specified type and returns its sc-address.
   *
   * @param nodeType A sc-type of the sc-node to create.
   * @return Returns the sc-address of the newly created sc-node.
   * @throws ExceptionInvalidParams if the specified type is not a valid sc-node type or if sc-memory is full.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr nodeAddr = context.GenerateNode(ScType::NodeConst);
   * @endcode
   */
  _SC_EXTERN ScAddr GenerateNode(ScType const & nodeType) noexcept(false);

  /*!
   * @brief Generates a new sc-node with the specified type.
   *
   * This method creates a new sc-node with the specified type and returns its sc-address.
   *
   * @param nodeType A sc-type of the sc-node to create.
   * @return Returns the sc-address of the newly created sc-node.
   * @throws ExceptionInvalidParams if the specified type is not a valid sc-node type or if sc-memory is full.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr nodeAddr = context.CreateNode(ScType::NodeConst);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `GenerateNode` instead for better readability and standards
   * compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `GenerateNode` instead for better readability and standards compliance.’")
  _SC_EXTERN ScAddr CreateNode(ScType const & nodeType) noexcept(false);

  /*!
   * @brief Generates a new sc-link with the specified type.
   *
   * This method creates a new sc-link with the specified type and returns its sc-address.
   *
   * @param linkType A sc-type of the sc-link to create (default is ScType::LinkConst).
   * @return Returns the sc-address of the newly created sc-link.
   * @throws ExceptionInvalidParams if the specified type is not a valid sc-link type or if sc-memory is full.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr linkAddr = context.GenerateLink();
   * @endcode
   */
  _SC_EXTERN ScAddr GenerateLink(ScType const & linkType = ScType::LinkConst) noexcept(false);

  /*!
   * @brief Generates a new sc-link with the specified type.
   *
   * This method creates a new sc-link with the specified type and returns its sc-address.
   *
   * @param linkType A sc-type of the sc-link to create (default is ScType::LinkConst).
   * @return Returns the sc-address of the newly created sc-link.
   * @throws ExceptionInvalidParams if the specified type is not a valid sc-link type or if sc-memory is full.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr linkAddr = context.CreateLink();
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `GenerateLink` instead for better readability and standards
   * compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `GenerateLink` instead for better readability and standards compliance.’")
  _SC_EXTERN ScAddr CreateLink(ScType const & linkType = ScType::LinkConst) noexcept(false);

  /*!
   * @brief Generates a new sc-connector with the specified type, source, and target.
   *
   * This method creates a new sc-connector with the specified type, source, and target, and returns its sc-address.
   *
   * @param connectorType A sc-type of the sc-connector to create.
   * @param sourceElementAddr A sc-address of the source sc-element.
   * @param targetElementAddr A sc-address of the target sc-element.
   * @return Returns the sc-address of the newly created sc-connector.
   * @throws ExceptionInvalidParams if the specified source or target sc-address is invalid or if sc-memory is full.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr sourceNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr targetNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcAddr = context.GenerateConnector(ScType::EdgeDCommonConst, sourceNodeAddr, targetNodeAddr);
   * @endcode
   */
  _SC_EXTERN ScAddr GenerateConnector(
      ScType const & connectorType,
      ScAddr const & sourceElementAddr,
      ScAddr const & targetElementAddr) noexcept(false);

  /*!
   * @brief Generates a new sc-connector with the specified type, source, and target.
   *
   * This method creates a new sc-connector with the specified type, source, and target, and returns its sc-address.
   *
   * @param type A sc-type of the sc-connector to create.
   * @param addrBeg A sc-address of the source sc-element.
   * @param addrEnd A sc-address of the target sc-element.
   * @return Returns the sc-address of the newly created sc-connector.
   * @throws ExceptionInvalidParams if the specified source or target sc-address is invalid or if sc-memory is full.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr sourceNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr targetNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcAddr = context.CreateEdge(ScType::EdgeDCommonConst, sourceNode, targetNode);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `GenerateConnector` instead for better readability and
   * standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `GenerateConnector` instead for better readability and standards compliance.’")
  _SC_EXTERN ScAddr CreateEdge(
      ScType const & connectorType,
      ScAddr const & sourceElementAddr,
      ScAddr const & targetElementAddr) noexcept(false);

  /*!
   * @brief Returns the type of the specified sc-element.
   *
   * This method retrieves the type of the sc-element identified by the given sc-address.
   *
   * @param addr A sc-address of the sc-element to query.
   * @return Returns the type of the specified sc-element.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * ScType elementType = context.GetElementType(elementAddr);
   * std::cout << "Element Type: " << elementType.GetName() << std::endl;
   * @endcode
   */
  _SC_EXTERN ScType GetElementType(ScAddr const & elementAddr) const noexcept(false);

  /*!
   * @brief Changes the subtype of an sc-element.
   *
   * This method changes the subtype of the sc-element identified by the given sc-address.
   *
   * @param elementAddr A sc-address of the sc-element to modify.
   * @param newSubtype A new subtype to set.
   * @return Returns true if the subtype was successfully changed; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * if (context.SetElementSubtype(elementAddr, ScType::NodeConstRole))
   * {
   *   // Subtype successfully changed.
   * }
   * else
   * {
   *   // Subtype change failed.
   * }
   * @endcode
   */
  _SC_EXTERN bool SetElementSubtype(ScAddr const & elementAddr, ScType newSubtype) noexcept(false);

  /*!
   * @brief Returns the source sc-element of a sc-connector.
   *
   * This method retrieves the source sc-element of the sc-connector identified by the given sc-address.
   *
   * @param arcAddr A sc-address of the sc-connector.
   * @return Returns the sc-address of the source sc-element.
   * @throws ExceptionInvalidParams if the specified sc-connector address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr sourceNode = context.GenerateNode(ScType::NodeConst);
   * ScAddr targetNode = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcAddr = context.GenerateConnector(ScType::EdgeDCommonConst, sourceNode, targetNode);
   * ScAddr sourceElement = context.GetEdgeSource(arcAddr);
   * @endcode
   */
  _SC_EXTERN ScAddr GetEdgeSource(ScAddr const & arcAddr) const noexcept(false);

  /*!
   * @brief Returns the target sc-element of a sc-connector.
   *
   * This method retrieves the target sc-element of the sc-connector identified by the given sc-address.
   *
   * @param arcAddr A sc-address of the sc-connector.
   * @return Returns the sc-address of the target sc-element.
   * @throws ExceptionInvalidParams if the specified sc-connector address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr sourceNode = context.GenerateNode(ScType::NodeConst);
   * ScAddr targetNode = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcAddr = context.GenerateConnector(ScType::EdgeDCommonConst, sourceNode, targetNode);
   * ScAddr targetElement = context.GetEdgeTarget(arcAddr);
   * @endcode
   */
  _SC_EXTERN ScAddr GetEdgeTarget(ScAddr const & arcAddr) const noexcept(false);

  /*!
   * @brief Returns the source and target sc-elements of a sc-connector.
   *
   * This method retrieves the source and target sc-elements of the sc-connector identified by the given sc-address.
   *
   * @param connectorAddr A sc-address of the sc-connector.
   * @return Returns a tuple of sc-connector incident sc-elements sc-addresses.
   * @throws ExceptionInvalidParams if the specified sc-connector address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr sourceNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr targetNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcAddr = context.GenerateConnector(ScType::EdgeDCommonConst, sourceNodeAddr, targetNodeAddr);
   * auto const [firstIncidentElementAddr, secondIncidentElementAddr] = context.GetConnectorIncidentElements(arcAddr);
   * @endcode
   */
  _SC_EXTERN std::tuple<ScAddr, ScAddr> GetConnectorIncidentElements(ScAddr const & connectorAddr) const
      noexcept(false);

  /*!
   * @brief Returns the source and target sc-elements of a sc-connector.
   *
   * This method retrieves the source and target sc-elements of the sc-connector identified by the given sc-address.
   *
   * @param connectorAddr A sc-address of the sc-connector.
   * @param outFirstIncidentElementAddr Reference to store the sc-address of the source sc-element.
   * @param outSecondIncidentElementAddr Reference to store the sc-address of the target sc-element.
   * @return Returns true if the information was successfully retrieved; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified sc-connector address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr sourceNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr targetNodeAddr = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcAddr = context.GenerateConnector(ScType::EdgeDCommonConst, sourceNodeAddr, targetNodeAddr);
   * ScAddr firstIncidentElementAddr, secondIncidentElementAddr;
   * if (context.GetEdgeInfo(arcAddr, firstIncidentElementAddr, secondIncidentElementAddr))
   * {
   *   // Information retrieved successfully.
   * }
   * else
   * {
   *   // Information retrieval failed.
   * }
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `GetConnectorIncidentElements` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `GetConnectorIncidentElements` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN bool GetEdgeInfo(
      ScAddr const & connectorAddr,
      ScAddr & outFirstIncidentElementAddr,
      ScAddr & outSecondIncidentElementAddr) const noexcept(false);

  /*!
   * @brief Sets the content of an sc-link with a stream.
   *
   * This method sets the content of an sc-link identified by the given sc-address using the provided stream.
   *
   * @param linkAddr A sc-address of the sc-link.
   * @param contentStream The stream containing the content.
   * @param isSearchableString Flag indicating whether the content is searchable as a string (default is true).
   * @return Returns true if the content was successfully set; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified sc-address or stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have erase and write
   * permissions.
   */
  _SC_EXTERN bool SetLinkContent(
      ScAddr const & linkAddr,
      ScStreamPtr const & contentStream,
      bool isSearchableString = true) noexcept(false);

  /*!
   * @brief Sets the content of an sc-link with a typed string.
   *
   * This method sets the content of an sc-link identified by the given sc-address using the provided typed string.
   *
   * @param linkAddr A sc-address of the sc-link.
   * @param linkContent The content of the sc-link.
   * @param isSearchableLinkContent Flag indicating whether the content is searchable as a string (default is true).
   * @return Returns true if the content was successfully set; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified sc-address or stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have erase and write
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr linkAddr = context.GenerateLink(ScType::LinkConst);
   * if (context.SetLinkContent(linkAddr, "my node"))
   * {
   *   // Content set successfully.
   * }
   * else
   * {
   *   // Content set failed.
   * }
   * @endcode
   *
   * @code
   * ScMemoryContext context;
   * ScAddr linkAddr = context.GenerateLink(ScType::LinkConst);
   * if (context.SetLinkContent(linkAddr, 105))
   * {
   *   // Content set successfully.
   * }
   * else
   * {
   *   // Content set failed.
   * }
   * @endcode
   */
  template <typename TContentType>
  _SC_EXTERN bool SetLinkContent(
      ScAddr const & linkAddr,
      TContentType const & linkContent,
      bool isSearchableLinkContent = true) noexcept(false)
  {
    return SetLinkContent(linkAddr, ScStreamMakeRead(linkContent), isSearchableLinkContent);
  }

  /*!
   * @brief Gets the content of an sc-link as a string.
   *
   * This method retrieves the content of an sc-link identified by the given sc-address as a string.
   *
   * @param addr A sc-address of the sc-link.
   * @param outLinkContent[out] A sc-typed content retrieved from the sc-link.
   * @return Returns true if the content was successfully retrieved; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr linkAddr = context.GenerateLink(ScType::LinkConst);
   * std::string linkContent;
   * if (context.GetLinkContent(linkAddr, linkContent))
   * {
   *   // Content get successfully.
   * }
   * else
   * {
   *   // Content get failed.
   * }
   * @endcode
   */
  _SC_EXTERN bool GetLinkContent(ScAddr const & linkAddr, std::string & outLinkContent) noexcept(false)
  {
    ScStreamPtr const & linkContentStream = GetLinkContent(linkAddr);
    return linkContentStream != nullptr && linkContentStream->IsValid()
           && ScStreamConverter::StreamToString(linkContentStream, outLinkContent);
  }

  /*!
   * @brief Gets the content of an sc-link as a stream.
   *
   * This method retrieves the content of an sc-link identified by the given sc-address as a stream.
   *
   * @param linkAddr A sc-address of the sc-link.
   * @return Returns a shared pointer to the stream containing the content.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   */
  _SC_EXTERN ScStreamPtr GetLinkContent(ScAddr const & linkAddr) noexcept(false);

  /*!
   * @brief Gets the content of an sc-link as a typed string.
   *
   * This method retrieves the content of an sc-link identified by the given sc-address as a typed string.
   *
   * @param linkAddr A sc-address of the sc-link.
   * @param outLinkContent A sc-link content to get.
   * @return Returns true if sc-link has content; otherwise, it returns false.
   * @throws ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr linkAddr = context.GenerateLink(ScType::LinkConst);
   * std::string linkContent;
   * if (context.GetLinkContent(linkAddr, linkContent))
   * {
   *   // Content get successfully.
   * }
   * else
   * {
   *   // Content get failed.
   * }
   * @endcode
   *
   * @code
   * ScMemoryContext context;
   * ScAddr linkAddr = context.GenerateLink(ScType::LinkConst);
   * size_t linkContent;
   * if (context.GetLinkContent(linkAddr, linkContent))
   * {
   *   // Content get successfully.
   * }
   * else
   * {
   *   // Content get failed.
   * }
   * @endcode
   */
  template <typename TContentType>
  _SC_EXTERN bool GetLinkContent(ScAddr const & linkAddr, TContentType & outLinkContent) noexcept(false)
  {
    std::string linkContent;
    ScStreamPtr const & linkContentStream = GetLinkContent(linkAddr);
    if (linkContentStream != nullptr && linkContentStream->IsValid()
        && ScStreamConverter::StreamToString(linkContentStream, linkContent))
    {
      std::istringstream streamString{linkContent};
      streamString >> outLinkContent;

      return true;
    }

    return true;
  }

  /*!
   * @brief Searches sc-links by content using a stream.
   *
   * This method finds sc-links by matching the content with the provided stream.
   *
   * @param stream The stream to use for content matching.
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   */
  _SC_EXTERN ScAddrVector SearchLinksByContent(ScStreamPtr const & linkContentStream) noexcept(false);

  /*!
   * @brief Searches sc-links by content using a stream.
   *
   * This method finds sc-links by matching the content with the provided stream.
   *
   * @param stream The stream to use for content matching.
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchLinksByContent` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchLinksByContent` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN ScAddrVector FindLinksByContent(ScStreamPtr const & linkContentStream) noexcept(false);

  /*!
   * @brief Searches sc-links by content using a typed string.
   *
   * This method finds sc-links by matching the content with the provided typed string.
   *
   * @param stream The stream to use for content matching.
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddrVector const & linkAddrsVector = context.SearchLinksByContent("my node");
   * for (auto const & linkAddr : linkAddrsVector)
   * {
   *   // Process sc-links.
   * }
   * @endcode
   */
  template <typename TContentType>
  _SC_EXTERN ScAddrVector SearchLinksByContent(TContentType const & linkContent) noexcept(false)
  {
    return SearchLinksByContent(ScStreamMakeRead(linkContent));
  }

  /*!
   * @brief Searches sc-links by content using a typed string.
   *
   * This method finds sc-links by matching the content with the provided typed string.
   *
   * @param stream The stream to use for content matching.
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddrVector const & linkAddrsVector = context.FindLinksByContent("my node");
   * for (auto const & linkAddr : linkAddrsVector)
   * {
   *   // Process sc-links.
   * }
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchLinksByContent` instead for better readability
   * and standards compliance.
   */
  template <typename TContentType>
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchLinksByContent` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN ScAddrVector FindLinksByContent(TContentType const & linkContent) noexcept(false)
  {
    return SearchLinksByContent(linkContent);
  }

  /*!
   * @brief Searches sc-links by content substring using a stream.
   *
   * This method finds sc-links by matching the content substring with the provided stream.
   *
   * @param linkContentSubstring The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddrVector const & linkAddrsVector = context.SearchLinksByContentSubstring("my");
   * for (auto const & linkAddr : linkAddrsVector)
   * {
   *   // Process sc-links.
   * }
   * @endcode
   */
  template <typename TContentType>
  _SC_EXTERN ScAddrVector SearchLinksByContentSubstring(
      TContentType const & linkContentSubstring,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false)
  {
    return SearchLinksByContentSubstring(ScStreamMakeRead(linkContentSubstring), maxLengthToSearchAsPrefix);
  }

  /*!
   * @brief Searches sc-links by content substring using a stream.
   *
   * This method finds sc-links by matching the content substring with the provided stream.
   *
   * @param linkContentSubstring The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddrVector const & linkAddrsVector = context.FindLinksByContentSubstring("my");
   * for (auto const & linkAddr : linkAddrsVector)
   * {
   *   // Process sc-links.
   * }
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchLinksByContentSubstring` instead for better readability
   * and standards compliance.
   */
  template <typename TContentType>
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchLinksByContentSubstring` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN ScAddrVector FindLinksByContentSubstring(
      TContentType const & linkContentSubstring,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false)
  {
    return SearchLinksByContentSubstring(linkContentSubstring, maxLengthToSearchAsPrefix);
  }

  /*!
   * @brief Searches sc-links by content substring using a typed string.
   *
   * This method finds sc-links by matching the content substring with the provided typed string.
   *
   * @param linkContentSubstringStream The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   */
  _SC_EXTERN ScAddrVector SearchLinksByContentSubstring(
      ScStreamPtr const & linkContentSubstringStream,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false);

  /*!
   * @brief Searches sc-links by content substring using a typed string.
   *
   * This method finds sc-links by matching the content substring with the provided typed string.
   *
   * @param linkContentSubstringStream The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of sc-addresses representing the found sc-links.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchLinksByContentSubstring` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchLinksByContentSubstring` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN ScAddrVector FindLinksByContentSubstring(
      ScStreamPtr const & linkContentSubstringStream,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false);

  /*!
   * @brief Searches sc-links contents by content substring using a stream.
   *
   * This method finds sc-links contents by matching the content substring with the provided stream.
   *
   * @param linkContent The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of strings representing the found sc-links contents.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * std::vector<std::string> const & linkContentVector = context.SearchLinksByContentSubstring("my");
   * for (auto const & content : linkContentVector)
   * {
   *   // Process contents.
   * }
   * @endcode
   */
  _SC_EXTERN template <typename TContentType>
  std::vector<std::string> SearchLinksContentsByContentSubstring(
      TContentType const & linkContentSubstring,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false)
  {
    return SearchLinksContentsByContentSubstring(ScStreamMakeRead(linkContentSubstring), maxLengthToSearchAsPrefix);
  }

  /*!
   * @brief Searches sc-links contents by content substring using a stream.
   *
   * This method finds sc-links contents by matching the content substring with the provided stream.
   *
   * @param linkContent The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of strings representing the found sc-links contents.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * std::vector<std::string> const & linkContentVector = context.SearchLinksByContentSubstring("my");
   * for (auto const & content : linkContentVector)
   * {
   *   // Process contents.
   * }
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchLinksByContentSubstring` instead for better readability
   * and standards compliance.
   */
  template <typename TContentType>
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchLinksByContentSubstring` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN std::vector<std::string> FindLinksContentsByContentSubstring(
      TContentType const & linkContentSubstring,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false)
  {
    return SearchLinksContentsByContentSubstring(linkContentSubstring, maxLengthToSearchAsPrefix);
  }

  /*!
   * @brief Searches sc-links contents by content substring using a typed string.
   *
   * This method finds sc-links contents by matching the content substring with the provided typed string.
   *
   * @param linkContentStream The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of strings representing the found sc-links contents.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   */
  _SC_EXTERN std::vector<std::string> SearchLinksContentsByContentSubstring(
      ScStreamPtr const & linkContentStreamSubstring,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false);

  /*!
   * @brief Searches sc-links contents by content substring using a typed string.
   *
   * This method finds sc-links contents by matching the content substring with the provided typed string.
   *
   * @param linkContentStream The stream to use for content matching.
   * @param maxLengthToSearchAsPrefix The maximum length to search as a prefix (default is 0).
   * @return Returns a vector of strings representing the found sc-links contents.
   * @throws ExceptionInvalidParams if the specified stream is invalid.
   * @throws ExceptionInvalidState if the file memory state is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchLinksByContentSubstring` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchLinksByContentSubstring` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN std::vector<std::string> FindLinksContentsByContentSubstring(
      ScStreamPtr const & linkContentStreamSubstring,
      size_t maxLengthToSearchAsPrefix = 0) noexcept(false);

  _SC_EXTERN template <
      typename ParamType1,
      typename ParamType2,
      typename ParamType3,
      typename ParamType4,
      typename ParamType5>
  std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> Iterator5(
      ParamType1 const & param1,
      ParamType2 const & param2,
      ParamType3 const & param3,
      ParamType4 const & param4,
      ParamType5 const & param5)
  {
    return std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>>(
        new ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>(
            *this, param1, param2, param3, param4, param5));
  }

  _SC_EXTERN template <typename ParamType1, typename ParamType2, typename ParamType3>
  std::shared_ptr<ScIterator3<ParamType1, ParamType2, ParamType3>> Iterator3(
      ParamType1 const & param1,
      ParamType2 const & param2,
      ParamType3 const & param3)
  {
    return std::shared_ptr<ScIterator3<ParamType1, ParamType2, ParamType3>>(
        new ScIterator3<ParamType1, ParamType2, ParamType3>(*this, param1, param2, param3));
  }

  /*!
   * @brief Generates an iterator for iterating over triples.
   *
   * This method creates an iterator for iterating over triples and calls the provided function for each result.
   *
   * @tparam ParamType1 A sc-type of the first parameter for the iterator.
   * @tparam ParamType2 A sc-type of the second parameter for the iterator.
   * @tparam ParamType3 A sc-type of the third parameter for the iterator.
   * @tparam FnT A sc-type of the function to be called for each result.
   * @param param1 The first parameter for the iterator.
   * @param param2 The second parameter for the iterator.
   * @param param3 The third parameter for the iterator.
   * @param fn The function to be called for each result.
   *
   * @note fn function should have 3 parameters (ScAddr/ScType const & source, ScAddr/ScType const & connector,
   * ScAddr/ScType const & target).
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   */
  _SC_EXTERN template <typename ParamType1, typename ParamType2, typename ParamType3, typename FnT>
  void ForEachIter3(ParamType1 const & param1, ParamType2 const & param2, ParamType3 const & param3, FnT && fn)
  {
    ScIterator3Ptr it = Iterator3(param1, param2, param3);
    while (it->Next())
      fn(it->Get(0), it->Get(1), it->Get(2));
  }

  /*!
   * @brief Generates an iterator for iterating over triples.
   *
   * This method creates an iterator for iterating over triples and calls the provided function for each result.
   *
   * @tparam ParamType1 A sc-type of the first parameter for the iterator.
   * @tparam ParamType2 A sc-type of the second parameter for the iterator.
   * @tparam ParamType3 A sc-type of the third parameter for the iterator.
   * @tparam ParamType4 A sc-type of the fourth parameter for the iterator.
   * @tparam ParamType5 A sc-type of the fifth parameter for the iterator.
   * @tparam FnT A sc-type of the function to be called for each result.
   * @param param1 The first parameter for the iterator.
   * @param param2 The second parameter for the iterator.
   * @param param3 The third parameter for the iterator.
   * @param param4 The fourth parameter for the iterator.
   * @param param5 The fifth parameter for the iterator.
   * @param fn The function to be called for each result.
   *
   * @note fn function should have 5 parameters (ScAddr/ScType const & source, ScAddr/ScType const & connector,
   * ScAddr/ScType const & target, ScAddr/ScType const & attrConnector, ScAddr/ScType const & attr)
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   */
  _SC_EXTERN template <
      typename ParamType1,
      typename ParamType2,
      typename ParamType3,
      typename ParamType4,
      typename ParamType5,
      typename FnT>
  void ForEachIter5(
      ParamType1 const & param1,
      ParamType2 const & param2,
      ParamType3 const & param3,
      ParamType4 const & param4,
      ParamType5 const & param5,
      FnT && fn)
  {
    ScIterator5Ptr it = Iterator5(param1, param2, param3, param4, param5);
    while (it->Next())
      fn(it->Get(0), it->Get(1), it->Get(2), it->Get(3), it->Get(4));
  }

  /*!
   * @brief Checks the existence of a sc-connector between two sc-elements with the specified type.
   *
   * This function checks if there is a sc-connector between the specified beginning and end sc-elements
   * with the given type. It returns true if the sc-connector is found; otherwise, it returns false.
   *
   * @param begin A sc-address of the beginning sc-element.
   * @param end A sc-address of the ending sc-element.
   * @param edgeType A sc-type of the sc-connector to check.
   * @return Returns true if the specified sc-connector exists; otherwise, returns false.
   * @throws ExceptionInvalidState if the sc-memory context is not valid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr nodeAddr1 = context.GenerateNode(ScType::NodeConst);
   * ScAddr nodeAddr2 = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcType = ScType::EdgeDCommonConst;
   * context.GenerateConnector(arcType, nodeAddr1, nodeAddr2);
   * bool doesArcExist = context.CheckConnector(nodeAddr1, nodeAddr2, arcType);
   * @endcode
   */
  _SC_EXTERN bool CheckConnector(
      ScAddr const & sourceElementAddr,
      ScAddr const & targetElementAddr,
      ScType const & connectorType) const noexcept(false);

  /*!
   * @brief Checks the existence of a sc-connector between two sc-elements with the specified type.
   *
   * This function checks if there is a sc-connector between the specified beginning and end sc-elements
   * with the given type. It returns true if the sc-connector is found; otherwise, it returns false.
   *
   * @param begin A sc-address of the beginning sc-element.
   * @param end A sc-address of the ending sc-element.
   * @param edgeType A sc-type of the sc-connector to check.
   * @return Returns true if the specified sc-connector exists; otherwise, returns false.
   * @throws ExceptionInvalidState if the sc-memory context is not valid.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr nodeAddr1 = context.GenerateNode(ScType::NodeConst);
   * ScAddr nodeAddr2 = context.GenerateNode(ScType::NodeConst);
   * ScAddr arcType = ScType::EdgeDCommonConst;
   * context.GenerateConnector(arcType, nodeAddr1, nodeAddr2);
   * bool doesArcExist = context.HelperCheckEdge(nodeAddr1, nodeAddr2, arcType);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `CheckConnector` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `CheckConnector` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN bool HelperCheckEdge(
      ScAddr const & sourceElementAddr,
      ScAddr const & targetElementAddr,
      ScType const & connectorType) const noexcept(false);

  /*!
   * @brief Resolves the sc-address of an sc-element by its system identifier.
   *
   * This function resolves the sc-address of an sc-element with the specified system identifier.
   * If the element is not found, it creates a new sc-node with the specified type and system identifier.
   *
   * @param sysIdtf The system identifier of the sc-element to resolve.
   * @param type A sc-type of the sc-node to create if the element is not found.
   * @return Returns the sc-address of the resolved or created sc-element.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid or the specified sc-address is invalid
   * or resolving sc-element type is not ScType::Node subtype
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr resolvedAddr = context.ResolveElementSystemIdentifier("example_identifier", ScType::NodeConstClass);
   * @endcode
   */
  _SC_EXTERN ScAddr ResolveElementSystemIdentifier(
      std::string const & systemIdentifier,
      ScType const & elementType = ScType()) noexcept(false);

  /*!
   * @brief Resolves the sc-address of an sc-element by its system identifier.
   *
   * This function resolves the sc-address of an sc-element with the specified system identifier.
   * If the element is not found, it creates a new sc-node with the specified type and system identifier.
   *
   * @param sysIdtf The system identifier of the sc-element to resolve.
   * @param type A sc-type of the sc-node to create if the element is not found.
   * @return Returns the sc-address of the resolved or created sc-element.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid or the specified sc-address is invalid
   * or resolving sc-element type is not ScType::Node subtype
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr resolvedAddr = context.HelperResolveSystemIdtf("example_identifier", ScType::NodeConstClass);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `ResolveElementSystemIdentifier` instead for better
   * readability and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ResolveElementSystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN ScAddr
  HelperResolveSystemIdtf(std::string const & systemIdentifier, ScType const & elementType = ScType()) noexcept(false);

  /*!
   * @brief Resolves the sc-address of an sc-element by its system identifier.
   *
   * This function resolves the sc-address of an sc-element with the specified system identifier.
   * If the element is not found, it creates a new sc-node with the specified type and system identifier.
   * Additionally, it returns the system identifier quintuple of the resolved or created sc-element.
   *
   * @param systemIdentifier A system identifier of the sc-element to resolve.
   * @param elementType A sc-type of the sc-node to create if the element is not found.
   * @param outQuintuple The 1st, 2d, 3d, 4th, 5th sc-element addresses of system identifier quintuple of resolved.
   *
   * @code
   *                              addr1 (resolved sc-element address)
   *                addr4           |
   *        addr5 --------------->  | addr2
   *     (nrel_system_identifier)   |
   *                              addr3 (system identifier sc-link)
   * @endcode
   *
   * @return Returns true if the sc-element is successfully resolved or created; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid or the specified sc-address is invalid
   *          or resolving sc-element type is not ScType::Node subtype.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScSystemIdentifierQuintuple resultQuintuple;
   * bool isResolved = context.ResolveElementSystemIdentifier("example_identifier", ScType::NodeConstClass,
   resultQuintuple);
   * @endcode
   */
  _SC_EXTERN bool ResolveElementSystemIdentifier(
      std::string const & systemIdentifier,
      ScType const & elementType,
      ScSystemIdentifierQuintuple & outQuintuple) noexcept(false);

  /*!
   * @brief Resolves the sc-address of an sc-element by its system identifier.
   *
   * This function resolves the sc-address of an sc-element with the specified system identifier.
   * If the element is not found, it creates a new sc-node with the specified type and system identifier.
   * Additionally, it returns the system identifier quintuple of the resolved or created sc-element.
   *
   * @param systemIdentifier A system identifier of the sc-element to resolve.
   * @param elementType A sc-type of the sc-node to create if the element is not found.
   * @param outQuintuple The 1st, 2d, 3d, 4th, 5th sc-element addresses of system identifier quintuple of resolved.
   *
   * @code
   *                              addr1 (resolved sc-element address)
   *                addr4           |
   *        addr5 --------------->  | addr2
   *     (nrel_system_identifier)   |
   *                              addr3 (system identifier sc-link)
   * @endcode
   *
   * @return Returns true if the sc-element is successfully resolved or created; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid or the specified sc-address is invalid
   *            or resolving sc-element type is not ScType::Node subtype.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScSystemIdentifierQuintuple resultQuintuple;
   * bool isResolved = context.ResolveElementSystemIdentifier("example_identifier", ScType::NodeConstClass,
   * resultQuintuple);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `ResolveElementSystemIdentifier` instead for better
   * readability and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `ResolveElementSystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN bool HelperResolveSystemIdtf(
      std::string const & systemIdentifier,
      ScType const & elementType,
      ScSystemIdentifierQuintuple & outQuintuple) noexcept(false);

  /*!
   * @brief Tries to set system identifier for sc-element `elementAddr`.
   * @param systemIdentifier A system identifier to set for sc-element `elementAddr`.
   * @param elementAddr A sc-element sc-address to set `sysIdtf` for it.
   * @returns false if `sysIdtf` set for other sc-element address.
   * @throws utils::ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * bool isSet = context.SetElementSystemIdentifier("example_identifier", elementAddr);
   * @endcode
   */
  _SC_EXTERN bool SetElementSystemIdentifier(std::string const & systemIdentifier, ScAddr const & elementAddr) noexcept(
      false);

  /*!
   * @brief Tries to set system identifier for sc-element `elementAddr`.
   * @param systemIdentifier A system identifier to set for sc-element `elementAddr`.
   * @param elementAddr A sc-element sc-address to set `sysIdtf` for it.
   * @returns false if `sysIdtf` set for other sc-element address.
   * @throws utils::ExceptionInvalidParams if the specified sc-address is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * bool isSet = context.HelperSetSystemIdtf("example_identifier", elementAddr);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SetElementSystemIdentifier` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SetElementSystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN bool HelperSetSystemIdtf(std::string const & systemIdentifier, ScAddr const & elementAddr) noexcept(false);

  /*!
   * @brief Tries to set system identifier for sc-element `elementAddr`.
   * @param systemIdentifier A system sc-identifier to set for sc-element `elementAddr`.
   * @param elementAddr A sc-element sc-address to set `sysIdtf` for it.
   * @param outQuintuple[out] The 1st, 2d, 3d, 4th, 5th sc-element addresses of system identifier quintuple of
   * sc-element `elementAddr` with set `systemIdentifier`.
   *
   * @code
   *                              addr1 (`elementAddr`)
   *                addr4           |
   *        addr5 --------------->  | addr2
   *     (nrel_system_identifier)   |
   *                              addr3 (system identifier sc-link)
   * @endcode
   *
   * @returns false if `sysIdtf` set for other sc-element address.
   * @throws utils::ExceptionInvalidParams if the specified sc-address is invalid
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * ScSystemIdentifierQuintuple resultQuintuple;
   * bool isSet = context.SetElementSystemIdentifier("example_identifier", elementAddr, resultQuintuple);
   * @endcode
   */
  _SC_EXTERN bool SetElementSystemIdentifier(
      std::string const & systemIdentifier,
      ScAddr const & elementAddr,
      ScSystemIdentifierQuintuple & outQuintuple) noexcept(false);

  /*!
   * @brief Tries to set system identifier for sc-element `elementAddr`.
   * @param systemIdentifier A system sc-identifier to set for sc-element `elementAddr`.
   * @param elementAddr A sc-element sc-address to set `sysIdtf` for it.
   * @param outQuintuple[out] The 1st, 2d, 3d, 4th, 5th sc-element addresses of system identifier quintuple of
   * sc-element `elementAddr` with set `systemIdentifier`.
   *
   * @code
   *                              addr1 (`elementAddr`)
   *                addr4           |
   *        addr5 --------------->  | addr2
   *     (nrel_system_identifier)   |
   *                              addr3 (system identifier sc-link)
   * @endcode
   *
   * @returns false if `sysIdtf` set for other sc-element address.
   * @throws utils::ExceptionInvalidParams if the specified sc-address is invalid
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write and erase
   * permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * ScSystemIdentifierQuintuple resultQuintuple;
   * bool isSet = context.HelperSetSystemIdtf("example_identifier", elementAddr, resultQuintuple);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SetElementSystemIdentifier` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SetElementSystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN bool HelperSetSystemIdtf(
      std::string const & systemIdentifier,
      ScAddr const & elementAddr,
      ScSystemIdentifierQuintuple & outQuintuple) noexcept(false);

  /*!
   * @brief Tries to get system identifier for sc-element `elementAddr`.
   * @param elementAddr A sc-element sc-address to get it system identifier.
   * @returns "" if system identifier doesn't exist for `elementAddr`.
   * @throws utils::ExceptionInvalidParams if the specified sc-address is invalid
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * std::string systemIdentifier = context.GetElementSystemIdentifier(elementAddr);
   * @endcode
   */
  _SC_EXTERN std::string GetElementSystemIdentifier(ScAddr const & elementAddr) noexcept(false);

  /*!
   * @brief Tries to get system identifier for sc-element `elementAddr`.
   * @param elementAddr A sc-element sc-address to get it system identifier.
   * @returns "" if system identifier doesn't exist for `elementAddr`.
   * @throws utils::ExceptionInvalidParams if the specified sc-address is invalid
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr elementAddr = context.GenerateNode(ScType::NodeConst);
   * std::string systemIdentifier = context.HelperGetSystemIdtf(elementAddr);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `GetElementSystemIdentifier` instead for better readability
   * and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `GetElementSystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN std::string HelperGetSystemIdtf(ScAddr const & elementAddr) noexcept(false);

  /*!
   * @brief Searches an sc-element by its system identifier and returns its sc-address.
   *
   * This function searches for an sc-element with the specified system identifier and returns its sc-address.
   * If the element is found, the sc-address is stored in the 'outAddr' parameter, and the function returns true;
   * otherwise, it returns false.
   *
   * @param systemIdentifier A system identifier of the sc-element to find.
   * @param outElementAddr A reference to store the sc-address of the found sc-element (if any).
   * @return Returns true if the sc-element is found; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr resultAddr;
   * bool isFound = context.SearchElementBySystemIdentifier("example_identifier", resultAddr);
   * @endcode
   */
  _SC_EXTERN bool SearchElementBySystemIdentifier(
      std::string const & systemIdentifier,
      ScAddr & outElementAddr) noexcept(false);

  /*!
   * @brief Searches an sc-element by its system identifier and returns its sc-address.
   *
   * This function searches for an sc-element with the specified system identifier and returns its sc-address.
   * If the element is found, the sc-address is stored in the 'outAddr' parameter, and the function returns true;
   * otherwise, it returns false.
   *
   * @param systemIdentifier A system identifier of the sc-element to find.
   * @param outElementAddr A reference to store the sc-address of the found sc-element (if any).
   * @return Returns true if the sc-element is found; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr resultAddr;
   * bool isFound = context.HelperFindBySystemIdtf("example_identifier", resultAddr);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchElementBySystemIdentifier` instead for better
   * readability and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchElementBySystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN bool HelperFindBySystemIdtf(std::string const & systemIdentifier, ScAddr & outElementAddr) noexcept(false);

  /*!
   * @brief Searches an sc-element by its system identifier and returns its sc-address.
   *
   * This function searches for an sc-element with the specified system identifier and returns its sc-address.
   * If the element is found, the function returns its sc-address; otherwise, it throws an exception.
   *
   * @param systemIdentifier A system identifier of the sc-element to find.
   * @return Returns the sc-address of the found sc-element.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr resultAddr = context.SearchElementBySystemIdentifier("example_identifier");
   * @endcode
   */
  _SC_EXTERN ScAddr SearchElementBySystemIdentifier(std::string const & systemIdentifier) noexcept(false);

  /*!
   * @brief Searches an sc-element by its system identifier and returns its sc-address.
   *
   * This function searches for an sc-element with the specified system identifier and returns its sc-address.
   * If the element is found, the function returns its sc-address; otherwise, it throws an exception.
   *
   * @param systemIdentifier A system identifier of the sc-element to find.
   * @return Returns the sc-address of the found sc-element.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScAddr resultAddr = context.HelperFindBySystemIdtf("example_identifier");
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchElementBySystemIdentifier` instead for better
   * readability and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchElementBySystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN ScAddr HelperFindBySystemIdtf(std::string const & systemIdentifier) noexcept(false);

  /*!
   * @brief Searches an sc-element by its system identifier and returns its sc-address as a system identifier quintuple.
   *
   * This function searches for an sc-element with the specified system identifier and returns its sc-address.
   * A sc-address is then converted into a system identifier quintuple, which is stored in the 'outQuintuple'
   * parameter. If the element is found, the function returns true; otherwise, it returns false.
   *
   * @param systemIdentifier The system identifier of the sc-element to find.
   * @param outQuintuple A reference to store the system identifier quintuple of the found sc-element.
   * @return Returns true if the sc-element is found; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScSystemIdentifierQuintuple resultQuintuple;
   * bool isFound = context.SearchElementBySystemIdentifier("example_identifier", resultQuintuple);
   * @endcode
   */
  _SC_EXTERN bool SearchElementBySystemIdentifier(
      std::string const & systemIdentifier,
      ScSystemIdentifierQuintuple & outQuintuple) noexcept(false);

  /*!
   * @brief Searches an sc-element by its system identifier and returns its sc-address as a system identifier quintuple.
   *
   * This function searches for an sc-element with the specified system identifier and returns its sc-address.
   * A sc-address is then converted into a system identifier quintuple, which is stored in the 'outQuintuple'
   * parameter. If the element is found, the function returns true; otherwise, it returns false.
   *
   * @param systemIdentifier The system identifier of the sc-element to find.
   * @param outQuintuple A reference to store the system identifier quintuple of the found sc-element.
   * @return Returns true if the sc-element is found; otherwise, returns false.
   * @throws ExceptionInvalidParams if the specified system identifier is invalid.
   * @throws ExceptionInvalidState if the sc-memory context is not valid or in an invalid state.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated.
   *
   * @code
   * ScMemoryContext context;
   * ScSystemIdentifierQuintuple resultQuintuple;
   * bool isFound = context.HelperFindBySystemIdtf("example_identifier", resultQuintuple);
   * @endcode
   *
   * @warning This method is deprecated since 0.10.0. Use `SearchElementBySystemIdentifier` instead for better
   * readability and standards compliance.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `SearchElementBySystemIdentifier` instead for better readability and standards "
      "compliance.’")
  _SC_EXTERN bool HelperFindBySystemIdtf(
      std::string const & systemIdentifier,
      ScSystemIdentifierQuintuple & outQuintuple) noexcept(false);

  /*!
   * @brief Generates sc-constructions by object of `ScTemplate` and accumulates generated sc-construction into
   * `result`.
   * @param templateToGenerate An object of `ScTemplate` to find constructions by it.
   * @param result A generated sc-construction.
   * @param params A map of specified sc-template sc-variables to user replacements.
   * @param resultCode A pointer to status of method completion.
   * @return Returns true if the construction is generated; otherwise, returns false. It is the same as `resultCode`.
   * @throws utils::ExceptionInvalidState if the object of `ScTemplate` is not valid.
   *
   * @code
   * ...
   * ScAddr const & classAddr = context.SearchElementBySystemIdentifier("my_class");
   * ...
   * ScTemplate templateToGenerate;
   * templ.Triple(
   *  classAddr,
   *  ScType::EdgeAccessVarPosPerm >> "_edge",
   *  ScType::Unknown >> "_addr2"
   * );
   *
   * ScTemplateResultItem result;
   * m_context->HelperGenTemplate(templateToGenerate, result);
   *
   * // handle generated sc-construction sc-elements
   * m_context->IsElement(item["_addr2"])
   * @endcode
   */
  _SC_EXTERN ScTemplate::Result HelperGenTemplate(
      ScTemplate const & templ,
      ScTemplateResultItem & result,
      ScTemplateParams const & params = ScTemplateParams::Empty,
      ScTemplateResultCode * resultCode = nullptr) noexcept(false);

  /*!
   * Searches sc-constructions by object of `ScTemplate` and accumulates found sc-constructions into `result`.
   * @param templateToFind An object of `ScTemplate` to find sc-constructions by it.
   * @param result A result vector of found sc-constructions.
   * @return Returns true if the sc-constructions are found; otherwise, returns false.
   * @throws utils::ExceptionInvalidState if the object of `ScTemplate` is not valid.
   *
   * @note Use this method if expected searchable sc-constructions vector is not big. If it is very big, please, use the
   * one of callback-based HelperSearchTemplate.
   *
   * @code
   * ...
   * ScAddr const & classAddr = context.SearchElementBySystemIdentifier("my_class");
   * ...
   * ScTemplate templateToFind;
   * templ.Triple(
   *  classAddr,
   *  ScType::EdgeAccessVarPosPerm >> "_edge",
   *  ScType::Unknown >> "_addr2"
   * );
   *
   * ScTemplateSearchResult result;
   * m_context->HelperSearchTemplate(templateToFind, result);
   *
   * // iterate by all result sc-constructions
   * for (size_t i = 0; i < result.Size(); ++i)
   * {
   *   ScTemplateResultItem const & item = result[i];
   *   // handle each result sc-construction sc-elements
   *   m_context->IsElement(item["_addr2"])
   * }
   * @endcode
   */
  _SC_EXTERN ScTemplate::Result HelperSearchTemplate(
      ScTemplate const & templateToFind,
      ScTemplateSearchResult & result) noexcept(false);

  /*!
   * Searches sc-constructions by object of `ScTemplate` and passes found sc-constructions to `callback`
   * lambda-function. If `filterCallback` passed, then all found constructions triples are filtered by `filterCallback`
   * condition.
   * @param templateToFind An object of `ScTemplate` to find sc-constructions by it.
   * @param callback A lambda-function, callable when each sc-construction triple was found.
   * @param filterCallback A lambda-function, that filters all found sc-constructions triples.
   * @param checkCallback A lambda-function, that filters all found triples by checking single sc-elements.
   * @return Returns true if the sc-constructions are found; otherwise, returns false.
   * @throws utils::ExceptionInvalidState if the object of `ScTemplate` is not valid.
   *
   * @code
   * ...
   * ...
   * ScAddr const & classAddr = context.SearchElementBySystemIdentifier("my_class");
   * ScAddr const & structureAddr = context.GenerateNode(ScType::NodeConstStruct);
   * ScAddr const & modelAddr = context.GenerateNode(ScType::NodeConstStruct);
   * ...
   * ScAddr const & setAddr = context.GenerateNode(ScType::NodeConst);
   * ScTemplate templateToFind;
   * templateToFind.Triple(
   *  classAddr,
   *  ScType::EdgeAccessVarPosPerm >> "_edge",
   *  ScType::Unknown >> "_addr2"
   * );
   * // Find all instances of specified class.
   * m_context->HelperSearchTemplate(templateToFind, [&context](ScTemplateSearchResultItem const & item) {
   *  // Add each checked instance of class to set.
   *  context.GenerateConnector(ScType::EdgeAccessConstPosTemp, setAddr, item["_addr2"]);
   * }, [&context](ScTemplateSearchResultItem const & item) -> bool {
   *  // Check that each sc-arc between class and its instance belongs to structure.
   *  return !context->CheckConnector(structureAddr, item["_edge"], ScType::EdgeAccessConstPosPerm);
   * }, [&context](ScAddr const & addr) -> bool {
   *  // Check that each sc-element of find sc-construction belongs to model.
   *  return context->CheckConnector(modelAddr, addr, ScType::EdgeAccessConstPosPerm);
   * });
   * @endcode
   */
  _SC_EXTERN void HelperSearchTemplate(
      ScTemplate const & templateToFind,
      ScTemplateSearchResultCallback const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) noexcept(false);

  /*!
   * Searches sc-constructions by object of `ScTemplate` and passes found sc-constructions to `callback`
   * lambda-function.
   * @param templateToFind An object of `ScTemplate` to find sc-constructions by it.
   * @param callback A lambda-function, callable when each sc-construction triple was found.
   * @param checkCallback A lambda-function, that filters all found triples by checking single sc-elements.
   * @return Returns true if the sc-constructions are found; otherwise, returns false.
   * @throws utils::ExceptionInvalidState if the object of `ScTemplate` is not valid.
   *
   * @code
   * ...
   * ...
   * ScAddr const & classAddr = context.SearchElementBySystemIdentifier("my_class");
   * ScAddr const & modelAddr = context.GenerateNode(ScType::NodeConstStruct);
   * ...
   * ScAddr const & setAddr = context.GenerateNode(ScType::NodeConst);
   * ScTemplate templateToFind;
   * templateToFind.Triple(
   *  classAddr,
   *  ScType::EdgeAccessVarPosPerm >> "_edge",
   *  ScType::Unknown >> "_addr2"
   * );
   * // Find all instances of specified class that belong to model and for which input sc-arc from class belongs to
   * structure. m_context->HelperSearchTemplate(templateToFind, [&context](ScTemplateSearchResultItem const & item) {
   *  // Add each checked instance of class to set.
   *  context.GenerateConnector(ScType::EdgeAccessConstPosTemp, setAddr, item["_addr2"]);
   * }, [&context](ScAddr const & addr) -> bool {
   *  // Check that each sc-element of find sc-construction belongs to model.
   *  return context->CheckConnector(modelAddr, addr, ScType::EdgeAccessConstPosPerm);
   * });
   * @endcode
   */
  _SC_EXTERN void HelperSearchTemplate(
      ScTemplate const & templateToFind,
      ScTemplateSearchResultCallback const & callback,
      ScTemplateSearchResultCheckCallback const & checkCallback) noexcept(false);

  /*!
   * Searches constructions by object of `ScTemplate` and pass found sc-constructions to `callback`
   * lambda-function. Lambda-function `callback` must return a request command value to manage sc-template search:
   *  - ScTemplateSearchRequest::CONTINUE,
   *  - ScTemplateSearchRequest::STOP,
   *  - ScTemplateSearchRequest::ERROR.
   * When ScTemplateSearchRequest::CONTINUE returns, sc-template search will be continued. If
   * ScTemplateSearchRequest::STOP or ScTemplateSearchRequest::ERROR returns, then sc-template search stops.
   * If sc-template search stopped by ScTemplateSearchRequest::ERROR, then HelperSmartSearchTemplate thrown
   * utils::ExceptionInvalidState.
   * If `filterCallback` passed, then all found sc-constructions triples are filtered by `filterCallback` condition.
   * @param templateToFind An object of `ScTemplate` to find sc-constructions by it.
   * @param callback A lambda-function, callable when each sc-construction triple was found.
   * @param filterCallback A lambda-function, that filters all found sc-constructions triples.
   * @param checkCallback A lambda-function, that filters all found triples by checking single sc-elements.
   * @return Returns true if the sc-constructions are found; otherwise, returns false.
   * @throws utils::ExceptionInvalidState if the object of `ScTemplate` is not valid.
   *
   * @code
   * ...
   * ...
   * ScAddr const & classAddr = context.SearchElementBySystemIdentifier("my_class");
   * ScAddr const & structureAddr = context.GenerateNode(ScType::NodeConstStruct);
   * ...
   * ScAddr const & setAddr = context.GenerateNode(ScType::NodeConst);
   * ScTemplate templateToFind;
   * templateToFind.Triple(
   *  classAddr,
   *  ScType::EdgeAccessVarPosPerm >> "_edge",
   *  ScType::Unknown >> "_addr2"
   * );
   * // Find random instance of specified class that belongs to set, but for which input sc-arc from class doesn't
   * belong to structure. m_context->HelperSmartSearchTemplate(templateToFind, [&context](ScTemplateSearchResultItem
   * const & item) -> ScTemplateSearchRequest
   * {
   *   ScAddr const & edgeAddr = item["_edge"];
   *   if (context->CheckConnector(structureAddr, edgeAddr, ScType::EdgeAccessConstPosPerm))
   *    return ScTemplateSearchRequest::CONTINUE;
   *
   *   if (context->CheckConnector(setAddr, item["_addr2"], ScType::EdgeAccessConstPosTemp))
   *    return ScTemplateSearchRequest::STOP;
   *
   *   return ScTemplateSearchRequest::CONTINUE;
   * });
   * @endcode
   */
  _SC_EXTERN void HelperSmartSearchTemplate(
      ScTemplate const & templateToFind,
      ScTemplateSearchResultCallbackWithRequest const & callback,
      ScTemplateSearchResultFilterCallback const & filterCallback = {},
      ScTemplateSearchResultCheckCallback const & checkCallback = {}) noexcept(false);

  _SC_EXTERN void HelperSmartSearchTemplate(
      ScTemplate const & templateToFind,
      ScTemplateSearchResultCallbackWithRequest const & callback,
      ScTemplateSearchResultCheckCallback const & checkCallback) noexcept(false);

  /*!
   * Translates a sc-template represented in sc-memory (sc-structure) into object of `ScTemplate`. After
   * sc-template translation you can use object of `ScTemplate` to search or generate sc-constructions: in
   * `HelperSearchTemplate` and `HelperGenTemplate` correspondingly.
   * @param resultTemplate An object of `ScTemplate` to be gotten.
   * @param translatableTemplateAddr A sc-address of sc-template structure to be translated.
   * @param params A map of specified sc-template sc-variables to their replacements.
   * @return Returns true if the sc-template represented in sc-memory (sc-structure) into object of `ScTemplate`;
   * otherwise, throws exceptions.
   * @throws utils::ExceptionInvalidState if sc-template represented in sc-memory is not valid.
   *
   * @code
   * ...
   * ...
   * ScTemplate resultTemplate;
   * ScAddr const & translatableTemplAddr = m_context->SearchElementBySystemIdentifier("my_template");
   * m_context->HelperBuildTemplate(resultTemplate, translatableTemplAddr);
   * ...
   * @endcode
   */
  _SC_EXTERN ScTemplate::Result HelperBuildTemplate(
      ScTemplate & resultTemplate,
      ScAddr const & translatableTemplateAddr,
      ScTemplateParams const & params = ScTemplateParams()) noexcept(false);

  /*!
   * Translates a sc-template represented in SCs-code into object of `ScTemplate`. After sc-template translation you can
   * use object of `ScTemplate` to search or generate sc-constructions: in `HelperSearchTemplate` and
   * `HelperGenTemplate` correspondingly.
   * @param resultTemplate An object of `ScTemplate` to be gotten.
   * @param translatableSCsTemplate A sc.s-representation of sc-template to be translated.
   * @return Returns true if the sc-template represented in SCs-code into object of `ScTemplate`; otherwise, throws
   * exceptions.
   * @throws utils::ExceptionInvalidState if sc-template represented in SCs-code is not valid.
   *
   * @code
   * ...
   * ...
   * ScTemplate resultTemplate;
   * std::string const translatableSCsTemplate = "concept_set _-> _var;;";
   * m_context->HelperBuildTemplate(resultTemplate, translatableSCsTemplate);
   * ...
   * @endcode
   */
  _SC_EXTERN ScTemplate::Result HelperBuildTemplate(
      ScTemplate & resultTemplate,
      std::string const & translatableSCsTemplate) noexcept(false);

protected:
  /*!
   * Translates an object of `ScTemplate` to sc-template in sc-memory (sc-structure).
   * This method is an inverse of the `HelperBuildTemplate` method.
   * @param translatableTemplate An object of `ScTemplate` to be translated.
   * @param resultTemplateAddr A sc-address of sc-template structure to be gotten in sc-memory.
   * @param params A map of specified sc-template sc-variables to their replacements.
   *
   * @code
   * ...
   * ...
   * ScTemplate translatableTemplate;
   * templ.Triple(
   *  classAddr,
   *  ScType::EdgeAccessVarPosPerm >> "_edge",
   *  ScType::Unknown >> "_addr2"
   * );
   * ScAddr resultTemplateAddr;
   * m_context->HelperLoadTemplate(translatableTemplate, resultTemplateAddr);
   * ...
   * @endcode
   */
  _SC_EXTERN void HelperLoadTemplate(
      ScTemplate & translatableTemplate,
      ScAddr & resultTemplateAddr,
      ScTemplateParams const & params = ScTemplateParams()) noexcept(false);

public:
  /*! Calculates sc-element counts.
   * @return Returns sc-nodes, sc-connectors and sc-links counts.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have read permissions.
   */
  _SC_EXTERN ScMemoryStatistics CalculateStat() const;

  /*!
   * @brief Saves the memory state.
   *
   * This method saves the state of the sc-memory.
   *
   * @return Returns true if the memory state was successfully saved; otherwise, returns false.
   * @throws ExceptionInvalidState if the sc-memory context is not authenticated or does not have write permissions.
   */
  _SC_EXTERN bool Save();

protected:
  _SC_EXTERN explicit ScMemoryContext(ScAddr const & userAddr) noexcept;

protected:
  sc_memory_context * m_context;
  ScAddr m_contextStructureAddr;
};

class ScMemoryContextEventsPendingGuard
{
public:
  _SC_EXTERN explicit ScMemoryContextEventsPendingGuard(ScMemoryContext & context)
    : m_context(context)
  {
    m_context.BeingEventsPending();
  }

  _SC_EXTERN ~ScMemoryContextEventsPendingGuard()
  {
    m_context.EndEventsPending();
  }

private:
  ScMemoryContext & m_context;
};

class ScMemoryContextEventsBlockingGuard
{
public:
  _SC_EXTERN explicit ScMemoryContextEventsBlockingGuard(ScMemoryContext & context)
    : m_context(context)
  {
    m_context.BeingEventsBlocking();
  }

  _SC_EXTERN ~ScMemoryContextEventsBlockingGuard()
  {
    m_context.EndEventsBlocking();
  }

private:
  ScMemoryContext & m_context;
};
