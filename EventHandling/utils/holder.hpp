#pragma once

#include <variant>

namespace events
{

	namespace ptr
	{
		template<typename Ret, typename ...Args>
		using FunctionPtr = Ret(*)(Args...);

		template<typename Object, typename Ret, typename ...Args>
		using MethodPtr = Ret(Object::*)(Args...);

		template<typename Object, typename Ret, typename ...Args>
		using ConstMethodPtr = Ret(Object::*)(Args...) const;

		template<typename Object, typename Ret, typename ...Args>
		using NoExceptMethodPtr = Ret(Object::*)(Args...) noexcept;

		template<typename Object, typename Ret, typename ...Args>
		using ConstNoExceptMethodPtr = Ret(Object::*)(Args...) const noexcept;
	}

	namespace holders
	{
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
			using Function = ptr::FunctionPtr<Ret, Args...>;
			using Type = FunctionHolder<Ret, Args...>;

			FunctionHolder(ptr::FunctionPtr<Ret, Args...> func) : _func(func) {}
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
			ptr::FunctionPtr<Ret, Args...> _func;
		};


		template<typename Object, typename Ret, typename ...Args>
		class MethodHolder : public Holder<Ret, Args...> {

		public:
			using Method = ptr::MethodPtr<Object, Ret, Args...>;
			using MethodConst = ptr::ConstMethodPtr<Object, Ret, Args...>;
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
			std::variant<
				ptr::MethodPtr<Object, Ret, Args...>,
				ptr::ConstMethodPtr<Object, Ret, Args...>> _method;
			//Method _method;

		};

		/*template<typename Callable, typename Ret, typename ...Args>
		class CallableHolder : public Holder<Ret, Args...> {

		public:
			using Type = CallableHolder<Callable, Ret, Args...>;

			CallableHolder(Callable& f){
				if (!std::is_invocable_r_v<Ret, decltype(Callable), Args...>) {
					throw std::invalid_argument("Wrong callable object");
				}
				obj = &f;
				fn = [](Args...args) -> Ret {
					return std::invoke(obj, args...);
				};
				isOwner = false;
			}
			CallableHolder(Callable&& f){
				if (std::is_invocable_r_v<Ret, decltype(Callable), Args...> &&
					std::is_class_v<Callable>) {
					throw std::invalid_argument("Wrong callable object");
				}
				obj = new Callable(std::move(f));
				fn = [](Args...args) -> Ret {
					return std::invoke(obj, args...);
				};
				isOwner = true;
			}

			~CallableHolder() {
				if (isOwner) {
					delete obj;
				}
			}

			Ret invoke(Args... args) override {
				return fn(std::forward<Args>(args)...);
			}

		protected:
			bool equals(const Holder<Ret, Args...>& other) const override {
				const Type* _other = dynamic_cast<const Type*>(&other);
				return _other != nullptr && obj == _other->obj && fn == _other->fn;
			}

		private:
			Callable obj;
			bool isOwner;
			FunctionPtr<Ret, Args...> fn;

		};*/

	}
}


