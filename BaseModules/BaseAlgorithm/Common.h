#pragma once

#include <functional>
#include <vector>
#include "Position.h"
#include "Annotation.h"
#include "../InteractiveBrokersClient/InteractiveBrokersClient.h"
#include "../InteractiveBrokersApi/Tick.h"
#include "../InteractiveBrokersApi/bar.h"

// the entire trading engine is implemented based on dispatching new tick data as callbacks
// through various layers and algorithms. aliasing here to reduce verbosity
using TickListener = std::function<void(const Tick& tick)>;
using CallbackHandle = int;
const CallbackHandle INVALID_CALLBACK_HANDLE = -1;

using PositionId = int;