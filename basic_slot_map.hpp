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

#include "slot_map_moon.hpp"
#include "empty_mutex.hpp"

namespace std {

#define basic_slot_map_invalid	(size_t)-1

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_slot_map;

namespace slot_internal {

template<typename T>
struct basic_slot {
	bool valid = false;
	alignas(alignof(T)) char obj[sizeof(T)];

	basic_slot() = default;
	basic_slot(basic_slot&& rhs) {
		valid = std::move(valid);
		if(valid)
			new ((T*)obj) T(*(T*)rhs.obj);

		rhs.valid = false;
	}
};

}

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_slot_map_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_slot_map_const_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_slot_map_reverse_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_slot_map_const_reverse_iterator;

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_slot_map_iterator {
private:
	basic_slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_slot_map_iterator(basic_slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_slot_map_const_iterator {
private:
	const basic_slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_slot_map_const_iterator(const basic_slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_slot_map_reverse_iterator {
private:
	basic_slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_slot_map_reverse_iterator(basic_slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_slot_map_const_reverse_iterator {
private:
	const basic_slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct basic_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_slot_map_const_reverse_iterator(const basic_slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_slot_map_handle {
private:
	slot_internal::slot_map_moon<Mut>* moon = 0;
	size_t idx = basic_slot_map_invalid;

	friend struct basic_slot_map<T, Mut, Alloc, MoonAlloc>;

	inline void clear() {
		moon = 0;
		idx = basic_slot_map_invalid;
	}

public:
	basic_slot_map_handle() = default;

	basic_slot_map_handle(const basic_slot_map_handle& rhs)
		: moon(0), idx(basic_slot_map_invalid) {
		if(rhs.moon && basic_slot_map<T, Mut, Alloc, MoonAlloc>::is_valid_external(const_cast<basic_slot_map_handle&>(rhs))) {
			moon = rhs.moon;
			idx = rhs.idx;

			if(moon && idx != basic_slot_map_invalid)
				basic_slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(*this);
		}
	}
	basic_slot_map_handle(basic_slot_map_handle&& rhs) {
		moon = rhs.moon;
		idx = rhs.idx;

		rhs.clear();
	}

	basic_slot_map_handle& operator=(const basic_slot_map_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_slot_map_handle();

		if(rhs.moon && basic_slot_map<T, Mut, Alloc, MoonAlloc>::is_valid_external(const_cast<basic_slot_map_handle&>(rhs))) {
			moon = rhs.moon;
			idx = rhs.idx;

			if(moon && idx != basic_slot_map_invalid)
				basic_slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(*this);
		}
		return *this;
	}
	basic_slot_map_handle& operator=(basic_slot_map_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_slot_map_handle();

		moon = rhs.moon;
		idx = rhs.idx;

		rhs.clear();
		return *this;
	}

	~basic_slot_map_handle() {
		if(moon && idx != basic_slot_map_invalid)
			basic_slot_map<T, Mut, Alloc, MoonAlloc>::decrement_handle_external(*this);
		clear();
	}

	inline T& operator*() {
		return *basic_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this);
	}
	inline T* operator->() {
		return basic_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*basic_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<basic_slot_map_handle&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(basic_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<basic_slot_map_handle&>(*this)));
	}

	inline operator T*() {
		return basic_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this);
	}
	inline operator const T*() const {
		return basic_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_slot_map {
private:
	struct slot_ref {
		size_t count = 0;
		size_t idx = basic_slot_map_invalid;
	};

	typedef typename slot_internal::slot_map_moon<Mut> MoonType;

	size_t itemcount = 0;
	size_t idxcount = 0;
	size_t nextitem = 0;
	size_t nextidx = 0;
	MoonType* moon = 0;
	std::vector<slot_internal::basic_slot<T>, Alloc> items;
	std::vector<slot_ref, Alloc> idxs;

	friend struct basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>;

	void initMoon() {
		MoonAlloc allctr;
		moon = allctr.allocate(1);
		new (moon) MoonType();
		moon->slot_map_ptr = this;
	}
	static void dtorMoon(MoonType* moon) {
		if(moon) {
			if(moon->count == 0) {
				moon->~MoonType();
				MoonAlloc allctr;
				allctr.deallocate(moon, 1);
			} else
				moon->slot_map_ptr = 0;
			moon = 0;
		}
	}
	void orphanMoon() {
		dtorMoon(moon);
		initMoon();
	}

	void reset(bool resetmoon, bool dtrMn) {
		//do some cleanup
		itemcount = 0;
		idxcount = 0;
		nextitem = 0;
		nextidx = 0;
		if(resetmoon)
			moon = 0;
		else {
			if(dtrMn)
				dtorMoon(moon);
			else
				orphanMoon();
		}
		items.clear();
		idxs.clear();
	}
public:
	basic_slot_map(size_t slots = 50) {
		initMoon();
		items.resize(slots);
		idxs.resize(slots);
	}
	basic_slot_map(const basic_slot_map& rhs) {
		initMoon();
		*this = rhs;
	}
	basic_slot_map(basic_slot_map&& rhs) {
		*this = std::move(rhs);
	}

	basic_slot_map& operator=(const basic_slot_map& rhs) {
		if(this == &rhs)
			return *this;

		reset(false, false);
		return *this;
	}
	basic_slot_map& operator=(basic_slot_map&& rhs) {
		if(this == &rhs)
			return *this;
		reset(false, true);

		itemcount = std::move(rhs.itemcount);
		idxcount = std::move(rhs.idxcount);
		nextitem = std::move(rhs.nextitem);
		nextidx = std::move(rhs.nextidx);
		moon = std::move(rhs.moon);
		items = std::move(rhs.items);
		idxs = std::move(rhs.idxs);

		rhs.reset(true, true);
		return *this;
	}

	template<typename A>
	basic_slot_map clone(std::vector<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>, A>& out) {
		//go through all of the values in this, insert them into the rtn result
		//return all of the handles to these values
		basic_slot_map rtn;
		for(auto it = begin(); it != end(); ++it)
			out.push_back(rtn.insert(*it));
		return rtn;
	}

	void lock() {
		moon->mut.lock();
	}
	void unlock() {
		moon->mut.unlock();
	}

	//same as normal vector
	typedef T value_type;
	typedef Alloc allocator_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef basic_slot_map_iterator<T, Mut, Alloc, MoonAlloc> iterator;
	typedef basic_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc> const_iterator;
	typedef basic_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc> reverse_iterator;
	typedef basic_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc> const_reverse_iterator;
	typedef basic_slot_map_handle<T, Mut, Alloc, MoonAlloc> handle;
private:
	void increment_handle(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>&>(hdl)) != 0)
			++idxs[hdl.idx].count;
	}
	void decrement_handle(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		//in the destructor of hdl
		if(get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>&>(hdl)) != 0) {
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
		const_cast<basic_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		size_type rtn = itemcount;
		const_cast<basic_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
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
		const_cast<basic_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		size_type rtn = items.capacity();
		const_cast<basic_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
		return rtn;
	}
	void reserve(size_type sz) {
		lock();
		if(sz < items.size()) {
			unlock();
			return;
		}
		items.resize(sz);
		idxs.resize(sz);
		unlock();
	}
	inline bool empty() const noexcept {
		const_cast<basic_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		bool rtn = size() == 0;
		const_cast<basic_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
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
	basic_slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(const T& val) {
		lock();
		size_t itemPos = 0;
		size_t idxPos = 0;
		get_next_free(itemPos, idxPos);

		basic_slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = idxPos;

		++moon->count;

		new (items[itemPos].obj) T(val);
		unlock();
		return rtn;
	}
	basic_slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(T&& val) {
		lock();
		size_t itemPos = 0;
		size_t idxPos = 0;
		get_next_free(itemPos, idxPos);

		basic_slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = idxPos;

		++moon->count;

		new (items[itemPos].obj) T(std::move(val));
		unlock();
		return rtn;
	}
	template<typename Itr>
	std::vector<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>> insert(Itr begin, Itr end) {
		lock();
		std::vector<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		unlock();
		return rtn;
	}

private:
	slot_internal::basic_slot<T>* get_object_internal(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
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
	void clear_internal() noexcept {
		for(auto it = items.begin(); it != items.end(); ++it)
			if(it->valid) {
				it->valid = false;
				((T*)it->obj)->~T();
			}
	}
public:

	inline bool is_valid(const basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return false;
		lock();
		bool rtn = get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>&>(hdl)) != 0;
		unlock();
		return rtn;
	}
	T* get_object(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return 0;
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
	const T* get_object(const basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return 0;
		lock();
		slot_internal::basic_slot<T>* obj = get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>&>(hdl));
		if(obj) {
			const T* rtn = (const T*)obj->obj;
			unlock();
			return rtn;
		}
		unlock();
		return 0;
	}

	void erase(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return;
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
		clear_internal();
		unlock();
	}
	void defragment() noexcept {
		lock();

		//if full don't defragment
		if(idxcount == idxs.size())
			return;

		//move object to dense positions in items
		size_t idxitem = 0;
		size_t idxidx = 0;
		auto it1 = items.begin();
		auto it2 = idxs.begin();
		for(; it1 != items.end() && it2 != idxs.end(); ++it1) {
			if(it1->valid == false) {
				//empty valid slot
				bool found = false;
				size_t pos = std::distance(items.begin(), it1);
				for(; it2 != idxs.end(); ++it2) {
					//can we move this
					if(it2->count > 0 && it2->idx > pos) {
						//do the move
						new (&*it1) slot_internal::basic_slot<T>(std::move(items[it2->idx]));

						//change the index to point to the correct position
						it2->idx = pos;

						//move to next
						++it2;
						found = true;
						break;
					}
				}
				if(!found)
					nextitem = pos;
			}
		}

		//find the first invalid for nextitem and nextidx
		for(size_t i = 0; i < idxs.size(); ++i)
			if(idxs[i].count == 0) {
				nextidx = i;
				break;
			}

		unlock();
	}
private:
	static basic_slot_map<T, Mut, Alloc, MoonAlloc>* getMap(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		//get the map and lock this
		if(hdl.moon == 0)
			return 0;
		//does this still point to a valid basic_slot_map?
		hdl.moon->mut.lock();
		if(hdl.moon->slot_map_ptr == 0) {
			--hdl.moon->count;
			if(hdl.moon->count == 0) {
				hdl.moon->mut.unlock();
				//do cleanup - object already removed remove lingering moon object
				//remove moon
				hdl.moon->~MoonType();
				MoonAlloc allctr;
				allctr.deallocate(hdl.moon, 1);
				hdl.clear();
				return 0;
			}
			hdl.moon->mut.unlock();
			hdl.clear();
			return 0;
		}
		return (basic_slot_map<T, Mut, Alloc, MoonAlloc>*)hdl.moon->slot_map_ptr;
	}
	static bool is_valid_external(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		basic_slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return false;
		bool rtn = map->get_object_internal(const_cast<basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>&>(hdl)) != 0;
		map->unlock();
		return rtn;
	}
	static void increment_handle_external(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		basic_slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return;
		++hdl.moon->count;
		map->increment_handle(hdl);
		map->unlock();
	}
	static void decrement_handle_external(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		basic_slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return;
		--hdl.moon->count;
		map->decrement_handle(hdl);
		map->unlock();
	}
	static T* get_object_external(basic_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		basic_slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return 0;
		slot_internal::basic_slot<T>* obj = map->get_object_internal(hdl);
		if(obj) {
			T* rtn = (T*)obj->obj;
			map->unlock();
			return rtn;
		}
		map->unlock();
		return 0;
	}

public:
	~basic_slot_map() {
		lock();
		clear_internal();
		dtorMoon(moon);
		unlock();
	}
};

}
