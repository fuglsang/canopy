#pragma once

#include "ca/core_delegate.h"

namespace ca
{
	namespace core
	{
		template <typename... P>
		using action_t = delegate_t<void(P...)>;
	}
}