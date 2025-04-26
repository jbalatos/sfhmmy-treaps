#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct itreap_node  itreap_node;
typedef        itreap_node *itreap;

#define itreap_split(t, key, l, r)  itreap_split_(t, key, &(l), &(r), 0)
#define itreap_merge(t, l, r)       itreap_merge_(&(t), l, r)

#define itreap_set(t, pos, val) \
	(void)((t) = itreap_set_(t, pos, val))
#define itreap_erase(t, pos) \
	(void)((t) = itreap_erase_(t, pos))
//      itreap_get(t, pos)
//      itreap_size(t)
//      itreap_sum(t)
#define itreap_add(t, from, to, x) \
	(void)((t) = treap_add_(t, from, to, x))
#define itreap_flip(t, from, to) \
	(void)((t) = treap_flip_(t, from, to))

#ifdef __cplusplus
extern "C" {
#endif

extern itreap  node_create(int32_t value);
extern itreap  itreap_add_(itreap this, size_t from, size_t to, int32_t add);
extern itreap  itreap_flip_(itreap this, size_t from, size_t to);
extern int32_t itreap_get(itreap this, size_t pos);
static treap   itreap_erase_(itreap this, size_t pos);
static void    itreap_push(itreap this);
extern itreap  itreap_set_(itreap this, size_t pos, int32_t val);
extern size_t  itreap_size(itreap this);
extern void    itreap_split_(itreap this, ptrdiff_t key, itreap *l, itreap *r,
		size_t prv);
extern int64_t itreap_sum(itreap this);
extern void    itreap_merge_(itreap *t, itreap l, itreap r);
static void    itreap_update(itreap this);

#ifdef __cplusplus
}
#endif

#ifdef ITREAP_IMPLEMENTATION

#ifndef SWAP
	#define SWAP(a, b) do {              \
		__typeof__(a) __tmp__ = (a); \
		(a) = (b); (b) = __tmp__;    \
	} while (0)
#endif

struct itreap_node {
	int32_t value;
	uint64_t priority;
	itreap_node *left, *right;
	size_t size;
	int64_t sum; /* auxiliary value */
	int32_t add; /* lazy values */
	bool flip;
};

itreap node_create (int32_t value)
{
	itreap_node *ret = malloc(sizeof(*ret));
	ret->value = value;
	ret->priority = ((uint64_t)rand() << 32) | rand();
	ret->left = ret->right = NULL;
	ret->size = 1;
	ret->sum = value;
	ret->add = ret->flip = 0;
	return ret;
}

itreap
itreap_add_ (itreap this, size_t from, size_t to, int32_t add)
{
	itreap l = {}, r = {};
	assert(from <= to && itreap_size(this) > to);

	itreap_split(this, to, this, r);
	itreap_split(this, (ptrdiff_t)from - 1, l, this);
	this->sum += this->size * add;
	this->value += add, this->add += add;
	itreap_merge(this, l, this), itreap_merge(this, this, r);
	return this;
}

itreap
itreap_flip_ (itreap this, size_t from, size_t to)
{
	itreap l = {}, r = {};
	assert(from <= to && itreap_size(this) > to);

	itreap_split(this, to, this, r);
	itreap_split(this, (ptrdiff_t)from - 1, l, this);
	SWAP(this->left, this->right);
	this->flip ^= 1;
	itreap_merge(this, l, this), itreap_merge(this, this, r);
	return this;
}

int32_t itreap_get (itreap this, size_t pos)
{
	int32_t ret;
	itreap l = {}, r = {};
	assert(itreap_size(this) > pos);

	itreap_split(this, pos, this, this->right);
	itreap_split(this, pos-1, l, this);
	ret = this->value;
	itreap_merge(this, l, this), itreap_merge(this, this, r);
	return ret;
}

itreap itreap_set_ (itreap this, size_t pos, int32_t val)
{
	itreap u = node_create(val),
	       l = {}, r = {};
	assert(itreap_size(this) >= pos);

	itreap_split(this, (ptrdiff_t)pos - 1,
		    l, r);
	itreap_merge(this, l, u);
	itreap_merge(this, this, r);
	return this;
}

treap itreap_erase_ (itreap this, size_t pos)
{
	itreap l, r;
	itreap_split(this, pos, this, r);
	itreap_split(this, (ptrdiff_t)pos - 1, l, this);
	itreap_merge(this, l, r);
	return this;
}

void itreap_push (itreap this)
{
	if (this && this->add) {
		if (this->left)
			this->left->sum += this->left->size * this->add,
			this->left->value += this->add,
			this->left->add += this->add;
		if (this->right)
			this->right->sum += this->right->size * this->add,
			this->right->value += this->add,
			this->right->add += this->add;
		this->add = 0;
	}
	if (this && this->flip) {
		SWAP(this->left, this->right);
		if (this->left) this->left->flip = !this->left->flip;
		if (this->right) this->right->flip = !this->right->flip;
		this->flip = false;
	}
}

size_t itreap_size (itreap this)
{ return this ? this->size : 0ul; }

void itreap_split_ (itreap this, ptrdiff_t key, itreap *l, itreap *r, size_t prv)
{
	if (!this) {
		*l = *r = NULL;
		return;
	}
	itreap_push(this);
	int cur_key = prv + itreap_size(this->left);
	if (cur_key <= key)
		itreap_split_(this->right, key, &(this->right), r, cur_key + 1),
			*l = this;
	else
		itreap_split_(this->left, key, l, &(this->left), prv), *r = this;
	itreap_update(this);
}

int64_t itreap_sum (itreap this)
{ return this ? this->sum : 0ll; }

void itreap_merge_ (itreap *t, itreap l, itreap r)
{
	if (!l || !r) {
		*t = l ? l : r;
		return;
	}
	itreap_push(l), itreap_push(r);
	if (l->priority > r->priority)
		itreap_merge_(&(l->right), l->right, r), *t = l;
	else
		itreap_merge_(&(r->left), l, r->left), *t = r;
	itreap_update(*t);
}

void itreap_update (itreap this)
{
	if (!this) return;
	this->size = 1 + itreap_size(this->left) + itreap_size(this->right);
	this->sum = this->value + itreap_sum(this->left) +
		itreap_sum(this->right);
}

#endif //TREAP_IMPLEMENTATION

#ifdef ITREAP_TEST

int main ()
{
	/* Example */
	itreap arr = {};
	itreap_set(arr, 0, 42);
	itreap_set(arr, 0, 17);
	for (size_t i=0; i<itreap_size(arr); ++i)
		printf("%d\n", itreap_get(arr, i));
}

#endif

