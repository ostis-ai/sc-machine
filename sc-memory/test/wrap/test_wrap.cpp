/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "wrap/sc_memory_headers.hpp"
#include "test_sc_object.hpp"
#include <glib.h>

void init_memory()
{
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.config_file = "sc-memory.ini";
    params.ext_path = 0;

    ScMemory::initialize(params);
}

void shutdown_memory(bool save)
{
    ScMemory::shutdown(save);
}

bool has_addr(tAddrVector const & v, ScAddr const & addr)
{
    for (tAddrVector::const_iterator it = v.begin(); it != v.end(); ++it)
    {
        if (*it == addr)
            return true;
    }

    return false;
}


void test_common_elements()
{
    init_memory();

    {
        ScMemoryContext ctx;
        ScAddr addr = ctx.createNode(sc_type_const);
        g_assert(addr.isValid());

        ScAddr link = ctx.createLink();
        g_assert(link.isValid());

        ScAddr arc = ctx.createArc(sc_type_arc_pos_const_perm, addr, link);
        g_assert(arc.isValid());

        g_assert(ctx.isElement(addr));
        g_assert(ctx.isElement(link));
        g_assert(ctx.isElement(arc));

        g_assert(ctx.getArcBegin(arc) == addr);
        g_assert(ctx.getArcEnd(arc) == link);

        g_assert(ctx.getElementType(addr) == (sc_type_node | sc_type_const));
        g_assert(ctx.getElementType(link) == sc_type_link);
        g_assert(ctx.getElementType(arc) == sc_type_arc_pos_const_perm);

        g_assert(ctx.setElementSubtype(addr, sc_type_var));
        g_assert(ctx.getElementType(addr) == (sc_type_node | sc_type_var));

        g_assert(ctx.eraseElement(addr));
        g_assert(!ctx.isElement(addr));
        g_assert(!ctx.isElement(arc));
        g_assert(ctx.isElement(link));
    }

    shutdown_memory(false);
}

void test_common_iterators()
{
    init_memory();

    {
        ScMemoryContext ctx;
        ScAddr addr1 = ctx.createNode(sc_type_const);
        ScAddr addr2 = ctx.createNode(sc_type_var);
        ScAddr arc1 = ctx.createArc(sc_type_arc_pos_const_perm, addr1, addr2);

        g_assert(addr1.isValid());
        g_assert(addr2.isValid());
        g_assert(arc1.isValid());

        g_assert(ctx.isElement(addr1));
        g_assert(ctx.isElement(addr2));
        g_assert(ctx.isElement(arc1));

        {
            ScIterator3Ptr iter3 = ctx.iterator3(addr1, sc_type_arc_pos_const_perm, addr2);
            g_assert(iter3->next());
            g_assert(iter3->value(0) == addr1);
            g_assert(iter3->value(1) == arc1);
            g_assert(iter3->value(2) == addr2);
        }

        {
            ScIterator3Ptr iter3 = ctx.iterator3(addr1, sc_type_arc_pos_const_perm, sc_type_node);
            g_assert(iter3->next());
            g_assert(iter3->value(0) == addr1);
            g_assert(iter3->value(1) == arc1);
            g_assert(iter3->value(2) == addr2);
        }

        {
            ScIterator3Ptr iter3 = ctx.iterator3(sc_type_node, sc_type_arc_pos_const_perm, addr2);
            g_assert(iter3->next());
            g_assert(iter3->value(0) == addr1);
            g_assert(iter3->value(1) == arc1);
            g_assert(iter3->value(2) == addr2);
        }

        ScAddr addr3 = ctx.createNode(sc_type_const);
        ScAddr arc2 = ctx.createArc(sc_type_arc_pos_const_perm, addr3, arc1);

        g_assert(addr3.isValid());
        g_assert(arc2.isValid());

        g_assert(ctx.isElement(addr3));
        g_assert(ctx.isElement(arc2));

        {
            ScIterator5Ptr iter5 = ctx.iterator5(sc_type_node, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, sc_type_node);

            g_assert(iter5->next());

            g_assert(iter5->value(0) == addr1);
            g_assert(iter5->value(1) == arc1);
            g_assert(iter5->value(2) == addr2);
            g_assert(iter5->value(3) == arc2);
            g_assert(iter5->value(4) == addr3);
        }

        {
            ScIterator5Ptr iter5 = ctx.iterator5(sc_type_node, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, addr3);

            g_assert(iter5->next());

            g_assert(iter5->value(0) == addr1);
            g_assert(iter5->value(1) == arc1);
            g_assert(iter5->value(2) == addr2);
            g_assert(iter5->value(3) == arc2);
            g_assert(iter5->value(4) == addr3);
        }

        {
            ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, sc_type_node);

            g_assert(iter5->next());

            g_assert(iter5->value(0) == addr1);
            g_assert(iter5->value(1) == arc1);
            g_assert(iter5->value(2) == addr2);
            g_assert(iter5->value(3) == arc2);
            g_assert(iter5->value(4) == addr3);
        }

        {
            ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, addr3);

            g_assert(iter5->next());

            g_assert(iter5->value(0) == addr1);
            g_assert(iter5->value(1) == arc1);
            g_assert(iter5->value(2) == addr2);
            g_assert(iter5->value(3) == arc2);
            g_assert(iter5->value(4) == addr3);
        }

        {
            ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, sc_type_node);

            g_assert(iter5->next());

            g_assert(iter5->value(0) == addr1);
            g_assert(iter5->value(1) == arc1);
            g_assert(iter5->value(2) == addr2);
            g_assert(iter5->value(3) == arc2);
            g_assert(iter5->value(4) == addr3);
        }

        {
            ScIterator5Ptr iter5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, addr2, sc_type_arc_pos_const_perm, addr3);

            g_assert(iter5->next());

            g_assert(iter5->value(0) == addr1);
            g_assert(iter5->value(1) == arc1);
            g_assert(iter5->value(2) == addr2);
            g_assert(iter5->value(3) == arc2);
            g_assert(iter5->value(4) == addr3);
        }

        {
            std::string str("test content string");
            ScStream stream(str.c_str(), (sc_uint32)str.size(), SC_STREAM_FLAG_READ);
            ScAddr link = ctx.createLink();

            g_assert(link.isValid());
            g_assert(ctx.isElement(link));
            g_assert(ctx.setLinkContent(link, stream));

            ScStream stream2;
            g_assert(ctx.getLinkContent(link, stream2));

            g_assert(stream.size() == stream2.size());
            g_assert(stream2.seek(SC_STREAM_SEEK_SET, 0));

            std::string str2;
            str2.resize(stream2.size());
            for (sc_uint i = 0; i < stream2.size(); ++i)
            {
                sc_char c;
                sc_uint32 readBytes;
                g_assert(stream2.read(&c, sizeof(c), readBytes));
                g_assert(readBytes == sizeof(c));

                str2[i] = c;
            }

            g_assert(str == str2);

            tAddrList result;
            g_assert(ctx.findLinksByContent(stream, result));
            g_assert(result.size() == 1);
            g_assert(result.front() == link);
        }

    }

    shutdown_memory(false);
}

void test_common_streams()
{
    init_memory();

    {
        static int const length = 1024;
        unsigned char buff[length];

        for (int i = 0; i < length; ++i)
            buff[i] = rand() % 256;

        ScStream stream((sc_char*)buff, length, SC_STREAM_FLAG_READ);

        g_assert(stream.isValid());
        g_assert(stream.hasFlag(SC_STREAM_FLAG_READ));
        g_assert(stream.hasFlag(SC_STREAM_FLAG_SEEK));
        g_assert(stream.hasFlag(SC_STREAM_FLAG_TELL));
        g_assert(!stream.eof());
        g_assert(stream.pos() == 0);
        g_assert(stream.size() == length);

        for (int i = 0; i < length; ++i)
        {
            unsigned char c;
            sc_uint32 readBytes;
            g_assert(stream.read((sc_char*)&c, sizeof(c), readBytes));
            g_assert(c == buff[i]);
        }

        g_assert(stream.eof());
        g_assert(stream.pos() == length);

        // random seek
        static int const seekOpCount = 1000000;
        for (int i = 0; i < seekOpCount; ++i)
        {
            sc_uint32 pos = rand() % length;
            g_assert(stream.seek(SC_STREAM_SEEK_SET, pos));


            unsigned char c;
            sc_uint32 readBytes;
            g_assert(stream.read((sc_char*)&c, sizeof(c), readBytes));
            g_assert(c == buff[pos]);
        }
    }

    shutdown_memory(false);
}

void test_common_templates()
{
	init_memory();
	{
		ScMemoryContext ctx;
		ScAddr const addr1 = ctx.createNode(0);
		ScAddr const addr2 = ctx.createNode(0);
		ScAddr const addr3 = ctx.createNode(0);

		ScAddr const edge1 = ctx.createArc(sc_type_arc_pos_const_perm, addr1, addr2);
		ScAddr const edge2 = ctx.createArc(sc_type_arc_pos_const_perm, addr3, edge1);

		{
			ScTemplate templ;

			templ
				.triple(addr1 >> "addr1",
						ScType(sc_type_arc_pos_const_perm) >> "edge1",
						ScType(sc_type_node) >> "addr2"
				)
                .triple(ScType(sc_type_node),
						ScType(sc_type_arc_pos_const_perm),
						"edge1"
				)
                .triple("addr2",
						ScType(sc_type_arc_common),
						"edge1");

			ScTemplateGenResult result;
			g_assert(ctx.helperGenTemplate(templ, result));

			g_assert(result[0] == result["addr1"]);
			g_assert(result[1] == result["edge1"]);
			g_assert(result[2] == result["addr2"]);
			g_assert(result[5] == result["edge1"]);
			g_assert(result[6] == result["addr2"]);
			g_assert(result[8] == result["edge1"]);

            ScIterator5Ptr it5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, sc_type_node);
			g_assert(it5->next());
			g_assert(it5->value(0) == result["addr1"]);
			g_assert(it5->value(1) == result["edge1"]);
			g_assert(it5->value(2) == result["addr2"]);
			g_assert(it5->value(3) == result[4]);
			g_assert(it5->value(4) == result[3]);

            ScIterator3Ptr it3 = ctx.iterator3(result["addr2"], sc_type_arc_common, sc_type_arc_pos_const_perm);
			g_assert(it3->next());
			g_assert(it3->value(0) == result["addr2"]);
			g_assert(it3->value(1) == result[7]);
            g_assert(it3->value(2) == result["edge1"]);


            // test template search
            {
                ScTemplateSearchResult searchResult;
                g_assert(ctx.helperSearchTemplate(templ, searchResult));

                g_assert(searchResult.getSize() == 1);

                ScTemplateSearchResultItem const & res = searchResult.getResult(0);
                g_assert(res[0] == res["addr1"]);
                g_assert(res[1] == res["edge1"]);
                g_assert(res[2] == res["addr2"]);
                g_assert(res[5] == res["edge1"]);
                g_assert(res[6] == res["addr2"]);
                g_assert(res[8] == res["edge1"]);

                g_assert(it5->value(0) == res["addr1"]);
                g_assert(it5->value(1) == res["edge1"]);
                g_assert(it5->value(2) == res["addr2"]);
                g_assert(it5->value(3) == res[4]);
                g_assert(it5->value(4) == res[3]);

                g_assert(it3->value(0) == res["addr2"]);
                g_assert(it3->value(1) == res[7]);
                g_assert(it3->value(2) == res["edge1"]);
            }

            // template search test 2
            {
                size_t const testCount = 10;
                tAddrVector nodes, edges;

                ScAddr addrSrc = ctx.createNode(sc_type_const);
                g_assert(addrSrc.isValid());
                for (size_t i = 0; i < testCount; ++i)
                {
                    ScAddr const addrTrg = ctx.createNode(sc_type_var);
                    g_assert(addrTrg.isValid());

                    ScAddr const addrEdge = ctx.createArc(sc_type_arc_pos_const_perm, addrSrc, addrTrg);
                    g_assert(addrEdge.isValid());

                    nodes.push_back(addrTrg);
                    edges.push_back(addrEdge);
                }

                ScTemplate templ2;

                templ2.triple(addrSrc >> "addrSrc",
                    ScType(sc_type_arc_pos_const_perm) >> "edge",
                    ScType(sc_type_node | sc_type_var) >> "addrTrg");

                ScTemplateSearchResult result2;
                g_assert(ctx.helperSearchTemplate(templ2, result2));
                size_t const count = result2.getSize();
                for (size_t i = 0; i < count; ++i)
                {
                    ScTemplateSearchResultItem r = result2.getResult(i);

                    g_assert(r["addrSrc"] == addrSrc);

                    g_assert(has_addr(edges, r["edge"]));
                    g_assert(has_addr(nodes, r["addrTrg"]));
                }
            }
		}
	}

	shutdown_memory(false);
}

void test_codegen_keynodes()
{
    init_memory();

    {
        ScMemoryContext ctx(sc_access_lvl_make_max);

        ScAddr addr1 = ctx.createNode(sc_type_const);
        g_assert(addr1.isValid());
        g_assert(ctx.helperSetSystemIdtf("test_keynode1", addr1));

        ScAddr addr2 = ctx.createNode(sc_type_var);
        g_assert(addr2.isValid());
        g_assert(ctx.helperSetSystemIdtf("test_keynode2", addr2));

        ScAddr addr3 = ctx.createNode(sc_type_var);
        g_assert(addr3.isValid());
        g_assert(ctx.helperSetSystemIdtf("test_keynode3", addr3));
        
        n1::n2::TestObject obj1;
        g_assert(addr1 == obj1.mTestKeynode1);
        g_assert(addr2 == obj1.mTestKeynode2);

        obj1.initGlobal();
        g_assert(addr3 == obj1.mTestKeynode3);

        ScAddr addrForce;
        g_assert(ctx.helperFindBySystemIdtf("test_keynode_force", addrForce));
        g_assert(addrForce == obj1.mTestKeynodeForce);
    }

    shutdown_memory(false);
}

int main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/common/elements", test_common_elements);
    g_test_add_func("/common/iterators", test_common_iterators);
    g_test_add_func("/common/streams", test_common_streams);
	g_test_add_func("/common/templates", test_common_templates);
    g_test_add_func("/codegen/keynodes", test_codegen_keynodes);

    g_test_run();

    return 0;
}
