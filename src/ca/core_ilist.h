#pragma once

#include "ca/core_action.h"
#include "ca/core_log.h"

namespace ca
{
	namespace core
	{
		template <typename T>
		struct ilist_t
		{
			ilist_t<T> * head;
			ilist_t<T> * next;
			ilist_t<T> * prev;
			T * data;
		};

		template <typename T>
		void create_ilist(ilist_t<T> * node)
		{
			node->head = node;
			node->next = node;
			node->prev = node;
			node->data = nullptr;
		}

		template <typename T>
		void create_ilist(ilist_t<T> * node, T * data)
		{
			node->head = node;
			node->next = node;
			node->prev = node;
			node->data = data;
		}

		template <typename T>
		void ilist_unlink(ilist_t<T> * node);

		template <typename T>
		void destroy_ilist(ilist_t<T> * node)
		{
			ilist_t<T> * head = node->head;
			if (node != head)
			{
				CA_WARN("destroy not targeting head: unlinking only current node");
				ilist_unlink(node);
			}
			else
			{
				while (head->next != head)
				{
					ilist_unlink(head->next);
				}
			}
		}

		template <typename T>
		void ilist_unlink(ilist_t<T> * node)
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->prev = node;
			node->next = node;
			node->head = node;
		}

		template <typename T>
		void ilist_link_after(ilist_t<T> * node, ilist_t<T> * other_node)
		{
			ilist_unlink(node);			
			node->prev = other_node;
			node->next = other_node->next;
			other_node->next = node;
			node->next->prev = node;
			node->head = other_node->head;
		}

		template <typename T>
		void ilist_link_at_head(ilist_t<T> * node, ilist_t<T> * other_node)
		{
			ilist_link_after(node, other_node->head);
		}

		template <typename T>
		void ilist_link_at_tail(ilist_t<T> * node, ilist_t<T> * other_node)
		{
			ilist_link_after(node, other_node->head->prev);
		}

		template <typename T>
		void ilist_foreach(ilist_t<T> * node, action_t<T *> action)
		{
			ilist_t<T> * head = node->head;
			node = node->next;
			while (node != head)
			{
				action(node->data);
				node = node->next;
			}
		}
	}
}