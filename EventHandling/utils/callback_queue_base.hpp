#pragma once

#include <list>
#include <memory>
#include <mutex>

#include "wrapped_call.hpp"

namespace eh {

namespace utils {

class ICallbackQueue : public std::enable_shared_from_this<ICallbackQueue> {
 public:
  virtual ~ICallbackQueue() {}

  virtual void addCallback(const WrappedCallBasePtr& callback) = 0;
  virtual WrappedCallBasePtr Get() = 0;

  virtual bool empty() const = 0;
};

using ICallbackQueuePtr = std::shared_ptr<ICallbackQueue>;
using ICallbackQueueWPtr = std::weak_ptr<ICallbackQueue>;

}  // namespace utils

}  // namespace eh
