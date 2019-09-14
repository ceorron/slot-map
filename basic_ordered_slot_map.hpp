/*----------------------------------------------------------------------------------*\
 |																					|
 | basic_ordered_slot_map.hpp 														|
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

#include "slot_map_algorithm.hpp"
#include "slot_map_moon.hpp"
#include "empty_mutex.hpp"
#include "generation_data.hpp"

namespace std {

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_ordered_slot_map;

namespace slot_internal {

template<typename T>
struct basic_ordered_slot {
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

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_ordered_slot_map_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_ordered_slot_map_const_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_ordered_slot_map_reverse_iterator;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_ordered_slot_map_const_reverse_iterator;

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_ordered_slot_map_iterator {
private:
	typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator itr;

	friend struct basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_ordered_slot_map_iterator(const typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator& it)
		: itr(it)
	{}
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;

	basic_ordered_slot_map_iterator() = default;
	inline T& operator*() {
		return itr->obj;
	}
	inline T* operator->() {
		return &itr->obj;
	}
	inline basic_ordered_slot_map_iterator& operator++() {
		++itr;
		return *this;
	}
	inline basic_ordered_slot_map_iterator operator++(int) {
		basic_ordered_slot_map_iterator it(*this);
		++*this;
		return it;
	}
	inline basic_ordered_slot_map_iterator& operator--() {
		--itr;
		return *this;
	}
	inline basic_ordered_slot_map_iterator operator--(int) {
		basic_ordered_slot_map_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_ordered_slot_map_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_ordered_slot_map_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_ordered_slot_map_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_ordered_slot_map_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_ordered_slot_map_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_ordered_slot_map_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_ordered_slot_map_const_iterator {
private:
	typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_iterator itr;

	friend struct basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_ordered_slot_map_const_iterator(const typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_iterator& it)
		: itr(it)
	{}
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;

	basic_ordered_slot_map_const_iterator() = default;
	inline const T& operator*() {
		return itr->obj;
	}
	inline const T* operator->() {
		return &itr->obj;
	}
	inline basic_ordered_slot_map_const_iterator& operator++() {
		++itr;
		return *this;
	}
	inline basic_ordered_slot_map_const_iterator operator++(int) {
		basic_ordered_slot_map_const_iterator it(*this);
		++*this;
		return it;
	}
	inline basic_ordered_slot_map_const_iterator& operator--() {
		--itr;
		return *this;
	}
	inline basic_ordered_slot_map_const_iterator operator--(int) {
		basic_ordered_slot_map_const_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_ordered_slot_map_const_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_ordered_slot_map_const_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_ordered_slot_map_const_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_ordered_slot_map_const_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_ordered_slot_map_const_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_ordered_slot_map_const_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
	inline operator basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_ordered_slot_map_reverse_iterator {
private:
	typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::reverse_iterator itr;

	friend struct basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_ordered_slot_map_reverse_iterator(const typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::reverse_iterator& it)
		: itr(it)
	{}
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;

	basic_ordered_slot_map_reverse_iterator() = default;
	inline T& operator*() {
		return itr->obj;
	}
	inline T* operator->() {
		return &itr->obj;
	}
	inline basic_ordered_slot_map_reverse_iterator& operator++() {
		++itr;
		return *this;
	}
	inline basic_ordered_slot_map_reverse_iterator operator++(int) {
		basic_ordered_slot_map_reverse_iterator it(*this);
		++*this;
		return it;
	}
	inline basic_ordered_slot_map_reverse_iterator& operator--() {
		--itr;
		return *this;
	}
	inline basic_ordered_slot_map_reverse_iterator operator--(int) {
		basic_ordered_slot_map_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_ordered_slot_map_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_ordered_slot_map_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_ordered_slot_map_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_ordered_slot_map_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_ordered_slot_map_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_ordered_slot_map_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_reverse_iterator(itr));
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_ordered_slot_map_const_reverse_iterator {
private:
	typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_reverse_iterator itr;

	friend struct basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	basic_ordered_slot_map_const_reverse_iterator(const typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_reverse_iterator& it)
		: itr(it)
	{}
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* pointer;
	typedef T const* const_pointer;

	basic_ordered_slot_map_const_reverse_iterator() = default;
	inline const T& operator*() {
		return itr->obj;
	}
	inline const T* operator->() {
		return &itr->obj;
	}
	inline basic_ordered_slot_map_const_reverse_iterator& operator++() {
		++itr;
		return *this;
	}
	inline basic_ordered_slot_map_const_reverse_iterator operator++(int) {
		basic_ordered_slot_map_const_reverse_iterator it(*this);
		++*this;
		return it;
	}
	inline basic_ordered_slot_map_const_reverse_iterator& operator--() {
		--itr;
		return *this;
	}
	inline basic_ordered_slot_map_const_reverse_iterator operator--(int) {
		basic_ordered_slot_map_const_reverse_iterator it(*this);
		--*this;
		return it;
	}
	inline bool operator==(const basic_ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr == rhs.itr;
	}
	inline bool operator!=(const basic_ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr != rhs.itr;
	}
	inline bool operator<(const basic_ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr < rhs.itr;
	}
	inline bool operator>(const basic_ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr > rhs.itr;
	}
	inline bool operator<=(const basic_ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr <= rhs.itr;
	}
	inline bool operator>=(const basic_ordered_slot_map_const_reverse_iterator& rhs) const {
		return itr >= rhs.itr;
	}

	inline operator basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator(itr));
	}
	inline operator basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::const_iterator(itr));
	}
	inline operator basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>() const {
		return basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>(typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::reverse_iterator(itr));
	}
};

template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_ordered_slot_map_weak_handle;
template<typename T, typename Mut, typename Alloc, typename MoonAlloc>
struct basic_ordered_slot_map_handle;

namespace slot_internal {

template<typename Mut>
struct internal_basic_ordered_slot_map_handle {
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
struct basic_ordered_slot_map_handle : slot_internal::internal_basic_ordered_slot_map_handle<Mut> {
	basic_ordered_slot_map_handle() = default;

	friend struct basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>;

	basic_ordered_slot_map_handle(const basic_ordered_slot_map_handle& rhs) {
		new (this) slot_internal::internal_basic_ordered_slot_map_handle<Mut>((const slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs);
	}
	basic_ordered_slot_map_handle(basic_ordered_slot_map_handle&& rhs) {
		new (this) slot_internal::internal_basic_ordered_slot_map_handle<Mut>(std::move((slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs));
	}

	basic_ordered_slot_map_handle(const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& rhs) {
		if(rhs.moon && basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(rhs), false))
			*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;
	}
	basic_ordered_slot_map_handle(slot_internal::internal_basic_ordered_slot_map_handle<Mut>&& rhs) {
		*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;
	}

	inline basic_ordered_slot_map_handle& operator=(const basic_ordered_slot_map_handle& rhs) {
		const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& tmp = rhs;
		return *this = const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(tmp);
	}
	inline basic_ordered_slot_map_handle& operator=(basic_ordered_slot_map_handle&& rhs) {
		slot_internal::internal_basic_ordered_slot_map_handle<Mut>& tmp = rhs;
		return *this = std::move(tmp);
	}

	basic_ordered_slot_map_handle& operator=(const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_ordered_slot_map_handle();

		if(rhs.moon && basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(rhs), false))
			*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;
		return *this;
	}
	basic_ordered_slot_map_handle& operator=(slot_internal::internal_basic_ordered_slot_map_handle<Mut>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_ordered_slot_map_handle();

		*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;

		rhs.clear();
		return *this;
	}

	~basic_ordered_slot_map_handle() {
		if(this->moon)
			basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::decrement_handle_external(*this, false);
		this->clear();
	}

	inline T& operator*() {
		return *basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}
	inline T* operator->() {
		return basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(*this), false));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(*this), false));
	}

	inline operator T*() {
		return basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}
	inline operator const T*() const {
		return basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, false);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_ordered_slot_map_weak_handle : slot_internal::internal_basic_ordered_slot_map_handle<Mut> {
	basic_ordered_slot_map_weak_handle() = default;

	friend struct basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>;

	basic_ordered_slot_map_weak_handle(const basic_ordered_slot_map_weak_handle& rhs) {
		new (this) slot_internal::internal_basic_ordered_slot_map_handle<Mut>((const slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs);
	}
	basic_ordered_slot_map_weak_handle(basic_ordered_slot_map_weak_handle&& rhs) {
		new (this) slot_internal::internal_basic_ordered_slot_map_handle<Mut>(std::move((slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs));
	}

	basic_ordered_slot_map_weak_handle(const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& rhs) {
		if(rhs.moon && basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(rhs), true))
			*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;
	}
	basic_ordered_slot_map_weak_handle(slot_internal::internal_basic_ordered_slot_map_handle<Mut>&& rhs) {
		*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;
	}

	inline basic_ordered_slot_map_weak_handle& operator=(const basic_ordered_slot_map_weak_handle& rhs) {
		const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& tmp = rhs;
		return *this = const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(tmp);
	}
	inline basic_ordered_slot_map_weak_handle& operator=(basic_ordered_slot_map_weak_handle&& rhs) {
		slot_internal::internal_basic_ordered_slot_map_handle<Mut>& tmp = rhs;
		return *this = std::move(tmp);
	}

	basic_ordered_slot_map_weak_handle& operator=(const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_ordered_slot_map_weak_handle();

		if(rhs.moon && basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::increment_handle_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(rhs), true))
			*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;
		return *this;
	}
	basic_ordered_slot_map_weak_handle& operator=(slot_internal::internal_basic_ordered_slot_map_handle<Mut>&& rhs) {
		if(this == &rhs)
			return *this;

		this->~basic_ordered_slot_map_weak_handle();

		*(slot_internal::internal_basic_ordered_slot_map_handle<Mut>*)this = (slot_internal::internal_basic_ordered_slot_map_handle<Mut>&)rhs;

		rhs.clear();
		return *this;
	}

	~basic_ordered_slot_map_weak_handle() {
		if(this->moon)
			basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::decrement_handle_external(*this, true);
		this->clear();
	}

	inline T& operator*() {
		return *basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}
	inline T* operator->() {
		return basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}

	inline const T& operator*() const {
		return const_cast<const T&>(*basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(*this), true));
	}
	inline const T* operator->() const {
		return const_cast<const T*>(basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(*this), true));
	}

	inline operator T*() {
		return basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}
	inline operator const T*() const {
		return basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>::get_object_external(*this, true);
	}
};


template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = std::allocator<T>,
		 typename MoonAlloc = std::allocator<slot_internal::slot_map_moon<Mut>>>
struct basic_ordered_slot_map {
private:
	struct slot_index {
		slot_internal::generation_data<uint32_t> gens;
		union slot_data {
			size_t next;								//used when object doesn't exist to reference the next object to allocate
			size_t idx;									//index into items
		} unn;
	};

	typedef typename slot_internal::slot_map_moon<Mut> MoonType;

	size_t count = 0;
	MoonType* moon = 0;
	slot_index* firstslot = 0;
	slot_index* lastslot = 0;
	std::vector<slot_internal::basic_ordered_slot<T>, Alloc> items;
	std::vector<slot_index, Alloc> indexes;

	friend struct basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc>;

	friend struct basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>;
	friend struct basic_ordered_slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>;

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
		memset((void*)&indexes[csze], 0, sizeof(slot_index) * extnd);
		for(size_t i = csze; i < csze + extnd; ++i)
			if(i == csze + extnd - 1)
				indexes[i].unn.next = nxt;
			else
				indexes[i].unn.next = i + 1;

		firstslot = &indexes[csze];
		if(nxt == 0)
			lastslot = &indexes[csze + extnd - 1];
	}

	void initMoon() {
		MoonAlloc allctr;
		moon = allctr.allocate(1);
		new (moon) MoonType();
		moon->slot_map_ptr = this;
	}
	void dtorMoon() {
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
		dtorMoon();
		initMoon();
	}

	void reset(bool resetmoon, bool dtrMn) {
		//do some cleanup
		count = 0;
		if(resetmoon)
			moon = 0;
		else {
			if(dtrMn)
				dtorMoon();
			else
				orphanMoon();
		}
		firstslot = 0;
		lastslot = 0;
		items.clear();
		indexes.clear();

		if(!resetmoon)
			extend(10);
	}
public:
	basic_ordered_slot_map(size_t slots = 50) {
		initMoon();
		extend(slots);
	}
	basic_ordered_slot_map(const basic_ordered_slot_map& rhs) {
		initMoon();
		*this = rhs;
	}
	basic_ordered_slot_map(basic_ordered_slot_map&& rhs) {
		*this = std::move(rhs);
	}

	basic_ordered_slot_map& operator=(const basic_ordered_slot_map& rhs) {
		if(this == &rhs)
			return *this;

		reset(false, false);
		return *this;
	}
	basic_ordered_slot_map& operator=(basic_ordered_slot_map&& rhs) {
		if(this == &rhs)
			return *this;
		reset(false, true);

		count = std::move(rhs.count);
		moon = std::move(rhs.moon);
		firstslot = std::move(rhs.firstslot);
		lastslot = std::move(rhs.lastslot);
		items = std::move(rhs.items);
		indexes = std::move(rhs.indexes);

		rhs.reset(true, true);
		return *this;
	}

	template<typename A>
	basic_ordered_slot_map clone(std::vector<basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>, A>& out) {
		//go through all of the values in this, insert them into the rtn result
		//return all of the handles to these values
		basic_ordered_slot_map rtn;
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
	typedef basic_ordered_slot_map_iterator<T, Mut, Alloc, MoonAlloc> iterator;
	typedef basic_ordered_slot_map_const_iterator<T, Mut, Alloc, MoonAlloc> const_iterator;
	typedef basic_ordered_slot_map_reverse_iterator<T, Mut, Alloc, MoonAlloc> reverse_iterator;
	typedef basic_ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, MoonAlloc> const_reverse_iterator;
	typedef basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> handle;
	typedef basic_ordered_slot_map_weak_handle<T, Mut, Alloc, MoonAlloc> weak_handle;
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
	bool increment_handle(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		slot_index* obj = get_object_internal(hdl, weak);
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
	void decrement_handle(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
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
		const_cast<basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		size_type rtn = count;
		const_cast<basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
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
		const_cast<basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->lock();
		size_type rtn = items.capacity();
		const_cast<basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>*>(this)->unlock();
		return rtn;
	}
	void reserve(size_type n) {
		lock();
		items.reserve(n);
		indexes.reserve(n);
		unlock();
	}
	inline bool empty() const noexcept {
		return size() == 0;
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
		typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
			[](const T& lhs, const T& rhs){
				return lhs < rhs;
			}, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::basic_ordered_slot<T> itm;
		itm.obj = val;
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	template<typename Less>
	size_t get_insert_pos(const T& val, Less comp) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
									 comp, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::basic_ordered_slot<T> itm;
		itm.obj = val;
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	size_t get_insert_pos(T&& val) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
			[](const T& lhs, const T& rhs){
				return lhs < rhs;
			}, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::basic_ordered_slot<T> itm;
		itm.obj = std::move(val);
		items.insert(out, std::move(itm));

		//change all of the object indexes for move from insert
		update_object_indexes(pos);
		return pos;
	}
	template<typename Less>
	size_t get_insert_pos(T&& val, Less comp) {
		//search and insert this, returning the position
		typename std::vector<slot_internal::basic_ordered_slot<T>, Alloc>::iterator out;
		slot_internal::binary_search(items.begin(), items.end(), (const T&)val,
									 comp, out);

		size_t pos = std::distance(items.begin(), out);
		slot_internal::basic_ordered_slot<T> itm;
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
	basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(const T& val) {
		lock();
		size_t idx = get_insert_pos(val);
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		++moon->count;

		unlock();
		return rtn;
	}
	template<typename Less>
	basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(const T& val, Less comp) {
		lock();
		size_t idx = get_insert_pos(val, comp);
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		++moon->count;

		unlock();
		return rtn;
	}
	basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(T&& val) {
		lock();
		size_t idx = get_insert_pos(std::move(val));
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		++moon->count;

		unlock();
		return rtn;
	}
	template<typename Less>
	basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> insert(T&& val, Less comp) {
		lock();
		size_t idx = get_insert_pos(std::move(val), comp);
		size_t itemPos = get_next_free(idx);

		items[idx].backidx = itemPos;

		basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc> rtn;
		rtn.moon = moon;
		rtn.idx = itemPos;
		rtn.gen = indexes[itemPos].gens.new_generation();
		++moon->count;

		unlock();
		return rtn;
	}
	template<typename Itr>
	std::vector<basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>> insert(Itr begin, Itr end) {
		lock();
		std::vector<basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin));
		unlock();
		return rtn;
	}
	template<typename Itr, typename Less>
	std::vector<basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>> insert(Itr begin, Itr end, Less comp) {
		lock();
		std::vector<basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert(*begin, comp));
		unlock();
		return rtn;
	}

private:
	slot_index* get_object_internal(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		slot_index& rf = indexes[hdl.idx];
		//test that the generation matches
		if(!rf.gens.is_valid() || !rf.gens.match_generation(hdl.gen, weak)) {
			rf.gens.decrement_generation(hdl.gen, weak);
			hdl.clear();
			return 0;
		}
		return &rf;
	}

	inline bool is_valid(const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		return get_object_internal(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(hdl), weak) != 0;
	}
	T* get_object(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		slot_index* obj = get_object_internal(hdl, weak);
		if(obj)
			return &items[obj->unn.idx].obj;
		return 0;
	}
	const T* get_object(const slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		slot_index* obj = get_object_internal(const_cast<slot_internal::internal_basic_ordered_slot_map_handle<Mut>&>(hdl), weak);
		if(obj)
			return &items[obj->unn.idx].obj;
		return 0;
	}

	void erase(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		slot_index* obj = get_object_internal(hdl, weak);
		if(obj)
			destruct_object(obj);
	}
public:

	inline bool is_valid(const basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return false;
		lock();
		bool rtn = is_valid(hdl, false);
		unlock();
		return rtn;
	}
	inline bool is_valid(const basic_ordered_slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return false;
		lock();
		bool rtn = is_valid(hdl, true);
		unlock();
		return rtn;
	}
	inline T* get_object(basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return 0;
		lock();
		T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline T* get_object(basic_ordered_slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return 0;
		lock();
		T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}
	inline const T* get_object(const basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return 0;
		lock();
		const T* rtn = get_object(hdl, false);
		unlock();
		return rtn;
	}
	inline const T* get_object(const basic_ordered_slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return 0;
		lock();
		const T* rtn = get_object(hdl, true);
		unlock();
		return rtn;
	}

	inline void erase(basic_ordered_slot_map_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return;
		lock();
		erase(hdl, false);
		unlock();
	}
	inline void erase(basic_ordered_slot_map_weak_handle<T, Mut, Alloc, MoonAlloc>& hdl) {
		if(hdl.moon != moon)
			return;
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
				basic_ordered_slot_map_weak_handle<T, Mut, Alloc, MoonAlloc> hdl;
				hdl.moon = moon;
				hdl.idx = std::distance(indexes.begin(), it);
				hdl.gen = it->gens.increment_generation(true);
				++moon->count;

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
	void defragment() noexcept {
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
private:
	static basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>* getMap(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl) {
		//get the map and lock this
		if(hdl.moon == 0)
			return 0;
		//does this still point to a valid basic_ordered_slot_map?
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
		return (basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>*)hdl.moon->slot_map_ptr;
	}
	static bool increment_handle_external(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return false;
		++hdl.moon->count;
		bool rtn = map->increment_handle(hdl, weak);
		map->unlock();
		return rtn;
	}
	static void decrement_handle_external(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return;
		--hdl.moon->count;
		map->decrement_handle(hdl, weak);
		map->unlock();
	}
	static T* get_object_external(slot_internal::internal_basic_ordered_slot_map_handle<Mut>& hdl, bool weak) {
		basic_ordered_slot_map<T, Mut, Alloc, MoonAlloc>* map = getMap(hdl);
		if(map == 0)
			return 0;
		lock();
		const T* rtn = map->get_object(hdl, weak);
		unlock();
		return rtn;
	}

public:
	~basic_ordered_slot_map() {
		dtorMoon();
	}
};

}
