#include <kernel/utils/plist.h>

bool plist_is_head_empty(const struct plist_head *head)
{
	return dlist_is_empty(&head->node_list);
}

bool plist_is_node_empty(const struct plist_node *node)
{
	return dlist_is_empty(&node->node_list);
}

struct plist_node *plist_first(const struct plist_head *head)
{
	return dlist_entry(head->node_list.next, struct plist_node, node_list);
}

struct plist_node *plist_last(const struct plist_head *head)
{
	return dlist_entry(head->node_list.prev, struct plist_node, node_list);
}

void plist_head_init(struct plist_head *head)
{
	dlist_head_init(&head->node_list);
}

void plist_node_init(struct plist_node *node, int prio)
{
	node->prio = prio;
	dlist_head_init(&node->prio_list);
	dlist_head_init(&node->node_list);
}

void plist_add(struct plist_node *node, struct plist_head *head)
{
	struct dlist_head *next_node = &head->node_list;

	if (plist_is_head_empty(head))
		goto add_node;

	struct plist_node *first = plist_first(head);
	struct plist_node *iter = first;
	struct plist_node *prev = NULL;

	do
	{
		if (node->prio < iter->prio)
		{
			next_node = &iter->node_list;
			break;
		}

		prev = iter;
		iter = dlist_entry(iter->prio_list.next, struct plist_node, prio_list);
	} while (iter != first);

	if (!prev || prev->prio != node->prio)
		dlist_add_tail(&node->prio_list, &iter->prio_list);
add_node:
	dlist_add_tail(&node->node_list, next_node);
}

void plist_remove(struct plist_node *node, struct plist_head *head)
{
	if (!dlist_is_empty(&node->prio_list))
	{
		if (node->node_list.next != &head->node_list)
		{
			struct plist_node *next = dlist_entry(node->node_list.next, struct plist_node, node_list);
			if (dlist_is_empty(&next->prio_list))
				dlist_add(&next->prio_list, &node->prio_list);
		}
		dlist_remove_init(&node->prio_list);
	}
	dlist_remove_init(&node->node_list);
}

void plist_requeue(struct plist_node *node, struct plist_head *head)
{
	struct dlist_head *node_next = &head->node_list;

	if (node == plist_last(head))
		return;

	struct plist_node *iter = plist_next(node);
	if (node->prio != iter->prio)
		return;
	plist_remove(node, head);

	plist_foreach_continue(iter, head)
	{
		if (node->prio != iter->prio)
		{
			node_next = &iter->node_list;
			break;
		}
	}
	dlist_add_tail(&node->node_list, node_next);
}
