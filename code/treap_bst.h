#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct treap_node  treap_node;
typedef        treap_node *treap;

#define treap_split(t, key, l, r) treap_split_(t, key, &(l), &(r))
#define treap_merge(t, l, r)      treap_merge_(&(t), l, r)

#define treap_insert(t, x) (void)((t) = treap_insert_node(t, node_create(x)))
#define treap_erase(t, x)  (void)((t) = treap_erase_(t, x))
#define treap_count(t, x)  (treap_find(t, x) != NULL)
//      treap_find(t, x)
//      treap_size(t)
//      treap_sum(t)





#ifdef __cplusplus
extern "C" {
#endif

extern treap   node_create(int64_t key);
extern treap   treap_erase_ (treap this, int64_t key);
extern treap   treap_find (treap this, int64_t key);
extern treap   treap_insert_node (treap this, treap /*single node*/ u);
extern size_t  treap_size(treap this);
extern void    treap_split_(treap this, int64_t key, treap *l, treap *r);
extern int64_t treap_sum(treap this);
extern void    treap_merge_(treap *t, treap l, treap r);
static void    treap_update(treap this);

#ifdef __cplusplus
}
#endif

#ifdef TREAP_IMPLEMENTATION

struct treap_node {
	int64_t key;
	uint64_t priority;
	treap_node *left, *right;
	size_t size;
	int64_t sum;
};

treap node_create (int64_t key)
{
	treap_node *ret = malloc(sizeof(*ret));
	ret->key = key;
	ret->priority = ((uint64_t)rand() << 32)
	    | rand();
	ret->left = ret->right = NULL;
	ret->size = 1;
	ret->sum = key;
	return ret;
}

treap treap_erase_ (treap this, int64_t key)
{
	treap tmp;

	if (!this) return NULL;
	if (key == this->key) {
		treap_merge(tmp, this->left, this->right);
		free(this);
		return tmp;
	}
	if (key < this->key)
		this->left = treap_erase_(this->left, key);
	else
		this->right = treap_erase_(this->right, key);
	treap_update(this);
	return this;
}

treap treap_find (treap this, int64_t key)
{
	if (!this) return NULL;
	else if (key == this->key)
		return this;
	else if (key < this->key)
		return treap_find(this->left, key);
	else
		return treap_find(this->right, key);
}

treap treap_insert_node (treap this, treap u)
{
	if (!this) return u;
	if (u->priority > this->priority) {
		treap_split(this, u->key, u->left, u->right);
		return u;
	}
	if (u->key < this->key)
		this->left = treap_insert_node(this->left, u);
	else if (u->key > this->key)
		this->right = treap_insert_node(this->right, u);
	treap_update(this);
	return this;
}

size_t treap_size (treap this)
{ return this ? this->size : 0ul; }

void treap_split_ (treap this, int64_t key, treap *l, treap *r)
{
	if (!this) *l = *r = NULL;
	else if (this->key <= key)
		treap_split_(this->right, key, &(this->right), r), *l = this;
	else
		treap_split_(this->left, key, l, &(this->left)), *r = this;
	treap_update(this);
}

int64_t treap_sum (treap this)
{ return this ? this->sum : 0ll; }

void treap_merge_ (treap *t, treap l, treap r)
{
	if (!l || !r)
		*t = l ? l : r;
	else if (l->priority > r->priority)
		treap_merge_(&(l->right), l->right, r), *t = l;
	else
		treap_merge_(&(r->left), l, r->left), *t = r;
	treap_update(*t);
}

void treap_update (treap this)
{
	if (!this) return;
	this->size = 1 + treap_size(this->left)
		+ treap_size(this->right);
	this->sum = this->key + treap_sum(this->left)
		+ treap_sum(this->right);
}

#endif //TREAP_IMPLEMENTATION
