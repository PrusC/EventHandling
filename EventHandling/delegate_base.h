#pragma once

#include <stdexcept>
#include "delegate_invoke_type.h"

namespace eh {

namespace delegates {

class DelegateException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};


template <typename Ret, typename... Args>
class IDelegate {
 public:
  virtual ~IDelegate() {}
  virtual Ret Invoke(Args&&... args) = 0;
  virtual bool IsEmpty() const = 0;
  virtual void Reset() = 0;
  virtual void SetInvokeType(InvokeType type) = 0;
  virtual void SetThreadId(std::thread::id id) = 0;

  Ret operator()(Args&&... args) { return Invoke(std::forward<Args>(args)...); }
  bool operator==(const IDelegate& other) const { return Equals(other); }
  bool operator!=(const IDelegate& other) const { return !(*this == other); }

 protected:
  virtual bool Equals(const IDelegate<Ret, Args...>& other) const = 0;

};

}  // namespace delegates

}  // namespace eh
