#include <stdlib.h>
#include <string.h>
#include "ringbuffer.h"

struct ringbuffer
{
    uint32_t tail;
    uint32_t head;
    uint32_t size;

    uint8_t buffer[];
};

rb_t rb_new(uint8_t *buff, uint32_t length)
{
    if (length < sizeof(struct ringbuffer) + 1)
        return NULL;

    rb_t rb = (rb_t)buff;
    rb->tail = 0;
    rb->head = 0;
    rb->size = length - sizeof(struct ringbuffer);

    return rb;
}

static inline uint16_t next_head(rb_t rb)
{
    return rb->head + 1 < rb->size ? rb->head + 1 : 0;
}

static inline uint16_t next_tail(rb_t rb)
{
    return rb->tail + 1 < rb->size ? rb->tail + 1 : 0;
}

bool rb_empty(rb_t rb)
{
    return rb->head == rb->tail;
}

bool rb_full(rb_t rb)
{
    return next_head(rb) == rb->tail;
}

bool rb_put(rb_t rb, uint8_t data)
{
    if (rb_full(rb))
        return false;

    rb->buffer[rb->head] = data;
    rb->head = next_head(rb);

    return true;
}

bool rb_get(rb_t rb, uint8_t *data)
{
    if (rb_empty(rb))
        return false;

    *data = rb->buffer[rb->tail];
    rb->tail = next_tail(rb);

    return true;
}

bool rb_puts(rb_t rb, const uint8_t *data, uint32_t size)
{
    while (size--)
    {
        if (!rb_put(rb, *data++))
            return false;
    }
    return true;
}

uint32_t rb_gets(rb_t rb, uint8_t *data, uint32_t size)
{
    uint32_t count = 0;
    while (size--)
    {
        if (!rb_get(rb, data++))
            break;
        count++;
    }
    return count;
}
