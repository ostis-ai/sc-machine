#include <gtest/gtest.h>

#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include "sc-ui/uiKeynodes.h"
#include "sc-ui/ui.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string const TEST_STRUCTURES_PATH = SC_KPM_TEST_SRC_PATH "/translators/test-structures/";
std::string const COMMAND_INIT_CHECKS_PATH = TEST_STRUCTURES_PATH + "command_init_checks/";

ScAddr findTranslation(ScMemoryContext & context, ScAddr const & constructionAddr)
{
  ScTemplate translationTemplate;
  translationTemplate.TripleWithRelation(
      constructionAddr,
      ScType::EdgeDCommonVar,
      ScType::LinkVar >> "_translation_link",
      ScType::EdgeAccessVarPosPerm,
      ScAddr(keynode_nrel_translation));
  ScTemplateSearchResult result;
  context.HelperSearchTemplate(translationTemplate, result);
  if (!result.IsEmpty())
    return result[0]["_translation_link"];
  return ScAddr::Empty;
}

ScAddr getTranslation(ScMemoryContext & context, ScAddr const & answerAddr)
{
  int const WAIT_TIME = 2;
  int waitTime = 0;
  ScAddr translation = findTranslation(context, answerAddr);
  while (!translation.IsValid())
  {
    sleep(1);
    waitTime += 1;
    if (waitTime > WAIT_TIME)
    {
      translation = ScAddr::Empty;
      break;
    }
    translation = findTranslation(context, answerAddr);
  }
  return translation;
}

bool GetContentFromFile(std::string & data, std::string const & url)
{
  std::ifstream ifs(url);
  if (!ifs.is_open())
  {
    return false;
  }

  data.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  ifs.close();
  return true;
}

bool GenerateByFileURL(SCsHelper & helper, std::string const & url)
{
  std::string data;
  if (!GetContentFromFile(data, url))
  {
    return false;
  }

  if (!helper.GenerateBySCsText(data))
  {
    SC_LOG_ERROR(helper.GetLastError());
    return false;
  }

  return true;
}

TEST_F(ScMemoryTest, test_successfull_result)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "init_ui_translator.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_TRUE(resultLink.IsValid());
  ScStreamPtr stream = m_ctx->GetLinkContent(resultLink);
  std::string result, expected;
  EXPECT_TRUE(GetContentFromFile(expected, TEST_STRUCTURES_PATH + "successfull_result_answer.txt"));

  EXPECT_TRUE(ScStreamConverter::StreamToString(stream, result));
  json expectedJson = json::parse(expected);
  json resultJson = json::parse(result);
  // TODO: implement mocked sc-addrs for json
  //EXPECT_EQ(expectedJson, resultJson);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, test_command_no_class)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "command_init_no_class.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_FALSE(resultLink.IsValid());

  sc_module_shutdown();
}


TEST_F(ScMemoryTest, test_command_no_lang)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "command_init_no_lang.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_TRUE(resultLink.IsValid());

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, test_command_no_format)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "command_init_no_format.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_FALSE(resultLink.IsValid());

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, test_struct_with_keynodes)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "init_ui_translator.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "subj_domain.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain_keynodes.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_TRUE(resultLink.IsValid());
  ScStreamPtr stream = m_ctx->GetLinkContent(resultLink);
  std::string result;
  EXPECT_TRUE(ScStreamConverter::StreamToString(stream, result));

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, test_with_definition)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "init_ui_translator.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "subj_domain.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain_keynodes.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "knowledge_definition.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  if (resultLink.IsValid())
  {
    ScStreamPtr stream = m_ctx->GetLinkContent(resultLink);
    std::string result;
    EXPECT_TRUE(ScStreamConverter::StreamToString(stream, result));
  }

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, test_with_order_list)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "init_ui_translator.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "order_list.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_TRUE(resultLink.IsValid());
  ScStreamPtr stream = m_ctx->GetLinkContent(resultLink);
  std::string result, expected;
  EXPECT_TRUE(GetContentFromFile(expected, TEST_STRUCTURES_PATH + "with_order_list_answer.txt"));

  EXPECT_TRUE(ScStreamConverter::StreamToString(stream, result));
  json expectedJson = json::parse(expected);
  json resultJson = json::parse(result);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, test_with_filter_list)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "init_ui_translator.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "section_subj_domain.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "filter_list.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_TRUE(resultLink.IsValid());
  ScStreamPtr stream = m_ctx->GetLinkContent(resultLink);
  std::string result, expected;
  EXPECT_TRUE(GetContentFromFile(expected, TEST_STRUCTURES_PATH + "with_filter_list_answer.txt"));

  EXPECT_TRUE(ScStreamConverter::StreamToString(stream, result));
  json expectedJson = json::parse(expected);
  json resultJson = json::parse(result);

  sc_module_shutdown();
}

TEST_F(ScMemoryTest, test_kb_fragment)
{
  sc_addr const structAddr = sc_memory_node_new(m_ctx->GetRealContext(), sc_type_node_struct | sc_type_const);
  sc_module_initialize_with_init_memory_generated_structure(structAddr);

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(GenerateByFileURL(helper, COMMAND_INIT_CHECKS_PATH + "init_ui_translator.scs"));
  EXPECT_TRUE(GenerateByFileURL(helper, TEST_STRUCTURES_PATH + "kb_fragment.scs"));

  ScAddr trans_cmd_addr = m_ctx->HelperResolveSystemIdtf("trans_cmd_addr");
  ScAddr answer_addr = m_ctx->HelperResolveSystemIdtf("answer_addr");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, keynode_command_initiated, trans_cmd_addr);
  ScAddr resultLink = getTranslation(*m_ctx, answer_addr);
  EXPECT_TRUE(resultLink.IsValid());

  ScStreamPtr stream = m_ctx->GetLinkContent(resultLink);
  std::string result, expected;
  EXPECT_TRUE(GetContentFromFile(expected, TEST_STRUCTURES_PATH + "successfull_result_kb_fragment.txt"));

  EXPECT_TRUE(ScStreamConverter::StreamToString(stream, result));
  json expectedJson = json::parse(expected);
  json resultJson = json::parse(result);

  sc_module_shutdown();
}
