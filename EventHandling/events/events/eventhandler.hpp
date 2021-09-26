#pragma once

#include "../delegates/delegate.hpp"
#include "../delegates/multicastdelegate.hpp"

namespace events
{

	template<typename ...Args>
	using EventHandler = delegates::Delegate<void, Args...>;

	template<typename ...Args>
	using EventFunctionHandler = delegates::FunctionDelegate<void, Args...>;

	template<typename Object, typename ...Args>
	using EventMethodHandler = delegates::MethodDelegate<Object, void, Args...>;

	template<typename ...Args>
	EventHandler<Args... > handler(delegates::FunctionPtr<void, Args...> function) {
		return EventFunctionHandler<Args...>(function);
	}

	template<typename Object, typename ...Args>
	EventHandler<Args... > handler(Object* obj, delegates::MethodPtr<Object, void, Args...> method) {
		return EventMethodHandler<Args...>(obj, method);
	}

}
