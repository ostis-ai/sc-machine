/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

extern "C"
{
#include <glib.h>
}

#include "../sctpClient.hpp"

#if defined (SC_PLATFORM_WIN)
    #include "../sockets/winSocket.hpp"
    sctp::Client sctpClient(new sctp::winSocket());
#else
    #include "../sockets/glibSocket.hpp"
    sctp::Client sctpClient(new sctp::glibSocket());
#endif

void test_connection()
{
    g_assert(sctpClient.Connect("127.0.0.1", "55770"));
}

void test_base_commands()
{
    {
        ScAddr addr = sctpClient.CreateNode(sc_type_node | sc_type_node_class);

        g_assert(addr.IsValid());
        g_assert(sctpClient.IsElement(addr));
        g_assert(sctpClient.GetElementType(addr) == (sc_type_node | sc_type_node_class));
        g_assert(sctpClient.EraseElement(addr));
        g_assert(!sctpClient.IsElement(addr));
	}

	{
		ScAddr addr1 = sctpClient.CreateNode(sc_type_node);
		ScAddr addr2 = sctpClient.CreateNode(sc_type_node);

		ScAddr arc = sctpClient.CreateArc(sc_type_arc_pos_const_perm, addr1, addr2);

		g_assert(arc.IsValid());
		g_assert(sctpClient.GetElementType(arc) == sc_type_arc_pos_const_perm);

		ScAddr a1, a2;
		g_assert(sctpClient.GetArcInfo(arc, a1, a2));
		g_assert(a1 == addr1);
		g_assert(a2 == addr2);
	}

	{
		ScAddr link = sctpClient.CreateLink();
		char * data = "Test data";
		IScStreamPtr stream(new ScStream(data, (sc_uint32)strlen(data), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK));

		g_assert(link.IsValid());
		g_assert(sctpClient.SetLinkContent(link, stream));

		g_assert(stream->Seek(SC_STREAM_SEEK_SET, 0));

		IScStreamPtr stream2;
		g_assert(sctpClient.GetLinkContent(link, stream2));
		g_assert(stream->Size() == stream2->Size());
		for (sc_uint32 i = 0; i < stream->Size(); ++i)
		{
			char v1, v2;
			sc_uint32 r1, r2;
			g_assert(stream->Read(&v1, sizeof(v1), r1));
			g_assert(stream2->Read(&v2, sizeof(v2), r2));

			g_assert(r1 == r2);
			g_assert(v1 == v2);
		}
	}
}

void test_iterators()
{
	{
		ScAddr addr = sctpClient.CreateNode(0);
		ScAddr addr1 = sctpClient.CreateNode(0);
		ScAddr addr2 = sctpClient.CreateNode(0);
		ScAddr addr3 = sctpClient.CreateNode(0);

		g_assert(addr.IsValid());
		g_assert(addr1.IsValid());
		g_assert(addr2.IsValid());
		g_assert(addr3.IsValid());

		ScAddr arc1 = sctpClient.CreateArc(sc_type_arc_pos_const_perm, addr, addr1);
		ScAddr arc2 = sctpClient.CreateArc(sc_type_arc_pos_const_perm, addr, addr2);
		ScAddr arc3 = sctpClient.CreateArc(sc_type_arc_pos_const_perm, addr, addr3);

		g_assert(arc1.IsValid());
		g_assert(arc2.IsValid());
		g_assert(arc3.IsValid());

		sctp::IteratorPtr iter = sctpClient.Iterator3(addr, sc_type_arc_pos_const_perm, sc_type_node);
		g_assert(iter->next());
		g_assert(iter->getValue(0) == addr);
		g_assert(iter->getValue(1) == arc3);
		g_assert(iter->getValue(2) == addr3);

		g_assert(iter->next());
		g_assert(iter->getValue(0) == addr);
		g_assert(iter->getValue(1) == arc2);
		g_assert(iter->getValue(2) == addr2);

		g_assert(iter->next());
		g_assert(iter->getValue(0) == addr);
		g_assert(iter->getValue(1) == arc1);
		g_assert(iter->getValue(2) == addr1);

		g_assert(!iter->next());
	}

	{
		ScAddr addr = sctpClient.CreateNode(0);
		ScAddr addr1 = sctpClient.CreateNode(0);
		ScAddr addr2 = sctpClient.CreateNode(0);
		ScAddr addr3 = sctpClient.CreateNode(0);

		g_assert(addr.IsValid());
		g_assert(addr1.IsValid());
		g_assert(addr2.IsValid());
		g_assert(addr3.IsValid());

		ScAddr arc1 = sctpClient.CreateArc(sc_type_arc_pos_const_perm, addr, addr1);
		ScAddr arc2 = sctpClient.CreateArc(sc_type_arc_pos_const_perm, addr2, arc1);
		ScAddr arc3 = sctpClient.CreateArc(sc_type_arc_pos_const_perm, addr3, arc1);

		g_assert(arc1.IsValid());
		g_assert(arc2.IsValid());
		g_assert(arc3.IsValid());

		sctp::IteratorPtr iter = sctpClient.Iterator5(addr, sc_type_arc_pos_const_perm, addr1, sc_type_arc_pos_const_perm, sc_type_node);

		g_assert(iter->next());
		g_assert(iter->getValue(0) == addr);
		g_assert(iter->getValue(1) == arc1);
		g_assert(iter->getValue(2) == addr1);
		g_assert(iter->getValue(3) == arc3);
		g_assert(iter->getValue(4) == addr3);
		
		g_assert(iter->next());
		g_assert(iter->getValue(0) == addr);
		g_assert(iter->getValue(1) == arc1);
		g_assert(iter->getValue(2) == addr1);
		g_assert(iter->getValue(3) == arc2);
		g_assert(iter->getValue(4) == addr2);

		g_assert(!iter->next());
	}
}

int main(int argc, char *argv[])
{
#if defined (SC_PLATFORM_WIN)
   sctp::winSocket::Initialize();
#endif

    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/sctp/connection", test_connection);
    g_test_add_func("/sctp/base_commands", test_base_commands);
	  g_test_add_func("/sctp/iterators", test_iterators);

    g_test_run();

#if defined (SC_PLATFORM_WIN)
   sctp::winSocket::Shutdown();
#endif
    return 0;
}
