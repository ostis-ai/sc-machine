/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

class ScTemplateBuilder
{
    friend class ScTemplate;

    typedef std::map<tRealAddr, size_t, RealAddrLessFunc> tAddrToIndexMap;

    struct ObjectInfo
    {
        ScAddr mAddr;
        ScType mType;
        std::string mSysIdtf;

        ObjectInfo(ScAddr const & inAddr, ScType const & inType, std::string const & inSysIdtf)
            : mAddr(inAddr)
            , mType(inType)
            , mSysIdtf(inSysIdtf)
        {
        }
    };

    struct EdgeInfo
    {
        size_t mSourceObject;
        size_t mTargetObject;
        ScType mType;
        size_t mEdgeIndex;

        EdgeInfo()
            : mSourceObject(-1)
            , mTargetObject(-1)
            , mType(0)
            , mEdgeIndex(-1)
        {
        }

        EdgeInfo(size_t inSourceObject, size_t inTargetObject, ScType const & inType, size_t inEdgeIndex)
            : mSourceObject(inSourceObject)
            , mTargetObject(inTargetObject)
            , mType(inType)
            , mEdgeIndex(inEdgeIndex)
        {
        }
    };

    struct EdgeCompareLess
    {
        EdgeCompareLess(ScTemplateBuilder * builder)
            : mBuilder(builder)
        {
            check_expr(mBuilder);
        }

        bool _isEdgeDependOnOther(EdgeInfo const & left, EdgeInfo const & right) const
        {
            check_expr(left.mTargetObject < mBuilder->mObjects.size());

            ObjectInfo & trgObj = mBuilder->mObjects[left.mTargetObject];
            if (trgObj.mType.isEdge())
            {
                tAddrToIndexMap::const_iterator it = mBuilder->mEdgeAddrToEdgeIndex.find(*trgObj.mAddr);
                check_expr(it != mBuilder->mEdgeAddrToEdgeIndex.end());
                check_expr(it->second < mBuilder->mEdges.size());

                return _isEdgeDependOnOther(mBuilder->mEdges[it->second], right);
            }

            return false;
        }

        bool operator() (EdgeInfo const & left, EdgeInfo const & right) const
        {
            ObjectInfo const & leftTrg = mBuilder->mObjects[left.mTargetObject];
            ObjectInfo const & rightTrg = mBuilder->mObjects[right.mTargetObject];

            // compare by end element type (edge - not good)
            bool const isLeftEdge = leftTrg.mType.isEdge();
            bool const isRightEdge = rightTrg.mType.isEdge();
            if (!isLeftEdge && isRightEdge)
                return true;
            if (isLeftEdge && !isRightEdge)
                return false;

            // check if edge depend on other edge
            if (_isEdgeDependOnOther(left, right))
                return false;

            return (left.mEdgeIndex < right.mEdgeIndex);
        }

    protected:
        ScTemplateBuilder * mBuilder;
    };

protected:
    ScTemplateBuilder(ScAddr const & inScTemplateAddr, ScMemoryContext & inCtx)
        : mScTemplateAddr(inScTemplateAddr)
        , mContext(inCtx)
    {
    }

    bool operator() (ScTemplate * inTemplate)
    {
        /// TODO: lock structure
        /// TODO: check for duplicates

        inTemplate->clear();

        // first of all iterate all objects in template
        size_t index = 0;
        mObjects.clear();
        mObjects.reserve(128);
        ScIterator3Ptr iterObj = mContext.iterator3(mScTemplateAddr, SC_TYPE(sc_type_arc_pos_const_perm), SC_TYPE(0));
        while (iterObj->next())
        {
            ScAddr const addr = iterObj->value(2);
            ScType const type = mContext.getElementType(addr);
            std::string idtf = mContext.helperGetSystemIdtf(addr);

            if (idtf.empty())
            {
                std::stringstream ss;
                ss << "obj_" << index++;
                idtf = ss.str();
            }

            size_t const index = mObjects.size();
            if (type.isConst())
            {
                mConstObjectIndicies.push_back(index);
            }
            else
            {
                mVarObjectIndicies.push_back(index);
            }

            if (type.isEdge())
                mEdgeIndicies.push_back(index);

            mAddrToIndex[*addr] = index;
            mObjects.push_back(ObjectInfo(addr, type, idtf));
        }

        // iterate all edges, to detect begin and end elements
        {
            size_t const edgeNum = mEdgeIndicies.size();
            mEdges.clear();
            mEdges.resize(edgeNum);
            for (size_t i = 0; i < edgeNum; ++i)
            {
                ObjectInfo const & object = mObjects[mEdgeIndicies[i]];
                check_expr(object.mType.isEdge());

                ScAddr edgeAddr = object.mAddr;
                EdgeInfo & edge = mEdges[i];

                check_expr(edgeAddr.isValid());

                ScAddr sourceAddr = mContext.getEdgeSource(edgeAddr);
                ScAddr targetAddr = mContext.getEdgeTarget(edgeAddr);
                check_expr(sourceAddr.isValid() && targetAddr.isValid());

                tAddrToIndexMap::const_iterator it = mAddrToIndex.find(*sourceAddr);
                if (it == mAddrToIndex.end())
                    return false;	/// TODO: provide error code

                edge.mSourceObject = it->second;

                it = mAddrToIndex.find(*targetAddr);
                if (it == mAddrToIndex.end())
                    return false;	/// TODO: provide error code

                edge.mTargetObject = it->second;
                edge.mType = object.mType;
                edge.mEdgeIndex = i;

                mEdgeAddrToEdgeIndex[*edgeAddr] = edge.mEdgeIndex;
            }
        }

        // sort edges by next rules:
        // - more constants at the begin
        // - more nodes at the begin
        // - edges that depends on other ones moved to the end
        {
            EdgeCompareLess compare(this);
            std::sort(mEdges.begin(), mEdges.end(), compare);
        }

        // build template
        {
            size_t const edgeNum = mEdges.size();
            for (size_t i = 0; i < edgeNum; ++i)
            {
                EdgeInfo const & edge = mEdges[i];
                check_expr(edge.mType.isVar());

                ObjectInfo const & edgeObj = mObjects[mEdgeIndicies[edge.mEdgeIndex]];
                ObjectInfo const & src = mObjects[edge.mSourceObject];
                ObjectInfo const & trg = mObjects[edge.mTargetObject];

                /// TODO: replacements
                if (src.mType.isConst())
                {
                    if (trg.mType.isConst())	// F_A_F
                    {
                        inTemplate->triple(src.mAddr >> src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mAddr >> trg.mSysIdtf);
                    }
                    else
                    {
                        if (inTemplate->hasReplacement(trg.mSysIdtf)) // F_A_F
                        {
                            inTemplate->triple(src.mAddr >> src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mSysIdtf);
                        }
                        else // F_A_A
                        {
                            inTemplate->triple(src.mAddr >> src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mType >> trg.mSysIdtf);
                        }
                    }
                }
                else if (trg.mType.isConst())
                {
                    if (inTemplate->hasReplacement(src.mSysIdtf)) // F_A_F
                    {
                        inTemplate->triple(src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mAddr >> trg.mSysIdtf);
                    }
                    else // A_A_F
                    {
                        inTemplate->triple(src.mType >> src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mAddr >> trg.mSysIdtf);
                    }
                }
                else
                {
                    bool const srcRepl = inTemplate->hasReplacement(src.mSysIdtf);
                    bool const trgRepl = inTemplate->hasReplacement(trg.mSysIdtf);

                    if (srcRepl && trgRepl) // F_A_F
                    {
                        inTemplate->triple(src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mSysIdtf);
                    }
                    else if (!srcRepl && trgRepl) // A_A_F
                    {
                        inTemplate->triple(src.mType >> src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mSysIdtf);
                    }
                    else if (srcRepl && !trgRepl) // F_A_A
                    {
                        inTemplate->triple(src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mType >> trg.mSysIdtf);
                    }
                    else
                    {
                        inTemplate->triple(src.mType >> src.mSysIdtf, edge.mType >> edgeObj.mSysIdtf, trg.mType >> trg.mSysIdtf);
                    }
                }
            }
        }

        return true;
    }

protected:
    ScAddr mScTemplateAddr;
    ScMemoryContext & mContext;

    // all objects in template
    std::vector<ObjectInfo> mObjects;
    std::vector<EdgeInfo> mEdges;
    tAddrToIndexMap mAddrToIndex;
    tAddrToIndexMap mEdgeAddrToEdgeIndex;

    // indecies of objects by type
    std::vector<size_t> mConstObjectIndicies;
    std::vector<size_t> mVarObjectIndicies;
    std::vector<size_t> mEdgeIndicies;
};

bool ScTemplate::fromScTemplate(ScMemoryContext & ctx, ScAddr const & scTemplateAddr)
{
    ScTemplateBuilder builder(scTemplateAddr, ctx);
    return builder(this);
}
