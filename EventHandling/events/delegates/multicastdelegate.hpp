#pragma once

#include "delegate.hpp"
#include <list>
#include <mutex>

namespace events
{
	namespace delegates
	{
		namespace
		{
			template<typename Ret, typename ...Args>
			struct MulticastDelegateCore {
				
				using DelegateSharedPtr = std::shared_ptr<Delegate<Ret, Args...>>;

				std::list<DelegateSharedPtr> handlers;
				mutable std::mutex coreMutex;

				inline auto find(const Delegate<Ret, Args...>& delegate) {
					for (auto it = handlers.begin(); it != handlers.end(); ++it) {
						if (*(*it) == delegate) {
							return it;
						}
					}
					return handlers.end();
				}

			};
		}

		template<typename Ret, typename ...Args>
		class MulticastDelegate: public AbstractDelegate<Ret, Args...> {
		
		public:
			MulticastDelegate(): core() {}
			MulticastDelegate(const MulticastDelegate& other) = delete;
			MulticastDelegate(const MulticastDelegate&& other) = delete;

			Ret invoke(Args... args) override {
				if (core.handlers.empty()) {
					throw DelegateException("Multicast Delegate list is empty");
				}
				auto it = core.handlers.begin();
				while (it != prev(core.handlers.end())) {
					std::lock_guard<std::mutex> g(core.coreMutex);
					(*it)->invoke(std::forward<Args>(args)...);
					++it;
				}
				std::lock_guard<std::mutex> g(core.coreMutex);
				return (*it)->invoke(std::forward<Args>(args)...);
			}

			explicit operator bool() const override {
				std::lock_guard<std::mutex> g(core.coreMutex);
				return core.handlers.empty();
			}

			void operator +=(const Delegate<Ret, Args...>& delegate) {
				std::lock_guard<std::mutex> g(core.coreMutex);
				core.handlers.push_back(std::make_shared<Delegate<Ret, Args...>>(delegate));
			}

			void operator +=(Delegate<Ret, Args...>&& delegate) {
				std::lock_guard<std::mutex> g(core.coreMutex);
				core.handlers.push_back(std::make_shared<Delegate<Ret, Args...>>(std::move(delegate)));
			}

			void operator +=(const std::shared_ptr<Delegate<Ret, Args...>>& delegate) {
				std::lock_guard<std::mutex> g(core.coreMutex);
				core.handlers.push_back(delegate);
			}

			void operator -=(const Delegate<Ret, Args...>& delegate) {
				std::lock_guard<std::mutex> g(core.coreMutex);
				auto it = core.find(delegate);
				if (it != core.handlers.end()) {
					core.handlers.erase(it);
				}
			}

		protected:
			bool equals(const AbstractDelegate<Ret, Args...>& other) const override {
				const Type* _other = dynamic_cast<const Type*>(&other);
				return _other && core.handlers == _other->core.handlers;
			}

		private:
			using Type = MulticastDelegate<Ret, Args...>;
			MulticastDelegateCore<Ret, Args...> core;

		};


		template<typename Ret, typename ...Args>
		MulticastDelegate<Ret, Args...> operator+(const Delegate<Ret, Args...>& ld, const Delegate<Ret, Args...>& rd) {
			MulticastDelegate<Ret, Args...> res;
			res += ld;
			res += rd;
			return res;
		}


	}
}
