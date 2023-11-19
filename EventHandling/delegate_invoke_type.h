#pragma once

namespace eh {

namespace delegates {

enum class InvokeType { 
  Direct, 
  Async, 
  Queued, 
  BlockQueued, 
  Auto 
};

}  // namespace delegates

}  // namespace eh
