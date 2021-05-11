#pragma once

#include <kernel/utils/dlist.h>

struct plist_head
{
	struct dlist_head node_list;
};

struct plist_node
{
	int prio;
	struct dlist_head prio_list;
	struct dlist_head node_list;
};

#define PLIST_HEAD_INIT(head)                      \
	{                                                \
		.node_list = DLIST_HEAD_INIT((head).node_list) \
	}

#define PLIST_HEAD(head) struct plist_head head = PLIST_HEAD_INIT(head)

#define PLIST_NODE_INIT(node, __prio)               \
	{                                                 \
		.prio = (__prio),                               \
		.prio_list = DLIST_HEAD_INIT((node).prio_list), \
		.node_list = DLIST_HEAD_INIT((node).node_list), \
	}

#define plist_foreach(pos, head) dlist_foreach_entry(pos, &(head)->node_list, node_list)
#define plist_foreach_continue(pos, head) dlist_foreach_entry_continue(pos, &(head)->node_list, node_list)
#define plist_foreach_safe(pos, n, head) dlist_foreach_entry_safe(pos, n, &(head)->node_list, node_list)
#define plist_foreach_entry(pos, head, mem) dlist_foreach_entry(pos, &(head)->node_list, mem.node_list)
#define plist_foreach_entry_continue(pos, head, m) dlist_foreach_entry_continue(pos, &(head)->node_list, m.node_list)
#define plist_foreach_entry_safe(pos, n, head, m) dlist_foreach_entry_safe(pos, n, &(head)->node_list, m.node_list)
#define plist_first_entry(head, type, member) dlist_container(plist_first(head), type, member)
#define plist_last_entry(head, type, member) dlist_container(plist_last(head), type, member)
#define plist_next(pos) dlist_entry_next(pos, node_list)
#define plist_prev(pos) dlist_entry_prev(pos, node_list)

void plist_head_init(struct plist_head *head);
void plist_add(struct plist_node *node, struct plist_head *head);
void plist_remove(struct plist_node *node, struct plist_head *head);
void plist_requeue(struct plist_node *node, struct plist_head *head);
struct plist_node *plist_first(const struct plist_head *head);
struct plist_node *plist_last(const struct plist_head *head);
bool plist_is_head_empty(const struct plist_head *head);
bool plist_is_node_empty(const struct plist_node *node);
