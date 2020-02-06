#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* Free queue structure */
    if (!q)
        return;

    /* Free head element until there's no element left,
     * since q_remove_head() return false when q is NULL or empty */
    while (q_remove_head(q, NULL, 0))
        ;

    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh; /* Add a new head */
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    newh->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!newh->value) {
        free(newh); /* Free the allocated space */
        return false;
    }
    strncpy(newh->value, s, strlen(s) + 1);

    newh->next = q->head;
    q->head = newh;
    if (!q_size(q)) /* The queue is empty */
        q->tail = newh;

    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newt; /* Add a new tail */
    newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;

    newt->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!newt->value) {
        free(newt); /* Free the allocated space */
        return false;
    }
    strncpy(newt->value, s, strlen(s) + 1);

    newt->next = NULL;
    if (!q_size(q))     /* The queue is empty */
        q->head = newt; /* This handles inserting to tail when queue is empty */
    else
        q->tail->next = newt;
    q->tail = newt;

    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q_size(q))
        return false;

    list_ele_t *newh;
    newh = q->head->next; /* Points to new head element */
    if (sp) {
        strncpy(sp, q->head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    free(q->head->value);
    free(q->head);
    q->head = newh;

    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q || !q->size)
        return 0;

    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q_size(q) || q_size(q) == 1)
        return;

    list_ele_t *pre = q->head;
    list_ele_t *cur = pre->next;
    list_ele_t *pos = cur->next;
    while (1) {
        cur->next = pre;
        pre = cur;
        if (!pos)
            break;
        cur = pos;
        pos = pos->next;
    }
    q->tail = q->head;
    q->tail->next = NULL;
    q->head = cur;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q_size(q) || q_size(q) == 1)
        return;

    merge_sort(&q->head);
}

/*
 * Implementation of merge sort.
 * Split the queue into two halves then recursively do
 * merge_sort() for each half
 * Then merge the splited two queue by ascending order
 * q_head is the reference of the head element of the queue
 * No effect if the queue is empty or there's only one element
 */
void merge_sort(list_ele_t **q_head)
{
    /* q_head is the pointer point to the address of the
    head element pointer, dereference to get pointer itself */
    list_ele_t *head = *q_head;

    if (!head ||
        !head->next) /* Return if q_head is NULL or there's only one element */
        return;

    /* Split the queue into two queue */
    list_ele_t *front = NULL;
    list_ele_t *back = NULL;
    split_queue(head, &front, &back);

    /* Sort each queue */
    merge_sort(&front);
    merge_sort(&back);

    *q_head = merge(front, back);
}

/*
 * Split current queue into two halves,
 * use *front and *back to point to each half.
 * Pass the reference of the two pointers as parameters.
 */
void split_queue(list_ele_t *q_head, list_ele_t **front, list_ele_t **back)
{
    list_ele_t *fast;
    list_ele_t *slow;
    slow = q_head;
    fast = q_head->next;

    /* fast iterate two time faster than slow.
     So that when fast reach the end of queue,
     slow is at middle of the queue. */
    while (fast) {
        fast = fast->next;
        if (fast) {
            fast = fast->next;
            slow = slow->next;
        }
    }

    *front = q_head;
    *back = slow->next;
    slow->next = NULL;
}

/*
 * Merge the given two sorted queue
 * Return the merged queue
 */
list_ele_t *merge(list_ele_t *a, list_ele_t *b)
{
    list_ele_t *result, *current;

    if (!a)
        return b;
    else if (!b)
        return a;

    /* Initialize the head of result */
    if (strcmp(a->value, b->value) < 0) {
        result = a;
        a = a->next;
    } else {
        result = b;
        b = b->next;
    }
    current = result;

    /* Iterative version of merge */
    while (1) {
        if (!a) {
            current->next = b;
            break;
        }
        if (!b) {
            current->next = a;
            break;
        }

        if (strcmp(a->value, b->value) < 0) {
            current->next = a;
            a = a->next;
        } else {
            current->next = b;
            b = b->next;
        }

        current = current->next;
    }

    return result;

    /* Recursive version of merge, this might cause TLE */
    /*
     * if (strcmp(a->value, b->value) < 0) {
     *     result = a;
     *     result->next = merge(a->next, b);
     * } else {
     *     result = b;
     *     result->next = merge(a, b->next);
     * }
     *
     * return result;
    list_ele_t *result_head = result;
     */
}
