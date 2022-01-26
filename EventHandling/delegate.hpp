#pragma once

#include <memory>
#include <exception>

#include "utils/holder.hpp"

namespace events
{
	namespace delegates
	{

		class DelegateException: std::exception {
		public:
			DelegateException(): std::exception() {}
			DelegateException(char const* const message): std::exception(message) {}
		};

		template<typename Ret, typename ...Args>
		class AbstractDelegate {

		public:
			virtual ~AbstractDelegate() {}
			virtual Ret invoke(Args... args) = 0;
			virtual explicit operator bool() const = 0;

			Ret operator()(Args... args) {
				return invoke(std::forward<Args>(args)...);
			}

			bool operator==(const AbstractDelegate& other) const {
				return equals(other);
			}

			bool operator!=(const AbstractDelegate& other) const {
				return !(*this == other);
			}

		protected:
			virtual bool equals(const AbstractDelegate<Ret, Args...>& other) const = 0;
		
		};

		template<typename Ret, typename ...Args>
		class Delegate: public AbstractDelegate<Ret, Args...> {

		public:

			Delegate(): _holder(nullptr) {}

			Delegate(const Delegate& other): _holder(other._holder) {}

			Delegate(Delegate&& other): _holder(std::move(other._holder)) {}

			template <typename Object>
			Delegate(Object* object, ptr::MethodPtr<Object, Ret, Args...> method): 
				_holder(std::make_shared<holders::MethodHolder<Object, Ret, Args...>>(object, method)) {}

			template <typename Object>
			Delegate(Object* object, ptr::ConstMethodPtr<Object, Ret, Args...> method) :
				_holder(std::make_shared<holders::MethodHolder<Object, Ret, Args...>>(object, method)) {}

			Delegate(ptr::FunctionPtr<Ret, Args...> function):
				_holder(std::make_shared<holders::FunctionHolder<Ret, Args...>>(function)) {}

			Ret invoke(Args...args) override {
				if (_holder) {
					return _holder->invoke(std::forward<Args>(args)...);
				}
				throw DelegateException("Delegate does not hold the function");
			}

			explicit operator bool() const override {
				return bool(_holder);
			}

		protected:
			bool equals(const AbstractDelegate<Ret, Args...>& other) const override {
				const Delegate<Ret, Args...>* _other = dynamic_cast<const Delegate<Ret, Args...>*>(&other);
				return _other && _holder && _holder.get() == (_other->_holder).get();
			}

			explicit Delegate(std::shared_ptr<holders::Holder<Ret, Args...>>&& holder) :
				_holder(std::move(holder)) {}

			std::shared_ptr<holders::Holder<Ret, Args...>> _holder;
		};



		template<typename Ret, typename ...Args>
		class FunctionDelegate : public Delegate<Ret, Args...> {

		private:
			using Holder = holders::FunctionHolder<Ret, Args...>;

		public:
			using Function = ptr::FunctionPtr<Ret, Args...>;

			FunctionDelegate() = default;

			FunctionDelegate(Function function): 
				Delegate<Ret, Args...>(std::make_shared<Holder>(function)) { }

			void bind(Function function) {
				this->_holder = std::make_shared<Holder>(function);
			}

		};


		template<typename Object, typename Ret, typename ...Args>
		class MethodDelegate : public Delegate<Ret, Args...> {

		private:
			using Holder = holders::MethodHolder<Object, Ret, Args...>;

		public:
			using Method = ptr::MethodPtr<Object, Ret, Args...>;
			using MethodConst = ptr::ConstMethodPtr<Object, Ret, Args...>;
			using ObjectPtr = Object*;

			MethodDelegate() = default;

			MethodDelegate(ObjectPtr object, Method method): 
				Delegate<Ret, Args...>(std::make_shared<Holder>(object, method)) { }

			MethodDelegate(ObjectPtr object, MethodConst method):
				Delegate<Ret, Args...>(std::make_shared<Holder>(object, method)) {}
		
			void bind(ObjectPtr object, Method method) {
				this->_holder = std::make_shared<Holder>(object, method);
			}

			void bind(ObjectPtr object, MethodConst method) {
				this->_holder = std::make_shared<Holder>(object, method);
			}
		};


		template<typename Obj, typename Ret, typename ...Args>
		using MethodPtr = ptr::MethodPtr<Obj, Ret, Args...>;


		template<typename Obj, typename Ret, typename ...Args>
		using ConstMethodPtr = ptr::ConstMethodPtr<Obj, Ret, Args...>;


		template<typename Ret, typename ...Args>
		using FunctionPtr = ptr::FunctionPtr<Ret, Args...>;



		template<typename Obj, typename Ret, typename ...Args>
		Delegate<Ret, Args...> delegate(Obj* obj, MethodPtr<Obj, Ret, Args...> method) {
			return Delegate<Ret, Args...>(obj, method);
		}

		template<typename Obj, typename Ret, typename ...Args>
		Delegate<Ret, Args...> delegate(Obj* obj, ConstMethodPtr<Obj, Ret, Args...> method) {
			return Delegate<Obj, Ret, Args...>(obj, method);
		}


		template<typename Ret, typename ...Args>
		Delegate<Ret, Args...> delegate(FunctionPtr<Ret, Args...> function) {
			return Delegate<Ret, Args...>(function);
		}

		template<typename Obj, typename Ret, typename ...Args>
		std::shared_ptr<Delegate<Ret, Args...>> delegate1(Obj* obj, MethodPtr<Obj, Ret, Args...> method) {
			return std::make_shared<Delegate<Ret, Args...>>(obj, method);
		}

		template<typename Ret, typename ...Args>
		std::shared_ptr<Delegate<Ret, Args...>> delegate1(FunctionPtr<Ret, Args...> function) {
			return std::make_shared<Delegate<Ret, Args...>>(function);
		}

	}
}


