#include <kernel/utils/dlist.h>
#include <stddef.h>

void dlist_head_init(struct dlist_head *list)
{
  list->next = list;
  list->prev = list;
}

bool dlist_is_first(const struct dlist_head *list, const struct dlist_head *head)
{
  return list->prev == head;
}

bool dlist_is_last(const struct dlist_head *list, const struct dlist_head *head)
{
  return list->next == head;
}

bool dlist_is_empty(const struct dlist_head *head)
{
  return head->next == head;
}

bool dlist_is_singular(const struct dlist_head *head)
{
  return !dlist_is_empty(head) && (head->next == head->prev);
}

void dlist_add_internal(struct dlist_head *new, struct dlist_head *prev, struct dlist_head *next)
{
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

void dlist_remove_internal(struct dlist_head *prev, struct dlist_head *next)
{
  next->prev = prev;
  prev->next = next;
}

void dlist_add(struct dlist_head *new, struct dlist_head *head)
{
  dlist_add_internal(new, head, head->next);
}

void dlist_add_tail(struct dlist_head *new, struct dlist_head *head)
{
  dlist_add_internal(new, head->prev, head);
}

void dlist_entry_remove(struct dlist_head *entry)
{
  dlist_remove_internal(entry->prev, entry->next);
}

void dlist_remove(struct dlist_head *entry)
{
  dlist_entry_remove(entry);
  entry->next = NULL;
  entry->prev = NULL;
}

void dlist_remove_init(struct dlist_head *entry)
{
  dlist_entry_remove(entry);
  dlist_head_init(entry);
}

void dlist_replace(struct dlist_head *old, struct dlist_head *new)
{
  new->next = old->next;
  new->next->prev = new;
  new->prev = old->prev;
  new->prev->next = new;
}

void dlist_replace_init(struct dlist_head *old, struct dlist_head *new)
{
  dlist_replace(old, new);
  dlist_head_init(old);
}

void dlist_swap(struct dlist_head *entry1, struct dlist_head *entry2)
{
  struct dlist_head *pos = entry2->prev;
  dlist_remove(entry2);
  dlist_replace(entry1, entry2);
  if (pos == entry1)
    pos = entry2;
  dlist_add(entry1, pos);
}

void dlist_move(struct dlist_head *list, struct dlist_head *head)
{
  dlist_entry_remove(list);
  dlist_add(list, head);
}

void dlist_move_tail(struct dlist_head *list, struct dlist_head *head)
{
  dlist_entry_remove(list);
  dlist_add_tail(list, head);
}

void dlist_move_tail_bulk(struct dlist_head *head, struct dlist_head *first, struct dlist_head *last)
{
  first->prev->next = last->next;
  last->next->prev = first->prev;

  head->prev->next = first;
  first->prev = head->prev;

  last->next = head;
  head->prev = last;
}

void dlist_remove_init_careful(struct dlist_head *entry)
{
  dlist_entry_remove(entry);
  entry->prev = entry;
  entry->next = entry;
}

int dlist_empty_careful(const struct dlist_head *head)
{
  struct dlist_head *next = head->next;
  return (next == head) && (next == head->prev);
}

void dlist_rotate_left(struct dlist_head *head)
{
  struct dlist_head *first;
  if (!dlist_is_empty(head))
  {
    first = head->next;
    dlist_move_tail(first, head);
  }
}

void dlist_rotate_front(struct dlist_head *list, struct dlist_head *head)
{
  dlist_move_tail(head, list);
}

void dlist_cut_position_internal(struct dlist_head *list, struct dlist_head *head, struct dlist_head *entry)
{
  struct dlist_head *new_first = entry->next;
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry;
  entry->next = list;
  head->next = new_first;
  new_first->prev = head;
}

void dlist_cut_position(struct dlist_head *list, struct dlist_head *head, struct dlist_head *entry)
{
  if (dlist_is_empty(head))
    return;
  if (dlist_is_singular(head) &&
      (head->next != entry && head != entry))
    return;
  if (entry == head)
    dlist_head_init(list);
  else
    dlist_cut_position_internal(list, head, entry);
}

void dlist_cut_before(struct dlist_head *list, struct dlist_head *head, struct dlist_head *entry)
{
  if (head->next == entry)
  {
    dlist_head_init(list);
    return;
  }
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry->prev;
  list->prev->next = list;
  head->next = entry;
  entry->prev = head;
}

void dlist_splice_internal(const struct dlist_head *list, struct dlist_head *prev, struct dlist_head *next)
{
  struct dlist_head *first = list->next;
  struct dlist_head *last = list->prev;

  first->prev = prev;
  prev->next = first;

  last->next = next;
  next->prev = last;
}

void dlist_splice(const struct dlist_head *list, struct dlist_head *head)
{
  if (!dlist_is_empty(list))
    dlist_splice_internal(list, head, head->next);
}

void dlist_splice_init(struct dlist_head *list, struct dlist_head *head)
{
  if (!dlist_is_empty(list))
  {
    dlist_splice_internal(list, head, head->next);
    dlist_head_init(list);
  }
}

void dlist_splice_tail(struct dlist_head *list, struct dlist_head *head)
{
  if (!dlist_is_empty(list))
    dlist_splice_internal(list, head->prev, head);
}

void dlist_splice_tail_init(struct dlist_head *list, struct dlist_head *head)
{
  if (!dlist_is_empty(list))
  {
    dlist_splice_internal(list, head->prev, head);
    dlist_head_init(list);
  }
}
