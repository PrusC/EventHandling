#pragma once

#include <variant>
#include <type_traits>
#include <functional>

namespace events {

  namespace ptr {

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

  namespace holders {

    template<typename Ret, typename ...Args>
    class Holder {

    public:
      virtual ~Holder() {}
      bool operator== (const Holder& other) const { return equals(other); }
      bool operator!= (const Holder& other) const { return !(*this == other); }
      Ret operator()(Args... args) { return invoke(args...); }
      virtual Ret invoke(Args... args) = 0;

    protected:
      virtual bool equals(const Holder<Ret, Args...>& other) const = 0;

    };

    template<typename Ret, typename ...Args>
    class Factory final {

    private:

      class FunctionHolder: public Holder<Ret, Args...> {

        using Type = FunctionHolder;

      public:
        explicit FunctionHolder(ptr::FunctionPtr<Ret, Args...> func): m_Func(func) {}
        explicit FunctionHolder(const FunctionHolder& other): m_Func(other.m_Func) {}

        Ret invoke(Args... args) override {
          return m_Func(std::forward<Args>(args)...);
        }

      protected:
        bool equals(const Holder<Ret, Args...>& other) const override {
          const Type* _other = dynamic_cast<const Type*>(&other);
          return _other != nullptr && m_Func == _other->m_Func;
        }

      private:
        ptr::FunctionPtr<Ret, Args...> m_Func;
      };

      template<typename Object, typename Method>
      using check_for_method = typename std::enable_if_t<
                std::is_member_function_pointer_v<Method>&&
                std::is_invocable_r_v<Ret, Method, Object&, Args...>>;

      template<typename Object, typename Method, typename = check_for_method<Object, Method>>
      class MethodHolder: public Holder<Ret, Args...> {

        using Type = MethodHolder;

      public:

        MethodHolder(Object* object, Method method):
            m_Object(object), m_Method(method) {}
        MethodHolder(const MethodHolder& other): 
            m_Object(other.m_Method), m_Method(other.m_Method) {}

        Ret invoke(Args... args) override {
          return (m_Object->*m_Method)(std::forward<Args>(args)...);
        }

      protected:
        bool equals(const Holder<Ret, Args...>& other) const override {
          const Type* _other = dynamic_cast<const Type*>(&other);
          return _other != nullptr && m_Object == _other->m_Object && m_Method == _other->m_Method;
        }

      private:
        Object* m_Object;
        Method m_Method;
      };

      template<typename Callable, typename = std::enable_if_t<std::is_invocable_r_v<Ret, Callable, Args...>>>
      class CallableHolder: public Holder<Ret, Args...> {
        using Type = CallableHolder;
      public:

        CallableHolder(const Callable& callable): m_Callable(callable) {}
        CallableHolder(Callable&& callable): m_Callable(std::move(callable)) {}
        CallableHolder(const CallableHolder& other): m_Callable(other.m_Callable) {}

        Ret invoke(Args... args) override {
          return m_Callable(std::forward<Args>(args)...);
        }

      protected:
        bool equals(const Holder<Ret, Args...>& other) const override {
          const Type* _other = dynamic_cast<const Type*>(&other);
          return _other != nullptr && m_Callable == _other->m_Callable;
        }

      private:
        Callable m_Callable;
      };

    public:

      constexpr static std::shared_ptr<Holder<Ret, Args...>> create(ptr::FunctionPtr<Ret, Args...> func) {
        return std::make_shared<FunctionHolder>(func);
      }

      template <typename Object, typename Method, typename=check_for_method<Object, Method>>
      constexpr static std::shared_ptr<Holder<Ret, Args...>> create(Object* object, Method method) {
        return std::make_shared<MethodHolder<Object, Method>>(object, method);
      }

      template <typename Callable, typename = std::enable_if_t<std::is_invocable_r_v<Ret, Callable, Args...>>>
      constexpr static std::shared_ptr<Holder<Ret, Args...>> create(Callable&& callable) {
        return std::make_shared<CallableHolder<Callable>>(std::move(callable));
      }

      template <typename Callable, typename = std::enable_if_t<std::is_invocable_r_v<Ret, Callable, Args...>>>
      constexpr static std::shared_ptr<Holder<Ret, Args...>> create(const Callable& callable) {
        return std::make_shared<CallableHolder<Callable>>(callable);
      }

    };

  }

}
