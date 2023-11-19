#pragma once

#include <memory>
#include <optional>
#include <thread>

#include "delegate.hpp"
#include "utils\callback_queue_base.hpp"
#include "utils\wrapped_call.hpp"

namespace eh {

namespace delegates {

enum class InvokeType { 
  Direct, 
  Async, 
  Queued, 
  BlockQueued, 
  Auto };

struct DelegeteInvokeParameters {
  InvokeType invoke_type{InvokeType::Direct};
  std::optional<std::thread::id> thread_id;
  utils::ICallbackQueuePtr callback_queue;
};

}  // namespace delegates

}  // namespace eh
