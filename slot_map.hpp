/*----------------------------------------------------------------------------------*\
 |																					|
 | slot_map.hpp 																	|
 |																					|
 | Author: (C) Copyright Richard Cookman, Eyra Software Systems 2019				|
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

namespace std {

#define slot_map_invalid	(unsigned)-1

template<typename T,
		 typename Alloc>
struct slot_map;

namespace slot_internal {

template<typename T>
struct slot {
	bool valid = false;
	alignas(alignof(T)) char obj[sizeof(T)];
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

	slot_map_iterator(const typename std::vector<slot_internal::slot<T>, Alloc>::iterator& it)
		: itr(it)
	{}
public:
	slot_map_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	inline slot_map_iterator& operator++() {
		do {
			++itr;
		} while(!itr->valid);
		return *this;
	}
	inline slot_map_iterator operator++(int) {
		slot_map_iterator it(*this);
		++*this;
		return it;
	}
	inline slot_map_iterator& operator--() {
		do {
			--itr;
		} while(!itr->valid);
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
		return slot_map_const_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Alloc>() const {
		return slot_map_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_const_iterator {
private:
	slot_map<T, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator itr;

	friend struct slot_map<T, Alloc>;

	friend struct slot_map_iterator<T, Alloc>;
	friend struct slot_map_reverse_iterator<T, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Alloc>;

	slot_map_const_iterator(const typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator& it)
		: itr(it)
	{}
public:
	slot_map_const_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	inline slot_map_const_iterator& operator++() {
		do {
			++itr;
		} while(!itr->valid);
		return *this;
	}
	inline slot_map_const_iterator operator++(int) {
		slot_map_const_iterator it(*this);
		++*this;
		return it;
	}
	inline slot_map_const_iterator& operator--() {
		do {
			--itr;
		} while(!itr->valid);
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
		return slot_map_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Alloc>() const {
		return slot_map_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
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

	slot_map_reverse_iterator(const typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator& it)
		: itr(it)
	{}
public:
	slot_map_reverse_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	inline slot_map_reverse_iterator& operator++() {
		do {
			++itr;
		} while(!itr->valid);
		return *this;
	}
	inline slot_map_reverse_iterator operator++(int) {
		slot_map_reverse_iterator it(*this);
		++*this;
		return it;
	}
	inline slot_map_reverse_iterator& operator--() {
		do {
			--itr;
		} while(!itr->valid);
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
		return slot_map_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Alloc>() const {
		return slot_map_const_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_const_reverse_iterator {
private:
	slot_map<T, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct slot_map<T, Alloc>;

	friend struct slot_map_iterator<T, Alloc>;
	friend struct slot_map_const_iterator<T, Alloc>;
	friend struct slot_map_reverse_iterator<T, Alloc>;

	slot_map_const_reverse_iterator(const typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator& it)
		: itr(it)
	{}
public:
	slot_map_const_reverse_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	inline slot_map_const_reverse_iterator& operator++() {
		do {
			++itr;
		} while(!itr->valid);
		return *this;
	}
	inline slot_map_const_reverse_iterator operator++(int) {
		slot_map_const_reverse_iterator it(*this);
		++*this;
		return it;
	}
	inline slot_map_const_reverse_iterator& operator--() {
		do {
			--itr;
		} while(!itr->valid);
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
		return slot_map_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Alloc>() const {
		return slot_map_const_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Alloc>() const {
		return slot_map_reverse_iterator<T, Alloc>(typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T,
		 typename Alloc = std::allocator<T>>
struct slot_map_handle {
private:
	slot_map<T, Alloc>* map = 0;
	unsigned idx = slot_map_invalid;

	friend struct slot_map<T, Alloc>;

	inline void clear() {
		map = 0;
		idx = slot_map_invalid;
	}

public:
	slot_map_handle() = default;

	slot_map_handle(const slot_map_handle& rhs)
		: map(0), idx(slot_map_invalid) {
		if(rhs.map && rhs.map->is_valid(const_cast<slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;

			if(map && idx != slot_map_invalid)
				map->increment_handle(*this);
		}
	}
	slot_map_handle(slot_map_handle&& rhs) {
		map = rhs.map;
		idx = rhs.idx;

		rhs.clear();
	}

	slot_map_handle& operator=(const slot_map_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		if(rhs.map && rhs.map->is_valid(const_cast<slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;

			if(map && idx != slot_map_invalid)
				map->increment_handle(*this);
		}
		return *this;
	}
	slot_map_handle& operator=(slot_map_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		map = rhs.map;
		idx = rhs.idx;

		rhs.clear();
		return *this;
	}

	~slot_map_handle() {
		if(map && idx != slot_map_invalid)
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
	struct slot_ref {
		unsigned count = 0;
		unsigned idx = slot_map_invalid;
	};

	unsigned itemcount = 0;
	unsigned idxcount = 0;
	unsigned nextitem = 0;
	unsigned nextidx = 0;
	std::vector<slot_internal::slot<T>, Alloc> items;
	std::vector<slot_ref, Alloc> idxs;

	friend struct slot_map_iterator<T, Alloc>;
	friend struct slot_map_const_iterator<T, Alloc>;
	friend struct slot_map_reverse_iterator<T, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Alloc>;

	friend struct slot_map_handle<T, Alloc>;
public:
	slot_map(unsigned slots = 50) {
		items.resize(slots);
		idxs.resize(slots);
	}
	slot_map(const slot_map& rhs) = default;
	slot_map(slot_map& rhs) = default;

	slot_map& operator=(const slot_map& rhs) = default;
	slot_map& operator=(slot_map& rhs) = default;

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
	void increment_handle(slot_map_handle<T, Alloc>& hdl) {
		if(is_valid(hdl))
			++idxs[hdl.idx].count;
	}
	void decrement_handle(slot_map_handle<T, Alloc>& hdl) {
		//in the destructor of hdl
		if(is_valid(hdl)) {
			slot_ref& rf = idxs[hdl.idx];
			--rf.count;
			if(rf.count == 0) {
				--idxcount;
				slot_internal::slot<T>& slt = items[rf.idx];
				slt.valid = false;
				((T*)slt.obj)->~T();
				rf.idx = slot_map_invalid;
				--itemcount;
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
		return itemcount;
	}
	inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	void resize(size_type sz) {
		if(sz < items.size())
			return;
		items.resize(sz);
		idxs.resize(sz);
	}
	inline size_type capacity() const noexcept {
		return items.capacity();
	}
	void reserve(size_type n) {
		items.resize(n);
		idxs.resize(n);
	}
	inline bool empty() const noexcept {
		return size() == 0;
	}
	void shrink_to_fit() {
		items.shrink_to_fit();
		idxs.shrink_to_fit();
	}

private:
	void get_next_free(unsigned& itemPos, unsigned& idxPos) {
		//resize if we are out of slots
		if(idxcount == idxs.size()) {
			//move the indexes to reference the free ones
			nextitem = idxs.size();
			nextidx = idxs.size();
			items.resize(idxs.size() * 2);
			idxs.resize(idxs.size() * 2);
		}
		//the allocation function
		itemPos = nextitem;
		idxPos = nextidx;

		items[itemPos].valid = true;
		idxs[idxPos].count = 1;
		idxs[idxPos].idx = itemPos;

		++itemcount;
		++idxcount;

		//get the next item and index
		do {
			++nextitem;
			if(nextitem == items.size())
				nextitem = 0;
		} while(items[nextitem].valid);

		do {
			++nextidx;
			if(nextidx == idxs.size())
				nextidx = 0;
		} while(idxs[nextidx].count > 0);
	}
public:
	slot_map_handle<T, Alloc> insert(const T& val) {
		unsigned itemPos = 0;
		unsigned idxPos = 0;
		get_next_free(itemPos, idxPos);

		slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = idxPos;

		new (items[itemPos].obj) T(val);
		return rtn;
	}
	slot_map_handle<T, Alloc> insert(T&& val) {
		unsigned itemPos = 0;
		unsigned idxPos = 0;
		get_next_free(itemPos, idxPos);

		slot_map_handle<T, Alloc> rtn;
		rtn.map = this;
		rtn.idx = idxPos;

		new (items[itemPos].obj) T(std::move(val));
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
		if(hdl.idx == slot_map_invalid)
			return 0;
		slot_ref& rf = idxs[hdl.idx];
		if(rf.idx == slot_map_invalid) {
			hdl.clear();
			--rf.count;
			if(rf.count == 0) --idxcount;
			return 0;
		}
		slot_internal::slot<T>& slt = items[rf.idx];
		if(!slt.valid) {
			hdl.clear();
			--rf.count;
			if(rf.count == 0) --idxcount;
			return 0;
		}
		return &items[rf.idx];
	}
public:

	inline bool is_valid(const slot_map_handle<T, Alloc>& hdl) {
		return get_object_internal(const_cast<slot_map_handle<T, Alloc>&>(hdl)) != 0;
	}
	T* get_object(slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(hdl);
		if(obj)
			return (T*)obj->obj;
		return 0;
	}
	const T* get_object(const slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(const_cast<slot_map_handle<T, Alloc>&>(hdl));
		if(obj)
			return (const T*)obj->obj;
		return 0;
	}

	void erase(slot_map_handle<T, Alloc>& hdl) {
		slot_internal::slot<T>* obj = get_object_internal(hdl);
		if(obj) {
			//clear the object
			obj->valid = false;
			((T*)obj->obj)->~T();
			--itemcount;

			//clear the handle too
			slot_ref& rf = idxs[hdl.idx];
			hdl.clear();
			--rf.count;
			if(rf.count == 0) --idxcount;
		}
		return;
	}

	void clear() noexcept {
		for(auto it = items.begin(); it != items.end(); ++it)
			if(it->valid) {
				it->valid = false;
				((T*)it->obj)->~T();
			}

		*this = slot_map(items.size());
	}

	~slot_map() {
		clear();
	}
};

}
