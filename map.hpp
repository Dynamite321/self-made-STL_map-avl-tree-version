/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>

//AVL Tree Version

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map
{
	friend class iteraotr;
	friend class const_iterator;
private:
	typedef pair<const Key, T> Value;
	struct node
	{
		Value data;
		node *left;
		node *right;
		node *parent;
		int height;
		bool is_end;
		node(const Value &v, node *l = NULL, node *r = NULL,node *p = NULL, int h = 0, bool b = false) :data(v),left(l), right(r), parent(p), height(h), is_end(b) {}
		~node() {}
	};
	node *root;
	node *header;
	size_t node_count;

	void init()
	{
		void *tmp = operator new(sizeof(node));
		header = reinterpret_cast<node*>(tmp);
		header->parent = root;
		header->left = header;
		header->right = header;
		header->is_end = true;
	}

	void clear(node *n)
	{
		if (n == NULL)
		{
			return;
		}
		clear(n->left);
		clear(n->right);
		delete n;
		n = NULL;
	}

	int height(node *t)
	{
		if (t == NULL)
			return -1;
		else
 			return t->height;
	}

	int max(int x, int y)
	{
		if (x > y)
			return x;
		else
			return y;
	}

	void LL(node* &t)
	{
		node *tmp = t->parent;
		node *t1 = t->left;
		t->left = t1->right;
		if (t1->right != NULL)
			t1->right->parent = t;
		t1->right = t;
		t->parent = t1;
		t->height = max(height(t->left), height(t->right)) + 1;
		t1->height = max(height(t1->left), height(t)) + 1;
		t1->parent = tmp;
		t = t1;
	}

	void RR(node* &t)
	{
		node *tmp = t->parent;
		node *t1 = t->right;
		t->right = t1->left;
		if (t1->left != NULL)
			t1->left->parent = t;
		t1->left = t;
		t->parent = t1;
		t->height = max(height(t->left), height(t->right)) + 1;
		t1->height = max(height(t1->right), height(t)) + 1;
		t1->parent = tmp;
		t = t1;
	}

	void LR(node* &t)
	{
		RR(t->left);
		LL(t);
	}

	void RL(node* &t)
	{
		LL(t->right);
		RR(t);
	}

	void insert(const Value &value, node* &x, node* y, node* &z)
	{
		if (x == NULL)
		{
			x = new node(value, NULL, NULL, y);
			z = x;
			++node_count;
			if (header->left == header && header->right == header)
			{
				header->left = x;
				header->right = x;
			}
			else
			{
				if (Compare()(value.first, header->left->data.first))
					header->left = x;
				else
				{
					if (Compare()(header->right->data.first, value.first))
						header->right = x;
				}
			}
			x->height = max(height(x->left), height(x->right)) + 1;
		}
		else if (Compare()(value.first, x->data.first))
		{
			insert(value, x->left, x, z);
			if (height(x->left) - height(x->right) == 2)
			{
				if (Compare()(value.first, x->left->data.first))
					LL(x);
				else
					LR(x);
			}
			x->height = max(height(x->left), height(x->right)) + 1;
		}
		else if (Compare()(x->data.first, value.first))
		{
			insert(value, x->right, x, z);
			if (height(x->right) - height(x->left) == 2)
			{
				if (Compare()(x->right->data.first, value.first))
					RR(x);
				else
					RL(x);
			}
			x->height = max(height(x->left), height(x->right)) + 1;
		}
		else
		{
			z = x;
		}
	}

	void remove(const Value &value, node* &x, node *y)
	{
		if (x != NULL)
		{
			if (Compare()(value.first, x->data.first))
			{
				remove(value, x->left, x);
				if (height(x->right) - height(x->left) == 2)
				{
					node *r = x->right;
					if (height(r->left) > height(r->right))
						RL(x);
					else
						RR(x);
				}
			}
			else if (Compare()(x->data.first, value.first))
			{
				remove(value, x->right, x);
				if (height(x->left) - height(x->right) == 2)
				{
					node *l = x->left;
					if (height(l->right) > height(l->left))
						LR(x);
					else
						LL(x);
				}
			}
			else
			{
				if (x->left != NULL && x->right != NULL)
				{
					bool isRight;
					node *n = x;
					node *tmp = x->right;
					while (tmp->left != NULL)
					{
						tmp = tmp->left;
					}
					if (tmp->parent == x)
						isRight = true;
					else
						isRight = false;
					node *a1 = tmp->parent;
					node *a2 = tmp->left;
					node *a3 = tmp->right;
					int h = tmp->height;
					tmp->height = x->height;
					tmp->left = x->left;
					if (x->left != NULL)
						x->left->parent = tmp;
					tmp->right = x->right;
					if (x->right != NULL)
						x->right->parent = tmp;
					tmp->parent = y;
					x = tmp;
					if (y == header)
						header->parent = x;
					if (isRight)
						n->parent = x;
					else
						n->parent = a1;
					n->left = a2;
					if (a2 != NULL)
						a2->parent = n;
					n->right = a3;
					if (a3 != NULL)
						a3->parent = n;
					n->height = h;
					if (isRight)
						x->right = n;
					else
						a1->left = n;
					remove(value, x->right, x);
					if (height(x->left) - height(x->right) == 2)
					{
						node *l = x->left;
						if (height(l->right) > height(l->left))
							LR(x);
						else
							LL(x);
					}
					/*node *p = tmp->parent;
					if (tmp != x->right)
					{
						p->left = tmp->right;
						if (tmp->right != NULL)
							tmp->right->parent = p;
					}
					else
					{
						p->right = tmp->right;
						if (tmp->right != NULL)
							tmp->right->parent = p;
					}
					node *tmp2 = p;
					while (tmp2 != x->parent)
					{
						tmp2->height = max(height(tmp2->left), height(tmp2->right)) + 1;
						tmp2 = tmp2->parent;
					}
					tmp->parent = y;
					tmp->left = x->left;
					if (x->left != NULL)
						x->left->parent = tmp;
					tmp->right = x->right;
					if (x->right != NULL)
						x->right->parent = tmp;
					tmp->height = max(height(x->left), height(x->right)) + 1;
					x = tmp;
					if (y == header)
						header->parent = x;
					if (p == oldNode)
						p = x;
					if (p->parent == oldNode)
						p->parent = x;
					while (p != x)
					{
						if (height(p->right) - height(p->left) == 2)
						{
							node *a = p->right;
							if (height(a->left) > height(a->right))
								RL(p);
							else
								RR(p);
						}
						p->height = max(height(p->left), height(p->right)) + 1;
						p = p->parent;
					}
					if (height(x->left) - height(x->right) == 2)
					{
						node *b = x->left;
						if (height(b->right) > height(b->left))
							LR(x);
						else
							LL(x);
					}
					x->height = max(height(x->left), height(x->right)) + 1;*/
					//delete oldNode;
				}
				else
				{
					if (x->left != NULL)
					{
						node *oldNode = x;
						node *tmp = x->left;
						tmp->parent = y;
						x = tmp;
						if (y == header)
							header->parent = x;
						delete oldNode;
					}
					else if (x->right != NULL)
					{
						node *oldNode = x;
						node *tmp = x->right;
						tmp->parent = y;
						x = tmp;
						if (y == header)
							header->parent = x;
						delete oldNode;
					}
					else
					{
						node *oldNode = x;
						x = NULL;
						if (y == header)
							header->parent = x;
						delete oldNode;
					}
				}
			}
			//if (x != NULL)
				//x->height = max(height(x->left), height(x->right)) + 1;
		}
	}

	void copy_tree(node *n, node* &x, node *y)
	{
		if (n == NULL)
		{
			return;
		}
		x = new node(n->data);
		x->parent = y;
		x->height = n->height;
		x->is_end = n->is_end;
		copy_tree(n->left, x->left, x);
		copy_tree(n->right, x->right, x);
	}

public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
		friend class map;
		friend class const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		node *ptr;
		const map *container;
	public:
		iterator(node *p = NULL, const map *c = NULL) :ptr(p), container(c)
		{
			// TODO
		}
		iterator(const iterator &other)
		{
			// TODO
			ptr = other.ptr;
			container = other.container;
		}
		iterator(const const_iterator &other)
		{
			ptr = other.ptr;
			container = other.container;
		}
		~iterator() {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator &operator=(const iterator &rhs)
		{
			if (this == &rhs)
				return *this;
			ptr = rhs.ptr;
			container = rhs.container;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				iterator itr(*this);
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return itr;
			}
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++()
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return *this;
			}
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				iterator itr(*this);
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return itr;
			}
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--()
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return *this;
			}
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const
		{
			return ptr->data;
		}
		bool operator==(const iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		bool operator==(const const_iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}

		value_type* operator->() const noexcept
		{
			return &(ptr->data);
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		friend class map;
		friend class iterator;
	private:
		// data members.
		node *ptr;
		const map *container;
	public:
		const_iterator(node *p = NULL, const map *c = NULL) :ptr(p), container(c)
		{
			// TODO
		}
		const_iterator(const const_iterator &other)
		{
			// TODO
			ptr = other.ptr;
			container = other.container;
		}
		const_iterator(const iterator &other)
		{
			// TODO
			ptr = other.ptr;
			container = other.container;
		}
		~const_iterator() {}
		// And other methods in iterator.
		// And other methods in iterator.
		// And other methods in iterator.
		const_iterator &operator=(const const_iterator &rhs)
		{
			if (this == &rhs)
				return *this;
			ptr = rhs.ptr;
			container = rhs.container;
			return *this;
		}
		const_iterator operator++(int)
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				const_iterator itr(*this);
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return itr;
			}
		}
		const_iterator &operator++()
		{
			if (ptr == container->header)
				throw invalid_iterator();
			else
			{
				if (ptr->right != NULL)
				{
					ptr = ptr->right;
					while (ptr->left != NULL)
						ptr = ptr->left;
				}
				else
				{
					node *tmp = ptr->parent;
					while (ptr == tmp->right)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					if (ptr->right != tmp)
					{
						ptr = tmp;
					}
				}
				return *this;
			}
		}
		const_iterator operator--(int)
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				const_iterator itr(*this);
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return itr;
			}
		}
		const_iterator operator--()
		{
			if (ptr == container->header->left)
				throw invalid_iterator();
			else
			{
				node *tmp;
				if (ptr->is_end)
					ptr = ptr->right;
				else if (ptr->left != NULL)
				{
					tmp = ptr->left;
					while (tmp->right != NULL)
						tmp = tmp->right;
					ptr = tmp;
				}
				else
				{
					tmp = ptr->parent;
					while (ptr == tmp->left)
					{
						ptr = tmp;
						tmp = tmp->parent;
					}
					ptr = tmp;
				}
				return *this;
			}
		}

		const value_type & operator*() const
		{
			return ptr->data;
		}
		bool operator==(const iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		bool operator==(const const_iterator &rhs) const
		{
			return (ptr == rhs.ptr);
		}
		bool operator!=(const iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return (ptr != rhs.ptr);
		}
		value_type* operator->() const noexcept
		{
			return &(ptr->data);
		}
	};
	/**
	 * TODO two constructors
	 */
	map()
	{
		root = NULL;
		init();
		node_count = 0;
	}
	map(const map &other)
	{
	    root = NULL;
		init();
		node_count = other.node_count;
		copy_tree(other.root, root, header);
		if (root != NULL)
		{
			node *tmp = root;
			while (tmp->left != NULL)
			{
				tmp = tmp->left;
			}
			header->left = tmp;
			node *tmp2 = root;
			while (tmp2->right != NULL)
			{
				tmp2 = tmp2->right;
			}
			header->right = tmp2;
		}
		header->parent = root;
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other)
	{
		if (this == &other)
			return *this;
		clear();
		node_count = other.node_count;
		copy_tree(other.root, root, header);
		if (root != NULL)
		{
			node *tmp = root;
			while (tmp->left != NULL)
			{
				tmp = tmp->left;
			}
			header->left = tmp;
			node *tmp2 = root;
			while (tmp2->right != NULL)
			{
				tmp2 = tmp2->right;
			}
			header->right = tmp2;
		}
		header->parent = root;
		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map()
	{
		clear();
		operator delete (header);
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key)
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			throw index_out_of_bound();
		else
			return tmp->data.second;
	}
	const T & at(const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			throw index_out_of_bound();
		else
			return tmp->data.second;
	}
	/**
	 * TODO
	 * access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key)
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
		{
			return((insert(value_type(key, T())).first.ptr->data).second);
		}
		else
			return tmp->data.second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			throw index_out_of_bound();
		else
			return tmp->data.second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin()
	{
		iterator itr(header->left, this);
		return itr;
	}
	const_iterator cbegin() const
	{
		const_iterator itr(header->left, this);
		return itr;
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end()
	{
		iterator itr(header, this);
		return itr;
	}
	const_iterator cend() const
	{
		const_iterator itr(header, this);
		return itr;
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const
	{
		return node_count == 0;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const
	{
		return node_count;
	}
	/**
	 * clears the contents
	 */
	void clear()
	{
		clear(root);
		header->left = header;
		header->right = header;
		header->parent = root = NULL;
		node_count = 0;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value)
	{
		size_t tmp = node_count;
		node *z;
		insert(value, root, header, z);
		iterator itr(z, this);
		if (node_count > tmp)
			return pair<iterator, bool>(itr, true);
		else
			return pair<iterator, bool>(itr, false);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos)
	{
		if (pos.ptr == NULL || pos.ptr == header || pos.container != this)
			throw invalid_iterator();
		else
		{
			bool is_min = false, is_max = false;
			if (pos.ptr == header->left)
				is_min = true;
			if (pos.ptr == header->right)
				is_max = true;
			remove(pos.ptr->data, root, header);
			--node_count;
			if (node_count != 0)
			{
				if (is_min)
				{
					node* tmp = root;
					while (tmp->left != NULL)
						tmp = tmp->left;
					header->left = tmp;
				}
				if (is_max)
				{
					node *tmp2 = root;
					while (tmp2->right != NULL)
						tmp2 = tmp2->right;
					header->right = tmp2;
				}
			}
			else
			{
				root = NULL;
				header->left = header;
				header->right = header;
				header->parent = root;
			}
		}
	}
	/**
	 * Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
			return 0;
		else
			return 1;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key)
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
		{
			iterator itr(header, this);
			return itr;
		}
		else
		{
			iterator itr(tmp, this);
			return itr;
		}
	}
	const_iterator find(const Key &key) const
	{
		node *tmp = root;
		while (tmp != NULL && (Compare()(tmp->data.first, key) || Compare()(key, tmp->data.first)))
		{
			if (Compare()(tmp->data.first, key))
				tmp = tmp->right;
			else
				tmp = tmp->left;
		}
		if (tmp == NULL)
		{
			const_iterator itr(header, this);
			return itr;
		}
		else
		{
			const_iterator itr(tmp, this);
			return itr;
		}
	}

};

}

#endif
