/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_types.hpp"
#include "sc_utils.hpp"
#include "sc_addr.hpp"

#include <functional>

/* Base class for sc-events
 */
class ScEvent
{
public:
	typedef std::function<bool(const ScAddr &, const ScAddr &)>	tDelegateFunc;

	typedef uint8_t Type;
	static Type AddOutputEdge;
	static Type AddInputEdge;
	static Type RemoveOutputEdge;
	static Type RemoveInputEdge;
	static Type EraseElement;
	static Type ContentChanged;

	explicit _SC_EXTERN ScEvent(const ScMemoryContext & ctx, const ScAddr & addr, Type eventType);
	explicit _SC_EXTERN ScEvent(const ScMemoryContext & ctx, const ScAddr & addr, Type eventType, tDelegateFunc func);
	virtual _SC_EXTERN ~ScEvent();

	// Don't allow copying of events
	ScEvent(const ScEvent & other) = delete;

	/* Set specified function as a delegate that will be calls on event emit */
	template <typename FuncT>
	void setDelegate(FuncT & func)
	{
		mDelegate = func;
	}

	void removeDelegate();

protected:
	static sc_result _handler(sc_event const * evt, sc_addr arg);
	static sc_result _handlerDelete(sc_event const * evt);

private:
	sc_event * mEvent;
	tDelegateFunc mDelegate;
};

SHARED_PTR_TYPE(ScEvent);

class ScEventAddOutputEdge final : public ScEvent
{
	friend class ScMemoryContext;

public:
	_SC_EXTERN ScEventAddOutputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::tDelegateFunc func)
		: ScEvent(ctx, addr, ScEvent::AddOutputEdge, func)
	{
	}
};

class ScEventAddInputEdge final : public ScEvent
{
	friend class ScMemoryContext;
public:
	_SC_EXTERN ScEventAddInputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::tDelegateFunc func)
		: ScEvent(ctx, addr, ScEvent::AddInputEdge, func)
	{
	}
};

class ScEventRemoveOutputEdge final : public ScEvent
{
	friend class ScMemoryContext;
public:
	_SC_EXTERN ScEventRemoveOutputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::tDelegateFunc func)
		: ScEvent(ctx, addr, ScEvent::RemoveOutputEdge, func)
	{
	}
};

class ScEventRemoveInputEdge final : public ScEvent
{
	friend class ScMemoryContext;
public:
	_SC_EXTERN ScEventRemoveInputEdge(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::tDelegateFunc func)
		: ScEvent(ctx, addr, ScEvent::RemoveInputEdge, func)
	{
	}
};

class ScEventEraseElement final : public ScEvent
{
	friend class ScMemoryContext;
public:
	_SC_EXTERN ScEventEraseElement(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::tDelegateFunc func)
		: ScEvent(ctx, addr, ScEvent::EraseElement, func)
	{
	}
};

class ScEventContentChanged final : public ScEvent
{
	friend class ScMemoryContext;
public:
	_SC_EXTERN ScEventContentChanged(const ScMemoryContext & ctx, const ScAddr & addr, ScEvent::tDelegateFunc func)
		: ScEvent(ctx, addr, ScEvent::ContentChanged, func)
	{
	}
};
