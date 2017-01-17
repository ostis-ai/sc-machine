/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"
#include "sc_debug.hpp"

#include <algorithm>

namespace
{

class ObjectInfo
{
public:
    ObjectInfo(ScAddr const & inAddr, ScType const & inType, std::string const & inSysIdtf)
        : mAddr(inAddr)
        , mType(inType)
        , mSysIdtf(inSysIdtf)
        , mSource(nullptr)
        , mTarget(nullptr)
    {
    }

    inline bool isEdge() const
    {
        return mType.isEdge();
    }

    inline ObjectInfo const * getSource() const
    {
        return mSource;
    }
    
    inline ObjectInfo const * getTarget() const
    {
        return mTarget;
    }
    
    inline ScAddr const & getAddr() const
    {
        return mAddr;
    }
    
    inline std::string const & getIdtf() const
    {
        return mSysIdtf;
    }
    
    inline ScType const & getType() const
    {
        return mType;
    }

    inline void setSource(ObjectInfo * src)
    {
        mSource = src;
    }

    inline void setTarget(ObjectInfo * trg)
    {
        mTarget = trg;
    }

    inline uint32_t calculateEdgeRank() const
    {
        SC_ASSERT(isEdge(), ());
        SC_ASSERT(mSource, ());
        SC_ASSERT(mTarget, ());
        return (mSource->getType().isConst() ? 1 : 0) +
               (mTarget->getType().isConst() ? 1 : 0);
    }

private:
    ScAddr mAddr;
    ScType mType;
    std::string mSysIdtf;

    // edge data
    ObjectInfo * mSource;
    ObjectInfo * mTarget;
};

class EdgeLessFunctor
{
    using EdgeDependMap = std::unordered_multimap<ScAddr::HashType, ScAddr::HashType>;
    using ObjectVector = std::vector<ObjectInfo>;

public:
    explicit EdgeLessFunctor(EdgeDependMap const & edgeDependMap, ObjectVector const & objects)
        : mEdgeDependMap(edgeDependMap)
        , mObjects(objects)
    {
    }

    // returns true, when edge srouce/target objects depend on otherEdge construction
    bool isEdgeDependOnOther(ScAddr::HashType const & edge, ScAddr::HashType const & otherEdge) const
    {
        // TODO: support possible loops
        auto const range = mEdgeDependMap.equal_range(edge);
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second)
            {
                return true;
            }
            else if (isEdgeDependOnOther(it->second, otherEdge))
            {
                return true;
            }
        }

        return false;
    }

    bool operator() (size_t const indexA, size_t const indexB) const
    {
        ObjectInfo const & objA = mObjects[indexA];
        ObjectInfo const & objB = mObjects[indexB];

        SC_ASSERT(objA.isEdge(), ());
        SC_ASSERT(objB.isEdge(), ());

        if (isEdgeDependOnOther(objA.getAddr().hash(), objB.getAddr().hash()))
            return false;

        uint32_t const rankA = objA.calculateEdgeRank();
        uint32_t const rankB = objB.calculateEdgeRank();

        if (rankA != rankB)
            return rankA < rankB;

        return objA.getAddr().hash() < objB.getAddr().hash();
    }

private:
    EdgeDependMap const & mEdgeDependMap;
    ObjectVector const & mObjects;
};

} // namespace

class ScTemplateBuilder
{
    friend class ScTemplate;  

protected:
    ScTemplateBuilder(ScAddr const & inScTemplateAddr, ScMemoryContext & inCtx)
        : mScTemplateAddr(inScTemplateAddr)
        , mContext(inCtx)
    {
    }

    bool operator() (ScTemplate * inTemplate)
    {
        // TODO: add struct blocking
        // TODO: provide error codes
        std::unordered_map<ScAddr::HashType, size_t> addrToObjectIndex;
        std::unordered_multimap<ScAddr::HashType, ScAddr::HashType> edgeDependMap;
        std::vector<size_t> edgeIndices;

        mObjects.reserve(1024);
        edgeIndices.reserve(512);

        size_t index = 0;

        ScIterator3Ptr iter = mContext.iterator3(
            mScTemplateAddr,
            *ScType::EDGE_ACCESS_CONST_POS_PERM,
            *ScType());

        while (iter->next())
        {
            ScAddr const objAddr = iter->value(2);
            ScAddr::HashType const objHash = objAddr.hash();

            auto const it = addrToObjectIndex.find(objHash);
            if (it != addrToObjectIndex.end())
                continue; // object already exist

            addrToObjectIndex[objHash] = mObjects.size();

            ScType const objType = mContext.getElementType(objAddr);
            if (!objType.isValid())
                return false; // template corrupted

            std::string objIdtf = mContext.helperGetSystemIdtf(objAddr);
            if (objIdtf.empty())
                objIdtf = "..obj_" + std::to_string(index++);

            if (objType.isEdge())
                edgeIndices.emplace_back(mObjects.size());

            mObjects.emplace_back(objAddr, objType, objIdtf);
        }

        // iterate all edges and determine source/target objects
        for (auto const i : edgeIndices)
        {
            ObjectInfo & obj = mObjects[i];

            // determine source and target objects
            ScAddr src, trg;
            if (!mContext.getEdgeInfo(obj.getAddr(), src, trg))
                return false; // edge already doesn't exist

            auto const itSrc = addrToObjectIndex.find(src.hash());
            if (itSrc == addrToObjectIndex.end())
                return false; // edge source doesn't exist in template

            auto const itTrg = addrToObjectIndex.find(trg.hash());
            if (itTrg == addrToObjectIndex.end())
                return false; // target source doesn't exist in template

            ObjectInfo * srcObj = &(mObjects[itSrc->second]);
            ObjectInfo * trgObj = &(mObjects[itTrg->second]);

            if (srcObj->isEdge())
                edgeDependMap.insert({ obj.getAddr().hash(), srcObj->getAddr().hash() });
            if (trgObj->isEdge())
                edgeDependMap.insert({ obj.getAddr().hash(), srcObj->getAddr().hash() });

            obj.setSource(srcObj);
            obj.setTarget(trgObj);
        }

        // now need to sort edges for suitable creation order
        std::sort(edgeIndices.begin(), edgeIndices.end(), EdgeLessFunctor(edgeDependMap, mObjects));

        // build template 
        {
            for (auto const i : edgeIndices)
            {
                ObjectInfo const & edge = mObjects[i];
                SC_ASSERT(edge.getType().isVar(), ());

                ObjectInfo const * src = edge.getSource();
                ObjectInfo const * trg = edge.getTarget();

                ScType const srcType = src->getType();
                ScType const trgType = trg->getType();

                if (srcType.isConst())
                {
                    if (trgType.isConst())  // F_A_F 
                    {
                        inTemplate->triple(
                            src->getAddr() >> src->getIdtf(), 
                            edge.getType() >> edge.getIdtf(),
                            trg->getAddr() >> trg->getIdtf());
                    }
                    else
                    {
                        if (inTemplate->hasReplacement(trg->getIdtf())) // F_A_F 
                        {
                            inTemplate->triple(
                                src->getAddr() >> src->getIdtf(),
                                edge.getType() >> edge.getIdtf(),
                                trg->getIdtf());
                        }
                        else // F_A_A 
                        {
                            inTemplate->triple(
                                src->getAddr() >> src->getIdtf(),
                                edge.getType() >> edge.getIdtf(),
                                trgType >> trg->getIdtf());
                        }
                    }
                }
                else if (trgType.isConst())
                {
                    if (inTemplate->hasReplacement(src->getIdtf())) // F_A_F 
                    {
                        inTemplate->triple(
                            src->getIdtf(),
                            edge.getType() >> edge.getIdtf(),
                            trg->getAddr() >> trg->getIdtf());
                    }
                    else // A_A_F 
                    {
                        inTemplate->triple(
                            srcType >> src->getIdtf(),
                            edge.getType() >> edge.getIdtf(),
                            trg->getAddr() >> trg->getIdtf());
                    }
                }
                else
                {
                    bool const srcRepl = inTemplate->hasReplacement(src->getIdtf());
                    bool const trgRepl = inTemplate->hasReplacement(trg->getIdtf());

                    if (srcRepl && trgRepl) // F_A_F 
                    {
                        inTemplate->triple(
                            src->getIdtf(),
                            edge.getType() >> edge.getIdtf(),
                            trg->getIdtf());
                    }
                    else if (!srcRepl && trgRepl) // A_A_F 
                    {
                        inTemplate->triple(
                            srcType >> src->getIdtf(),
                            edge.getType() >> edge.getIdtf(),
                            trg->getIdtf());
                    }
                    else if (srcRepl && !trgRepl) // F_A_A 
                    {
                        inTemplate->triple(
                            src->getIdtf(),
                            edge.getType() >> edge.getIdtf(),
                            trgType >> trg->getIdtf());
                    }
                    else
                    {
                        inTemplate->triple(
                            srcType >> src->getIdtf(),
                            edge.getType() >> edge.getIdtf(),
                            trgType >> trg->getIdtf());
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
};

bool ScTemplate::fromScTemplate(ScMemoryContext & ctx, ScAddr const & scTemplateAddr)
{
    ScTemplateBuilder builder(scTemplateAddr, ctx);
    return builder(this);
}
