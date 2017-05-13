#pragma once

#include "ca/core_action.h"
#include "ca/core_ilist.h"

namespace ca
{
	namespace core
	{
		template <typename... P>
		struct eventlistener_t
		{
			action_t<P...> action;
			ilist_t<eventlistener_t<P...>> node;
		};

		template <typename... P>
		struct event_t
		{
			ilist_t<eventlistener_t<P...>> node;
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
			auto dispatch_with_args = [&](eventlistener_t<P...> * listener)
			{
				listener->action(args...);
			};
			ilist_foreach(&event->node, make_delegate(&dispatch_with_args));
		}

		template <typename... P>
		void create_eventlistener(eventlistener_t<P...> * listener, action_t<P...> action, event_t<P...> * event)
		{
			listener->action = action;
			create_ilist(&listener->node, listener);
			ilist_link_at_tail(&listener->node, &event->node);
		}

		template <typename... P>
		void destroy_eventlistener(eventlistener_t<P...> * listener)
		{
			ilist_unlink(&listener->node);
		}
	}
}