#pragma once

#include <stdbool.h>

struct dlist_head
{
  struct dlist_head *next;
  struct dlist_head *prev;
};

#define DLIST_HEAD_INIT(name) \
  {                           \
    &(name), &(name)          \
  }

#define dlist_container(ptr, type, member) (             \
    {                                                    \
      const typeof(((type *)0)->member)                  \
          *__mptr = (ptr);                               \
      (type *)((char *)__mptr - offsetof(type, member)); \
    })

#define dlist_entry(ptr, type, member) dlist_container(ptr, type, member)
#define dlist_entry_first(ptr, type, member) dlist_entry((ptr)->next, type, member)
#define dlist_entry_first_or_null(ptr, type, member) (           \
    {                                                            \
      struct dlist_head *head__ = (ptr);                         \
      struct dlist_head *pos__ = head__->next;                   \
      pos__ != head__ ? dlist_entry(pos__, type, member) : NULL; \
    })
#define dlist_entry_last(ptr, type, member) dlist_entry((ptr)->prev, type, member)
#define dlist_entry_next(pos, member) dlist_entry((pos)->member.next, typeof(*(pos)), member)
#define dlist_entry_prev(pos, member) dlist_entry((pos)->member.prev, typeof(*(pos)), member)
#define dlist_entry_prepare(pos, head, member) ((pos) ?: dlist_entry(head, typeof(*pos), member))
#define dlist_next_reset_safe(pos, n, member) n = dlist_entry_next(pos, member)
#define dlist_entry_is_head(pos, head, member) (&pos->member == (head))

#define dlist_foreach(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)
#define dlist_foreach_continue(pos, head) for (pos = pos->next; pos != (head); pos = pos->next)
#define dlist_foreach_prev(pos, head) for (pos = (head)->prev; pos != (head); pos = pos->prev)
#define dlist_foreach_prev_safe(pos, n, head) for (pos = (head)->prev, n = pos->prev; pos != (head); pos = n, n = pos->prev)
#define dlist_foreach_safe(pos, n, head) for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#define dlist_foreach_entry(pos, head, member) \
  for (pos = dlist_entry_first(head, typeof(*pos), member); !dlist_entry_is_head(pos, head, member); pos = dlist_entry_next(pos, member))
#define dlist_foreach_entry_reverse(pos, head, member) \
  for (pos = dlist_entry_last(head, typeof(*pos), member); !dlist_entry_is_head(pos, head, member); pos = dlist_entry_prev(pos, member))

#define dlist_foreach_entry_continue(pos, head, member) \
  for (pos = dlist_entry_next(pos, member); !dlist_entry_is_head(pos, head, member); pos = dlist_entry_next(pos, member))
#define dlist_foreach_entry_continue_reverse(pos, head, member) \
  for (pos = dlist_entry_prev(pos, member); !dlist_entry_is_head(pos, head, member); pos = dlist_entry_prev(pos, member))

#define dlist_foreach_entry_from(pos, head, member) for (; !dlist_entry_is_head(pos, head, member); pos = dlist_entry_next(pos, member))
#define dlist_foreach_entry_from_reverse(pos, head, member) \
  for (; !dlist_entry_is_head(pos, head, member); pos = dlist_entry_prev(pos, member))

#define dlist_foreach_entry_safe(pos, n, head, member)                                         \
  for (pos = dlist_entry_first(head, typeof(*pos), member), n = dlist_entry_next(pos, member); \
       !dlist_entry_is_head(pos, head, member); pos = n, n = dlist_entry_next(n, member))
#define dlist_foreach_entry_safe_continue(pos, n, head, member)                \
  for (pos = dlist_entry_next(pos, member), n = dlist_entry_next(pos, member); \
       !dlist_entry_is_head(pos, head, member); pos = n, n = dlist_entry_next(n, member))
#define dlist_foreach_entry_safe_from(pos, n, head, member) \
  for (n = dlist_entry_next(pos, member); !dlist_entry_is_head(pos, head, member); pos = n, n = dlist_entry_next(n, member))
#define dlist_foreach_entry_safe_reverse(pos, n, head, member)                                \
  for (pos = dlist_entry_last(head, typeof(*pos), member), n = dlist_entry_prev(pos, member); \
       !dlist_entry_is_head(pos, head, member); pos = n, n = dlist_entry_prev(n, member))

void dlist_add(struct dlist_head *new, struct dlist_head *head);
void dlist_add_tail(struct dlist_head *new, struct dlist_head *head);
void dlist_remove(struct dlist_head *entry);
void dlist_replace(struct dlist_head *old, struct dlist_head *new);
void dlist_swap(struct dlist_head *entry1, struct dlist_head *entry2);
void dlist_move(struct dlist_head *list, struct dlist_head *head);
void dlist_cut_position(struct dlist_head *list, struct dlist_head *head, struct dlist_head *entry);
void dlist_cut_before(struct dlist_head *list, struct dlist_head *head, struct dlist_head *entry);
void dlist_splice(const struct dlist_head *list, struct dlist_head *head);
void dlist_splice_tail(struct dlist_head *list, struct dlist_head *head);
bool dlist_is_empty(const struct dlist_head *head);
bool dlist_is_first(const struct dlist_head *list, const struct dlist_head *head);
bool dlist_is_last(const struct dlist_head *list, const struct dlist_head *head);
bool dlist_is_singular(const struct dlist_head *head);
