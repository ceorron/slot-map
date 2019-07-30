/*----------------------------------------------------------------------------------*\
 |																					|
 | slot_map.hpp 																	|
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
#include <string.h>
#include "generation_data.hpp"

namespace std {

template<typename T,
		 typename Alloc>
struct slot_map;

namespace slot_internal {

template<typename T>
struct slot {
	slot_internal::generation_data<uint32_t> gens;
	union slot_data {
		unsigned next;								//used when object doesn't exist to reference the next object to allocate
		alignas(alignof(T)) char obj[sizeof(T)];
	} unn;
};

}

template<typename T, typename Alloc>
struct slot_map_iterator;
template<typename T, typename Alloc>
struct slot_map_const_iterator;
template<typename T, typename Alloc>
struct slot_map_reverse_iterator;
template<typename T, typename Alloc>
struct slot_map_const_reverse_iterator;

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_iterator {
private:
	slot_map<T, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::iterator itr;

	friend struct slot_map<T, Alloc>;

	friend struct slot_map_const_iterator<T, Alloc>;
	friend struct slot_map_reverse_iterator<T, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Alloc>;

	slot_map_iterator(slot_map<T, Alloc>* mp,
					  const typename std::vector<slot_internal::slot<T>, Alloc>::iterator& it)
		: map(mp), itr(it)
	{}
public:
	slot_map_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->unn.obj;
	}
	inline T* operator->() {
		return (T*)itr->unn.obj;
	}
	slot_map_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->end() && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_iterator operator++(int) {
		slot_map_iterator it(*this);
		++*this;
		return it;
	}
	slot_map_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->begin())
				end = true;
			--itr;
		} while(end && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_iterator operator--(int) {
		slot_map_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const slot_map_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const slot_map_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const slot_map_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const slot_map_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const slot_map_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const slot_map_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator slot_map_const_iterator<T, Alloc>() const {
		return slot_map_const_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Alloc>() const {
		return slot_map_reverse_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_const_iterator {
private:
	const slot_map<T, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator itr;

	friend struct slot_map<T, Alloc>;

	friend struct slot_map_iterator<T, Alloc>;
	friend struct slot_map_reverse_iterator<T, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Alloc>;

	slot_map_const_iterator(const slot_map<T, Alloc>* mp,
							const typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator& it)
		: map(mp), itr(it)
	{}
public:
	slot_map_const_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->unn.obj;
	}
	inline T* operator->() {
		return (T*)itr->unn.obj;
	}
	slot_map_const_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->cend() && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_const_iterator operator++(int) {
		slot_map_const_iterator it(*this);
		++*this;
		return it;
	}
	slot_map_const_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->cbegin())
				end = true;
			--itr;
		} while(end && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_const_iterator operator--(int) {
		slot_map_const_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const slot_map_const_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const slot_map_const_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const slot_map_const_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const slot_map_const_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const slot_map_const_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const slot_map_const_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator slot_map_iterator<T, Alloc>() const {
		return slot_map_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Alloc>() const {
		return slot_map_reverse_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_reverse_iterator {
private:
	slot_map<T, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator itr;

	friend struct slot_map<T, Alloc>;

	friend struct slot_map_iterator<T, Alloc>;
	friend struct slot_map_const_iterator<T, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Alloc>;

	slot_map_reverse_iterator(slot_map<T, Alloc>* mp,
							  const typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator& it)
		: map(mp), itr(it)
	{}
public:
	slot_map_reverse_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->unn.obj;
	}
	inline T* operator->() {
		return (T*)itr->unn.obj;
	}
	slot_map_reverse_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->rend() && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_reverse_iterator operator++(int) {
		slot_map_reverse_iterator it(*this);
		++*this;
		return it;
	}
	slot_map_reverse_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->rbegin())
				end = true;
			--itr;
		} while(end && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_reverse_iterator operator--(int) {
		slot_map_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const slot_map_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const slot_map_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const slot_map_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const slot_map_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const slot_map_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const slot_map_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator slot_map_iterator<T, Alloc>() const {
		return slot_map_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Alloc>() const {
		return slot_map_const_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_const_reverse_iterator {
private:
	const slot_map<T, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct slot_map<T, Alloc>;

	friend struct slot_map_iterator<T, Alloc>;
	friend struct slot_map_const_iterator<T, Alloc>;
	friend struct slot_map_reverse_iterator<T, Alloc>;

	slot_map_const_reverse_iterator(const slot_map<T, Alloc>* mp,
									const typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator& it)
		: map(mp), itr(it)
	{}
public:
	slot_map_const_reverse_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->unn.obj;
	}
	inline T* operator->() {
		return (T*)itr->unn.obj;
	}
	slot_map_const_reverse_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->crend() && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_const_reverse_iterator operator++(int) {
		slot_map_const_reverse_iterator it(*this);
		++*this;
		return it;
	}
	slot_map_const_reverse_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->crbegin())
				end = true;
			--itr;
		} while(end && !itr->gens.is_valid());
		return *this;
	}
	inline slot_map_const_reverse_iterator operator--(int) {
		slot_map_const_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const slot_map_const_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const slot_map_const_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const slot_map_const_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const slot_map_const_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const slot_map_const_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const slot_map_const_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator slot_map_iterator<T, Alloc>() const {
		return slot_map_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Alloc>() const {
		return slot_map_const_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Alloc>() const {
		return slot_map_reverse_iterator<T, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_handle {
private:
	slot_map<T, Alloc>* map = 0;
	unsigned idx = 0;
	unsigned gen = 0;

	friend struct slot_map<T, Alloc>;

	inline void clear() {
		map = 0;
		idx = 0;
		gen = 0;
	}

public:
	slot_map_handle() = default;

	slot_map_handle(const slot_map_handle& rhs)
		: map(0), idx(0), gen(0) {
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;
			gen = rhs.gen;
		}
	}
	slot_map_handle(slot_map_handle&& rhs) {
		map = rhs.map;
		idx = rhs.idx;
		gen = rhs.gen;

		rhs.clear();
	}

	slot_map_handle& operator=(const slot_map_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		if(rhs.map && rhs.map->increment_handle(const_cast<slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;
			gen = rhs.gen;
		}
		return *this;
	}
	slot_map_handle& operator=(slot_map_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		map = rhs.map;
		idx = rhs.idx;
		gen = rhs.gen;

		rhs.clear();
		return *this;
	}

	~slot_map_handle() {
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
		return const_cast<const T&>(*map->get_object(const_cast<slot_map_handle&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(map->get_object(const_cast<slot_map_handle&>(*this)));
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
struct slot_map {
private:
	unsigned count = 0;
	slot_internal::slot<T>* firstslot = 0;
	slot_internal::slot<T>* lastslot = 0;
	std::vector<slot_internal::slot<T>, Alloc> items;

	friend struct slot_map_iterator<T, Alloc>;
	friend struct slot_map_const_iterator<T, Alloc>;
	friend struct slot_map_reverse_iterator<T, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Alloc>;

	friend struct slot_map_handle<T, Alloc>;

	void extend(unsigned extnd) {
		if(extnd == 0)
			return;

		unsigned csze = items.size();
		items.resize(csze + extnd);

		unsigned nxt = 0;
		if(firstslot)
			nxt = std::distance(&items[0], firstslot);

		//append all of the new items onto the front of the slot list
		memset((void*)&items[csze], 0, sizeof(slot_internal::slot<T>) * extnd);
		for(unsigned i = csze; i < csze + extnd; ++i)
			if(i == csze + extnd - 1)
				items[i].unn.next = nxt;
			else
				items[i].unn.next = i + 1;

		firstslot = &items[csze];
		if(nxt == 0)
			lastslot = &items[csze + extnd - 1];
	}

public:
	slot_map(unsigned slots = 50) {
		extend(slots);
	}
	slot_map(const slot_map& rhs) = delete;
	slot_map(slot_map&& rhs) = delete;

	slot_map& operator=(const slot_map& rhs) = delete;
	slot_map& operator=(slot_map&& rhs) = delete;

	//same as normal vector
	typedef T value_type;
	typedef Alloc allocator_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef slot_map_iterator<T, Alloc> iterator;
	typedef slot_map_const_iterator<T, Alloc> const_iterator;
	typedef slot_map_reverse_iterator<T, Alloc> reverse_iterator;
	typedef slot_map_const_reverse_iterator<T, Alloc> const_reverse_iterator;
	typedef slot_map_handle<T, Alloc> handle;

private:
	void destruct_object(slot_internal::slot<T>* obj) {
		//remove object
		obj->gens.set_invalid();
		((T*)obj->unn.obj)->~T();

		//add to the start of the free list
		if(firstslot == 0) {
			obj->unn.next = 0;
			firstslot = obj;
			lastslot = obj;
		} else {
			obj->unn.next = std::distance(&items[0], firstslot);
			firstslot = obj;
		}
		--count;
	}
	bool increment_handle(slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(hdl);
		if(obj) {
			++obj->gens.get_generation_count(hdl.gen);
			return true;
		}
		return false;
	}
	void decrement_handle(slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(hdl);
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
		return iterator(this, items.begin());
	}
	inline const_iterator begin() const noexcept {
		return const_iterator(this, items.begin());
	}
	inline iterator end() noexcept {
		return iterator(this, items.end());
	}
	inline const_iterator end() const noexcept {
		return const_iterator(this, items.end());
	}

	inline reverse_iterator rbegin() noexcept {
		return reverse_iterator(this, items.rbegin());
	}
	inline const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(this, items.rbegin());
	}
	inline reverse_iterator rend() noexcept {
		return reverse_iterator(this, items.rend());
	}
	inline const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(this, items.rend());
	}

	inline const_iterator cbegin() const noexcept {
		return const_iterator(this, items.cbegin());
	}
	inline const_iterator cend() const noexcept {
		return const_iterator(this, items.cend());
	}
	inline const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(this, items.crbegin());
	}
	inline const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(this, items.crend());
	}

	// capacity:
	inline size_type size() const noexcept {
		return count;
	}
	inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	void resize(size_type sz) {
		if(sz < items.size())
			return;
		extend(sz - items.size());
	}
	inline size_type capacity() const noexcept {
		return items.capacity();
	}
	void reserve(size_type n) {
		items.reserve(n);
	}
	inline bool empty() const noexcept {
		return size() == 0;
	}
	inline void shrink_to_fit() {
		items.shrink_to_fit();
	}

private:
	unsigned get_next_free() {
		if(count == items.size())
			//double the size
			extend(items.size());

		unsigned pos = std::distance(&items[0], firstslot);

		slot_internal::slot<T>* nxt = &items[firstslot->unn.next];
		if(firstslot == lastslot)
			nxt = 0;

		if(nxt == 0) {
			firstslot = 0;
			lastslot = 0;
		} else
			firstslot = nxt;
		++count;
		return pos;
	}
public:
	slot_map_handle<T, Alloc> insert(const T& val) {
		unsigned itemPos = get_next_free();

		slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = items[itemPos].gens.new_generation();

		new (items[itemPos].unn.obj) T(val);
		return rtn;
	}
	slot_map_handle<T, Alloc> insert(T&& val) {
		unsigned itemPos = get_next_free();

		slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = items[itemPos].gens.new_generation();

		new (items[itemPos].unn.obj) T(std::move(val));
		return rtn;
	}
	template<typename Itr>
	std::vector<slot_map_handle<T, Alloc>> insert(Itr begin, Itr end) {
		std::vector<slot_map_handle<T, Alloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		return rtn;
	}

private:
	slot_internal::slot<T>* get_object_internal(slot_map_handle<T, Alloc>& hdl) {
		if(hdl.map == 0)
			return 0;
		slot_internal::slot<T>& rf = items[hdl.idx];
		//test that the generation matches
		if(!rf.gens.is_valid() || !rf.gens.match_generation(hdl.gen)) {
			rf.gens.decrement_generation(hdl.gen);
			hdl.clear();
			return 0;
		}
		return &rf;
	}
public:

	inline bool is_valid(const slot_map_handle<T, Alloc>& hdl) {
		return get_object_internal(const_cast<slot_map_handle<T, Alloc>&>(hdl)) != 0;
	}
	T* get_object(slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(hdl);
		if(obj)
			return (T*)obj->unn.obj;
		return 0;
	}
	const T* get_object(const slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(const_cast<slot_map_handle<T, Alloc>&>(hdl));
		if(obj)
			return (const T*)obj->unn.obj;
		return 0;
	}

	void erase(slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(hdl);
		if(obj)
			destruct_object(obj);
	}

	void clear() noexcept {
		//just clear the data, erase everything
		unsigned i = 0;
		for(auto it = items.begin(); it != items.end(); ++it, ++i) {
			if(it->gens.is_valid()) {
				slot_map_handle<T, Alloc> hdl;
				hdl.map = this;
				hdl.idx = std::distance(items.begin(), it);
				hdl.gen = it->gens.increment_generation();

				erase(hdl);
			}
			if(i == items.size())
				items[i].unn.next = 0;
			else
				items[i].unn.next = i + 1;
		}

		firstslot = &items[0];
		lastslot = &items[items.size() - 1];
	}
	void defrag() noexcept {
		//order the allocations
		bool set = false;
		unsigned last = 0;

		firstslot = 0;
		lastslot = 0;

		for(unsigned i = 0; i < items.size(); ++i)
			if(!items[i].gens.is_valid()) {
				lastslot = &items[i];
				if(!set)
					firstslot = &items[i];
				else
					items[last].unn.next = i;

				last = i;
				set = true;
			}
		if(set)
			items[last].unn.next = 0;
	}

	~slot_map() {
		//erase everything
		for(auto it = items.begin(); it != items.end(); ++it)
			if(it->gens.is_valid())
				((T*)it->unn.obj)->~T();
	}
};

}

