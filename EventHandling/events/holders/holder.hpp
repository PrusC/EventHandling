#pragma once

#include <variant>

namespace events
{
	namespace holders
	{

		template<typename Ret, typename ...Args>
		using FunctionPtr = Ret(*)(Args...);

		template<typename Object, typename Ret, typename ...Args>
		using MethodPtr = Ret(Object::*)(Args...);

		template<typename Object, typename Ret, typename ...Args>
		using ConstMethodPtr = Ret(Object::*)(Args...) const;

		//template<typename Object, typename Ret, typename ...Args>
		//using NoExceptMethodPtr = MethodPtr<Object, Ret, Args...> noexcept;

		//template<typename Object, typename Ret, typename ...Args>
		//using ConstNoExceptMethodPtr = MethodPtr<Object, Ret, Args...> const noexcept;

		template<typename Ret, typename ...Args>
		class Holder {

		public:

			virtual ~Holder() {}

			bool operator== (const Holder& other) const {
				return equals(other);
			}

			bool operator!= (const Holder& other) const {
				return !(*this == other);
			}

			Ret operator()(Args... args) {
				return invoke(args...);
			}

			virtual Ret invoke(Args... args) = 0;

		protected:
			virtual bool equals(const Holder<Ret, Args...>& other) const = 0;

		};


		template<typename Ret, typename ...Args>
		class FunctionHolder : public Holder<Ret, Args...> {

		public:;
			using Function = FunctionPtr<Ret, Args...>;
			using Type = FunctionHolder<Ret, Args...>;

			FunctionHolder(Function func) : _func(func) {}
			FunctionHolder(const FunctionHolder& other): _func(other._func) {}

			Ret invoke(Args... args) override {
				return _func(std::forward<Args>(args)...);
			}

		protected:
			bool equals(const Holder<Ret, Args...>& other) const override {
				const Type* _other = dynamic_cast<const Type*>(&other);
				return _other != nullptr && _func == _other->_func;
			}

		private:
			Function _func;
		};


		template<typename Object, typename Ret, typename ...Args>
		class MethodHolder : public Holder<Ret, Args...> {

		public:
			using Method = MethodPtr<Object, Ret, Args...>;
			using MethodConst = ConstMethodPtr<Object, Ret, Args...>;
			using Type = MethodHolder<Object, Ret, Args...>;

			MethodHolder(Object* object, Method method) : _object(object), _method(method) {}
			MethodHolder(Object* object, MethodConst method): _object(object), _method(method) {}
			MethodHolder(const MethodHolder& other): _object(other._object), _method(other._method) {}

			Ret invoke(Args... args) override {
				if (std::holds_alternative<Method>(_method)) {
					return (_object->*(std::get<Method>(_method)))(std::forward<Args>(args)...);
				}
				else {
					return (_object->*(std::get<MethodConst>(_method)))(std::forward<Args>(args)...);
				}
				//return (_object->*_method)(std::forward<Args>(args)...);
			}

		protected:
			bool equals(const Holder<Ret, Args...>& other) const override {
				const Type* _other = dynamic_cast<const Type*>(&other);
				return _other != nullptr && _object == _other->_object && _method == _other->_method;
			}

		private:

			Object* _object;
			std::variant<Method, MethodConst> _method;
			//Method _method;

		};

		//template<typename Callable, typename Ret, typename ...Args>
		//class FuncObjectHolder : public Holder<Ret, Args...> {

		//public:
		//	using Type = MethodHolder<FunObject, Ret, Args...>;

		//	FuncObjectHolder(FunObject & f){
		//		if (std::is_invocable_r_v<Ret, decltype(Callable), Args...>) {
		//			throw std::invalid_argument("Wrong callable object");
		//		}
		//		obj = f;
		//		fn = [](Args...args) {
		//			return std::invoke(obj, args...);
		//		}
		//	}
		//	FuncObjectHolder(FunObject && f){
		//		if (std::is_invocable_r_v<Ret, decltype(Callable), Args...>) {
		//			throw std::invalid_argument("Wrong callable object");
		//		}
		//		obj = std::move(f);
		//		fn = [](Args...args) {
		//			return std::invoke(obj, args...);
		//		}
		//	}

		//	Ret invoke(Args... args) override {
		//		return fn(std::forward<Args>(args)...);
		//	}

		//protected:
		//	bool equals(const Holder<Ret, Args...>& other) const override {
		//		const Type* _other = dynamic_cast<const Type*>(&other);
		//		return _other != nullptr && _object == _other->_object && _method == _other->_method;
		//	}

		//private:
		//	FunObject obj;
		//	FunctionPtr fn;

		//};

	}
}


