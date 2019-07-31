/*----------------------------------------------------------------------------------*\
 |																					|
 | ordered_slot_map.hpp 															|
 |																					|
 | Author: (C) Copyright Richard Cookman 2019										|
 |																					|
 | Permission is hereby granted, free of charge, to any person obtaining a copy		|
 | of this software and associated documentation files (the "Software"), to deal	|
 | in the Software without restriction, including without limitation the rights		|
 | to use, copy, modify, merge, publish, distribute, sublicense, and/or sell		|
 | copies of the Software, and to permit persons to whom the Software is			|
 | furnished to do so, subject to the following conditions:							|
 |																					|
 | The above copyright notice and this permission notice shall be included in all	|
 | copies or substantial portions of the Software.									|
 |																					|
 | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR		|
 | IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			|
 | FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE		|
 | AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER			|
 | LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	|
 | OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE	|
 | SOFTWARE.																		|
 |																					|
\*----------------------------------------------------------------------------------*/
#pragma once

#include <limits>
#include <vector>

#include "empty_mutex.hpp"
#include "generation_data.hpp"

namespace std {

template<typename T, typename Mut, typename Alloc>
struct ordered_slot_map;

namespace slot_internal {

template<typename U>
inline ptrdiff_t dist(U* first, U* last) {
	return last - first;
}
inline int midpoint(unsigned imin, unsigned imax) {
	return (imin + imax) >> 1;
}
//basic binary search
template<typename Itr, typename T, typename Less>
bool binary_search(Itr beg, Itr end, const T& item,
				   Less comp, Itr& out) {
	//binary search return the insertion point, in both the found and not found case
	int sze = std::distance(beg, end);
	if(sze == 0) {
		out = end;
		return false;
	}

	int imin = 0;
	int imax = sze - 1;
	int imid = 0;

	// continue searching while [imin,imax] is not empty
	while(imin <= imax) {
		// calculate the midpoint for roughly equal partition
		imid = midpoint(imin, imax);
		// determine which subarray to search
		if (comp(*(beg + imid), item))
			// change min index to search upper subarray
			imin = imid + 1;
		else if(comp(item, *(beg + imid)))
			// change max index to search lower subarray
			imax = imid - 1;
		else {
			out = (beg + imid);
			return true;
		}
	}
	// item was not found return the insertion point
	out = (beg + imin);
	return false;
}

template<typename T>
struct ordered_slot {
	size_t backidx;
	T obj;

	inline operator T&() {
		return obj;
	}
	inline operator const T&() const {
		return obj;
	}
};

}

template<typename T, typename Mut, typename Alloc>
struct ordered_slot_map_iterator;
template<typename T, typename Mut, typename Alloc>
struct ordered_slot_map_const_iterator;
template<typename T, typename Mut, typename Alloc>
struct ordered_slot_map_reverse_iterator;
template<typename T, typename Mut, typename Alloc>
struct ordered_slot_map_const_reverse_iterator;

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc>;

	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	ordered_slot_map_iterator(const typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_iterator() = default;
	inline T& operator*() {
		return itr->obj;
	}
	inline T* operator->() {
		return &itr->obj;
	}
	inline ordered_slot_map_iterator& operator++() {
		++itr;
		return *this;
	}
	inline ordered_slot_map_iterator operator++(int) {
		ordered_slot_map_iterator it(*this);
		++*this;
		return it;
	}
	inline ordered_slot_map_iterator& operator--() {
		--itr;
		return *this;
	}
	inline ordered_slot_map_iterator operator--(int) {
		ordered_slot_map_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const ordered_slot_map_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const ordered_slot_map_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const ordered_slot_map_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const ordered_slot_map_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const ordered_slot_map_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const ordered_slot_map_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator ordered_slot_map_const_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_const_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_const_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc>;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	ordered_slot_map_const_iterator(const typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_const_iterator() = default;
	inline const T& operator*() {
		return itr->obj;
	}
	inline const T* operator->() {
		return &itr->obj;
	}
	inline ordered_slot_map_const_iterator& operator++() {
		++itr;
		return *this;
	}
	inline ordered_slot_map_const_iterator operator++(int) {
		ordered_slot_map_const_iterator it(*this);
		++*this;
		return it;
	}
	inline ordered_slot_map_const_iterator& operator--() {
		--itr;
		return *this;
	}
	inline ordered_slot_map_const_iterator operator--(int) {
		ordered_slot_map_const_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const ordered_slot_map_const_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const ordered_slot_map_const_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const ordered_slot_map_const_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const ordered_slot_map_const_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const ordered_slot_map_const_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const ordered_slot_map_const_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator ordered_slot_map_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_reverse_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc>;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	ordered_slot_map_reverse_iterator(const typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_reverse_iterator() = default;
	inline T& operator*() {
		return itr->obj;
	}
	inline T* operator->() {
		return &itr->obj;
	}
	inline ordered_slot_map_reverse_iterator& operator++() {
		++itr;
		return *this;
	}
	inline ordered_slot_map_reverse_iterator operator++(int) {
		ordered_slot_map_reverse_iterator it(*this);
		++*this;
		return it;
	}
	inline ordered_slot_map_reverse_iterator& operator--() {
		--itr;
		return *this;
	}
	inline ordered_slot_map_reverse_iterator operator--(int) {
		ordered_slot_map_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const ordered_slot_map_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const ordered_slot_map_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const ordered_slot_map_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const ordered_slot_map_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const ordered_slot_map_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const ordered_slot_map_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator ordered_slot_map_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_const_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_const_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_const_reverse_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc>;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc>;

	ordered_slot_map_const_reverse_iterator(const typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_const_reverse_iterator() = default;
	inline const T& operator*() {
		return itr->obj;
	}
	inline const T* operator->() {
		return &itr->obj;
	}
	inline ordered_slot_map_const_reverse_iterator& operator++() {
		++itr;
		return *this;
	}
	inline ordered_slot_map_const_reverse_iterator operator++(int) {
		ordered_slot_map_const_reverse_iterator it(*this);
		++*this;
		return it;
	}
	inline ordered_slot_map_const_reverse_iterator& operator--() {
		--itr;
		return *this;
	}
	inline ordered_slot_map_const_reverse_iterator operator--(int) {
		ordered_slot_map_const_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator ordered_slot_map_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_const_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_const_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return ordered_slot_map_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T, typename Mut, typename Alloc>
struct ordered_slot_map_weak_handle;
template<typename T, typename Mut, typename Alloc>
struct ordered_slot_map_handle;

namespace slot_internal {

template<typename T, typename Mut, typename Alloc>
struct internal_ordered_slot_map_handle {
	ordered_slot_map<T, Mut, Alloc>* map = 0;
	size_t idx = 0;
	size_t gen = 0;

	inline void clear() {
		map = 0;
		idx = 0;
		gen = 0;
	}
};

}

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_handle : slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc> {
	ordered_slot_map_handle() = default;

	friend struct ordered_slot_map<T, Mut, Alloc>;

	ordered_slot_map_handle(const ordered_slot_map_handle& rhs) {
		const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(tmp), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	ordered_slot_map_handle(ordered_slot_map_handle&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	ordered_slot_map_handle(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(rhs), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	ordered_slot_map_handle(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	ordered_slot_map_handle& operator=(const ordered_slot_map_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_handle();

		const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(tmp), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	ordered_slot_map_handle& operator=(ordered_slot_map_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	ordered_slot_map_handle& operator=(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_handle();

		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(rhs), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	ordered_slot_map_handle& operator=(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	~ordered_slot_map_handle() {
		if(this->map)
			this->map->decrement_handle(*this, false);
		this->clear();
	}

	inline T& operator*() {
		return *this->map->get_object(*this);
	}
	inline T* operator->() {
		return this->map->get_object(*this);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*this->map->get_object(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(this->map->get_object(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(*this)));
	}

	inline operator T*() {
		return this->map->get_object(*this);
	}
	inline operator const T*() const {
		return this->map->get_object(*this);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_weak_handle : slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc> {
	ordered_slot_map_weak_handle() = default;

	friend struct ordered_slot_map<T, Mut, Alloc>;

	ordered_slot_map_weak_handle(const ordered_slot_map_weak_handle& rhs) {
		const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(tmp), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	ordered_slot_map_weak_handle(ordered_slot_map_weak_handle&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	ordered_slot_map_weak_handle(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(rhs), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	ordered_slot_map_weak_handle(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	ordered_slot_map_weak_handle& operator=(const ordered_slot_map_weak_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_weak_handle();

		const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(tmp), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	ordered_slot_map_weak_handle& operator=(ordered_slot_map_weak_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_weak_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	ordered_slot_map_weak_handle& operator=(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_weak_handle();

		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(rhs), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	ordered_slot_map_weak_handle& operator=(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_weak_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	~ordered_slot_map_weak_handle() {
		if(this->map)
			this->map->decrement_handle(*this, true);
		this->clear();
	}

	inline T& operator*() {
		return *this->map->get_object(*this);
	}
	inline T* operator->() {
		return this->map->get_object(*this);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*this->map->get_object(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(this->map->get_object(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(*this)));
	}

	inline operator T*() {
		return this->map->get_object(*this);
	}
	inline operator const T*() const {
		return this->map->get_object(*this);
	}
};


template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map {
private:
	struct slot_index {
		slot_internal::generation_data<uint32_t> gens;
		union slot_data {
			size_t next;									//used when object doesn't exist to reference the next object to allocate
			size_t idx;									//index into items
		} unn;
	};
	size_t count = 0;
	Mut mut;
	slot_index* firstslot = 0;
	slot_index* lastslot = 0;
	std::vector<slot_internal::ordered_slot<T>, Alloc> items;
	std::vector<slot_index, Alloc> indexes;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	friend struct ordered_slot_map_handle<T, Mut, Alloc>;
	friend struct ordered_slot_map_weak_handle<T, Mut, Alloc>;

	void extend(size_t extnd) {
		if(extnd == 0)
			return;

		size_t csze = indexes.size();
		items.reserve(items.size() + extnd);
		indexes.resize(csze + extnd);

		size_t nxt = 0;
		if(firstslot)
			nxt = std::distance(&indexes[0], firstslot);

		//append all of the new indexes onto the front of the slot list
		memset((void*)&indexes[csze], 0, sizeof(slot_internal::slot<T>) * extnd);
		for(size_t i = csze; i < csze + extnd; ++i)
			if(i == csze + extnd - 1)
				indexes[i].unn.next = nxt;
			else
				indexes[i].unn.next = i + 1;

		firstslot = &indexes[csze];
		if(nxt == 0)
			lastslot = &indexes[csze + extnd - 1];
	}

	void lock() {
		mut.lock();
	}
	void unlock() {
		mut.unlock();
	}
public:
	ordered_slot_map(size_t slots = 50) {
		extend(slots);
	}
	ordered_slot_map(const ordered_slot_map& rhs) = delete;
	ordered_slot_map(ordered_slot_map&& rhs) = delete;

	ordered_slot_map& operator=(const ordered_slot_map& rhs) = delete;
	ordered_slot_map& operator=(ordered_slot_map&& rhs) = delete;

	//same as normal vector
	typedef T value_type;
	typedef Alloc allocator_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef ordered_slot_map_iterator<T, Mut, Alloc> iterator;
	typedef ordered_slot_map_const_iterator<T, Mut, Alloc> const_iterator;
	typedef ordered_slot_map_reverse_iterator<T, Mut, Alloc> reverse_iterator;
	typedef ordered_slot_map_const_reverse_iterator<T, Mut, Alloc> const_reverse_iterator;
	typedef ordered_slot_map_handle<T, Mut, Alloc> handle;
	typedef ordered_slot_map_weak_handle<T, Mut, Alloc> weak_handle;

private:
	void destruct_object(slot_index* obj) {
		//remove object
		obj->gens.set_invalid();

		//remove this object
		items.erase(items.begin() + obj->unn.idx);
		//change all of the indexes to the displaced objects
		for(size_t idx = obj->unn.idx; idx < items.size(); ++idx)
			indexes[items[idx].backidx].unn.idx = idx;

		obj->unn.idx = 0;

		//add to the start of the free list
		if(firstslot == 0) {
			obj->unn.next = 0;
			firstslot = obj;
			lastslot = obj;
		} else {
			obj->unn.next = std::distance(&indexes[0], firstslot);
			firstslot = obj;
		}
		--count;
	}
	bool increment_handle(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		lock();
		slot_index* obj = get_object_internal(hdl, weak);
		if(obj) {
			slot_internal::generation_data<uint32_t>::counts& tmp = obj->gens.get_generation_count(hdl.gen);
			if(weak)
				++tmp.weakcount;
			else
				++tmp.strongcount;
			unlock();
			return true;
		}
		unlock();
		return false;
	}
	void decrement_handle(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		lock();
		slot_index* obj = get_object_internal(hdl, weak);
		if(obj) {
			slot_internal::generation_data<uint32_t>::counts& tmp = obj->gens.get_generation_count(hdl.gen);
			if(weak)
				--tmp.weakcount;
			else {
				--tmp.strongcount;
				if(tmp.strongcount == 0) {
					destruct_object(obj);
					hdl.clear();
				}
			}
		}
		unlock();
	}
public:

	// iterators:
	inline iterator begin() noexcept {
		return iterator(items.begin());
	}
	inline const_iterator begin() const noexcept {
		return const_iterator(items.begin());
	}
	inline iterator end() noexcept {
		return iterator(items.end());
	}
	inline const_iterator end() const noexcept {
		return const_iterator(items.end());
	}

	inline reverse_iterator rbegin() noexcept {
		return reverse_iterator(items.rbegin());
	}
	inline const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(items.rbegin());
	}
	inline reverse_iterator rend() noexcept {
		return reverse_iterator(items.rend());
	}
	inline const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(items.rend());
	}

	inline const_iterator cbegin() const noexcept {
		return const_iterator(items.cbegin());
	}
	inline const_iterator cend() const noexcept {
		return const_iterator(items.cend());
	}
	inline const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(items.crbegin());
	}
	inline const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(items.crend());
	}

	// capacity:
	inline size_type size() const noexcept {
		lock();
		size_type rtn = count;
		unlock();
		return rtn;
	}
	inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	void resize(size_type sz) {
		lock();
		if(sz < indexes.size()) {
			unlock();
			return;
		}
		extend(sz - indexes.size());
		unlock();
	}
	inline size_type capacity() const noexcept {
		//returns the smaller of the two
		lock();
		size_type rtn = items.capacity();
		unlock();
		return rtn;
	}
	void reserve(size_type n) {
		lock();
		items.reserve(n);
		indexes.reserve(n);
		unlock();
	}
	inline bool empty() const noexcept {
		lock();
		bool rtn = size() == 0;
		unlock();
		return rtn;
	}
	inline void shrink_to_fit() {
		lock();
		items.shrink_to_fit();
		indexes.shrink_to_fit();
		unlock();
	}

private:
	void update_object_indexes(size_t pos) {
		for(size_t i = pos + 1; i < items.size(); ++i)
			indexes[items[i].backidx].unn.idx = i;
	}
	size_t get_insert_pos(const T& val) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
			[](const T& lhs, const T& rhs){
				return lhs < rhs;
			}, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = val;
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	template<typename Less>
	size_t get_insert_pos(const T& val, Less comp) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
									 comp, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = val;
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	size_t get_insert_pos(T&& val) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
			[](const T& lhs, const T& rhs){
				return lhs < rhs;
			}, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = std::move(val);
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	template<typename Less>
	size_t get_insert_pos(T&& val, Less comp) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
									 comp, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = std::move(val);
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	size_t get_next_free(size_t nidx) {
		if(count == indexes.size())
			//double the size
			extend(indexes.size());

		size_t pos = std::distance(&indexes[0], firstslot);

		slot_index* nxt = &indexes[firstslot->unn.next];
		if(firstslot == lastslot)
			nxt = 0;

		if(nxt == 0) {
			firstslot = 0;
			lastslot = 0;
		} else
			firstslot = nxt;

		++count;
		indexes[pos].unn.idx = nidx;
		return pos;
	}
public:
	ordered_slot_map_handle<T, Mut, Alloc> insert(const T& val) {
		lock();
		size_t idx = get_insert_pos(val);
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		unlock();
		return rtn;
	}
	template<typename Less>
	ordered_slot_map_handle<T, Mut, Alloc> insert(const T& val, Less comp) {
		lock();
		size_t idx = get_insert_pos(val, comp);
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		unlock();
		return rtn;
	}
	ordered_slot_map_handle<T, Mut, Alloc> insert(T&& val) {
		lock();
		size_t idx = get_insert_pos(std::move(val));
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		unlock();
		return rtn;
	}
	template<typename Less>
	ordered_slot_map_handle<T, Mut, Alloc> insert(T&& val, Less comp) {
		lock();
		size_t idx = get_insert_pos(std::move(val), comp);
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		unlock();
		return rtn;
	}
	template<typename Itr>
	std::vector<ordered_slot_map_handle<T, Mut, Alloc>> insert(Itr begin, Itr end) {
		lock();
		std::vector<ordered_slot_map_handle<T, Mut, Alloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		unlock();
		return rtn;
	}
	template<typename Itr, typename Less>
	std::vector<ordered_slot_map_handle<T, Mut, Alloc>> insert(Itr begin, Itr end, Less comp) {
		lock();
		std::vector<ordered_slot_map_handle<T, Mut, Alloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin, comp));
		unlock();
		return rtn;
	}

private:
	slot_index* get_object_internal(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		if(hdl.map == 0)
			return 0;
		slot_index& rf = indexes[hdl.idx];
		//test that the generation matches
		if(!rf.gens.is_valid() || !rf.gens.match_generation(hdl.gen, weak)) {
			rf.gens.decrement_generation(hdl.gen, weak);
			hdl.clear();
			return 0;
		}
		return &rf;
	}

	inline bool is_valid(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		return get_object_internal(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(hdl), weak) != 0;
	}
	T* get_object(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		slot_index* obj = get_object_internal(hdl, weak);
		if(obj)
			return &items[obj->unn.idx].obj;
		return 0;
	}
	const T* get_object(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		slot_index* obj = get_object_internal(const_cast<slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>&>(hdl), weak);
		if(obj)
			return &items[obj->unn.idx].obj;
		return 0;
	}

	void erase(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		slot_index* obj = get_object_internal(hdl, weak);
		if(obj)
			destruct_object(obj);
	}
public:

	inline bool is_valid(const ordered_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		bool rtn = is_valid(hdl, false);
		unlock();
		return rtn;
	}
	inline bool is_valid(const ordered_slot_map_weak_handle<T, Mut, Alloc>& hdl) {
		lock();
		bool rtn = is_valid(hdl, true);
		unlock();
		return rtn;
	}
	inline T* get_object(ordered_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline T* get_object(ordered_slot_map_weak_handle<T, Mut, Alloc>& hdl) {
		lock();
		T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}
	inline const T* get_object(const ordered_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		const T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline const T* get_object(const ordered_slot_map_weak_handle<T, Mut, Alloc>& hdl) {
		lock();
		const T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}

	inline void erase(ordered_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		erase(hdl, false);
		unlock();
	}
	inline void erase(ordered_slot_map_weak_handle<T, Mut, Alloc>& hdl) {
		lock();
		erase(hdl, true);
		unlock();
	}

	void clear() noexcept {
		lock();
		//just clear the data, erase everything
		size_t i = 0;
		for(auto it = indexes.begin(); it != indexes.end(); ++it, ++i) {
			if(it->gens.is_valid()) {
				ordered_slot_map_weak_handle<T, Mut, Alloc> hdl;
				hdl.map = this;
				hdl.idx = std::distance(indexes.begin(), it);
				hdl.gen = it->gens.increment_generation(true);

				erase(hdl);
			}
			if(i == indexes.size())
				indexes[i].unn.next = 0;
			else
				indexes[i].unn.next = i + 1;
		}

		firstslot = &indexes[0];
		lastslot = &indexes[indexes.size() - 1];
		unlock();
	}
	void defrag() noexcept {
		lock();
		//order the allocations
		bool set = false;
		size_t last = 0;

		firstslot = 0;
		lastslot = 0;

		for(size_t i = 0; i < indexes.size(); ++i)
			if(!indexes[i].gens.is_valid()) {
				lastslot = &indexes[i];
				if(!set)
					firstslot = &indexes[i];
				else
					indexes[last].unn.next = i;

				last = i;
				set = true;
			}
		if(set)
			indexes[last].unn.next = 0;
		unlock();
	}
};

}

