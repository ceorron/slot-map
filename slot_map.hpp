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

#include "empty_mutex.hpp"
#include "generation_data.hpp"

namespace std {

template<typename T, typename Mut, typename Alloc>
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

template<typename T, typename Mut, typename Alloc>
struct slot_map_iterator;
template<typename T, typename Mut, typename Alloc>
struct slot_map_const_iterator;
template<typename T, typename Mut, typename Alloc>
struct slot_map_reverse_iterator;
template<typename T, typename Mut, typename Alloc>
struct slot_map_const_reverse_iterator;

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct slot_map_iterator {
private:
	slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::iterator itr;

	friend struct slot_map<T, Mut, Alloc>;

	friend struct slot_map_const_iterator<T, Mut, Alloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc>;

	slot_map_iterator(slot_map<T, Mut, Alloc>* mp,
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

	inline operator slot_map_const_iterator<T, Mut, Alloc>() const {
		return slot_map_const_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return slot_map_reverse_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct slot_map_const_iterator {
private:
	const slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator itr;

	friend struct slot_map<T, Mut, Alloc>;

	friend struct slot_map_iterator<T, Mut, Alloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc>;

	slot_map_const_iterator(const slot_map<T, Mut, Alloc>* mp,
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

	inline operator slot_map_iterator<T, Mut, Alloc>() const {
		return slot_map_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return slot_map_reverse_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct slot_map_reverse_iterator {
private:
	slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator itr;

	friend struct slot_map<T, Mut, Alloc>;

	friend struct slot_map_iterator<T, Mut, Alloc>;
	friend struct slot_map_const_iterator<T, Mut, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc>;

	slot_map_reverse_iterator(slot_map<T, Mut, Alloc>* mp,
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

	inline operator slot_map_iterator<T, Mut, Alloc>() const {
		return slot_map_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Mut, Alloc>() const {
		return slot_map_const_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_const_reverse_iterator<T, Mut, Alloc>() const {
		return slot_map_const_reverse_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>>
struct slot_map_const_reverse_iterator {
private:
	const slot_map<T, Mut, Alloc>* map;
	typename std::vector<slot_internal::slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct slot_map<T, Mut, Alloc>;

	friend struct slot_map_iterator<T, Mut, Alloc>;
	friend struct slot_map_const_iterator<T, Mut, Alloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc>;

	slot_map_const_reverse_iterator(const slot_map<T, Mut, Alloc>* mp,
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

	inline operator slot_map_iterator<T, Mut, Alloc>() const {
		return slot_map_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::iterator(itr));
	}
	inline operator slot_map_const_iterator<T, Mut, Alloc>() const {
		return slot_map_const_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator slot_map_reverse_iterator<T, Mut, Alloc>() const {
		return slot_map_reverse_iterator<T, Mut, Alloc>(map, typename std::vector<slot_internal::slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T, typename Mut, typename Alloc>
struct slot_map_weak_handle;
template<typename T, typename Mut, typename Alloc>
struct slot_map_handle;

namespace slot_internal {

template<typename T, typename Mut, typename Alloc>
struct internal_slot_map_handle {
	slot_map<T, Mut, Alloc>* map = 0;
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
struct slot_map_handle : slot_internal::internal_slot_map_handle<T, Mut, Alloc> {
	slot_map_handle() = default;

	friend struct slot_map<T, Mut, Alloc>;

	slot_map_handle(const slot_map_handle& rhs) {
		const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(tmp), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	slot_map_handle(slot_map_handle&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	slot_map_handle(const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(rhs), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	slot_map_handle(slot_internal::internal_slot_map_handle<T, Mut, Alloc>&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	slot_map_handle& operator=(const slot_map_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(tmp), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	slot_map_handle& operator=(slot_map_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	slot_map_handle& operator=(const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(rhs), false))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	slot_map_handle& operator=(slot_internal::internal_slot_map_handle<T, Mut, Alloc>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	~slot_map_handle() {
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
		return const_cast<const T&>(*this->map->get_object(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(this->map->get_object(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(*this)));
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
struct slot_map_weak_handle : slot_internal::internal_slot_map_handle<T, Mut, Alloc> {
	slot_map_weak_handle() = default;

	friend struct slot_map<T, Mut, Alloc>;

	slot_map_weak_handle(const slot_map_weak_handle& rhs) {
		const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(tmp), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	slot_map_weak_handle(slot_map_weak_handle&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	slot_map_weak_handle(const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(rhs), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
	}
	slot_map_weak_handle(slot_internal::internal_slot_map_handle<T, Mut, Alloc>&& rhs) {
		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
	}

	slot_map_weak_handle& operator=(const slot_map_weak_handle& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_weak_handle();

		const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& tmp = rhs;
		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(tmp), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	slot_map_weak_handle& operator=(slot_map_weak_handle&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_weak_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	slot_map_weak_handle& operator=(const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_weak_handle();

		if(rhs.map && rhs.map->increment_handle(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(rhs), true))
			*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;
		return *this;
	}
	slot_map_weak_handle& operator=(slot_internal::internal_slot_map_handle<T, Mut, Alloc>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~slot_map_weak_handle();

		*(slot_internal::internal_slot_map_handle<T, Mut, Alloc>*)this = (slot_internal::internal_slot_map_handle<T, Mut, Alloc>&)rhs;

		rhs.clear();
		return *this;
	}

	~slot_map_weak_handle() {
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
		return const_cast<const T&>(*this->map->get_object(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(*this)));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(this->map->get_object(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(*this)));
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
struct slot_map {
private:
	size_t count = 0;
	Mut mut;
	slot_internal::slot<T>* firstslot = 0;
	slot_internal::slot<T>* lastslot = 0;
	std::vector<slot_internal::slot<T>, Alloc> items;

	friend struct slot_map_iterator<T, Mut, Alloc>;
	friend struct slot_map_const_iterator<T, Mut, Alloc>;
	friend struct slot_map_reverse_iterator<T, Mut, Alloc>;
	friend struct slot_map_const_reverse_iterator<T, Mut, Alloc>;

	friend struct slot_map_weak_handle<T, Mut, Alloc>;
	friend struct slot_map_handle<T, Mut, Alloc>;

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

public:
	slot_map(size_t slots = 50) {
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
	typedef slot_map_iterator<T, Mut, Alloc> iterator;
	typedef slot_map_const_iterator<T, Mut, Alloc> const_iterator;
	typedef slot_map_reverse_iterator<T, Mut, Alloc> reverse_iterator;
	typedef slot_map_const_reverse_iterator<T, Mut, Alloc> const_reverse_iterator;
	typedef slot_map_handle<T, Mut, Alloc> handle;
	typedef slot_map_weak_handle<T, Mut, Alloc> weak_handle;

	void lock() {
		mut.lock();
	}
	void unlock() {
		mut.unlock();
	}
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
	bool increment_handle(slot_internal::internal_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		lock();
		slot_internal::slot<T>* obj = get_object_internal(hdl, weak);
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
	void decrement_handle(slot_internal::internal_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		lock();
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
		unlock();
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
		if(sz < items.size()) {
			unlock();
			return;
		}
		extend(sz - items.size());
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
		items.reserve(n);
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
	slot_map_handle<T, Mut, Alloc> insert(const T& val) {
		lock();
		size_t itemPos = get_next_free();

		slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = items[itemPos].gens.new_generation();

		new (items[itemPos].unn.obj) T(val);
		unlock();
		return rtn;
	}
	slot_map_handle<T, Mut, Alloc> insert(T&& val) {
		lock();
		size_t itemPos = get_next_free();

		slot_map_handle<T, Mut, Alloc> rtn;
		rtn.map = this;
		rtn.idx = itemPos;
		rtn.gen = items[itemPos].gens.new_generation();

		new (items[itemPos].unn.obj) T(std::move(val));
		unlock();
		return rtn;
	}
	template<typename Itr>
	std::vector<slot_map_handle<T, Mut, Alloc>> insert(Itr begin, Itr end) {
		lock();
		std::vector<slot_map_handle<T, Mut, Alloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		unlock();
		return rtn;
	}

private:
	slot_internal::slot<T>* get_object_internal(slot_internal::internal_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		if(hdl.map == 0)
			return 0;
		slot_internal::slot<T>& rf = items[hdl.idx];
		//test that the generation matches
		if(!rf.gens.is_valid() || !rf.gens.match_generation(hdl.gen, weak)) {
			rf.gens.decrement_generation(hdl.gen, weak);
			hdl.clear();
			return 0;
		}
		return &rf;
	}

	inline bool is_valid(const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		return get_object_internal(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(hdl), weak) != 0;
	}
	T* get_object(slot_internal::internal_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(hdl, weak);
		if(obj)
			return (T*)obj->unn.obj;
		return 0;
	}
	const T* get_object(const slot_internal::internal_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(const_cast<slot_internal::internal_slot_map_handle<T, Mut, Alloc>&>(hdl), weak);
		if(obj)
			return (const T*)obj->unn.obj;
		return 0;
	}

	void erase(slot_internal::internal_slot_map_handle<T, Mut, Alloc>& hdl, bool weak) {
		slot_internal::slot<T>* obj = get_object_internal(hdl, weak);
		if(obj)
			destruct_object(obj);
	}
public:

	inline bool is_valid(const slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		bool rtn = is_valid(hdl, false);
		unlock();
		return rtn;
	}
	inline bool is_valid(const slot_map_weak_handle<T, Mut, Alloc>& hdl) {
		lock();
		bool rtn = is_valid(hdl, true);
		unlock();
		return rtn;
	}
	inline T* get_object(slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline T* get_object(slot_map_weak_handle<T, Mut, Alloc>& hdl) {
		lock();
		T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}
	inline const T* get_object(const slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		const T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline const T* get_object(const slot_map_weak_handle<T, Mut, Alloc>& hdl) {
		lock();
		const T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}

	inline void erase(slot_map_handle<T, Mut, Alloc>& hdl) {
		lock();
		erase(hdl, false);
		unlock();
	}
	inline void erase(slot_map_weak_handle<T, Mut, Alloc>& hdl) {
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
				slot_map_weak_handle<T, Mut, Alloc> hdl;
				hdl.map = this;
				hdl.idx = std::distance(items.begin(), it);
				hdl.gen = it->gens.increment_generation(true);

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
	void defrag() noexcept {
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

	~slot_map() {
		lock();
		//erase everything
		for(auto it = items.begin(); it != items.end(); ++it)
			if(it->gens.is_valid())
				((T*)it->unn.obj)->~T();
		unlock();
	}
};

}

