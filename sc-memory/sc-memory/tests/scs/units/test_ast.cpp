/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "test_scs_utils.hpp"

using ScJson = nlohmann::json;

bool json_equal(ScJson const & j1, ScJson const & j2)
{
  if (j1.type() != j2.type())
    return false;

  if (j1.is_object())
  {
    std::vector<std::string> keys1;
    for (auto it = j1.begin(); it != j1.end(); ++it)
      keys1.push_back(it.key());

    std::vector<std::string> keys2;
    for (auto it = j2.begin(); it != j2.end(); ++it)
      keys2.push_back(it.key());

    if (keys1 != keys2)
      return false;

    for (auto & k : keys1)
      if (!json_equal(j1[k], j2[k]))
        return false;

    return true;
  }
  if (j1.is_array())
  {
    if (j1.size() != j2.size())
      return false;
    std::vector<bool> matched(j2.size(), false);
    for (auto & el1 : j1)
    {
      bool found = false;
      for (size_t i = 0; i < j2.size(); ++i)
      {
        if (!matched[i] && json_equal(el1, j2[i]))
        {
          matched[i] = true;
          found = true;
          break;
        }
      }
      if (!found)
        return false;
    }
    return true;
  }
  return j1 == j2;
}

TEST(SCsASTTest, BuildAST_ForTripleSentence_Successfully)
{
  sc_char const * data = "x -> y;;";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_TRUE(json_equal(R"({
    "errors": [],
    "root": {
      "children": [
        {
          "children": [
            {
              "children": [
                {
                  "children": [
                    {
                      "children": [
                        {
                          "children": [
                            {
                              "children": [],
                              "position": {
                                "beginIndex": 0,
                                "beginLine": 1,
                                "endIndex": 1,
                                "endLine": 1
                              },
                              "ruleType": "idtf_system",
                              "token": "x"
                            }
                          ],
                          "position": {
                            "beginIndex": 0,
                            "beginLine": 1,
                            "endIndex": 1,
                            "endLine": 1
                          },
                          "ruleType": "idtf_atomic"
                        }
                      ],
                      "position": {
                        "beginIndex": 0,
                        "beginLine": 1,
                        "endIndex": 1,
                        "endLine": 1
                      },
                      "ruleType": "idtf_common"
                    },
                    {
                      "children": [
                        {
                          "children": [],
                          "position": {
                            "beginIndex": 2,
                            "beginLine": 1,
                            "endIndex": 4,
                            "endLine": 1
                          },
                          "ruleType": "connector",
                          "token": "->"
                        },
                        {
                          "children": [
                            {
                              "children": [
                                {
                                  "children": [
                                    {
                                      "children": [],
                                      "position": {
                                        "beginIndex": 5,
                                        "beginLine": 1,
                                        "endIndex": 6,
                                        "endLine": 1
                                      },
                                      "ruleType": "idtf_system",
                                      "token": "y"
                                    }
                                  ],
                                  "position": {
                                    "beginIndex": 5,
                                    "beginLine": 1,
                                    "endIndex": 6,
                                    "endLine": 1
                                  },
                                  "ruleType": "idtf_atomic"
                                }
                              ],
                              "position": {
                                "beginIndex": 5,
                                "beginLine": 1,
                                "endIndex": 6,
                                "endLine": 1
                              },
                              "ruleType": "idtf_common"
                            }
                          ],
                          "position": {
                            "beginIndex": 5,
                            "beginLine": 1,
                            "endIndex": 6,
                            "endLine": 1
                          },
                          "ruleType": "idtf_list"
                        }
                      ],
                      "position": {
                        "beginIndex": 2,
                        "beginLine": 1,
                        "endIndex": 6,
                        "endLine": 1
                      },
                      "ruleType": "sentence_lvl_4_list_item"
                    }
                  ],
                  "position": {
                    "beginIndex": 0,
                    "beginLine": 1,
                    "endIndex": 6,
                    "endLine": 1
                  },
                  "ruleType": "sentence_lvl_common"
                }
              ],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 6,
                "endLine": 1
              },
              "ruleType": "sentence"
            }
          ],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 8,
            "endLine": 1
          },
          "ruleType": "sentence_wrap",
          "token": ";;"
        }
      ],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 13,
        "endLine": 1
      },
      "ruleType": "syntax",
      "token": "<EOF>"
    }
  })"_json, ScJson::parse(ast)));
}

TEST(SCsASTTest, BuildAST_ForSentenceWithContourAndNestedContent_Successfully)
{
  sc_char const * data = "x -> [* y _=> [test*];; *];;";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_TRUE(json_equal(R"({
    "errors": [],
    "root": {
      "children": [
        {
          "children": [
            {
              "children": [
                {
                  "children": [
                    {
                      "children": [
                        {
                          "children": [
                            {
                              "children": [],
                              "position": {
                                "beginIndex": 0,
                                "beginLine": 1,
                                "endIndex": 1,
                                "endLine": 1
                              },
                              "ruleType": "idtf_system",
                              "token": "x"
                            }
                          ],
                          "position": {
                            "beginIndex": 0,
                            "beginLine": 1,
                            "endIndex": 1,
                            "endLine": 1
                          },
                          "ruleType": "idtf_atomic"
                        }
                      ],
                      "position": {
                        "beginIndex": 0,
                        "beginLine": 1,
                        "endIndex": 1,
                        "endLine": 1
                      },
                      "ruleType": "idtf_common"
                    },
                    {
                      "children": [
                        {
                          "children": [],
                          "position": {
                            "beginIndex": 2,
                            "beginLine": 1,
                            "endIndex": 4,
                            "endLine": 1
                          },
                          "ruleType": "connector",
                          "token": "->"
                        },
                        {
                          "children": [
                            {
                              "children": [
                                {
                                  "children": [
                                    {
                                      "children": [],
                                      "position": {
                                        "beginIndex": 5,
                                        "beginLine": 1,
                                        "endIndex": 7,
                                        "endLine": 1
                                      },
                                      "ruleType": "contour_begin",
                                      "token": "[*"
                                    },
                                    {
                                      "children": [
                                        {
                                          "children": [
                                            {
                                              "children": [
                                                {
                                                  "children": [
                                                    {
                                                      "children": [
                                                        {
                                                          "children": [],
                                                          "position": {
                                                            "beginIndex": 8,
                                                            "beginLine": 1,
                                                            "endIndex": 9,
                                                            "endLine": 1
                                                          },
                                                          "ruleType": "idtf_system",
                                                          "token": "y"
                                                        }
                                                      ],
                                                      "position": {
                                                        "beginIndex": 8,
                                                        "beginLine": 1,
                                                        "endIndex": 9,
                                                        "endLine": 1
                                                      },
                                                      "ruleType": "idtf_atomic"
                                                    }
                                                  ],
                                                  "position": {
                                                    "beginIndex": 8,
                                                    "beginLine": 1,
                                                    "endIndex": 9,
                                                    "endLine": 1
                                                  },
                                                  "ruleType": "idtf_common"
                                                },
                                                {
                                                  "children": [
                                                    {
                                                      "children": [],
                                                      "position": {
                                                        "beginIndex": 10,
                                                        "beginLine": 1,
                                                        "endIndex": 13,
                                                        "endLine": 1
                                                      },
                                                      "ruleType": "connector",
                                                      "token": "_=>"
                                                    },
                                                    {
                                                      "children": [
                                                        {
                                                          "children": [
                                                            {
                                                              "children": [],
                                                              "position": {
                                                                "beginIndex": 14,
                                                                "beginLine": 1,
                                                                "endIndex": 21,
                                                                "endLine": 1
                                                              },
                                                              "ruleType": "content",
                                                              "token": "[test*]"
                                                            }
                                                          ],
                                                          "position": {
                                                            "beginIndex": 14,
                                                            "beginLine": 1,
                                                            "endIndex": 21,
                                                            "endLine": 1
                                                          },
                                                          "ruleType": "idtf_common"
                                                        }
                                                      ],
                                                      "position": {
                                                        "beginIndex": 14,
                                                        "beginLine": 1,
                                                        "endIndex": 21,
                                                        "endLine": 1
                                                      },
                                                      "ruleType": "idtf_list"
                                                    }
                                                  ],
                                                  "position": {
                                                    "beginIndex": 10,
                                                    "beginLine": 1,
                                                    "endIndex": 21,
                                                    "endLine": 1
                                                  },
                                                  "ruleType": "sentence_lvl_4_list_item"
                                                }
                                              ],
                                              "position": {
                                                "beginIndex": 8,
                                                "beginLine": 1,
                                                "endIndex": 21,
                                                "endLine": 1
                                              },
                                              "ruleType": "sentence_lvl_common"
                                            }
                                          ],
                                          "position": {
                                            "beginIndex": 8,
                                            "beginLine": 1,
                                            "endIndex": 21,
                                            "endLine": 1
                                          },
                                          "ruleType": "sentence"
                                        }
                                      ],
                                      "position": {
                                        "beginIndex": 8,
                                        "beginLine": 1,
                                        "endIndex": 23,
                                        "endLine": 1
                                      },
                                      "ruleType": "sentence_wrap",
                                      "token": ";;"
                                    },
                                    {
                                      "children": [],
                                      "position": {
                                        "beginIndex": 24,
                                        "beginLine": 1,
                                        "endIndex": 26,
                                        "endLine": 1
                                      },
                                      "ruleType": "contour_end",
                                      "token": "*]"
                                    }
                                  ],
                                  "position": {
                                    "beginIndex": 5,
                                    "beginLine": 1,
                                    "endIndex": 26,
                                    "endLine": 1
                                  },
                                  "ruleType": "contour"
                                }
                              ],
                              "position": {
                                "beginIndex": 5,
                                "beginLine": 1,
                                "endIndex": 26,
                                "endLine": 1
                              },
                              "ruleType": "idtf_common"
                            }
                          ],
                          "position": {
                            "beginIndex": 5,
                            "beginLine": 1,
                            "endIndex": 26,
                            "endLine": 1
                          },
                          "ruleType": "idtf_list"
                        }
                      ],
                      "position": {
                        "beginIndex": 2,
                        "beginLine": 1,
                        "endIndex": 26,
                        "endLine": 1
                      },
                      "ruleType": "sentence_lvl_4_list_item"
                    }
                  ],
                  "position": {
                    "beginIndex": 0,
                    "beginLine": 1,
                    "endIndex": 26,
                    "endLine": 1
                  },
                  "ruleType": "sentence_lvl_common"
                }
              ],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 26,
                "endLine": 1
              },
              "ruleType": "sentence"
            }
          ],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 28,
            "endLine": 1
          },
          "ruleType": "sentence_wrap",
          "token": ";;"
        }
      ],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 33,
        "endLine": 1
      },
      "ruleType": "syntax",
      "token": "<EOF>"
    }
  })"_json, ScJson::parse(ast)));
}

TEST(SCsASTTest, BuildAST_ForInputMissingSentenceTerminator_ReportsError)
{
  sc_char const * data = "x";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_TRUE(json_equal(R"({
    "errors": [
      {
        "msg": "no viable alternative at input 'x'",
        "position": {
          "beginIndex": 1,
          "beginLine": 1,
          "endIndex": 6,
          "endLine": 1
        },
        "token": "<EOF>"
      }
    ],
    "root": {
      "children": [
        {
          "children": [
            {
              "children": [],
              "position": {
                "beginIndex": 0,
                "beginLine": 1
              },
              "ruleType": "sentence"
            }
          ],
          "position": {
            "beginIndex": 0,
            "beginLine": 1
          },
          "ruleType": "sentence_wrap"
        },
        {
          "children": [
            {
              "children": [],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 1,
                "endLine": 1
              },
              "ruleType": "sentence"
            }
          ],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 1,
            "endLine": 1
          },
          "ruleType": "sentence_wrap"
        }
      ],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 6,
        "endLine": 1
      },
      "ruleType": "syntax",
      "token": "<EOF>"
    }
  })"_json, ScJson::parse(ast)));
}

TEST(SCsASTTest, BuildAST_ForInputWithExtraBracket_ReportsErrors)
{
  sc_char const * data = "x => nrel_idtf: [text]]";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_TRUE(json_equal(R"({
    "errors": [
      {
        "msg": "token recognition error at: ']'",
        "position": {
          "beginIndex": 22,
          "beginLine": 1,
          "endIndex": 22,
          "endLine": 1
        },
        "token": null
      },
      {
        "msg": "missing ';;' at '<EOF>'",
        "position": {
          "beginIndex": 23,
          "beginLine": 1,
          "endIndex": 28,
          "endLine": 1
        },
        "token": "<EOF>"
      }
    ],
    "root": {
      "children": [
        {
          "children": [
            {
              "children": [
                {
                  "children": [
                    {
                      "children": [
                        {
                          "children": [],
                          "position": {
                            "beginIndex": 2,
                            "beginLine": 1,
                            "endIndex": 4,
                            "endLine": 1
                          },
                          "ruleType": "connector",
                          "token": "=>"
                        },
                        {
                          "children": [],
                          "position": {
                            "beginIndex": 5,
                            "beginLine": 1,
                            "endIndex": 15,
                            "endLine": 1
                          },
                          "ruleType": "attr_list",
                          "token": ":"
                        },
                        {
                          "children": [
                            {
                              "children": [
                                {
                                  "children": [],
                                  "position": {
                                    "beginIndex": 16,
                                    "beginLine": 1,
                                    "endIndex": 22,
                                    "endLine": 1
                                  },
                                  "ruleType": "content",
                                  "token": "[text]"
                                }
                              ],
                              "position": {
                                "beginIndex": 16,
                                "beginLine": 1,
                                "endIndex": 22,
                                "endLine": 1
                              },
                              "ruleType": "idtf_common"
                            }
                          ],
                          "position": {
                            "beginIndex": 16,
                            "beginLine": 1,
                            "endIndex": 22,
                            "endLine": 1
                          },
                          "ruleType": "idtf_list"
                        }
                      ],
                      "position": {
                        "beginIndex": 2,
                        "beginLine": 1,
                        "endIndex": 22,
                        "endLine": 1
                      },
                      "ruleType": "sentence_lvl_4_list_item"
                    },
                    {
                      "children": [
                        {
                          "children": [
                            {
                              "children": [],
                              "position": {
                                "beginIndex": 0,
                                "beginLine": 1,
                                "endIndex": 1,
                                "endLine": 1
                              },
                              "ruleType": "idtf_system",
                              "token": "x"
                            }
                          ],
                          "position": {
                            "beginIndex": 0,
                            "beginLine": 1,
                            "endIndex": 1,
                            "endLine": 1
                          },
                          "ruleType": "idtf_atomic"
                        }
                      ],
                      "position": {
                        "beginIndex": 0,
                        "beginLine": 1,
                        "endIndex": 1,
                        "endLine": 1
                      },
                      "ruleType": "idtf_common"
                    }
                  ],
                  "position": {
                    "beginIndex": 0,
                    "beginLine": 1,
                    "endIndex": 22,
                    "endLine": 1
                  },
                  "ruleType": "sentence_lvl_common"
                }
              ],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 22,
                "endLine": 1
              },
              "ruleType": "sentence"
            }
          ],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 22,
            "endLine": 1
          },
          "ruleType": "sentence_wrap"
        }
      ],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 28,
        "endLine": 1
      },
      "ruleType": "syntax",
      "token": "<EOF>"
    }
  })"_json, ScJson::parse(ast)));
}

TEST(SCsASTTest, BuildAST_ForInputWithInvalidTrailingCharacters_ReportsErrors)
{
  sc_char const * data = "x => nrel_idtf: adsa;;жоо";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_TRUE(json_equal(R"({
    "errors": [
      {
        "msg": "token recognition error at: 'ж'",
        "position": {
          "beginIndex": 22,
          "beginLine": 1,
          "endIndex": 22,
          "endLine": 1
        },
        "token": null
      },
      {
        "msg": "token recognition error at: 'о'",
        "position": {
          "beginIndex": 23,
          "beginLine": 1,
          "endIndex": 23,
          "endLine": 1
        },
        "token": null
      },
      {
        "msg": "token recognition error at: 'о'",
        "position": {
          "beginIndex": 24,
          "beginLine": 1,
          "endIndex": 24,
          "endLine": 1
        },
        "token": null
      }
    ],
    "root": {
      "children": [
        {
          "children": [
            {
              "children": [
                {
                  "children": [
                    {
                      "children": [
                        {
                          "children": [],
                          "position": {
                            "beginIndex": 2,
                            "beginLine": 1,
                            "endIndex": 4,
                            "endLine": 1
                          },
                          "ruleType": "connector",
                          "token": "=>"
                        },
                        {
                          "children": [
                            {
                              "children": [
                                {
                                  "children": [
                                    {
                                      "children": [],
                                      "position": {
                                        "beginIndex": 16,
                                        "beginLine": 1,
                                        "endIndex": 20,
                                        "endLine": 1
                                      },
                                      "ruleType": "idtf_system",
                                      "token": "adsa"
                                    }
                                  ],
                                  "position": {
                                    "beginIndex": 16,
                                    "beginLine": 1,
                                    "endIndex": 20,
                                    "endLine": 1
                                  },
                                  "ruleType": "idtf_atomic"
                                }
                              ],
                              "position": {
                                "beginIndex": 16,
                                "beginLine": 1,
                                "endIndex": 20,
                                "endLine": 1
                              },
                              "ruleType": "idtf_common"
                            }
                          ],
                          "position": {
                            "beginIndex": 16,
                            "beginLine": 1,
                            "endIndex": 20,
                            "endLine": 1
                          },
                          "ruleType": "idtf_list"
                        },
                        {
                          "children": [],
                          "position": {
                            "beginIndex": 5,
                            "beginLine": 1,
                            "endIndex": 15,
                            "endLine": 1
                          },
                          "ruleType": "attr_list",
                          "token": ":"
                        }
                      ],
                      "position": {
                        "beginIndex": 2,
                        "beginLine": 1,
                        "endIndex": 20,
                        "endLine": 1
                      },
                      "ruleType": "sentence_lvl_4_list_item"
                    },
                    {
                      "children": [
                        {
                          "children": [
                            {
                              "children": [],
                              "position": {
                                "beginIndex": 0,
                                "beginLine": 1,
                                "endIndex": 1,
                                "endLine": 1
                              },
                              "ruleType": "idtf_system",
                              "token": "x"
                            }
                          ],
                          "position": {
                            "beginIndex": 0,
                            "beginLine": 1,
                            "endIndex": 1,
                            "endLine": 1
                          },
                          "ruleType": "idtf_atomic"
                        }
                      ],
                      "position": {
                        "beginIndex": 0,
                        "beginLine": 1,
                        "endIndex": 1,
                        "endLine": 1
                      },
                      "ruleType": "idtf_common"
                    }
                  ],
                  "position": {
                    "beginIndex": 0,
                    "beginLine": 1,
                    "endIndex": 20,
                    "endLine": 1
                  },
                  "ruleType": "sentence_lvl_common"
                }
              ],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 20,
                "endLine": 1
              },
              "ruleType": "sentence"
            }
          ],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 22,
            "endLine": 1
          },
          "ruleType": "sentence_wrap",
          "token": ";;"
        }
      ],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 30,
        "endLine": 1
      },
      "ruleType": "syntax",
      "token": "<EOF>"
    }
  })"_json, ScJson::parse(ast)));
}
