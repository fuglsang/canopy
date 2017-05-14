#pragma once

#include "ca/core_action.h"
#include "ca/core_ilist.h"

namespace ca
{
	namespace core
	{
		template <typename... P>
		struct eventhandler_t
		{
			action_t<P...> action;
			ilist_t<eventhandler_t<P...>> node;
		};

		template <typename... P>
		struct event_t
		{
			ilist_t<eventhandler_t<P...>> node;
		};

		template <typename... P>
		void create_event(event_t<P...> * event)
		{
			create_ilist(&event->node);
		}

		template <typename... P>
		void destroy_event(event_t<P...> * event)
		{
			destroy_ilist(&event->node);
		}

		template <typename... P>
		void event_dispatch(event_t<P...> * event, P... args)
		{
			auto dispatch_with_args = [&](eventhandler_t<P...> * handler)
			{
				handler->action(args...);
			};
			ilist_foreach(&event->node, CA_DELEGATE_ANON(&dispatch_with_args));
		}

		template <typename... P>
		void create_eventhandler(eventhandler_t<P...> * handler, event_t<P...> * event, action_t<P...> action)
		{
			handler->action = action;
			create_ilist(&handler->node, handler);
			ilist_link_at_tail(&handler->node, &event->node);
		}

		template <typename... P>
		void destroy_eventhandler(eventhandler_t<P...> * handler)
		{
			ilist_unlink(&handler->node);
		}
	}
}