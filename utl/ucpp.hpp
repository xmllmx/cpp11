#pragma once

#include <cassert>

#define Assert  assert
#define Abort() abort()

#if defined(NDEBUG)
/**/#define Print(_x_)
#else
/**/#define Print(_x_) Log(_x_)
#endif

#include <tuple>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>

#include <internal/stdcpp.hpp>
#include <internal/log.hpp>