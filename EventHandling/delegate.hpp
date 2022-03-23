#pragma once

#include <memory>
#include <exception>

#include "utils/holder.hpp"

namespace events {

  namespace delegates {

    class DelegateException: public std::exception {
    public:
      //explicit DelegateException(): std::exception(), msg("") {}
      explicit DelegateException(char const* message): msg(message) {}
      const char* what() const noexcept override { return msg; }
    private:
      const char* msg;
    };

    template<typename Ret, typename ...Args>
    class IDelegate {

    public:
      virtual ~IDelegate() {}
      virtual Ret invoke(Args... args) = 0;
      virtual explicit operator bool() const = 0;

      Ret operator()(Args... args) {
        return invoke(std::forward<Args>(args)...);
      }

      bool operator==(const IDelegate& other) const {
        return equals(other);
      }

      bool operator!=(const IDelegate& other) const {
        return !(*this == other);
      }

    protected:
      virtual bool equals(const IDelegate<Ret, Args...>& other) const = 0;

    };

    template<typename Ret, typename ...Args>
    class Delegate: public IDelegate<Ret, Args...> {

    public:

      Delegate(): m_Holder(nullptr) {}

      Delegate(const Delegate& other): m_Holder(other.m_Holder) {}

      Delegate(Delegate&& other): m_Holder(std::move(other.m_Holder)) {}

      template <typename Object, typename Method>
      Delegate(Object* object, Method method) :
        m_Holder(holders::Factory<Ret, Args...>::create(object, method)) {}

      Delegate(ptr::FunctionPtr<Ret, Args...> function):
        m_Holder(holders::Factory<Ret, Args...>::create(function)) {}

      Ret invoke(Args...args) override {
        if (m_Holder) {
          return m_Holder->invoke(std::forward<Args>(args)...);
        }
        throw DelegateException("Delegate holds nothing");
      }

      explicit operator bool() const override {
        return bool(m_Holder);
      }

    protected:
      bool equals(const IDelegate<Ret, Args...>& other) const override {
        const Delegate<Ret, Args...>* _other = dynamic_cast<const Delegate<Ret, Args...>*>(&other);
        return _other && m_Holder && *m_Holder.get() == *(_other->m_Holder).get();
      }

      std::shared_ptr<holders::Holder<Ret, Args...>> m_Holder;
    };


    template<typename Obj, typename Ret, typename ...Args>
    Delegate<Ret, Args...> delegate(Obj* obj, ptr::MethodPtr<Obj, Ret, Args...> method) {
      return Delegate<Ret, Args...>(obj, method);
    }

    template<typename Obj, typename Ret, typename ...Args>
    Delegate<Ret, Args...> delegate(Obj* obj, ptr::ConstMethodPtr<Obj, Ret, Args...> method) {
      return Delegate<Obj, Ret, Args...>(obj, method);
    }

    template<typename Ret, typename ...Args>
    Delegate<Ret, Args...> delegate(ptr::FunctionPtr<Ret, Args...> function) {
      return Delegate<Ret, Args...>(function);
    }

    template<typename Obj, typename Ret, typename ...Args>
    std::shared_ptr<Delegate<Ret, Args...>> delegate_ptr(Obj* obj, ptr::MethodPtr<Obj, Ret, Args...> method) {
      return std::make_shared<Delegate<Ret, Args...>>(obj, method);
    }

    template<typename Ret, typename ...Args>
    std::shared_ptr<Delegate<Ret, Args...>> delegate_ptr(ptr::FunctionPtr<Ret, Args...> function) {
      return std::make_shared<Delegate<Ret, Args...>>(function);
    }

  }
}


