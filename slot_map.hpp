/*----------------------------------------------------------------------------------*\
 |																					|
 | slot_map.hpp 																	|
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
#include <string.h>

#include "slot_map_moon.hpp"
#include "empty_mutex.hpp"
#include "generation_data.hpp"

namespace std {

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct slot_map;

namespace slot_internal {

template<typename T>
struct slot {
	slot_internal::generation_data<uint32_t> gens;
	union slot_data {
		size_t next;								//used when object doesn't exist to reference the next object to allocate
		alignas(alignof(T)) char obj[sizeof(T)];
	} unn;
};

}

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct slot_map_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct slot_map_const_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct slot_map_reverse_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct slot_map_const_reverse_iterator;

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct slot_map_iterator {
private:
	slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::iterator itr;

	friend struct slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	slot_map_iterator(slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct slot_map_const_iterator {
private:
	const slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator itr;

	friend struct slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	slot_map_const_iterator(const slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct slot_map_reverse_iterator {
private:
	slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator itr;

	friend struct slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	slot_map_reverse_iterator(slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct slot_map_const_reverse_iterator {
private:
	const slot_map<T, Mut, Alloc, MoonAlloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	slot_map_const_reverse_iterator(const slot_map<T, Mut, Alloc, MoonAlloc>* mp,
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

	inline operator slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct slot_map_weak_handle;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct slot_map_handle;

namespace slot_internal {

template<typename Mut>
struct internal_slot_map_handle {
	slot_map_moon<Mut>* moon = 0;
	size_t idx = 0;
	size_t gen = 0;

	void clear() {
		moon = 0;
		idx = 0;
		gen = 0;
	}
};

}

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct slot_map_handle : slot_internal::internal_slot_map_handle<Mut> {
	slot_map_handle() = default;

	friend struct slot_map<T, Mut, Alloc, MoonAlloc>;

	slot_map_handle(const slot_map_handle& rhs) {
		new (this) slot_internal::internal_slot_map_handle<Mut>((const slot_internal::internal_slot_map_handle<Mut>&)rhs);
	}
	slot_map_handle(slot_map_handle&& rhs) {
		new (this) slot_internal::internal_slot_map_handle<Mut>(std::move((slot_internal::internal_slot_map_handle<Mut>&)rhs));
	}

	slot_map_handle(const slot_internal::internal_slot_map_handle<Mut>& rhs) {
		if(rhs.moon && slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(rhs), false))
			*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;
	}
	slot_map_handle(slot_internal::internal_slot_map_handle<Mut>&& rhs) {
		*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;
	}

	inline slot_map_handle& operator=(const slot_map_handle& rhs) {
		const slot_internal::internal_slot_map_handle<Mut>& tmp = rhs;
		return *this = const_cast<slot_internal::internal_slot_map_handle<Mut>&>(tmp);
	}
	inline slot_map_handle& operator=(slot_map_handle&& rhs) {
		slot_internal::internal_slot_map_handle<Mut>& tmp = rhs;
		return *this = std::move(tmp);
	}

	slot_map_handle& operator=(const slot_internal::internal_slot_map_handle<Mut>& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		if(rhs.moon && slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(rhs), false))
			*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;
		return *this;
	}
	slot_map_handle& operator=(slot_internal::internal_slot_map_handle<Mut>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;

		rhs.clear();
		return *this;
	}

	~slot_map_handle() {
		if(this->moon)
			slot_map<T, Mut, Alloc, MoonAlloc>::decrement_handle_external(*this, false);
		this->clear();
	}

	inline T& operator*() {
		return *slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}
	inline T* operator->() {
		return slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(*this), false));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(*this), false));
	}

	inline operator T*() {
		return slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}
	inline operator const T*() const {
		return slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct slot_map_weak_handle : slot_internal::internal_slot_map_handle<Mut> {
	slot_map_weak_handle() = default;

	friend struct slot_map<T, Mut, Alloc, MoonAlloc>;

	slot_map_weak_handle(const slot_map_weak_handle& rhs) {
		new (this) slot_internal::internal_slot_map_handle<Mut>((const slot_internal::internal_slot_map_handle<Mut>&)rhs);
	}
	slot_map_weak_handle(slot_map_weak_handle&& rhs) {
		new (this) slot_internal::internal_slot_map_handle<Mut>(std::move((slot_internal::internal_slot_map_handle<Mut>&)rhs));
	}

	slot_map_weak_handle(const slot_internal::internal_slot_map_handle<Mut>& rhs) {
		if(rhs.moon && slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(rhs), true))
			*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;
	}
	slot_map_weak_handle(slot_internal::internal_slot_map_handle<Mut>&& rhs) {
		*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;
	}

	inline slot_map_weak_handle& operator=(const slot_map_weak_handle& rhs) {
		const slot_internal::internal_slot_map_handle<Mut>& tmp = rhs;
		return *this = const_cast<slot_internal::internal_slot_map_handle<Mut>&>(tmp);
	}
	inline slot_map_weak_handle& operator=(slot_map_weak_handle&& rhs) {
		slot_internal::internal_slot_map_handle<Mut>& tmp = rhs;
		return *this = std::move(tmp);
	}

	slot_map_weak_handle& operator=(const slot_internal::internal_slot_map_handle<Mut>& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_weak_handle();

		if(rhs.moon && slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(rhs), true))
			*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;
		return *this;
	}
	slot_map_weak_handle& operator=(slot_internal::internal_slot_map_handle<Mut>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_weak_handle();

		*(slot_internal::internal_slot_map_handle<Mut>*)this = (slot_internal::internal_slot_map_handle<Mut>&)rhs;

		rhs.clear();
		return *this;
	}

	~slot_map_weak_handle() {
		if(this->moon)
			slot_map<T, Mut, Alloc, MoonAlloc>::decrement_handle_external(*this, true);
		this->clear();
	}

	inline T& operator*() {
		return *slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}
	inline T* operator->() {
		return slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(*this), true));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(*this), true));
	}

	inline operator T*() {
		return slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}
	inline operator const T*() const {
		return slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}
};


template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct slot_map {
private:
	typedef typename slot_internal::slot_map_moon<Mut> MoonType;

	size_t count = 0;
	MoonType* moon = 0;
	slot_internal::slot<T>* firstslot = 0;
	slot_internal::slot<T>* lastslot = 0;
	std::vector<slot_internal::slot<T>, Alloc> items;

	friend struct slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	friend struct slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>;
	friend struct slot_map_handle<T, Mut, Alloc, MoonAlloc>;

	void extend(size_t extnd) {
		if(extnd == 0)
			return;

		size_t csze = items.size();
		items.resize(csze + extnd);

		size_t nxt = 0;
		if(firstslot)
			nxt = std::distance(&items[0], firstslot);

		//append all of the new items onto the front of the slot list
		memset((void*)&items[csze], 0, sizeof(slot_internal::slot<T>) * extnd);
		for(size_t i = csze; i < csze + extnd; ++i)
			if(i == csze + extnd - 1)
				items[i].unn.next = nxt;
			else
				items[i].unn.next = i + 1;

		firstslot = &items[csze];
		if(nxt == 0)
			lastslot = &items[csze + extnd - 1];
	}

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
		if(!resetmoon)
			clear_internal();

		count = 0;
		if(resetmoon)
			moon = 0;
		else {
			if(dtrMn)
				dtorMoon(moon);
			else
				orphanMoon();
		}
		firstslot = 0;
		lastslot = 0;
		items.clear();

		if(!resetmoon)
			extend(10);
	}
public:
	slot_map(size_t slots = 50) {
		initMoon();
		extend(slots);
	}
	slot_map(const slot_map& rhs) {
		initMoon();
		*this = rhs;
	}
	slot_map(slot_map&& rhs) {
		*this = std::move(rhs);
	}

	slot_map& operator=(const slot_map& rhs) {
		if(this == &rhs)
			return *this;

		reset(false, false);
		return *this;
	}
	slot_map& operator=(slot_map&& rhs) {
		if(this == &rhs)
			return *this;
		reset(false, true);

		count = std::move(rhs.count);
		moon = std::move(rhs.moon);
		firstslot = std::move(rhs.firstslot);
		lastslot = std::move(rhs.lastslot);
		items = std::move(rhs.items);

		rhs.reset(true, true);
		return *this;
	}

	template<typename A>
	slot_map clone(std::vector<slot_map_handle<T, Mut, Alloc, MoonAlloc>, A>& out) {
		//go through all of the values in this, insert them into the rtn result
		//return all of the handles to these values
		slot_map rtn;
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
	typedef slot_map_iterator<T, Mut, Alloc, MoonAlloc> iterator;
	typedef slot_map_const_iterator<T, Mut, Alloc, MoonAlloc> const_iterator;
	typedef slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc> reverse_iterator;
	typedef slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc> const_reverse_iterator;
	typedef slot_map_handle<T, Mut, Alloc, MoonAlloc> handle;
	typedef slot_map_weak_handle<T, Mut, Alloc, MoonAlloc> weak_handle;
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
	bool increment_handle(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(hdl, weak);
		if(obj) {
			slot_internal::generation_data<uint32_t>::counts& tmp = obj->gens.get_generation_count(hdl.gen);
			if(weak)
				++tmp.weakcount;
			else
				++tmp.strongcount;
			return true;
		}
		return false;
	}
	void decrement_handle(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(hdl, weak);
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
	}
public:

	// iterators:
	inline iterator begin() noexcept {
		auto it = items.begin();
		for(; it != items.end() && !it->gens.is_valid(); ++it);
		return iterator(this, it);
	}
	inline const_iterator begin() const noexcept {
		auto it = items.begin();
		for(; it != items.end() && !it->gens.is_valid(); ++it);
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
		for(; it != items.rend() && !it->gens.is_valid(); ++it);
		return reverse_iterator(this, it);
	}
	inline const_reverse_iterator rbegin() const noexcept {
		auto it = items.rbegin();
		for(; it != items.rend() && !it->gens.is_valid(); ++it);
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
		for(; it != items.cend() && !it->gens.is_valid(); ++it);
		return const_iterator(this, it);
	}
	inline const_iterator cend() const noexcept {
		return const_iterator(this, items.cend());
	}
	inline const_reverse_iterator crbegin() const noexcept {
		auto it = items.crbegin();
		for(; it != items.crend() && !it->gens.is_valid(); ++it);
		return const_reverse_iterator(this, it);
	}
	inline const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(this, items.crend());
	}

	// capacity:
	inline size_type size() const noexcept {
		const_cast<slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		size_type rtn = count;
		const_cast<slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
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
		extend(sz - items.size());
		unlock();
	}
	inline size_type capacity() const noexcept {
		const_cast<slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		size_type rtn = items.capacity();
		const_cast<slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
		return rtn;
	}
	void reserve(size_type n) {
		lock();
		items.reserve(n);
		unlock();
	}
	inline bool empty() const noexcept {
		const_cast<slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		bool rtn = size() == 0;
		const_cast<slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
		return rtn;
	}
	inline void shrink_to_fit() {
		lock();
		items.shrink_to_fit();
		unlock();
	}

private:
	size_t get_next_free() {
		if(count == items.size())
			//double the size
			extend(items.size());

		size_t pos = std::distance(&items[0], firstslot);

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
	slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(const T& val) {
		lock();
		size_t itemPos = get_next_free();

		slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = itemPos;
		rtn.gen = items[itemPos].gens.new_generation();
		++moon->count;

		new (items[itemPos].unn.obj) T(val);
		unlock();
		return rtn;
	}
	slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(T&& val) {
		lock();
		size_t itemPos = get_next_free();

		slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = itemPos;
		rtn.gen = items[itemPos].gens.new_generation();
		++moon->count;

		new (items[itemPos].unn.obj) T(std::move(val));
		unlock();
		return rtn;
	}
	template<typename Itr>
	std::vector<slot_map_handle<T, Mut, Alloc, MoonAlloc>> insert(Itr begin, Itr end) {
		lock();
		std::vector<slot_map_handle<T, Mut, Alloc, MoonAlloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		unlock();
		return rtn;
	}

private:
	slot_internal::slot<T>* get_object_internal(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_internal::slot<T>& rf = items[hdl.idx];
		//test that the generation matches
		if(!rf.gens.is_valid() || !rf.gens.match_generation(hdl.gen, weak)) {
			rf.gens.decrement_generation(hdl.gen, weak);
			hdl.clear();
			return 0;
		}
		return &rf;
	}

	inline bool is_valid(const slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		return get_object_internal(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(hdl), weak) != 0;
	}
	T* get_object(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(hdl, weak);
		if(obj)
			return (T*)obj->unn.obj;
		return 0;
	}
	const T* get_object(const slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(const_cast<slot_internal::internal_slot_map_handle<Mut>&>(hdl), weak);
		if(obj)
			return (const T*)obj->unn.obj;
		return 0;
	}

	void erase(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(hdl, weak);
		if(obj)
			destruct_object(obj);
	}
public:

	inline bool is_valid(const slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		bool rtn = is_valid(hdl, false);
		unlock();
		return rtn;
	}
	inline bool is_valid(const slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		bool rtn = is_valid(hdl, true);
		unlock();
		return rtn;
	}
	inline T* get_object(slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline T* get_object(slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}
	inline const T* get_object(const slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		const T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline const T* get_object(const slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		const T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}

	inline void erase(slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		erase(hdl, false);
		unlock();
	}
	inline void erase(slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		lock();
		erase(hdl, true);
		unlock();
	}

	void clear() noexcept {
		lock();
		//just clear the data, erase everything
		size_t i = 0;
		for(auto it = items.begin(); it != items.end(); ++it, ++i) {
			if(it->gens.is_valid()) {
				slot_map_weak_handle<T, Mut, Alloc, MoonAlloc> hdl;
				hdl.moon = moon;
				hdl.idx = std::distance(items.begin(), it);
				hdl.gen = it->gens.increment_generation(true);
				++moon->count;

				erase(hdl);
			}
			if(i == items.size())
				items[i].unn.next = 0;
			else
				items[i].unn.next = i + 1;
		}

		firstslot = &items[0];
		lastslot = &items[items.size() - 1];
		unlock();
	}
	void defragment() noexcept {
		lock();
		//order the allocations
		bool set = false;
		size_t last = 0;

		firstslot = 0;
		lastslot = 0;

		for(size_t i = 0; i < items.size(); ++i)
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
		unlock();
	}
private:
	static slot_map<T, Mut, Alloc, MoonAlloc>* getMap(slot_internal::internal_slot_map_handle<Mut>& hdl) {
		//get the map and lock this
		if(hdl.moon == 0)
			return 0;
		//does this still point to a valid slot_map?
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
		return (slot_map<T, Mut, Alloc, MoonAlloc>*)hdl.moon->slot_map_ptr;
	}
	static bool increment_handle_external(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return false;
		++hdl.moon->count;
		bool rtn = map->increment_handle(hdl, weak);
		map->unlock();
		return rtn;
	}
	static void decrement_handle_external(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return;
		--hdl.moon->count;
		map->decrement_handle(hdl, weak);
		map->unlock();
	}
	static T* get_object_external(slot_internal::internal_slot_map_handle<Mut>& hdl, bool weak) {
		slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return 0;
		lock();
		const T* rtn = map->get_object(hdl, weak);
		unlock();
		return rtn;
	}

	void clear_internal() {
		//erase everything
		for(auto it = items.begin(); it != items.end(); ++it)
			if(it->gens.is_valid())
				((T*)it->unn.obj)->~T();
	}
public:
	~slot_map() {
		lock();
		dtorMoon(moon);
		clear_internal();
		unlock();
	}
};

}

