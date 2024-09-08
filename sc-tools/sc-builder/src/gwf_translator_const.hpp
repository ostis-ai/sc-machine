/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

namespace Constants
{
std::string const EDGE = "edge";
std::string const NODE = "node";
std::string const BUS = "bus";
std::string const PAIR = "pair";
std::string const ARC = "arc";
std::string const CONTOUR = "contour";

std::string const UNDERSCORE = "_";
std::string const DASH = "-";
std::string const ALIAS_PREFIX = "@";

std::string const N_REL_SYSTEM_IDTF = "nrel_system_identifier";
std::string const N_REL_MAIN_IDTF = "nrel_main_idtf";
std::string const FORMAT_EDGE = "@format_edge";
std::string const NREL_FORMAT_EDGE = "@nrel_format_edge";
std::string const NREL_FORMAT = "nrel_format";
std::string const EDGE_FROM_CONTOUR = "@edge_from_contour";
std::string const EDGE_TO_NODE = "to_node";

std::string const SC_EDGE_MAIN_L = "<-";
std::string const SC_EDGE_MAIN_R = "->";
std::string const SC_EDGE_DCOMMON_R = "=>";
std::string const SC_EDGE_DCOMMON_L = "<=";

std::string const ELEMENT_END = ";;";
std::string const EQUAL = "=";
std::string const SPACE = " ";
std::string const NEWLINE = "\n";
std::string const DOUBLE_QUOTE = "\"";
std::string const COLON = ":";

std::string const VAR = "/var/";
std::string const INT64 = "int64:";
std::string const FLOAT = "float:";
std::string const FILE_PREFIX = "file://";
std::string const EL_PREFIX = "..el";
std::string const EL_VAR_PREFIX = ".._el";
std::string const SCS_EXTENTION = ".scs";

std::string const OPEN_PARENTHESIS = "(";
std::string const CLOSE_PARENTHESIS = ")";
std::string const OPEN_BRACKET = "[";
std::string const CLOSE_BRACKET = "]";
std::string const OPEN_CONTOUR = "[*";
std::string const CLOSE_CONTOUR = "*]";

std::string const BLOCK_BEGIN = "[";
std::string const BLOCK_END = "];;";
std::string const EDGE_BEGIN = "(";
std::string const EDGE_END = ");;";
std::string const CONTOUR_BEGIN = "[*";
std::string const CONTOUR_END = "*];;";

std::string const ID = "id";
std::string const PARENT = "parent";
std::string const IDENTIFIER = "idtf";
std::string const TYPE = "type";
std::string const MIME_TYPE = "mime_type";
std::string const FILE_NAME = "file_name";
std::string const OWNER = "owner";
std::string const ID_B = "id_b";
std::string const ID_E = "id_e";

std::string const NO_CONTENT = "0";
std::string const NO_PARENT = "0";

std::unordered_map<std::string, std::string> const IMAGE_FORMATS = {{".png", "format_png"}};
}  // namespace Constants

using SCgElements = std::unordered_map<std::string, std::shared_ptr<class SCgElement>>;
using SCgConnectors = std::unordered_map<std::shared_ptr<class SCgConnector>, std::pair<std::string, std::string>>;
using SCgContours = std::unordered_map<std::shared_ptr<class SCgContour>, std::string>;
