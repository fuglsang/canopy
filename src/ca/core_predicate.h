#pragma once

#include "ca/core_delegate.h"

namespace ca
{
	namespace core
	{
		template <typename... P>
		using predicate_t = delegate_t<bool(P...)>;
	}
}