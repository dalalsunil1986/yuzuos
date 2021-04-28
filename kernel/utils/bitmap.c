#include <kernel/utils/bitmap.h>

void bitmap_set(uint32_t *bitmap, uint32_t bit)
{
  bitmap[BITMAP_INDEX(bit)] |= BITMAP_OFFSET(bit);
}

void bitmap_unset(uint32_t *bitmap, uint32_t bit)
{
  bitmap[BITMAP_INDEX(bit)] &= ~BITMAP_OFFSET(bit);
}

bool bitmap_test(uint32_t *bitmap, uint32_t bit)
{
  return bitmap[BITMAP_INDEX(bit)] & BITMAP_OFFSET(bit);
}

uint32_t bitmap_get_first(uint32_t *bitmap, uint32_t frames)
{
  for (uint32_t i = 0; i < BITMAP_INDEX(frames); i++)
    if (bitmap[i] != BITMAP_NULL)
      for (int j = 0; j < 32; j++)
      {
        int bit = 1 << j;
        if (!(bitmap[i] & bit))
          return i * 32 + j;
      }

  return BITMAP_BLANK;
}

int bitmap_get_n_first(uint32_t *bitmap, uint32_t frames, uint32_t len)
{
  if (len == 0)
    return BITMAP_BLANK;

  if (len == 1)
    return bitmap_get_first(bitmap, frames);

  for (uint32_t i = 0; i < BITMAP_INDEX(frames); i++)
    if (bitmap[i] != BITMAP_NULL)
      for (int j = 0; j < 32; j++)
      {
        int bit = 1 << j;
        if (!(bitmap[i] & bit))
        {
          uint32_t start = i * 32 + j;
          uint32_t free = 0;
          for (uint32_t count = 0; count <= len; count++)
          {
            if (!bitmap_test(bitmap, start + count))
              free++;
            if (free == len)
              return i * 32 + j;
          }
        }
      }

  return BITMAP_BLANK;
}
