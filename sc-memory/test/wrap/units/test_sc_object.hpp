/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"

#include "test_sc_object.generated.hpp"

namespace n1
{
    namespace n2
    {
        class TestObject : public ScObject
        {
			SC_CLASS()
            SC_GENERATED_BODY()

        public:
            /// TODO: autogenerate code to call _initInternal
            TestObject() { init(); } /// TEST

        public:

            SC_PROPERTY(Keynode("test_keynode1"))
            ScAddr mTestKeynode1;

            SC_PROPERTY(Keynode("test_keynode2"))
            ScAddr mTestKeynode2;

            SC_PROPERTY(Keynode("test_keynode3"))
            static ScAddr mTestKeynode3;

            // not created object
            SC_PROPERTY(Keynode("test_keynode_force"), ForceCreate)
            ScAddr mTestKeynodeForce;

			SC_PROPERTY(Template("test_template"))
			ScTemplate mTestTemplate;

			SC_PROPERTY(Template("test_template2"))
			static ScTemplate mTestTemplate2;
        };
    }
}