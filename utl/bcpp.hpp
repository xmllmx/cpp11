#pragma once

#include <internal/stdcpp.hpp>

#if defined(_ZW_KERNEL_ONLY_)
/**/#include <internal/kmacro.hpp>
/**/#include <internal/knew.hpp>
#else
/**/#include <new>
/**/#include <cassert>
/**/#define Assert  assert
/**/#define Abort() abort()

/**/#if defined(NDEBUG)
/******/#define Print(_x_)
/**/#else
/******/#define Print(_x_) Log(_x_)
/**/#endif

#endif

#define AssertIf(condition, expr) Assert((condition) ? !!(expr) : true)