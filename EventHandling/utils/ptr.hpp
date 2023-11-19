#pragma once
#include <type_traits>
#include <memory>

namespace eh {

namespace ptr {

template <typename Ret, typename... Args>
using FunctionPtr = Ret (*)(Args...);

template <typename Object, typename Ret, typename... Args>
using MethodPtr = Ret (Object::*)(Args...);

template <typename Object, typename Ret, typename... Args>
using ConstMethodPtr = Ret (Object::*)(Args...) const;

template <typename Object, typename Ret, typename... Args>
using NoExceptMethodPtr = Ret (Object::*)(Args...) noexcept;

template <typename Object, typename Ret, typename... Args>
using ConstNoExceptMethodPtr = Ret (Object::*)(Args...) const noexcept;

}  // namespace ptr

template <typename T>
class Storage;

template <typename T>
class Storage<T*> {
 public:
  using Type = T*;
  explicit Storage(const Type& object) : _object(object) {}
  Type lock() { return _object; }
  bool expired() const { return _object == nullptr; }

 private:
  T* _object;
};

template <typename T>
class Storage<std::shared_ptr<T>> {
 public:
  using Type = std::shared_ptr<T>;
  explicit Storage(const Type& object) : _object(object) {}
  Type lock() { return _object.lock(); }
  bool expired() const { return _object.expired(); }

 private:
  std::weak_ptr<T> _object;
};

namespace traits {

template <typename Func>
struct FunctionPointer {
  constexpr static bool is_pointer_member = false;
};

template <typename Ret, typename... Args>
struct FunctionPointer<Ret (*)(Args...)> {
  using ReturnType = Ret;
  using Function = Ret (*)(Args...);
  using Arguments = std::tuple<Args...>;

  constexpr static bool is_pointer_member = false;

  static ReturnType invoke(Function f, void*, Args...) {
    return std::invoke(std::forward<Function>(f), std::forward<Args>...);
  }
};

template <typename Obj, typename Ret, typename... Args>
struct FunctionPointer<ptr::MethodPtr<Obj, Ret, Args...>> {
  using ReturnType = Ret;
  using Object = Obj;
  using Function = ptr::MethodPtr<Obj, Ret, Args...>;
  using Arguments = std::tuple<Args...>;

  constexpr static bool is_pointer_member = true;

  static ReturnType invoke(Function f, void* obj, Args...) {
    return std::invoke(std::forward<Function>(f), static_cast<Obj*>(obj),
                       std::forward<Args>...);
  }
};

template <typename Obj, typename Ret, typename... Args>
struct FunctionPointer<ptr::ConstMethodPtr<Obj, Ret, Args...>> {
  using ReturnType = Ret;
  using Object = Obj;
  using Function = ptr::ConstMethodPtr<Obj, Ret, Args...>;
  using Arguments = std::tuple<Args...>;

  constexpr static bool is_pointer_member = true;

  static ReturnType invoke(Function f, void* obj, Args...) {
    return std::invoke(std::forward<Function>(f), static_cast<Obj*>(obj),
                       std::forward<Args>...);
  }
};

template <typename Obj, typename Ret, typename... Args>
struct FunctionPointer<ptr::NoExceptMethodPtr<Obj, Ret, Args...>> {
  using ReturnType = Ret;
  using Object = Obj;
  using Function = ptr::NoExceptMethodPtr<Obj, Ret, Args...>;
  using Arguments = std::tuple<Args...>;

  constexpr static bool is_pointer_member = true;

  static ReturnType invoke(Function f, void* obj, Args...) {
    return std::invoke(std::forward<Function>(f), static_cast<Obj*>(obj),
                       std::forward<Args>...);
  }
};

template <typename Obj, typename Ret, typename... Args>
struct FunctionPointer<ptr::ConstNoExceptMethodPtr<Obj, Ret, Args...>> {
  using ReturnType = Ret;
  using Object = Obj;
  using Function = ptr::ConstNoExceptMethodPtr<Obj, Ret, Args...>;
  using Arguments = std::tuple<Args...>;

  constexpr static bool is_pointer_member = true;

  static ReturnType invoke(Function f, void* obj, Args...) {
    return std::invoke(std::forward<Function>(f), static_cast<Obj*>(obj),
                       std::forward<Args>...);
  }
};

}  // namespace traits

}  // namespace eh
