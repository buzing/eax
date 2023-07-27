#pragma once

template< class T, class I = int >
class CUtlMemory {
public:
	__forceinline T& operator[ ](I i) {
		return m_pMemory[i];
	}

	static int CalcNewAllocationCount(int count, int size, int requested, int bytes) {
		if (size)
			count = ((1 + ((requested - 1) / size)) * size);

		else {
			if (!count)
				count = (31 + bytes) / bytes;

			while (count < requested)
				count *= 2;
		}

		return count;
	}

	__forceinline void Grow(int count = 1) {
		if (IsExternallyAllocated())
			return;

		int requested = m_nAllocationCount + count;
		int new_alloc_count = CalcNewAllocationCount(m_nAllocationCount, m_nGrowSize, requested, sizeof(T));

		if ((int)(I)new_alloc_count < requested) {

			if ((int)(I)new_alloc_count == 0 && (int)(I)(new_alloc_count - 1) >= requested)
				--new_alloc_count;

			else {
				if ((int)(I)requested != requested)
					return;

				while ((int)(I)new_alloc_count < requested)
					new_alloc_count = (new_alloc_count + requested) / 2;
			}
		}

		m_nAllocationCount = new_alloc_count;

		if (m_pMemory)
			m_pMemory = (T*)g_pMemAlloc->Realloc(m_pMemory, m_nAllocationCount * sizeof(T));

		else
			m_pMemory = (T*)g_pMemAlloc->Alloc(m_nAllocationCount * sizeof(T));
	}

	__forceinline bool IsExternallyAllocated() const {
		return m_nGrowSize < 0;
	}

protected:
	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template < class T >
__forceinline T* Construct(T* pMemory) {
	return ::new(pMemory) T;
}

template <class T>
__forceinline void Destruct(T* pMemory) {
	pMemory->~T();
}

template< class T, class A = CUtlMemory< T > >
class CUtlVector {
	typedef A CAllocator;
public:
	__forceinline T& operator[](int i) {
		return m_Memory[i];
	}

	__forceinline T& Element(int i) {
		return m_Memory[i];
	}

	__forceinline T* Base() {
		return m_Memory.Base();
	}

	__forceinline int Count() const {
		return m_Size;
	}

	__forceinline void RemoveAll() {
		for (int i = m_Size; --i >= 0; )
			Destruct(&Element(i));

		m_Size = 0;
	}

	__forceinline int AddToTail() {
		return InsertBefore(m_Size);
	}

	__forceinline int InsertBefore(int elem) {
		GrowVector();
		ShiftElementsRight(elem);
		Construct(&Element(elem));

		return elem;
	}

protected:
	__forceinline void GrowVector(int num = 1) {
		if (m_Size + num > m_Memory.NumAllocated())
			m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());

		m_Size += num;
		ResetDbgInfo();
	}

	__forceinline void ShiftElementsRight(int elem, int num = 1) {
		int numToMove = m_Size - elem - num;
		if ((numToMove > 0) && (num > 0))
			memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
	}

	CAllocator m_Memory;
	int        m_Size;
	T* m_pElements;

	__forceinline void ResetDbgInfo() {
		m_pElements = Base();
	}
};

inline int UtlMemory_CalcNewAllocationcount(int nAllocationcount, int nGrowSize, int nNewSize, int nBytesItem) {
	if (nGrowSize) {
		nAllocationcount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
	}
	else {
		if (!nAllocationcount) {
			// Compute an allocation which is at least as big as a cache line...
			nAllocationcount = (31 + nBytesItem) / nBytesItem;
		}

		while (nAllocationcount < nNewSize) {
			nAllocationcount *= 2;
		}
	}

	return nAllocationcount;
}

template< class T, class I = int >
class c_utl_memory {
public:
	bool is_valid_index(I i) const {
		long x = i;
		return (x >= 0) && (x < m_alloc_count);
	}

	T& operator[](I i);
	const T& operator[](I i) const;

	T* base() {
		return m_memory;
	}

	int num_allocated() const {
		return m_alloc_count;
	}

	void grow(int num) {
		assert(num > 0);

		auto old_alloc_count = m_alloc_count;
		// Make sure we have at least numallocated + num allocations.
		// Use the grow rules specified for this memory (in m_grow_size)
		int alloc_requested = m_alloc_count + num;

		int new_alloc_count = UtlMemory_CalcNewAllocationcount(m_alloc_count, m_grow_size, alloc_requested, sizeof(T));

		// if m_alloc_requested wraps index type I, recalculate
		if ((int)(I)new_alloc_count < alloc_requested) {
			if ((int)(I)new_alloc_count == 0 && (int)(I)(new_alloc_count - 1) >= alloc_requested) {
				--new_alloc_count; // deal w/ the common case of m_alloc_count == MAX_USHORT + 1
			}
			else {
				if ((int)(I)alloc_requested != alloc_requested) {
					// we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
					assert(0);
					return;
				}
				while ((int)(I)new_alloc_count < alloc_requested) {
					new_alloc_count = (new_alloc_count + alloc_requested) / 2;
				}
			}
		}

		m_alloc_count = new_alloc_count;

		if (m_memory) {
			auto ptr = new unsigned char[m_alloc_count * sizeof(T)];

			memcpy(ptr, m_memory, old_alloc_count * sizeof(T));
			m_memory = (T*)ptr;
		}
		else {
			m_memory = (T*)new unsigned char[m_alloc_count * sizeof(T)];
		}
	}

protected:
	T* m_memory;
	int m_alloc_count;
	int m_grow_size;
};

template< class T, class I >
T& c_utl_memory< T, I >::operator[](I i) {
	assert(is_valid_index(i));
	return m_memory[i];
}

template< class T, class I >
const T& c_utl_memory< T, I >::operator[](I i) const {
	assert(is_valid_index(i));
	return m_memory[i];
}

template< class T >
void destruct(T* memory) {
	memory->~T();
}

template< class T >
T* construct(T* memory) {
	return ::new(memory) T;
}

template< class T >
T* copy_construct(T* memory, T const& src) {
	return ::new(memory) T(src);
}

template< class T, class A = c_utl_memory< T > >
class c_utl_vector {
	typedef A c_allocator;

	typedef T* iterator;
	typedef const T* const_iterator;
public:
	T& operator[](int i);
	const T& operator[](int i) const;

	T& element(int i) {
		return m_memory[i];
	}

	const T& element(int i) const {
		assert(is_valid_index(i));
		return m_memory[i];
	}

	T* base() {
		return m_memory.base();
	}

	int count() const {
		return m_size;
	}

	void remove_all() {
		for (int i = m_size; --i >= 0; )
			destruct(&element(i));

		m_size = 0;
	}

	bool is_valid_index(int i) const {
		return (i >= 0) && (i < m_size);
	}

	void shift_elements_right(int elem, int num = 1) {
		assert(is_valid_index(elem) || (m_size == 0) || (num == 0));
		int num_to_move = m_size - elem - num;
		if ((num_to_move > 0) && (num > 0))
			memmove(&element(elem + num), &element(elem), num_to_move * sizeof(T));
	}

	void shift_elements_left(int elem, int num = 1) {
		assert(is_valid_index(elem) || (m_size == 0) || (num == 0));
		int numToMove = m_size - elem - num;
		if ((numToMove > 0) && (num > 0))
			memmove(&element(elem), &element(elem + num), numToMove * sizeof(T));
	}

	void grow_vector(int num = 1) {
		if (m_size + num > m_memory.num_allocated()) {
			m_memory.grow(m_size + num - m_memory.num_allocated());
		}

		m_size += num;
	}

	int insert_before(int elem) {
		// Can insert at the end
		assert((elem == count()) || is_valid_index(elem));

		grow_vector();
		shift_elements_right(elem);
		construct(&element(elem));
		return elem;
	}

	int add_to_head() {
		return insert_before(0);
	}

	int add_to_tail() {
		return insert_before(m_size);
	}

	int insert_before(int elem, const T& src) {
		// Can't insert something that's in the list... reallocation may hose us
		assert((base() == NULL) || (&src < base()) || (&src >= (base() + count())));

		// Can insert at the end
		assert((elem == count()) || is_valid_index(elem));

		grow_vector();
		shift_elements_right(elem);
		copy_construct(&element(elem), src);
		return elem;
	}

	int add_to_tail(const T& src) {
		// Can't insert something that's in the list... reallocation may hose us
		return insert_before(m_size, src);
	}

	int find(const T& src) const {
		for (int i = 0; i < count(); ++i) {
			if (element(i) == src)
				return i;
		}
		return -1;
	}

	void remove(int elem) {
		destruct(&element(elem));
		shift_elements_left(elem);
		--m_size;
	}

	bool find_and_remove(const T& src) {
		int elem = find(src);
		if (elem != -1) {
			remove(elem);
			return true;
		}
		return false;
	}

	iterator begin() {
		return base();
	}

	const_iterator begin() const {
		return base();
	}

	iterator end() {
		return base() + count();
	}

	const_iterator end() const {
		return base() + count();
	}

protected:
	c_allocator m_memory;
	int m_size;
	T* m_elements;
};

template< typename T, class A >
T& c_utl_vector< T, A >::operator[](int i) {
	assert(i < m_size);
	return m_memory[i];
}

template< typename T, class A >
const T& c_utl_vector< T, A >::operator[](int i) const {
	assert(i < m_size);
	return m_memory[i];
}