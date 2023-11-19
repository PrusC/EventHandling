#pragma once

#include <exception>
#include <optional>
#include <stdexcept>
//#include <utility>
#include <future>

namespace eh {

namespace utils {

template <typename T>
class ResultStore final {
 public:
  T Retrieve() {
    if (m_value.has_value()) {
      return std::move(*m_value);
    }
    if (m_exception) {
      std::rethrow_exception(m_exception);
    }
    throw std::logic_error("result store is not ready");
  }

  const T& Get() const {
    if (m_value.has_value()) {
      return *m_value;
    }
    if (m_exception) {
      std::rethrow_exception(m_exception);
    }
    throw std::logic_error("result store is not ready");
  };

  bool HasValue() const { return m_value.has_value(); }

  void SetValue(const T& value) { 
    m_value = value;
    m_promise.set_value(m_value.value());
  }
  void SetValue(T&& value) { 
    m_value.emplace(std::move(value));
    m_promise.set_value(m_value.value());
  }
  void SetException(std::exception_ptr&& exception) noexcept {
    m_exception = std::move(exception);
    m_promise.set_exception(m_exception);
  }

  std::future<T> GetFuture() { return std::move(m_promise.get_future()); }

 private:
  std::optional<T> m_value;
  std::exception_ptr m_exception;
  std::promise<T> m_promise;
};

template <>
class ResultStore<void> final {
 public:
  void Retrieve() { Get(); }

  void Get() const {
    if (m_has_value) {
      return;
    }
    if (m_exception) {
      std::rethrow_exception(m_exception);
    }
    throw std::logic_error("result store is not ready");
  };

   bool HasValue() const { return m_has_value; }

  void SetValue() noexcept { 
    m_has_value = true;
    m_promise.set_value();
  }

  void SetException(std::exception_ptr&& exception) noexcept {
    m_exception = std::move(exception);
    m_promise.set_exception(m_exception);
  }

  std::future<void> GetFuture() { return std::move(m_promise.get_future()); }

 private:
  bool m_has_value{false};
  std::exception_ptr m_exception;
  std::promise<void> m_promise;
};

}  // namespace utils

}  // namespace eh