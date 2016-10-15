/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sctpCommand.h"
#include "sctpStatistic.h"
#include "sctpEventManager.h"

#include <QIODevice>
#include <QDataStream>
#include <QDebug>
#include <QBuffer>
#include <QCoreApplication>

#include <limits>
#include <assert.h>


#define SCTP_READ_TIMEOUT   3000

#define READ_PARAM(__val)   if (params->readRawData((char*)&__val, sizeof(__val)) != sizeof(__val)) \
                                  return SCTP_ERROR_CMD_READ_PARAMS;


namespace
{
	class IterConstsr
	{
	public:
		typedef std::vector<sc_addr> ScAddrVec;
		ScAddrVec m_results;

	private:
		struct IteratorData
		{
			struct IterParam
			{
				sc_iterator_param m_param;
				quint8 m_repl;

				IterParam()
					: m_repl(255)
				{
					memset(&m_param, 0, sizeof(m_param));
				}

				bool isRepl() const
				{
					return m_repl != 255;
				}
			};

			quint8 m_type;
			quint8 m_repl[5];
			quint8 m_replCount;
			IterParam m_args[5];

			sc_iterator3 *m_it3;
			sc_iterator5 *m_it5;

			IteratorData()
				: m_type(SCTP_ITERATOR_COUNT)
				, m_replCount(0)
				, m_it3(0)
				, m_it5(0)
			{
				memset(&m_repl[0], 255, sizeof(uint8_t)* 5);
			}

			~IteratorData()
			{
				stopIterate();
			}

			quint8 argsCount() const
			{
				if (m_type >= SCTP_ITERATOR_3F_A_A && m_type <= SCTP_ITERATOR_3F_A_F)
					return 3;
				if (m_type >= SCTP_ITERATOR_5F_A_A_A_F && m_type <= SCTP_ITERATOR_5A_A_F_A_A)
					return 5;

				return 0;
			}

			quint8 fixedCount() const
			{
				switch (m_type)
				{
				case SCTP_ITERATOR_5F_A_A_A_A:
				case SCTP_ITERATOR_5A_A_F_A_A:
				case SCTP_ITERATOR_3F_A_A:
				case SCTP_ITERATOR_3A_A_F:
					return 1;

				case SCTP_ITERATOR_5F_A_F_A_A:
				case SCTP_ITERATOR_5A_A_F_A_F:
				case SCTP_ITERATOR_5F_A_A_A_F:
				case SCTP_ITERATOR_3F_A_F:
					return 2;

				case SCTP_ITERATOR_5F_A_F_A_F:
					return 3;
				}

				return 0;
			}

			quint8 assignCount() const
			{
				return argsCount() - fixedCount();
			}

			bool isValidType() const
			{
				return argsCount() != 0;
			}

			bool buildRepl(QDataStream *params)
			{
				quint8 count = fixedCount();
				params->readRawData((char*)&m_repl[0], count);

				for (quint8 i = 0; i < count; ++i)
				{
					if (m_repl[i] != 255)
						++m_replCount;
				}

				return true;
			}

			void preapreArgs()
			{
				switch (m_type)
				{
				case SCTP_ITERATOR_3F_A_F:
					m_args[0].m_param.is_type = m_args[2].m_param.is_type = false;
					m_args[1].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_3F_A_A:
					m_args[0].m_param.is_type = false;
					m_args[1].m_param.is_type = m_args[2].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_3A_A_F:
					m_args[2].m_param.is_type = false;
					m_args[0].m_param.is_type = m_args[1].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_5F_A_F_A_F:
					m_args[0].m_param.is_type = m_args[2].m_param.is_type = m_args[4].m_param.is_type = false;
					m_args[1].m_param.is_type = m_args[3].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_5A_A_F_A_F:
					m_args[2].m_param.is_type = m_args[4].m_param.is_type = false;
					m_args[0].m_param.is_type = m_args[1].m_param.is_type = m_args[3].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_5F_A_F_A_A:
					m_args[0].m_param.is_type = m_args[2].m_param.is_type = false;
					m_args[1].m_param.is_type = m_args[3].m_param.is_type = m_args[4].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_5F_A_A_A_F:
					m_args[0].m_param.is_type = m_args[4].m_param.is_type = false;
					m_args[1].m_param.is_type = m_args[2].m_param.is_type = m_args[3].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_5F_A_A_A_A:
					m_args[0].m_param.is_type = false;
					m_args[1].m_param.is_type = m_args[2].m_param.is_type = m_args[3].m_param.is_type = m_args[4].m_param.is_type = true;
					break;

				case SCTP_ITERATOR_5A_A_F_A_A:
					m_args[2].m_param.is_type = false;
					m_args[0].m_param.is_type = m_args[1].m_param.is_type = m_args[3].m_param.is_type = m_args[4].m_param.is_type = true;
					break;
				}
			}

			bool buildParams(QDataStream *params)
			{
				quint8 count = argsCount();

				preapreArgs();

				quint8 rCount = 0;
				for (quint8 i = 0; i < count; ++i)
				{
					IterParam & p = m_args[i];
					if (p.m_param.is_type)
						params->readRawData((char*)&p.m_param.type, sizeof(p.m_param.type));
					else
					{
						p.m_repl = m_repl[rCount];
						if (!p.isRepl())
							params->readRawData((char*)&p.m_param.addr, sizeof(p.m_param.addr));
						++rCount;
					}
				}

				return true;
			}

			sc_iterator3_type scIterator3Type(quint8 type) const
			{
				switch (type)
				{
				case SCTP_ITERATOR_3F_A_A:
					return sc_iterator3_f_a_a;
				case SCTP_ITERATOR_3A_A_F:
					return sc_iterator3_a_a_f;
				case SCTP_ITERATOR_3F_A_F:
					return sc_iterator3_f_a_f;
				}

				return (sc_iterator3_type)-1;
			}

			sc_iterator5_type scIterator5Type(quint8 type) const
			{
				switch (type)
				{
				case SCTP_ITERATOR_5F_A_A_A_F:
					return sc_iterator5_f_a_a_a_f;
				case SCTP_ITERATOR_5A_A_F_A_F:
					return sc_iterator5_a_a_f_a_f;
				case SCTP_ITERATOR_5F_A_F_A_F:
					return sc_iterator5_f_a_f_a_f;
				case SCTP_ITERATOR_5F_A_F_A_A:
					return sc_iterator5_f_a_f_a_a;
				case SCTP_ITERATOR_5F_A_A_A_A:
					return sc_iterator5_f_a_a_a_a;
				case SCTP_ITERATOR_5A_A_F_A_A:
					return sc_iterator5_a_a_f_a_a;
				}
				return (sc_iterator5_type)-1;
			}

			qint8 fixedPos(quint8 pos) const
			{
				switch (m_type)
				{
				case SCTP_ITERATOR_3F_A_A:
				{
					if (pos == 0)
						return 0;
					return -1;
				}
				case SCTP_ITERATOR_3A_A_F:
				{
					if (pos == 0)
						return 2;
					return -1;
				}
				case SCTP_ITERATOR_3F_A_F:
				{
					if (pos == 0)
						return 0;
					if (pos == 1)
						return 2;
					return -1;
				}
				case SCTP_ITERATOR_5F_A_A_A_F:
				{
					if (pos == 0)
						return 0;
					if (pos == 1)
						return 4;
					return -1;
				}
				case SCTP_ITERATOR_5A_A_F_A_F:
				{
					if (pos == 0)
						return 2;
					if (pos == 1)
						return 4;
					return -1;
				}
				case SCTP_ITERATOR_5F_A_F_A_F:
				{
					if (pos == 0)
						return 0;
					if (pos == 1)
						return 2;
					if (pos == 2)
						return 4;
					return -1;
				}
				case SCTP_ITERATOR_5F_A_F_A_A:
				{
					if (pos == 0)
						return 0;
					if (pos == 1)
						return 2;
					return -1;
				}
				case SCTP_ITERATOR_5F_A_A_A_A:
				{
					if (pos == 0)
						return 0;
					return -1;
				}
				case SCTP_ITERATOR_5A_A_F_A_A:
				{
					if (pos == 0)
						return 2;
					return -1;
				}

				} // switch

				return -1;
			}

			sc_addr generateNodeLink(sc_memory_context const * ctx, sc_type type)
			{
				if (type & sc_type_link)
					return sc_memory_link_new(ctx);

				if (type & sc_type_node)
					return sc_memory_node_new(ctx, type);

				static sc_addr empty;
				SC_ADDR_MAKE_EMPTY(empty);

				return empty;
			}

			bool generate(sc_memory_context const * ctx, ScAddrVec & result, quint8 pos)
			{
				quint8 const count = argsCount();
				if (count == 3)
				{
					sc_iterator3_type itType = scIterator3Type(m_type);

					/* generate start element */
					if (itType == sc_iterator3_a_a_f)
						result[pos] = generateNodeLink(ctx, m_args[0].m_param.type);

					if (SC_ADDR_IS_EMPTY(result[pos]))
						return false;

					/* generate end element */
					if (itType == sc_iterator3_f_a_a)
						result[pos + 2] = generateNodeLink(ctx, m_args[2].m_param.type);

					if (SC_ADDR_IS_EMPTY(result[pos + 2]))
					{
						sc_memory_element_free(ctx, result[pos]);
						SC_ADDR_MAKE_EMPTY(result[pos]);
						return false;
					}

					result[pos + 1] = sc_memory_arc_new(ctx, m_args[1].m_param.type, result[pos], result[pos + 2]);
					return SC_ADDR_IS_NOT_EMPTY(result[pos + 1]);
				}
				else if (count == 5)
				{
					
				}

				return false;
			}

			void startIterate(sc_memory_context const * ctx)
			{
				stopIterate();

				quint8 const count = argsCount();
				if (count == 3)
					m_it3 = sc_iterator3_new(ctx, scIterator3Type(m_type), m_args[0].m_param, m_args[1].m_param, m_args[2].m_param);
				else if (count == 5)
					m_it5 = sc_iterator5_new(ctx, scIterator5Type(m_type), m_args[0].m_param, m_args[1].m_param, m_args[2].m_param, m_args[3].m_param, m_args[4].m_param);
			}

			void stopIterate()
			{
				quint8 const count = argsCount();
				if (count == 3)
				{
					sc_iterator3_free(m_it3);
					m_it3 = 0;
				}
				else if (count == 5)
				{
					sc_iterator5_free(m_it5);
					m_it5 = 0;
				}
			}

			bool nextIterate() const
			{
				quint8 const count = argsCount();
				if (count == 3 && m_it3)
					return (sc_iterator3_next(m_it3) == SC_TRUE);
				else if (count == 5 && m_it5)
					return (sc_iterator5_next(m_it5) == SC_TRUE);
				return false;
			}

			void copyResults(ScAddrVec & result, quint8 pos) const
			{
				quint8 const count = argsCount();
				if (count == 3)
				{
					for (quint8 i = 0; i < count; ++i)
						result[pos + i] = sc_iterator3_value(m_it3, i);
				}
				else if (count == 5)
				{
					for (quint8 i = 0; i < count; ++i)
						result[pos + i] = sc_iterator5_value(m_it5, i);
				}

			}

		}; // IteratorData

		typedef std::vector<IteratorData> IteratorDataVec;
		IteratorDataVec m_iterators;

	public:

		bool build(QDataStream *params)
		{
			quint8 iterCount;

			if (params->readRawData((char*)&iterCount, sizeof(iterCount)) != sizeof(iterCount))
				return false;

			if (iterCount > 50)
				return false;

			m_iterators.resize(iterCount);
			for (size_t i = 0; i <iterCount; ++i)
			{
				IteratorData & it = m_iterators[i];

				if (params->readRawData((char*)&it.m_type, sizeof(it.m_type)) != sizeof(it.m_type))
					return false;
				if (i > 0)
					it.buildRepl(params);
				it.buildParams(params);
			}

			return true;
		}

		quint8 oneResultSize()
		{
			quint8 r = 0;
			for (quint8 i = 0; i < m_iterators.size(); ++i)
			{
				IteratorData & it = m_iterators[i];
				r += it.argsCount();
			}
			return r;
		}

		void iterateStep(sc_memory_context const * ctx, ScAddrVec & result, quint8 resultPos, quint8 itIdx)
		{
			Q_ASSERT(itIdx < m_iterators.size());
			Q_ASSERT(resultPos < result.size());

			IteratorData & it = m_iterators[itIdx];
			// apply replaces
			if (itIdx > 0)
			{
				quint8 count = it.fixedCount();
				for (quint8 i = 0; i < count; ++i)
				{
					quint8 const repl = it.m_repl[i];
					if (repl == 255)
						continue;

					Q_ASSERT(repl < resultPos);
					qint8 pos = it.fixedPos(i);

					Q_ASSERT(pos != -1 && pos < it.argsCount());
					Q_ASSERT(SC_ADDR_IS_NOT_EMPTY(result[repl]));
					Q_ASSERT(it.m_args[pos].isRepl());
					Q_ASSERT(!it.m_args[pos].m_param.is_type);

					it.m_args[pos].m_param.addr = result[repl];
				}
			}
			it.startIterate(ctx);

			while (it.nextIterate())
			{
				it.copyResults(result, resultPos);
				if (itIdx == m_iterators.size() - 1)
					m_results.insert(m_results.end(), result.begin(), result.end());
				else
					iterateStep(ctx, result, resultPos + it.argsCount(), itIdx + 1);
			}

			it.stopIterate();
		}

		void iterate(sc_memory_context const * ctx)
		{
			ScAddrVec result;
			result.resize(oneResultSize());

			iterateStep(ctx, result, 0, 0);
		}

		bool generateStep(sc_memory_context const * ctx, ScAddrVec & result, quint8 resultPos, quint8 itIdx)
		{
			Q_ASSERT(itIdx < m_iterators.size());
			Q_ASSERT(resultPos < result.size());

			IteratorData & it = m_iterators[itIdx];
			// apply replaces
			if (itIdx > 0)
			{
				quint8 count = it.fixedCount();
				for (quint8 i = 0; i < count; ++i)
				{
					quint8 const repl = it.m_repl[i];
					if (repl == 255)
						continue;

					Q_ASSERT(repl < resultPos);
					qint8 pos = it.fixedPos(i);

					Q_ASSERT(pos != -1 && pos < it.argsCount());
					Q_ASSERT(SC_ADDR_IS_NOT_EMPTY(result[repl]));
					Q_ASSERT(it.m_args[pos].isRepl());
					Q_ASSERT(!it.m_args[pos].m_param.is_type);

					it.m_args[pos].m_param.addr = result[repl];
				}
			}

			if (it.generate(ctx, result, resultPos))
			{
				return generateStep(ctx, result, resultPos + it.argsCount(), itIdx + 1);
			}

			return false;
		}

		bool generate(sc_memory_context const * ctx)
		{
			m_results.resize(oneResultSize());
			memset(m_results.data(), 0, sizeof(ScAddrVec::value_type) * m_results.size());

			return generateStep(ctx, m_results, 0, 0);
		}


		ScAddrVec const & result() const
		{
			return m_results;
		}

	};
}


// -----------------------------

sctpCommand::sctpCommand(QObject *parent)
    : QObject(parent)
    , mSendEventsCount(0)
    , mContext(0)
{
}

sctpCommand::~sctpCommand()
{
}

void sctpCommand::init()
{
    mContext = sc_memory_context_new(sc_access_lvl_make_min);
}

void sctpCommand::shutdown()
{
    tEventsSet::iterator it, itEnd = mEventsSet.end();
    for (it = mEventsSet.begin(); it != itEnd; ++it)
        sctpEventManager::getSingleton()->destroyEvent(*it);
    mEventsSet.clear();

    sc_memory_context_free(mContext);
    mContext = 0;
}

eSctpErrorCode sctpCommand::processCommand(QIODevice *inDevice, QIODevice *outDevice)
{
    quint8 cmdCode = SCTP_CMD_UNKNOWN;
    quint8 cmdFlags = 0;
    quint32 cmdId = 0;
    quint32 cmdParamSize = 0;

    // read command header
    if (!waitAvailableBytes(inDevice, cmdHeaderSize()))
        return SCTP_ERROR_CMD_HEADER_READ_TIMEOUT;

    inDevice->read((char*)&cmdCode, sizeof(cmdCode));
    inDevice->read((char*)&cmdFlags, sizeof(cmdFlags));
    inDevice->read((char*)&cmdId, sizeof(cmdId));
    inDevice->read((char*)&cmdParamSize, sizeof(cmdParamSize));

    // read params data
    QByteArray paramsData(cmdParamSize, 0);
    if (!waitAvailableBytes(inDevice, cmdParamSize))
        return SCTP_ERROR_CMD_PARAM_READ_TIMEOUT;

    inDevice->read((char*)paramsData.data(), paramsData.size());
    QDataStream paramsStream(paramsData);

    switch (cmdCode)
    {
    case SCTP_CMD_CHECK_ELEMENT:
        return processCheckElement(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_GET_ELEMENT_TYPE:
        return processGetElementType(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_ERASE_ELEMENT:
        return processElementErase(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_CREATE_NODE:
        return processCreateNode(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_CREATE_LINK:
        return processCreateLink(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_CREATE_ARC:
        return processCreateArc(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_GET_ARC:
        return processGetArc(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_GET_LINK_CONTENT:
        return processGetLinkContent(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_FIND_LINKS:
        return processFindLinks(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_SET_LINK_CONTENT:
        return processSetLinkContent(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_ITERATE_ELEMENTS:
        return processIterateElements(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_ITERATE_CONSTRUCTION:
        return processIterateConstruction(cmdFlags, cmdId, &paramsStream, outDevice);

	case SCTP_CMD_GENERATE_CONSTRUCTION:
		return processGenerateConstruction(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_EVENT_CREATE:
        return processCreateEvent(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_EVENT_DESTROY:
        return processDestroyEvent(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_EVENT_EMIT:
        return processEmitEvent(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_FIND_ELEMENT_BY_SYSITDF:
        return processFindElementBySysIdtf(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_SET_SYSIDTF:
        return processSetSysIdtf(cmdFlags, cmdId, &paramsStream, outDevice);

    case SCTP_CMD_STATISTICS:
        return processStatistics(cmdFlags, cmdId, &paramsStream, outDevice);

    default:
        return SCTP_ERROR_UNKNOWN_CMD;
    }

    return SCTP_ERROR;
}

bool sctpCommand::waitAvailableBytes(QIODevice *stream, quint32 bytesNum)
{
    while (stream->bytesAvailable() < bytesNum)
    {
        if (!stream->waitForReadyRead(SCTP_READ_TIMEOUT))
        {
            return false;
        }
    }

    return true;
}

void sctpCommand::writeResultHeader(eSctpCommandCode cmdCode, quint32 cmdId, eSctpResultCode resCode, quint32 resSize, QIODevice *outDevice)
{
    Q_ASSERT(outDevice != 0);
    quint8 code = cmdCode;

    outDevice->write((const char*)&code, sizeof(code));
    outDevice->write((const char*)&cmdId, sizeof(cmdId));

    code = resCode;
    outDevice->write((const char*)&code, sizeof(code));
    outDevice->write((const char*)&resSize, sizeof(resSize));
}

quint32 sctpCommand::cmdHeaderSize()
{
    return 2 * sizeof(quint8) + 2 * sizeof(quint32);
}


// ----------- process commands -------------
eSctpErrorCode sctpCommand::processCheckElement(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-add from parameters
    READ_PARAM(addr);

    eSctpResultCode resCode = sc_memory_is_element(mContext, addr) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;

    // send result
    writeResultHeader(SCTP_CMD_CHECK_ELEMENT, cmdId, resCode, 0, outDevice);

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processGetElementType(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    READ_PARAM(addr);

    sc_type type = 0;
    eSctpResultCode resCode = (sc_memory_get_element_type(mContext, addr, &type) == SC_RESULT_OK) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;
    quint32 resSize = (resCode == SCTP_RESULT_OK) ? sizeof(type) : 0;

    // send result
    writeResultHeader(SCTP_CMD_GET_ELEMENT_TYPE, cmdId, resCode, resSize, outDevice);
    if (resCode == SCTP_RESULT_OK)
        outDevice->write((const char*)&type, sizeof(type));

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processElementErase(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    READ_PARAM(addr);

    eSctpResultCode resCode = (sc_memory_element_free(mContext, addr) == SC_RESULT_OK) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;
    // send result
    writeResultHeader(SCTP_CMD_ERASE_ELEMENT, cmdId, resCode, 0, outDevice);

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processCreateNode(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    sc_type type;
    // read type of node
    READ_PARAM(type);

    sc_addr addr = sc_memory_node_new(mContext, type);

    // send result
    eSctpErrorCode result;
    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
        writeResultHeader(SCTP_CMD_CREATE_NODE, cmdId, SCTP_RESULT_OK, sizeof(addr), outDevice);
        outDevice->write((const char*)&addr, sizeof(addr));

        result = SCTP_NO_ERROR;
    }else
    {
        writeResultHeader(SCTP_CMD_CREATE_NODE, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        result = SCTP_ERROR;
    }

    return result;
}

eSctpErrorCode sctpCommand::processCreateLink(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    Q_UNUSED(cmdFlags);

    sc_addr addr = sc_memory_link_new(mContext);

    // send result
    eSctpErrorCode result;
    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
        writeResultHeader(SCTP_CMD_CREATE_LINK, cmdId, SCTP_RESULT_OK, sizeof(addr), outDevice);
        outDevice->write((const char*)&addr, sizeof(addr));

        result = SCTP_NO_ERROR;
    }else
    {
        writeResultHeader(SCTP_CMD_CREATE_LINK, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        result = SCTP_ERROR;
    }

    return result;
}

eSctpErrorCode sctpCommand::processCreateArc(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    sc_type type;
    sc_addr begin_addr, end_addr;

    // read type of sc-arc
    READ_PARAM(type);

    // read sc-addr of begin and end elements
    READ_PARAM(begin_addr);
    READ_PARAM(end_addr);

    sc_addr addr = sc_memory_arc_new(mContext, type, begin_addr, end_addr);
    eSctpErrorCode result;
    if (SC_ADDR_IS_NOT_EMPTY(addr))
    {
        writeResultHeader(SCTP_CMD_CREATE_ARC, cmdId, SCTP_RESULT_OK, sizeof(addr), outDevice);
        outDevice->write((const char*)&addr, sizeof(addr));

        result = SCTP_NO_ERROR;
    }else
    {
        writeResultHeader(SCTP_CMD_CREATE_LINK, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        result = SCTP_ERROR;
    }

    return result;
}

eSctpErrorCode sctpCommand::processGetArc(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    sc_addr arc, begin, end;

    READ_PARAM(arc);

    if (sc_memory_get_arc_begin(mContext, arc, &begin) != SC_RESULT_OK ||
        sc_memory_get_arc_end(mContext, arc, &end) != SC_RESULT_OK)
    {
        writeResultHeader(SCTP_CMD_GET_ARC, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        return SCTP_ERROR;
    }

    writeResultHeader(SCTP_CMD_GET_ARC, cmdId, SCTP_RESULT_OK, sizeof(sc_addr) * 2, outDevice);
    outDevice->write((const char*)&begin, sizeof(begin));
    outDevice->write((const char*)&end, sizeof(end));
    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processGetLinkContent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_stream *stream = (sc_stream*)null_ptr;
    sc_char data_buffer[512];
    sc_uint32 data_len = 0;
    sc_uint32 data_written = 0;
    sc_uint32 data_read = 0;

    Q_UNUSED(cmdFlags);
    Q_ASSERT(params != 0);

    // read sc-addr of sc-element from parameters
    if (params->readRawData((char*)&addr, sizeof(addr)) != sizeof(addr))
        return SCTP_ERROR_CMD_READ_PARAMS;

    eSctpResultCode resCode = (sc_memory_get_link_content(mContext, addr, &stream) == SC_RESULT_OK) ? SCTP_RESULT_OK : SCTP_RESULT_FAIL;


    if (resCode == SCTP_RESULT_OK)
    {
        if (sc_stream_get_length(stream, &data_len) != SC_RESULT_OK)
        {
            resCode = SCTP_RESULT_FAIL;
            sc_stream_free(stream);
            stream = (sc_stream*)null_ptr;
        }
    }
    // send result
    writeResultHeader(SCTP_CMD_GET_LINK_CONTENT, cmdId, resCode, data_len, outDevice);

    if (resCode == SCTP_RESULT_FAIL)
    {
        if (stream != null_ptr)
            sc_stream_free(stream);

        return SCTP_NO_ERROR;
    }

    // write content data
    while (sc_stream_eof(stream) != SC_TRUE)
    {
        // if there are any error to read data, then
        // write null into output
        if (sc_stream_read_data(stream, data_buffer, 512, &data_read) != SC_RESULT_OK)
        {
            if (data_written < data_len)
            {
                quint32 len = data_len - data_written;
                sc_char *data = new sc_char[len];
                memset(data, 0, len);
                outDevice->write(data, len);
                delete []data;

                sc_stream_free(stream);
                return SCTP_ERROR;
            }
        }

        outDevice->write(data_buffer, data_read);
        data_written += data_read;
    }

    Q_ASSERT(data_written == data_len);

    if (resCode == SCTP_RESULT_OK)
        sc_stream_free(stream);


    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processFindLinks(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_int32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    sc_stream *stream = sc_stream_memory_new(data, data_len, SC_STREAM_FLAG_READ, SC_FALSE);
    sc_uint32 result_count = 0;
    sc_addr *result = 0;

    if (sc_memory_find_links_with_content(mContext, stream, &result, &result_count) != SC_RESULT_OK)
        writeResultHeader(SCTP_CMD_FIND_LINKS, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
    else
    {
        writeResultHeader(SCTP_CMD_FIND_LINKS, cmdId, SCTP_RESULT_OK, result_count * sizeof(sc_addr) + sizeof(result_count), outDevice);
        outDevice->write((const char*)&result_count, sizeof(result_count));
        outDevice->write((const char*)result, sizeof(sc_addr) * result_count);
    }
    delete[] data;
    sc_stream_free(stream);

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processSetLinkContent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_int32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(addr);
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    sc_stream *stream = sc_stream_memory_new(data, data_len, SC_STREAM_FLAG_READ, SC_FALSE);

    sc_result result = sc_memory_set_link_content(mContext, addr, stream);
    writeResultHeader(SCTP_CMD_SET_LINK_CONTENT, cmdId, result == SC_RESULT_OK ? SCTP_RESULT_OK : SCTP_RESULT_FAIL, 0, outDevice);
    sc_stream_free(stream);
    delete[] data;

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processIterateElements(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_uchar iterator_type = 0;
    sc_type type1, type2, type3, type4;
    sc_addr addr1, addr2, addr3;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != null_ptr);

    // read iterator type
    READ_PARAM(iterator_type);

    Q_ASSERT(iterator_type < SCTP_ITERATOR_COUNT);

    // 3-elements iterators
    if (iterator_type <= SCTP_ITERATOR_3F_A_F)
    {
        sc_iterator3 *it = (sc_iterator3*)null_ptr;

        switch (iterator_type)
        {
        case SCTP_ITERATOR_3A_A_F:
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(addr1);
            it = sc_iterator3_a_a_f_new(mContext, type1, type2, addr1);
            break;

        case SCTP_ITERATOR_3F_A_A:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(type2);
            it = sc_iterator3_f_a_a_new(mContext, addr1, type1, type2);
            break;

        case SCTP_ITERATOR_3F_A_F:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(addr2);
            it = sc_iterator3_f_a_f_new(mContext, addr1, type1, addr2);
            break;

        default:
            return SCTP_ERROR;
        }

        if (it == null_ptr)
            writeResultHeader(SCTP_CMD_ITERATE_ELEMENTS, cmdId, SCTP_RESULT_FAIL, 0, outDevice);

        // create results data
        QByteArray results;
        QBuffer buffer(&results);
        sc_uint32 results_count = 0;
        sc_addr addr;

        buffer.open(QBuffer::WriteOnly);
        while (sc_iterator3_next(it) == SC_TRUE)
        {
            results_count++;
            for (sc_uint i = 0; i < 3; i++)
            {
                addr = sc_iterator3_value(it, i);
                buffer.write((const char*)&addr, sizeof(addr));
            }
        }
        buffer.close();

        // write result
        writeResultHeader(SCTP_CMD_ITERATE_ELEMENTS, cmdId, SCTP_RESULT_OK, results.size() + sizeof(results_count), outDevice);
        outDevice->write((const char*)&results_count, sizeof(results_count));
        if (results_count > 0)
            outDevice->write((const char*)results.constData(), results.size());

        sc_iterator3_free(it);

    }else
    {
        // 5-elements iterators
        sc_iterator5 *it = (sc_iterator5*)null_ptr;

        switch (iterator_type)
        {
        case SCTP_ITERATOR_5F_A_A_A_F:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(type3);
            READ_PARAM(addr2);
            it = sc_iterator5_f_a_a_a_f_new(mContext, addr1, type1, type2, type3, addr2);
            break;

        case SCTP_ITERATOR_5A_A_F_A_A:
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(addr1);
            READ_PARAM(type3);
            READ_PARAM(type4);
            it = sc_iterator5_a_a_f_a_a_new(mContext, type1, type2, addr1, type3, type4);
            break;

        case SCTP_ITERATOR_5A_A_F_A_F:
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(addr1);
            READ_PARAM(type3);
            READ_PARAM(addr2);
            it = sc_iterator5_a_a_f_a_f_new(mContext, type1, type2, addr1, type3, addr2);
            break;

        case SCTP_ITERATOR_5F_A_A_A_A:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(type2);
            READ_PARAM(type3);
            READ_PARAM(type4);
            it = sc_iterator5_f_a_a_a_a_new(mContext, addr1, type1, type2, type3, type4);
            break;

        case SCTP_ITERATOR_5F_A_F_A_A:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(addr2);
            READ_PARAM(type2);
            READ_PARAM(type3);
            it = sc_iterator5_f_a_f_a_a_new(mContext, addr1, type1, addr2, type2, type3);
            break;

        case SCTP_ITERATOR_5F_A_F_A_F:
            READ_PARAM(addr1);
            READ_PARAM(type1);
            READ_PARAM(addr2);
            READ_PARAM(type2);
            READ_PARAM(addr3);
            it = sc_iterator5_f_a_f_a_f_new(mContext, addr1, type1, addr2, type2, addr3);
            break;

        default:
            return SCTP_ERROR;
        }

        if (it == null_ptr)
            writeResultHeader(SCTP_CMD_ITERATE_ELEMENTS, cmdId, SCTP_RESULT_FAIL, 0, outDevice);

        // create results data
        QByteArray results;
        QBuffer buffer(&results);
        sc_uint32 results_count = 0;
        sc_addr addr;

        buffer.open(QBuffer::WriteOnly);
        while (sc_iterator5_next(it) == SC_TRUE)
        {
            results_count++;
            for (sc_uint i = 0; i < 5; i++)
            {
                addr = sc_iterator5_value(it, i);
                buffer.write((const char*)&addr, sizeof(addr));
            }
        }
        buffer.close();

        // write result
        writeResultHeader(SCTP_CMD_ITERATE_ELEMENTS, cmdId, SCTP_RESULT_OK, results.size() + sizeof(results_count), outDevice);
        outDevice->write((const char*)&results_count, sizeof(results_count));
        if (results_count > 0)
            outDevice->write((const char*)results.constData(), results.size());

        sc_iterator5_free(it);
    }

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processIterateConstruction(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    IterConstsr constr;
    if (constr.build(params))
    {
        constr.iterate(mContext);
        quint8 stride = constr.oneResultSize();
        IterConstsr::ScAddrVec const & result = constr.result();
        Q_ASSERT(result.size() % stride == 0);

        quint32 const n = (quint32)result.size();
        quint32 const s = sizeof(sc_addr) * n;
        quint32 const count = n / stride;

        if (result.size() > 0)
        {
            writeResultHeader(SCTP_CMD_ITERATE_CONSTRUCTION, cmdId, SCTP_RESULT_OK, s + sizeof(quint32), outDevice);
            outDevice->write((const char*)&count, sizeof(count));
            outDevice->write((const char*)result.data(), s);
        }
        else
            writeResultHeader(SCTP_CMD_ITERATE_CONSTRUCTION, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
    }
    else
        writeResultHeader(SCTP_CMD_ITERATE_CONSTRUCTION, cmdId, SCTP_RESULT_FAIL, 0, outDevice);

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processGenerateConstruction(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
	IterConstsr constr;
	if (constr.build(params) && constr.generate(mContext))
	{
		IterConstsr::ScAddrVec const & result = constr.result();
		quint32 const byteSize = sizeof(sc_addr) * (quint32)result.size();

		writeResultHeader(SCTP_CMD_GENERATE_CONSTRUCTION, cmdId, SCTP_RESULT_OK, byteSize, outDevice);
		outDevice->write((const char *)result.data(), byteSize);
	}
	else
	{
		writeResultHeader(SCTP_CMD_GENERATE_CONSTRUCTION, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
	}

	return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processCreateEvent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_uint8 event_type;
    sc_addr addr;

    Q_UNUSED(cmdFlags);
    Q_ASSERT(params != 0);

    READ_PARAM(event_type);
    READ_PARAM(addr);


    tEventId event = 0;
    if (!sctpEventManager::getSingleton()->createEvent(mContext, (sc_event_type)event_type, addr, this, event))
    {
        writeResultHeader(SCTP_CMD_EVENT_CREATE, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
        return SCTP_ERROR;
    }

    if (mEventsSet.find(event) != mEventsSet.end())
        return SCTP_ERROR;

    mEventsSet.insert(event);

    writeResultHeader(SCTP_CMD_EVENT_CREATE, cmdId, SCTP_RESULT_OK, sizeof(tEventId), outDevice);
    outDevice->write((const char*)&event, sizeof(event));

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processDestroyEvent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    tEventId eventId = 0;

    Q_UNUSED(cmdFlags);
    Q_ASSERT(params != 0);

    READ_PARAM(eventId);

    if (sctpEventManager::getSingleton()->destroyEvent(eventId))
    {
        writeResultHeader(SCTP_CMD_EVENT_DESTROY, cmdId, SCTP_RESULT_OK, sizeof(eventId), outDevice);
        outDevice->write((const char*)&eventId, sizeof(eventId));
        return SCTP_NO_ERROR;
    }

    writeResultHeader(SCTP_CMD_EVENT_DESTROY, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
    return SCTP_ERROR;
}

eSctpErrorCode sctpCommand::processEmitEvent(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    QMutexLocker locker(&mSendMutex);

    quint32 resSize = sizeof(mSendEventsCount) + mSendData.size();
    writeResultHeader(SCTP_CMD_EVENT_EMIT, cmdId, SCTP_RESULT_OK, resSize, outDevice);
    outDevice->write((const char*)&mSendEventsCount, sizeof(mSendEventsCount));
    outDevice->write(mSendData);

    mSendData.clear();
    mSendEventsCount = 0;

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processFindElementBySysIdtf(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_int32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);	// just for a test

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    sc_addr result;
    if (sc_helper_find_element_by_system_identifier(mContext, data, data_len, &result) != SC_RESULT_OK)
        writeResultHeader(SCTP_CMD_FIND_ELEMENT_BY_SYSITDF, cmdId, SCTP_RESULT_FAIL, 0, outDevice);
    else
    {
        writeResultHeader(SCTP_CMD_FIND_ELEMENT_BY_SYSITDF, cmdId, SCTP_RESULT_OK, sizeof(sc_addr), outDevice);
        outDevice->write((const char*)&result, sizeof(sc_addr));
    }
    delete[] data;

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processSetSysIdtf(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    sc_addr addr;
    sc_int32 data_len = 0;
    sc_char *data = 0;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);

    // read length of content data
    READ_PARAM(addr);
    READ_PARAM(data_len);
    Q_ASSERT(data_len > 0);

    data = new sc_char[data_len];
    if (params->readRawData(data, data_len) != data_len)
    {
        delete[] data;
        return SCTP_ERROR_CMD_READ_PARAMS;
    }

    writeResultHeader(SCTP_CMD_SET_SYSIDTF, cmdId, sc_helper_set_system_identifier(mContext, addr, data, data_len) != SC_RESULT_OK ? SCTP_RESULT_FAIL : SCTP_RESULT_OK, 0, outDevice);

    delete[] data;

    return SCTP_NO_ERROR;
}

eSctpErrorCode sctpCommand::processStatistics(quint32 cmdFlags, quint32 cmdId, QDataStream *params, QIODevice *outDevice)
{
    quint64 begin_time;
    quint64 end_time;

    Q_UNUSED(cmdFlags);

    Q_ASSERT(params != 0);
    READ_PARAM(begin_time);
    READ_PARAM(end_time);

    tStatItemVector stat;
    sctpStatistic::getInstance()->getStatisticsInTimeRange(begin_time, end_time, stat);

    writeResultHeader(SCTP_CMD_STATISTICS, cmdId, SCTP_RESULT_OK, sizeof(quint32) + sStatItem::realSize() * stat.size(), outDevice);
    // write result
    quint32 res_count = stat.size();
    outDevice->write((const char*)&res_count, sizeof(res_count));
    for (quint32 idx = 0; idx < res_count; ++idx)
        outDevice->write((const char*)&(stat[idx]), sStatItem::realSize());

    return SCTP_NO_ERROR;
}

sc_result sctpCommand::processEventEmit(tEventId eventId, sc_addr el_addr, sc_addr arg_addr)
{    
    QMutexLocker locker(&mSendMutex);

    mSendData.append((char*)&eventId, sizeof(eventId));
    mSendData.append((char*)&el_addr, sizeof(el_addr));
    mSendData.append((char*)&arg_addr, sizeof(arg_addr));

    ++mSendEventsCount;

    return SC_RESULT_OK;
}
