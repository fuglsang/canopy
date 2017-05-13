#pragma once

namespace ca
{
	namespace core
	{
		template <typename Signature>
		struct delegate_t;

		template <typename R, typename... P>
		struct delegate_t<R(P...)>
		{
			typedef R(*stub_t)(void *, P...);

			void * instance;
			stub_t function;

			inline R operator() (P... args) const
			{
				return function(instance, args...);
			}

			template <R(*f)(P...)>
			static inline R stub_static(void * instance, P... args)
			{
				return (*f)(args...);
			}

			template <typename C, R(C::*f)(P...)>
			static inline R stub_member(void * instance, P... args)
			{
				return (static_cast<C *>(instance)->*f)(args...);
			}

			template <typename C, R(C::*f)(P...) const>
			static inline R stub_member(void * instance, P... args)
			{
				return (static_cast<C *>(instance)->*f)(args...);
			}

			template <typename C>
			static inline R stub_object(void * instance, P... args)
			{
				return (*static_cast<C *>(instance))(args...);
			}
		};

		template <typename Signature>
		struct resolve_delegate_t;

		template <typename R, typename... P>
		struct resolve_delegate_t<R(*)(P...)>
		{
			typedef delegate_t<R(P...)> delegate_t;
		};

		template <typename C, typename R, typename... P>
		struct resolve_delegate_t<R(C::*)(P...)>
		{
			typedef delegate_t<R(P...)> delegate_t;
		};

		template <typename C, typename R, typename... P>
		struct resolve_delegate_t<R(C::*)(P...) const>
		{
			typedef delegate_t<R(P...)> delegate_t;
		};

		template <typename F, F f>
		auto make_delegate()
		{
			resolve_delegate_t<F>::delegate_t d;
			d.instance = nullptr;
			d.function = &resolve_delegate_t<F>::delegate_t::stub_static<f>;
			return d;
		}

		template <typename F, F f, typename C>
		auto make_delegate(C * instance)
		{
			resolve_delegate_t<F>::delegate_t d;
			d.instance = instance;
			d.function = &resolve_delegate_t<F>::delegate_t::stub_member<C, f>;
			return d;
		}

		template <typename C>
		auto make_delegate(C * instance)
		{
			resolve_delegate_t<decltype(&C::operator())>::delegate_t d;
			d.instance = instance;
			d.function = &resolve_delegate_t<decltype(&C::operator())>::delegate_t::stub_object<C>;
			return d;
		}
	}
}