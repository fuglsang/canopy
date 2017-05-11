#pragma once

#include "ca/core_action.h"

namespace ca
{
	namespace core
	{
		template <typename T>
		struct ilist_t
		{
			ilist_t * head;
			ilist_t * next;
			ilist_t * prev;
			T data;
		};

		template <typename T>
		void create_ilist(ilist_t<T> * node, T data)
		{
			node->head = node;
			node->next = node;
			node->prev = node;
			node->data = data;
		}

		template <typename T>
		void ilist_disconnect(ilist_t<T> * node)
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->prev = node;
			node->next = node;
			node->head = node;
		}

		template <typename T>
		void ilist_connect_after(ilist_t<T> * node, ilist_t<T> * other_node)
		{
			ilist_disconnect(node);

			node->prev = other_node;
			node->next = other_node->next;
			other_node->next = node;
			node->next->prev = node;
			node->head = other_node->head;
		}

		template <typename T>
		void ilist_foreach(ilist_t<T> * node, action_t<T> action)
		{
			ilist_t<T> * head = node->head;			
			while (node != head)
			{
				action(node->data);
				node = node->next;
			}
		}
	}
}