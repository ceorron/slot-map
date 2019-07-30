/*----------------------------------------------------------------------------------*\
 |																					|
 | ordered_slot_map.hpp 															|
 |																					|
 | Author: (C) Copyright Richard Cookman											|
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
#include "generation_data.hpp"

namespace std {

template<typename T,
		 typename Alloc>
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
	unsigned backidx;
	T obj;

	inline operator T&() {
		return obj;
	}
	inline operator const T&() const {
		return obj;
	}
};

}

template<typename T, typename Alloc>
struct ordered_slot_map_iterator;
template<typename T, typename Alloc>
struct ordered_slot_map_const_iterator;
template<typename T, typename Alloc>
struct ordered_slot_map_reverse_iterator;
template<typename T, typename Alloc>
struct ordered_slot_map_const_reverse_iterator;

template<typename T,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator itr;

	friend struct ordered_slot_map<T, Alloc>;

	friend struct ordered_slot_map_const_iterator<T, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Alloc>;

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

	inline operator ordered_slot_map_const_iterator<T, Alloc>() const {
		return ordered_slot_map_const_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Alloc>() const {
		return ordered_slot_map_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Alloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_const_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator itr;

	friend struct ordered_slot_map<T, Alloc>;

	friend struct ordered_slot_map_iterator<T, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Alloc>;

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

	inline operator ordered_slot_map_iterator<T, Alloc>() const {
		return ordered_slot_map_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Alloc>() const {
		return ordered_slot_map_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Alloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_reverse_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator itr;

	friend struct ordered_slot_map<T, Alloc>;

	friend struct ordered_slot_map_iterator<T, Alloc>;
	friend struct ordered_slot_map_const_iterator<T, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Alloc>;

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

	inline operator ordered_slot_map_iterator<T, Alloc>() const {
		return ordered_slot_map_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_const_iterator<T, Alloc>() const {
		return ordered_slot_map_const_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Alloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_const_reverse_iterator {
private:
	typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct ordered_slot_map<T, Alloc>;

	friend struct ordered_slot_map_iterator<T, Alloc>;
	friend struct ordered_slot_map_const_iterator<T, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Alloc>;

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

	inline operator ordered_slot_map_iterator<T, Alloc>() const {
		return ordered_slot_map_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_const_iterator<T, Alloc>() const {
		return ordered_slot_map_const_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Alloc>() const {
		return ordered_slot_map_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map_handle {
private:
	ordered_slot_map<T, Alloc>* map = 0;
	unsigned idx = 0;
	unsigned gen = 0;

	friend struct ordered_slot_map<T, Alloc>;

	inline void clear() {
		map = 0;
		idx = 0;
		gen = 0;
	}

public:
	ordered_slot_map_handle() = default;

	ordered_slot_map_handle(const ordered_slot_map_handle& rhs)
		: map(0), idx(0), gen(0) {
		if(rhs.map && rhs.map->increment_handle(const_cast<ordered_slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;
			gen = rhs.gen;
		}
	}
	ordered_slot_map_handle(ordered_slot_map_handle&& rhs) {
		map = rhs.map;
		idx = rhs.idx;
		gen = rhs.gen;

		rhs.clear();
	}

	ordered_slot_map_handle& operator=(const ordered_slot_map_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_handle();

		if(rhs.map && rhs.map->increment_handle(const_cast<ordered_slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;
			gen = rhs.gen;
		}
		return *this;
	}
	ordered_slot_map_handle& operator=(ordered_slot_map_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~ordered_slot_map_handle();

		map = rhs.map;
		idx = rhs.idx;
		gen = rhs.gen;

		rhs.clear();
		return *this;
	}

	~ordered_slot_map_handle() {
		if(map)
			map->decrement_handle(*this);
		clear();
	}

	inline T& operator*() {
		return *map->get_object(*this);
	}
	inline T* operator->() {
		return map->get_object(*this);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*map->get_object(const_cast<ordered_slot_map_handle&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(map->get_object(const_cast<ordered_slot_map_handle&>(*this)));
	}

	inline operator T*() {
		return map->get_object(*this);
	}
	inline operator const T*() const {
		return map->get_object(*this);
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct ordered_slot_map {
private:
	struct slot_index {
		slot_internal::generation_data<uint32_t> gens;
		unsigned next;									//used when object doesn't exist to reference the next object to allocate
		unsigned idx;									//index into items
	};
	unsigned count = 0;
	slot_index* firstslot = 0;
	slot_index* lastslot = 0;
	std::vector<slot_internal::ordered_slot<T>, Alloc> items;
	std::vector<slot_index, Alloc> indexes;

	friend struct ordered_slot_map_iterator<T, Alloc>;
	friend struct ordered_slot_map_const_iterator<T, Alloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Alloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Alloc>;

	friend struct ordered_slot_map_handle<T, Alloc>;

	void extend(unsigned extnd) {
		if(extnd == 0)
			return;

		unsigned csze = indexes.size();
		items.reserve(items.size() + extnd);
		indexes.resize(csze + extnd);

		unsigned nxt = 0;
		if(firstslot)
			nxt = std::distance(&indexes[0], firstslot);

		//append all of the new indexes onto the front of the slot list
		memset((void*)&indexes[csze], 0, sizeof(slot_internal::slot<T>) * extnd);
		for(unsigned i = csze; i < csze + extnd; ++i)
			if(i == csze + extnd - 1)
				indexes[i].next = nxt;
			else
				indexes[i].next = i + 1;

		firstslot = &indexes[csze];
		if(nxt == 0)
			lastslot = &indexes[csze + extnd - 1];
	}

public:
	ordered_slot_map(unsigned slots = 50) {
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
	typedef ordered_slot_map_iterator<T, Alloc> iterator;
	typedef ordered_slot_map_const_iterator<T, Alloc> const_iterator;
	typedef ordered_slot_map_reverse_iterator<T, Alloc> reverse_iterator;
	typedef ordered_slot_map_const_reverse_iterator<T, Alloc> const_reverse_iterator;
	typedef ordered_slot_map_handle<T, Alloc> handle;

private:
	void destruct_object(slot_index* obj) {
		//remove object
		obj->gens.set_invalid();

		//remove this object
		items.erase(items.begin() + obj->idx);
		//change all of the indexes to the displaced objects
		for(unsigned idx = obj->idx; idx < items.size(); ++idx)
			indexes[items[idx].backidx].idx = idx;

		obj->idx = 0;

		//add to the start of the free list
		if(firstslot == 0) {
			obj->next = 0;
			firstslot = obj;
			lastslot = obj;
		} else {
			obj->next = std::distance(&indexes[0], firstslot);
			firstslot = obj;
		}
		--count;
	}
	bool increment_handle(ordered_slot_map_handle<T, Alloc>& hdl) {
		slot_index* obj = get_object_internal(hdl);
		if(obj) {
			++obj->gens.get_generation_count(hdl.gen);
			return true;
		}
		return false;
	}
	void decrement_handle(ordered_slot_map_handle<T, Alloc>& hdl) {
		slot_index* obj = get_object_internal(hdl);
		if(obj) {
			unsigned& cnt = obj->gens.get_generation_count(hdl.gen);
			--cnt;
			if(cnt == 0) {
				destruct_object(obj);
				hdl.clear();
			}
		}
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
		return count;
	}
	inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	void resize(size_type sz) {
		if(sz < indexes.size())
			return;
		extend(sz - indexes.size());
	}
	inline size_type capacity() const noexcept {
		return items.capacity();
	}
	void reserve(size_type n) {
		items.reserve(n);
		indexes.reserve(n);
	}
	inline bool empty() const noexcept {
		return size() == 0;
	}
	inline void shrink_to_fit() {
		items.shrink_to_fit();
		indexes.shrink_to_fit();
	}

private:
	void update_object_indexes(unsigned pos) {
		for(unsigned i = pos + 1; i < items.size(); ++i)
			indexes[items[i].backidx].idx = i;
	}
	unsigned get_insert_pos(const T& val) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
			[](const T& lhs, const T& rhs){
				return lhs < rhs;
			}, out);

		unsigned pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = val;
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	template<typename Less>
	unsigned get_insert_pos(const T& val, Less comp) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
									 comp, out);

		unsigned pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = val;
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	unsigned get_insert_pos(T&& val) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
			[](const T& lhs, const T& rhs){
				return lhs < rhs;
			}, out);

		unsigned pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = std::move(val);
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	template<typename Less>
	unsigned get_insert_pos(T&& val, Less comp) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
									 comp, out);

		unsigned pos = std::distance(items.begin(), out);
		slot_internal::ordered_slot<T> itm;
		itm.obj = std::move(val);
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	unsigned get_next_free(unsigned nidx) {
		if(count == indexes.size())
			//double the size
			extend(indexes.size());

		unsigned pos = std::distance(&indexes[0], firstslot);

		slot_index* nxt = &indexes[firstslot->next];
		if(firstslot == lastslot)
			nxt = 0;

		if(nxt == 0) {
			firstslot = 0;
			lastslot = 0;
		} else
			firstslot = nxt;

		++count;
		indexes[pos].idx = nidx;
		return pos;
	}
public:
	ordered_slot_map_handle<T, Alloc> insert(const T& val) {
		unsigned idx = get_insert_pos(val);
		unsigned itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		return rtn;
	}
	template<typename Less>
	ordered_slot_map_handle<T, Alloc> insert(const T& val, Less comp) {
		unsigned idx = get_insert_pos(val, comp);
		unsigned itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		return rtn;
	}
	ordered_slot_map_handle<T, Alloc> insert(T&& val) {
		unsigned idx = get_insert_pos(std::move(val));
		unsigned itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		return rtn;
	}
	template<typename Less>
	ordered_slot_map_handle<T, Alloc> insert(T&& val, Less comp) {
		unsigned idx = get_insert_pos(std::move(val), comp);
		unsigned itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		ordered_slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		return rtn;
	}
	template<typename Itr>
	std::vector<ordered_slot_map_handle<T, Alloc>> insert(Itr begin, Itr end) {
		std::vector<ordered_slot_map_handle<T, Alloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		return rtn;
	}
	template<typename Itr, typename Less>
	std::vector<ordered_slot_map_handle<T, Alloc>> insert(Itr begin, Itr end, Less comp) {
		std::vector<ordered_slot_map_handle<T, Alloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin, comp));
		return rtn;
	}

private:
	slot_index* get_object_internal(ordered_slot_map_handle<T, Alloc>& hdl) {
		if(hdl.map == 0)
			return 0;
		slot_index& rf = indexes[hdl.idx];
		//test that the generation matches
		if(!rf.gens.is_valid() || !rf.gens.match_generation(hdl.gen)) {
			rf.gens.decrement_generation(hdl.gen);
			hdl.clear();
			return 0;
		}
		return &rf;
	}

public:
	inline bool is_valid(const ordered_slot_map_handle<T, Alloc>& hdl) {
		return get_object_internal(const_cast<ordered_slot_map_handle<T, Alloc>&>(hdl)) != 0;
	}
	T* get_object(ordered_slot_map_handle<T, Alloc>& hdl) {
		slot_index* obj = get_object_internal(hdl);
		if(obj)
			return &items[obj->idx].obj;
		return 0;
	}
	const T* get_object(const ordered_slot_map_handle<T, Alloc>& hdl) {
		slot_index* obj = get_object_internal(const_cast<ordered_slot_map_handle<T, Alloc>&>(hdl));
		if(obj)
			return &items[obj->idx].obj;
		return 0;
	}

	void erase(ordered_slot_map_handle<T, Alloc>& hdl) {
		slot_index* obj = get_object_internal(hdl);
		if(obj)
			destruct_object(obj);
	}

	void clear() noexcept {
		//just clear the data, erase everything
		unsigned i = 0;
		for(auto it = indexes.begin(); it != indexes.end(); ++it, ++i) {
			if(it->gens.is_valid()) {
				slot_map_handle<T, Alloc> hdl;
				hdl.map = this;
				hdl.idx = std::distance(indexes.begin(), it);
				hdl.gen = it->gens.increment_generation();

				erase(hdl);
			}
			if(i == indexes.size())
				indexes[i].next = 0;
			else
				indexes[i].next = i + 1;
		}

		firstslot = &indexes[0];
		lastslot = &indexes[indexes.size() - 1];
	}
	void defrag() noexcept {
		//order the allocations
		bool set = false;
		unsigned last = 0;

		firstslot = 0;
		lastslot = 0;

		for(unsigned i = 0; i < indexes.size(); ++i)
			if(!indexes[i].gens.is_valid()) {
				lastslot = &indexes[i];
				if(!set)
					firstslot = &indexes[i];
				else
					indexes[last].next = i;

				last = i;
				set = true;
			}
		if(set)
			indexes[last].next = 0;
	}
};

}
