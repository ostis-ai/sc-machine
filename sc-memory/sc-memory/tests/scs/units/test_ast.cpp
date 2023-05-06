/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "test_scs_utils.hpp"

TEST(scs_ast, CorrectAST1)
{
  sc_char const * data = "x -> y;;";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_EQ(R"({
    "children": [{
      "children": [{
        "children": [{
          "children": [{
            "children": [{
              "children": [],
              "position": {
                "beginIndex": 2,
                "beginLine": 1,
                "endIndex": 2,
                "endLine": 1
              },
              "ruleType": "connector",
              "token": "->"
            }, {
              "children": [{
                "children": [{
                  "children": [{
                    "children": [],
                    "position": {
                      "beginIndex": 4,
                      "beginLine": 1,
                      "endIndex": 4,
                      "endLine": 1
                    },
                    "ruleType": "idtf_system",
                    "token": "y"
                  }],
                  "position": {
                    "beginIndex": 4,
                    "beginLine": 1,
                    "endIndex": 4,
                    "endLine": 1
                  },
                  "ruleType": "idtf_atomic"
                }],
                "position": {
                  "beginIndex": 4,
                  "beginLine": 1,
                  "endIndex": 4,
                  "endLine": 1
                },
                "ruleType": "idtf_common"
              }],
              "position": {
                "beginIndex": 4,
                "beginLine": 1,
                "endIndex": 4,
                "endLine": 1
              },
              "ruleType": "idtf_list"
            }],
            "position": {
              "beginIndex": 2,
              "beginLine": 1,
              "endIndex": 4,
              "endLine": 1
            },
            "ruleType": "sentence_lvl_4_list_item"
          }, {
            "children": [{
              "children": [{
                "children": [],
                "position": {
                  "beginIndex": 0,
                  "beginLine": 1,
                  "endIndex": 0,
                  "endLine": 1
                },
                "ruleType": "idtf_system",
                "token": "x"
              }],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 0,
                "endLine": 1
              },
              "ruleType": "idtf_atomic"
            }],
            "position": {
              "beginIndex": 0,
              "beginLine": 1,
              "endIndex": 0,
              "endLine": 1
            },
            "ruleType": "idtf_common"
          }],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 4,
            "endLine": 1
          },
          "ruleType": "sentence_lvl_common"
        }],
        "position": {
          "beginIndex": 0,
          "beginLine": 1,
          "endIndex": 4,
          "endLine": 1
        },
        "ruleType": "sentence"
      }],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 5,
        "endLine": 1
      },
      "ruleType": "sentence_wrap",
      "token": ";;"
    }],
    "errors": [],
    "ruleType": "syntax"
    })"_json.dump(), ast);
}

TEST(scs_ast, CorrectAST2)
{
  sc_char const * data = "x -> [* y _=> [test*];; *];;";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_EQ(R"({
    "children": [{
      "children": [{
        "children": [{
          "children": [{
            "children": [{
              "children": [],
              "position": {
                "beginIndex": 2,
                "beginLine": 1,
                "endIndex": 2,
                "endLine": 1
              },
              "ruleType": "connector",
              "token": "->"
            }, {
              "children": [{
                "children": [{
                  "children": [{
                    "children": [{
                      "children": [{
                        "children": [{
                          "children": [{
                            "children": [],
                            "position": {
                              "beginIndex": 8,
                              "beginLine": 1,
                              "endIndex": 8,
                              "endLine": 1
                            },
                            "ruleType": "connector",
                            "token": "_=>"
                          }, {
                            "children": [{
                              "children": [{
                                "children": [],
                                "position": {
                                  "beginIndex": 10,
                                  "beginLine": 1,
                                  "endIndex": 10,
                                  "endLine": 1
                                },
                                "ruleType": "content",
                                "token": "[test*]"
                              }],
                              "position": {
                                "beginIndex": 10,
                                "beginLine": 1,
                                "endIndex": 10,
                                "endLine": 1
                              },
                              "ruleType": "idtf_common"
                            }],
                            "position": {
                              "beginIndex": 10,
                              "beginLine": 1,
                              "endIndex": 10,
                              "endLine": 1
                            },
                            "ruleType": "idtf_list"
                          }],
                          "position": {
                            "beginIndex": 8,
                            "beginLine": 1,
                            "endIndex": 10,
                            "endLine": 1
                          },
                          "ruleType": "sentence_lvl_4_list_item"
                        }, {
                          "children": [{
                            "children": [{
                              "children": [],
                              "position": {
                                "beginIndex": 6,
                                "beginLine": 1,
                                "endIndex": 6,
                                "endLine": 1
                              },
                              "ruleType": "idtf_system",
                              "token": "y"
                            }],
                            "position": {
                              "beginIndex": 6,
                              "beginLine": 1,
                              "endIndex": 6,
                              "endLine": 1
                            },
                            "ruleType": "idtf_atomic"
                          }],
                          "position": {
                            "beginIndex": 6,
                            "beginLine": 1,
                            "endIndex": 6,
                            "endLine": 1
                          },
                          "ruleType": "idtf_common"
                        }],
                        "position": {
                          "beginIndex": 6,
                          "beginLine": 1,
                          "endIndex": 10,
                          "endLine": 1
                        },
                        "ruleType": "sentence_lvl_common"
                      }],
                      "position": {
                        "beginIndex": 6,
                        "beginLine": 1,
                        "endIndex": 10,
                        "endLine": 1
                      },
                      "ruleType": "sentence"
                    }],
                    "position": {
                      "beginIndex": 6,
                      "beginLine": 1,
                      "endIndex": 11,
                      "endLine": 1
                    },
                    "ruleType": "sentence_wrap",
                    "token": ";;"
                  }],
                  "position": {
                    "beginIndex": 4,
                    "beginLine": 1,
                    "endIndex": 13,
                    "endLine": 1
                  },
                  "ruleType": "contour",
                  "token": "*]"
                }],
                "position": {
                  "beginIndex": 4,
                  "beginLine": 1,
                  "endIndex": 13,
                  "endLine": 1
                },
                "ruleType": "idtf_common"
              }],
              "position": {
                "beginIndex": 4,
                "beginLine": 1,
                "endIndex": 13,
                "endLine": 1
              },
              "ruleType": "idtf_list"
            }],
            "position": {
              "beginIndex": 2,
              "beginLine": 1,
              "endIndex": 13,
              "endLine": 1
            },
            "ruleType": "sentence_lvl_4_list_item"
          }, {
            "children": [{
              "children": [{
                "children": [],
                "position": {
                  "beginIndex": 0,
                  "beginLine": 1,
                  "endIndex": 0,
                  "endLine": 1
                },
                "ruleType": "idtf_system",
                "token": "x"
              }],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 0,
                "endLine": 1
              },
              "ruleType": "idtf_atomic"
            }],
            "position": {
              "beginIndex": 0,
              "beginLine": 1,
              "endIndex": 0,
              "endLine": 1
            },
            "ruleType": "idtf_common"
          }],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 13,
            "endLine": 1
          },
          "ruleType": "sentence_lvl_common"
        }],
        "position": {
          "beginIndex": 0,
          "beginLine": 1,
          "endIndex": 13,
          "endLine": 1
        },
        "ruleType": "sentence"
      }],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 14,
        "endLine": 1
      },
      "ruleType": "sentence_wrap",
      "token": ";;"
    }],
    "errors": [],
    "ruleType": "syntax"
    })"_json.dump(), ast);
}

TEST(scs_ast, IncorrectAST1)
{
  sc_char const * data = "x";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_EQ(R"({
    "children": [{
      "children": [{
        "children": [],
        "position": {
          "beginIndex": 0,
          "beginLine": 1
        },
        "ruleType": "sentence"
      }],
      "position": {
        "beginIndex": 0,
        "beginLine": 1
      },
      "ruleType": "sentence_wrap"
    }, {
      "children": [{
        "children": [],
        "position": {
          "beginIndex": 0,
          "beginLine": 1,
          "endIndex": 0,
          "endLine": 1
        },
        "ruleType": "sentence"
      }],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 0,
        "endLine": 1
      },
      "ruleType": "sentence_wrap"
    }],
    "errors": [{
      "charPositionInLine": 1,
      "line": 1,
      "msg": "no viable alternative at input 'x'",
      "token": "<EOF>"
    }],
    "ruleType": "syntax"
    })"_json.dump(), ast);
}

TEST(scs_ast, IncorrectAST2)
{
  sc_char const * data = "x => nrel_idtf: [text]]";

  scs::Parser parser;
  std::string const & ast = parser.BuildAST(data);

  EXPECT_EQ(R"({
    "children": [{
      "children": [{
        "children": [{
          "children": [{
            "children": [{
              "children": [],
              "position": {
                "beginIndex": 2,
                "beginLine": 1,
                "endIndex": 2,
                "endLine": 1
              },
              "ruleType": "connector",
              "token": "=>"
            }, {
              "children": [],
              "position": {
                "beginIndex": 4,
                "beginLine": 1,
                "endIndex": 5,
                "endLine": 1
              },
              "ruleType": "attr_list",
              "token": ":"
            }, {
              "children": [{
                "children": [{
                  "children": [],
                  "position": {
                    "beginIndex": 7,
                    "beginLine": 1,
                    "endIndex": 7,
                    "endLine": 1
                  },
                  "ruleType": "content",
                  "token": "[text]"
                }],
                "position": {
                  "beginIndex": 7,
                  "beginLine": 1,
                  "endIndex": 7,
                  "endLine": 1
                },
                "ruleType": "idtf_common"
              }],
              "position": {
                "beginIndex": 7,
                "beginLine": 1,
                "endIndex": 7,
                "endLine": 1
              },
              "ruleType": "idtf_list"
            }],
            "position": {
              "beginIndex": 2,
              "beginLine": 1,
              "endIndex": 7,
              "endLine": 1
            },
            "ruleType": "sentence_lvl_4_list_item"
          }, {
            "children": [{
              "children": [{
                "children": [],
                "position": {
                  "beginIndex": 0,
                  "beginLine": 1,
                  "endIndex": 0,
                  "endLine": 1
                },
                "ruleType": "idtf_system",
                "token": "x"
              }],
              "position": {
                "beginIndex": 0,
                "beginLine": 1,
                "endIndex": 0,
                "endLine": 1
              },
              "ruleType": "idtf_atomic"
            }],
            "position": {
              "beginIndex": 0,
              "beginLine": 1,
              "endIndex": 0,
              "endLine": 1
            },
            "ruleType": "idtf_common"
          }],
          "position": {
            "beginIndex": 0,
            "beginLine": 1,
            "endIndex": 7,
            "endLine": 1
          },
          "ruleType": "sentence_lvl_common"
        }],
        "position": {
          "beginIndex": 0,
          "beginLine": 1,
          "endIndex": 7,
          "endLine": 1
        },
        "ruleType": "sentence"
      }],
      "position": {
        "beginIndex": 0,
        "beginLine": 1,
        "endIndex": 7,
        "endLine": 1
      },
      "ruleType": "sentence_wrap"
    }],
    "errors": [{
      "charPositionInLine": 22,
      "line": 1,
      "msg": "token recognition error at: ']'",
      "token": "Invalid token"
    }, {
      "charPositionInLine": 23,
      "line": 1,
      "msg": "missing ';;' at '<EOF>'",
      "token": "<EOF>"
    }],
    "ruleType": "syntax"
    })"_json.dump(), ast);
}
