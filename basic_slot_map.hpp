/*----------------------------------------------------------------------------------*\
 |																					|
 | basic_slot_map.hpp 																|
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

namespace std {

#define basic_slot_map_invalid	(size_t)-1

template<typename T, typename Mut, typename Alloc>
struct basic_slot_map;

namespace slot_internal {

template<typename T>
struct basic_slot {
	bool valid = false;
	alignas(alignof(T)) char obj[sizeof(T)];
};

}

template<typename T, typename Mut, typename Alloc>
struct basic_slot_map_iterator;
template<typename T, typename Mut, typename Alloc>
struct basic_slot_map_const_iterator;
template<typename T, typename Mut, typename Alloc>
struct basic_slot_map_reverse_iterator;
template<typename T, typename Mut, typename Alloc>
struct basic_slot_map_const_reverse_iterator;

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct basic_slot_map_iterator {
private:
	basic_slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc>;

	friend struct basic_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	basic_slot_map_iterator(basic_slot_map<T, Mut, Alloc>* mp,
							const typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator& it)
		: map(mp), itr(it)
	{}
public:
	basic_slot_map_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	basic_slot_map_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->end() && !itr->valid);
		return *this;
	}
	inline basic_slot_map_iterator operator++(int) {
		basic_slot_map_iterator it(*this);
		++*this;
		return it;
	}
	basic_slot_map_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->begin())
				end = true;
			--itr;
		} while(end && !itr->valid);
		return *this;
	}
	inline basic_slot_map_iterator operator--(int) {
		basic_slot_map_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_slot_map_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_slot_map_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_slot_map_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_slot_map_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_slot_map_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_slot_map_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_slot_map_const_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_const_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator basic_slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_const_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct basic_slot_map_const_iterator {
private:
	const basic_slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc>;

	friend struct basic_slot_map_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	basic_slot_map_const_iterator(const basic_slot_map<T, Mut, Alloc>* mp,
								  const typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator& it)
		: map(mp), itr(it)
	{}
public:
	basic_slot_map_const_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	basic_slot_map_const_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->cend() && !itr->valid);
		return *this;
	}
	inline basic_slot_map_const_iterator operator++(int) {
		basic_slot_map_const_iterator it(*this);
		++*this;
		return it;
	}
	basic_slot_map_const_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->cbegin())
				end = true;
			--itr;
		} while(end && !itr->valid);
		return *this;
	}
	inline basic_slot_map_const_iterator operator--(int) {
		basic_slot_map_const_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_slot_map_const_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_slot_map_const_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_slot_map_const_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_slot_map_const_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_slot_map_const_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_slot_map_const_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_slot_map_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator basic_slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_const_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct basic_slot_map_reverse_iterator {
private:
	basic_slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc>;

	friend struct basic_slot_map_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	basic_slot_map_reverse_iterator(basic_slot_map<T, Mut, Alloc>* mp,
									const typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator& it)
		: map(mp), itr(it)
	{}
public:
	basic_slot_map_reverse_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	basic_slot_map_reverse_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->rend() && !itr->valid);
		return *this;
	}
	inline basic_slot_map_reverse_iterator operator++(int) {
		basic_slot_map_reverse_iterator it(*this);
		++*this;
		return it;
	}
	basic_slot_map_reverse_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->rbegin())
				end = true;
			--itr;
		} while(end && !itr->valid);
		return *this;
	}
	inline basic_slot_map_reverse_iterator operator--(int) {
		basic_slot_map_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_slot_map_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_slot_map_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_slot_map_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_slot_map_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_slot_map_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_slot_map_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_slot_map_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_slot_map_const_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_const_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_const_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct basic_slot_map_const_reverse_iterator {
private:
	const basic_slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc>;

	friend struct basic_slot_map_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc>;

	basic_slot_map_const_reverse_iterator(const basic_slot_map<T, Mut, Alloc>* mp,
										  const typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator& it)
		: map(mp), itr(it)
	{}
public:
	basic_slot_map_const_reverse_iterator() = default;
	inline T& operator*() {
		return *(T*)itr->obj;
	}
	inline T* operator->() {
		return (T*)itr->obj;
	}
	basic_slot_map_const_reverse_iterator& operator++() {
		do {
			++itr;
		} while(*this != map->crend() && !itr->valid);
		return *this;
	}
	inline basic_slot_map_const_reverse_iterator operator++(int) {
		basic_slot_map_const_reverse_iterator it(*this);
		++*this;
		return it;
	}
	basic_slot_map_const_reverse_iterator& operator--() {
		bool end = false;
		do {
			if(*this == map->crbegin())
				end = true;
			--itr;
		} while(end && !itr->valid);
		return *this;
	}
	inline basic_slot_map_const_reverse_iterator operator--(int) {
		basic_slot_map_const_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_slot_map_const_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_slot_map_const_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_slot_map_const_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_slot_map_const_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_slot_map_const_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_slot_map_const_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_slot_map_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_slot_map_const_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_const_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return basic_slot_map_reverse_iterator<T, Mut, Alloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct basic_slot_map_handle {
private:
	basic_slot_map<T, Mut, Alloc>* map = 0;
	size_t idx = basic_slot_map_invalid;

	friend struct basic_slot_map<T, Mut, Alloc>;

	inline void clear() {
		map = 0;
		idx = basic_slot_map_invalid;
	}

public:
	basic_slot_map_handle() = default;

	basic_slot_map_handle(const basic_slot_map_handle& rhs)
		: map(0), idx(basic_slot_map_invalid) {
		if(rhs.map && rhs.map->is_valid(const_cast<basic_slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;

			if(map && idx != basic_slot_map_invalid)
				map->increment_handle(*this);
		}
	}
	basic_slot_map_handle(basic_slot_map_handle&& rhs) {
		map = rhs.map;
		idx = rhs.idx;

		rhs.clear();
	}

	basic_slot_map_handle& operator=(const basic_slot_map_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_slot_map_handle();

		if(rhs.map && rhs.map->is_valid(const_cast<basic_slot_map_handle&>(rhs))) {
			map = rhs.map;
			idx = rhs.idx;

			if(map && idx != basic_slot_map_invalid)
				map->increment_handle(*this);
		}
		return *this;
	}
	basic_slot_map_handle& operator=(basic_slot_map_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_slot_map_handle();

		map = rhs.map;
		idx = rhs.idx;

		rhs.clear();
		return *this;
	}

	~basic_slot_map_handle() {
		if(map && idx != basic_slot_map_invalid)
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
		return const_cast<const T&>(*map->get_object(const_cast<basic_slot_map_handle&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(map->get_object(const_cast<basic_slot_map_handle&>(*this)));
	}

	inline operator T*() {
		return map->get_object(*this);
	}
	inline operator const T*() const {
		return map->get_object(*this);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct basic_slot_map {
private:
	struct slot_ref {
		size_t count = 0;
		size_t idx = basic_slot_map_invalid;
	};

	size_t itemcount = 0;
	size_t idxcount = 0;
	size_t nextitem = 0;
	size_t nextidx = 0;
	Mut mut;
	std::vector<slot_internal::basic_slot<T>, Alloc> items;
	std::vector<slot_ref, Alloc> idxs;

	friend struct basic_slot_map_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_const_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc>;

	friend struct basic_slot_map_handle<T, Mut, Alloc>;
public:
	basic_slot_map(size_t slots = 50) {
		items.resize(slots);
		idxs.resize(slots);
	}
	basic_slot_map(const basic_slot_map& rhs) = default;
	basic_slot_map(basic_slot_map&& rhs) = delete;

	basic_slot_map& operator=(const basic_slot_map& rhs) = default;
	basic_slot_map& operator=(basic_slot_map&& rhs) = delete;

	//same as normal vector
	typedef T value_type;
	typedef Alloc allocator_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef basic_slot_map_iterator<T, Mut, Alloc> iterator;
	typedef basic_slot_map_const_iterator<T, Mut, Alloc> const_iterator;
	typedef basic_slot_map_reverse_iterator<T, Mut, Alloc> reverse_iterator;
	typedef basic_slot_map_const_reverse_iterator<T, Mut, Alloc> const_reverse_iterator;
	typedef basic_slot_map_handle<T, Mut, Alloc> handle;

	void lock() {
		mut.lock();
	}
	void unlock() {
		mut.unlock();
	}
private:
	void increment_handle(basic_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		if(get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc>&>(hdl)) != 0)
			++idxs[hdl.idx].count;
		unlock();
	}
	void decrement_handle(basic_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		//in the destructor of hdl
		if(get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc>&>(hdl)) != 0) {
			slot_ref& rf = idxs[hdl.idx];
			--rf.count;
			if(rf.count == 0) {
				--idxcount;
				slot_internal::basic_slot<T>& slt = items[rf.idx];
				slt.valid = false;
				((T*)slt.obj)->~T();
				rf.idx = basic_slot_map_invalid;
				--itemcount;
			}
		}
		unlock();
	}
public:

	// iterators:
	inline iterator begin() noexcept {
		auto it = items.begin();
		for(; it != items.end() && !it->valid; ++it);
		return iterator(this, it);
	}
	inline const_iterator begin() const noexcept {
		auto it = items.begin();
		for(; it != items.end() && !it->valid; ++it);
		return const_iterator(this, it);
	}
	inline iterator end() noexcept {
		return iterator(this, items.end());
	}
	inline const_iterator end() const noexcept {
		return const_iterator(this, items.end());
	}

	inline reverse_iterator rbegin() noexcept {
		auto it = items.rbegin();
		for(; it != items.rend() && !it->valid; ++it);
		return reverse_iterator(this, it);
	}
	inline const_reverse_iterator rbegin() const noexcept {
		auto it = items.rbegin();
		for(; it != items.rend() && !it->valid; ++it);
		return const_reverse_iterator(this, it);
	}
	inline reverse_iterator rend() noexcept {
		return reverse_iterator(this, items.rend());
	}
	inline const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(this, items.rend());
	}

	inline const_iterator cbegin() const noexcept {
		auto it = items.cbegin();
		for(; it != items.cend() && !it->valid; ++it);
		return const_iterator(this, it);
	}
	inline const_iterator cend() const noexcept {
		return const_iterator(this, items.cend());
	}
	inline const_reverse_iterator crbegin() const noexcept {
		auto it = items.crbegin();
		for(; it != items.crend() && !it->valid; ++it);
		return const_reverse_iterator(this, it);
	}
	inline const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(this, items.crend());
	}

	// capacity:
	inline size_type size() const noexcept {
		lock();
		size_type rtn = itemcount;
		unlock();
		return rtn;
	}
	inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	void resize(size_type sz) {
		lock();
		if(sz < items.size()) {
			unlock();
			return;
		}
		items.resize(sz);
		idxs.resize(sz);
		unlock();
	}
	inline size_type capacity() const noexcept {
		lock();
		size_type rtn = items.capacity();
		unlock();
		return rtn;
	}
	void reserve(size_type n) {
		lock();
		items.resize(n);
		idxs.resize(n);
		unlock();
	}
	inline bool empty() const noexcept {
		lock();
		bool rtn = size() == 0;
		unlock();
		return rtn;
	}
	void shrink_to_fit() {
		lock();
		items.shrink_to_fit();
		idxs.shrink_to_fit();
		unlock();
	}

private:
	void get_next_free(size_t& itemPos, size_t& idxPos) {
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
	basic_slot_map_handle<T, Mut, Alloc> insert(const T& val) {
		lock();
		size_t itemPos = 0;
		size_t idxPos = 0;
		get_next_free(itemPos, idxPos);

		basic_slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = idxPos;

		new (items[itemPos].obj) T(val);
		unlock();
		return rtn;
	}
	basic_slot_map_handle<T, Mut, Alloc> insert(T&& val) {
		lock();
		size_t itemPos = 0;
		size_t idxPos = 0;
		get_next_free(itemPos, idxPos);

		basic_slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = idxPos;

		new (items[itemPos].obj) T(std::move(val));
		unlock();
		return rtn;
	}
	template<typename Itr>
	std::vector<basic_slot_map_handle<T, Mut, Alloc>> insert(Itr begin, Itr end) {
		lock();
		std::vector<basic_slot_map_handle<T, Mut, Alloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		unlock();
		return rtn;
	}

private:
	slot_internal::basic_slot<T>* get_object_internal(basic_slot_map_handle<T, Mut, Alloc>& hdl) {
		if(hdl.idx == basic_slot_map_invalid)
			return 0;
		slot_ref& rf = idxs[hdl.idx];
		if(rf.idx == basic_slot_map_invalid) {
			hdl.clear();
			--rf.count;
			if(rf.count == 0) --idxcount;
			return 0;
		}
		slot_internal::basic_slot<T>& slt = items[rf.idx];
		if(!slt.valid) {
			hdl.clear();
			--rf.count;
			if(rf.count == 0) --idxcount;
			return 0;
		}
		return &items[rf.idx];
	}
public:

	inline bool is_valid(const basic_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		bool rtn = get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc>&>(hdl)) != 0;
		unlock();
		return rtn;
	}
	T* get_object(basic_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		slot_internal::basic_slot<T>* obj = get_object_internal(hdl);
		if(obj) {
			T* rtn = (T*)obj->obj;
			unlock();
			return rtn;
		}
		unlock();
		return 0;
	}
	const T* get_object(const basic_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		slot_internal::basic_slot<T>* obj = get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc>&>(hdl));
		if(obj) {
			const T* rtn = (const T*)obj->obj;
			unlock();
			return rtn;
		}
		unlock();
		return 0;
	}

	void erase(basic_slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		slot_internal::basic_slot<T>* obj = get_object_internal(hdl);
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
		unlock();
	}

	void clear() noexcept {
		lock();
		for(auto it = items.begin(); it != items.end(); ++it)
			if(it->valid) {
				it->valid = false;
				((T*)it->obj)->~T();
			}
		unlock();
	}

	~basic_slot_map() {
		clear();
	}
};

}

