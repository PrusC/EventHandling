#pragma once

#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include "helper.hpp"


namespace eh {

namespace utils {

class ArgumentPackApplyError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

template <typename... Args>
class ArgumentPack {
 public:
  using ArgsTuple = std::tuple<Args...>;

  explicit ArgumentPack(Args&&... args)
      : m_args(std::forward_as_tuple(args...)) {}

  ArgumentPack(const ArgumentPack& pack) : m_args(pack.m_args) {}
  ArgumentPack(ArgumentPack&& pack) = delete;

  const ArgsTuple& Arguments() const { return m_args; }
  ArgsTuple& Arguments() { return m_args; }

  template <typename Callable>
  decltype(auto) Apply(Callable&& c) {
    if constexpr (is_shared_or_weak_ptr_v<
                      std::remove_cv_t<std::remove_reference_t<Callable>>>) {
      return ApplyPtr(std::forward<Callable>(c));
    } else {
      return std::apply(std::forward<Callable>(c),
                        std::forward<ArgsTuple>(m_args));
    }
  }

 private:
  template <typename Callable>
  decltype(auto) ApplyPtr(const std::shared_ptr<Callable>& c) {
    return std::apply(*c, std::forward<ArgsTuple>(m_args));
  }

  template <typename Callable>
  decltype(auto) ApplyPtr(const std::weak_ptr<Callable>& c) {
    if (auto lock = c.lock(); lock != nullptr) {
      return ApplyPtr<Callable>(lock);
    }
    throw ArgumentPackApplyError("The Callable object was already deleted");
  }

 private:
  ArgsTuple m_args;
};

}  // namespace utils

}  // namespace eh
