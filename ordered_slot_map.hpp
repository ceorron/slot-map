/*----------------------------------------------------------------------------------*\
 |																					|
 | ordered_slot_map.hpp																|
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
#include "slot_map_algorithm.hpp"
#include "empty_mutex.hpp"

namespace std {

template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct ordered_slot_map;

template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct ordered_slot_map_iterator;
template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct ordered_slot_map_const_iterator;
template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct ordered_slot_map_reverse_iterator;
template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct ordered_slot_map_const_reverse_iterator;

template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct ordered_slot_map_weak_handle;
template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct ordered_slot_map_handle;

namespace slot_internal {

template<typename Mut = slot_internal::empty_mutex>
struct ordered_slot_map_moon {
	void* map = 0;
	Mut mtx;
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex>
struct ordered_slot_map_object {
	size_t strongcount = 0;
	size_t weakcount = 0;
	ordered_slot_map_moon<Mut>* moon = 0;
	Mut mtx;
	alignas(alignof(T)) char obj[sizeof(T)];

	ordered_slot_map_object() = default;

	//for completeness
	ordered_slot_map_object(const ordered_slot_map_object& rhs) {
		*(T*)obj = *(T*)rhs.obj;
	}
	ordered_slot_map_object(ordered_slot_map_object&& rhs) {
		*(T*)obj = std::move(*(T*)rhs.obj);
	}

	ordered_slot_map_object& operator=(const ordered_slot_map_object& rhs) {
		if(this == &rhs)
			return *this;
		*(T*)obj = *(T*)rhs.obj;
		return *this;
	}
	ordered_slot_map_object& operator=(ordered_slot_map_object&& rhs) {
		if(this == &rhs)
			return *this;
		*(T*)obj = std::move(*(T*)rhs.obj);
		return *this;
	}

	~ordered_slot_map_object() {
		((T*)obj)->~T();
	}
};

template<typename T, typename Mut, typename Alloc, typename ObjAlloc, typename MoonAlloc>
struct internal_ordered_slot_map_handle {
	ordered_slot_map_object<T, Mut>* ptr = 0;

	ordered_slot_map_object<T, Mut>* get_obj() {
		if(ptr == 0)
			return 0;
		ptr->mtx.lock();
		if(ptr->strongcount == 0) {
			--ptr->weakcount;
			if(ptr->weakcount == 0) {
				ptr->mtx.unlock();
				//deallocate the memory
				ObjAlloc allctr;
				allctr.deallocate(ptr, 1);
			} else
				ptr->mtx.unlock();
			ptr = 0;
			return 0;
		}
		return ptr;
	}
	ordered_slot_map_object<T, Mut>* get_obj() const {
		return const_cast<internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->get_obj();
	}
	ordered_slot_map_object<T, Mut>* get_obj_nolock() {
		if(ptr == 0)
			return 0;
		ptr->mtx.lock();
		if(ptr->strongcount == 0) {
			--ptr->weakcount;
			if(ptr->weakcount == 0) {
				ptr->mtx.unlock();
				//deallocate the memory
				ObjAlloc allctr;
				allctr.deallocate(ptr, 1);
			} else
				ptr->mtx.unlock();
			ptr = 0;
			return 0;
		}
		ptr->mtx.unlock();
		return ptr;
	}
	ordered_slot_map_object<T, Mut>* get_obj_nolock() const {
		return const_cast<internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->get_obj_nolock();
	}

	void destruct_internal(bool strong) {
		//notify the container
		ptr->moon->mtx.lock();
		((ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>*)ptr->moon->map)->erase_internal(ptr);
		ptr->moon->mtx.unlock();

		//actually do destruction
		ptr->moon = 0;
		((T*)ptr->obj)->~T();
	}
	void clear_internal(bool strong) {
		//contact the owning object
		if(ptr == 0)
			return;
		if(strong) {
			ptr->mtx.lock();
			if(ptr->strongcount != 0) {
				--ptr->strongcount;
				if(ptr->strongcount == 0)
					destruct_internal(true);
			} else
				--ptr->weakcount;
			if(ptr->weakcount == 0 && ptr->strongcount == 0) {
				ptr->mtx.unlock();
				//deallocate the memory
				ObjAlloc allctr;
				allctr.deallocate(ptr, 1);
				ptr = 0;
				return;
			}
			ptr->mtx.unlock();
			ptr = 0;
			return;
		} else {
			ptr->mtx.lock();
			--ptr->weakcount;
			if(ptr->weakcount == 0 && ptr->strongcount == 0) {
				ptr->mtx.unlock();
				//deallocate the memory
				ObjAlloc allctr;
				allctr.deallocate(ptr, 1);
				ptr = 0;
				return;
			}
			ptr->mtx.unlock();
			ptr = 0;
			return;
		}
	}
	void erase_internal(bool strong) {
		//contact the owning object
		if(ptr == 0)
			return;
		if(strong) {
			ptr->mtx.lock();
			if(ptr->strongcount != 0) {
				ptr->weakcount += ptr->strongcount - 1;
				ptr->strongcount = 0;
				destruct_internal(true);
			} else
				--ptr->weakcount;
			if(ptr->weakcount == 0) {
				ptr->mtx.unlock();
				//deallocate the memory
				ObjAlloc allctr;
				allctr.deallocate(ptr, 1);
				ptr = 0;
				return;
			}
			ptr->mtx.unlock();
			ptr = 0;
			return;
		} else {
			ptr->mtx.lock();
			--ptr->weakcount;
			if(ptr->strongcount != 0) {
				ptr->weakcount += ptr->strongcount;
				ptr->strongcount = 0;
				destruct_internal(false);
			}
			if(ptr->weakcount == 0) {
				ptr->mtx.unlock();
				//deallocate the memory
				ObjAlloc allctr;
				allctr.deallocate(ptr, 1);
				ptr = 0;
				return;
			}
			ptr->mtx.unlock();
			ptr = 0;
			return;
		}
	}
};

}

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*>::allocator_type,
		 typename ObjAlloc = std::allocator<slot_internal::ordered_slot_map_object<T, Mut>>,
		 typename MoonAlloc = std::allocator<slot_internal::ordered_slot_map_moon<Mut>>>
struct ordered_slot_map_iterator {
private:
	typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	slot_internal::ordered_slot_map_object<T, Mut>* get_internal() {
		return *itr;
	}

	ordered_slot_map_iterator(const typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_iterator() = default;
	inline T& operator*() {
		return *(T*)(*itr)->obj;
	}
	inline T* operator->() {
		return (T*)(*itr)->obj;
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

	inline operator ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::reverse_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_reverse_iterator(itr));
	}

	Mut* get_mutex() const {
		return &const_cast<Mut>(itr->mtx);
	}
	Mut* get_mutex() {
		return &const_cast<Mut>(itr->mtx);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*>::allocator_type,
		 typename ObjAlloc = std::allocator<slot_internal::ordered_slot_map_object<T, Mut>>,
		 typename MoonAlloc = std::allocator<slot_internal::ordered_slot_map_moon<Mut>>>
struct ordered_slot_map_const_iterator {
private:
	typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	const slot_internal::ordered_slot_map_object<T, Mut>* get_internal() {
		return *itr;
	}

	ordered_slot_map_const_iterator(const typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_const_iterator() = default;
	inline const T& operator*() {
		return *(const T*)(*itr)->obj;
	}
	inline const T* operator->() {
		return (const T*)(*itr)->obj;
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

	inline operator ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::reverse_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_reverse_iterator(itr));
	}

	Mut* get_mutex() const {
		return &const_cast<Mut>(itr->mtx);
	}
	Mut* get_mutex() {
		return &const_cast<Mut>(itr->mtx);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*>::allocator_type,
		 typename ObjAlloc = std::allocator<slot_internal::ordered_slot_map_object<T, Mut>>,
		 typename MoonAlloc = std::allocator<slot_internal::ordered_slot_map_moon<Mut>>>
struct ordered_slot_map_reverse_iterator {
private:
	typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::reverse_iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	slot_internal::ordered_slot_map_object<T, Mut>* get_internal() {
		return *itr;
	}

	ordered_slot_map_reverse_iterator(const typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::reverse_iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_reverse_iterator() = default;
	inline T& operator*() {
		return *(T*)(*itr)->obj;
	}
	inline T* operator->() {
		return (T*)(*itr)->obj;
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

	inline operator ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_reverse_iterator(itr));
	}

	Mut* get_mutex() const {
		return &const_cast<Mut>(itr->mtx);
	}
	Mut* get_mutex() {
		return &const_cast<Mut>(itr->mtx);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*>::allocator_type,
		 typename ObjAlloc = std::allocator<slot_internal::ordered_slot_map_object<T, Mut>>,
		 typename MoonAlloc = std::allocator<slot_internal::ordered_slot_map_moon<Mut>>>
struct ordered_slot_map_const_reverse_iterator {
private:
	typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_reverse_iterator itr;

	friend struct ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	const slot_internal::ordered_slot_map_object<T, Mut>* get_internal() {
		return *itr;
	}

	ordered_slot_map_const_reverse_iterator(const typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_reverse_iterator& it)
		: itr(it)
	{}
public:
	ordered_slot_map_const_reverse_iterator() = default;
	inline const T& operator*() {
		return *(const T*)(*itr)->obj;
	}
	inline const T* operator->() {
		return (const T*)(*itr)->obj;
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

	inline operator ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::iterator(itr));
	}
	inline operator ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::const_iterator(itr));
	}
	inline operator ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>() const {
		return ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>(typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::reverse_iterator(itr));
	}

	Mut* get_mutex() const {
		return &const_cast<Mut>(itr->mtx);
	}
	Mut* get_mutex() {
		return &const_cast<Mut>(itr->mtx);
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*>::allocator_type,
		 typename ObjAlloc = std::allocator<slot_internal::ordered_slot_map_object<T, Mut>>,
		 typename MoonAlloc = std::allocator<slot_internal::ordered_slot_map_moon<Mut>>>
struct ordered_slot_map_handle : slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> {
	ordered_slot_map_handle() = default;

	friend struct ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	ordered_slot_map_handle(const ordered_slot_map_handle& rhs) {
		//copy this
		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->strongcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
	}
	ordered_slot_map_handle(ordered_slot_map_handle&& rhs) {
		//move this
		this->ptr = std::move(rhs.ptr);
		rhs.ptr = 0;
	}

	ordered_slot_map_handle(const ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& rhs) {
		//copy this
		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->strongcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
	}
	ordered_slot_map_handle(ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&& rhs) {
		//move this
		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->strongcount;
			--rslt->weakcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
		rhs.ptr = 0;
	}

	inline ordered_slot_map_handle& operator=(const ordered_slot_map_handle& rhs) {
		if(this == &rhs || this->ptr == rhs.ptr)
			return *this;
		//copy this
		clear();

		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->strongcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
		return *this;
	}
	inline ordered_slot_map_handle& operator=(ordered_slot_map_handle&& rhs) {
		if(this->ptr == rhs.ptr) {
			rhs.clear();
			return *this;
		}
		//move this
		clear();

		this->ptr = std::move(rhs.ptr);
		rhs.ptr = 0;
		return *this;
	}

	ordered_slot_map_handle& operator=(const ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& rhs) {
		if(this->ptr == rhs.ptr)
			return *this;
		//copy this
		clear();

		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->strongcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
		return *this;
	}
	ordered_slot_map_handle& operator=(ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&& rhs) {
		if(this->ptr == rhs.ptr) {
			rhs.clear();
			return *this;
		}
		//move this
		clear();

		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->strongcount;
			--rslt->weakcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
		rhs.ptr = 0;
		return *this;
	}

	~ordered_slot_map_handle() {
		//destruct this
		clear();
	}

	inline void clear() {
		this->clear_internal(true);
	}
	inline void del() {
		this->erase_internal(true);
	}

	Mut* get_mutex() const {
		return &this->get_obj_nolock()->mtx;
	}
	Mut* get_mutex() {
		return &this->get_obj_nolock()->mtx;
	}

	inline T& operator*() {
		return *(T*)this->get_obj_nolock()->obj;
	}
	inline T* operator->() {
		return (T*)this->get_obj_nolock()->obj;
	}

	inline const T& operator*() const {
		return *(const T*)this->get_obj_nolock()->obj;
	}
	inline const T* operator->() const {
		return (const T*)this->get_obj_nolock()->obj;
	}

	inline operator T*() {
		return (T*)this->get_obj_nolock()->obj;
	}
	inline operator const T*() const {
		return (const T*)this->get_obj_nolock()->obj;
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*>::allocator_type,
		 typename ObjAlloc = std::allocator<slot_internal::ordered_slot_map_object<T, Mut>>,
		 typename MoonAlloc = std::allocator<slot_internal::ordered_slot_map_moon<Mut>>>
struct ordered_slot_map_weak_handle : slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> {
	ordered_slot_map_weak_handle() = default;

	friend struct ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	ordered_slot_map_weak_handle(const ordered_slot_map_weak_handle& rhs) {
		//copy this
		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->weakcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
	}
	ordered_slot_map_weak_handle(ordered_slot_map_weak_handle&& rhs) {
		//move this
		this->ptr = std::move(rhs.ptr);
		rhs.ptr = 0;
	}

	ordered_slot_map_weak_handle(const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& rhs) {
		//copy this
		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->weakcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
	}
	ordered_slot_map_weak_handle(ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&& rhs) {
		//move this
		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			this->ptr = rslt;
			--rslt->strongcount;
			if(rslt->strongcount == 0) {
				this->destruct_internal(false);
				if(rslt->weakcount == 0) {
					rslt->mtx.unlock();
					//deallocate the memory
					ObjAlloc allctr;
					allctr.deallocate(rslt, 1);
				} else
					rslt->mtx.unlock();
				rhs.ptr = 0;
				this->ptr = 0;
				return;
			}
			++rslt->weakcount;
			rslt->mtx.unlock();
		}
		rhs.ptr = 0;
	}

	inline ordered_slot_map_weak_handle& operator=(const ordered_slot_map_weak_handle& rhs) {
		//copy this
		if(this == &rhs || this->ptr == rhs.ptr)
			return *this;
		clear();

		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			++rslt->weakcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}
		return *this;
	}
	inline ordered_slot_map_weak_handle& operator=(ordered_slot_map_weak_handle&& rhs) {
		//move this
		if(this->ptr == rhs.ptr) {
			rhs.clear();
			return *this;
		}
		clear();

		this->ptr = std::move(rhs.ptr);
		rhs.ptr = 0;
		return *this;
	}

	ordered_slot_map_weak_handle& operator=(const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& rhs) {
		//copy this
		if(this->ptr == rhs.ptr)
			return *this;
		clear();

		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_internal_ptr();
		if(rslt) {
			++rslt->weakcount;
			this->ptr = rslt;
			rslt->mtx.unlock();
		}

		return *this;
	}
	ordered_slot_map_weak_handle& operator=(ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&& rhs) {
		//move this
		if(this->ptr == rhs.ptr) {
			rhs.clear();
			return *this;
		}
		clear();

		slot_internal::ordered_slot_map_object<T, Mut>* rslt = rhs.get_obj();
		if(rslt) {
			this->ptr = rslt;
			--rslt->strongcount;
			if(rslt->strongcount == 0) {
				this->destruct_internal(false);
				if(rslt->weakcount == 0) {
					rslt->mtx.unlock();
					//deallocate the memory
					ObjAlloc allctr;
					allctr.deallocate(rslt, 1);
				} else
					rslt->mtx.unlock();
				rhs.ptr = 0;
				this->ptr = 0;
				return *this;
			}
			++rslt->weakcount;
			rslt->mtx.unlock();
		}
		rhs.ptr = 0;
		return *this;
	}

	~ordered_slot_map_weak_handle() {
		//destruct this
		clear();
	}

	inline void clear() {
		this->clear_internal(false);
	}
	inline void del() {
		this->erase_internal(false);
	}

	Mut* get_mutex() const {
		return &this->get_obj_nolock()->mtx;
	}
	Mut* get_mutex() {
		return &this->get_obj_nolock()->mtx;
	}

	inline T& operator*() {
		return *(T*)this->get_obj_nolock()->obj;
	}
	inline T* operator->() {
		return (T*)this->get_obj_nolock()->obj;
	}

	inline const T& operator*() const {
		return *(const T*)this->get_obj_nolock()->obj;
	}
	inline const T* operator->() const {
		return (const T*)this->get_obj_nolock()->obj;
	}

	inline operator T*() {
		return (T*)this->get_obj_nolock()->obj;
	}
	inline operator const T*() const {
		return (const T*)this->get_obj_nolock()->obj;
	}
};

template<typename T,
		 typename Mut = slot_internal::empty_mutex,
		 typename Alloc = typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*>::allocator_type,
		 typename ObjAlloc = std::allocator<slot_internal::ordered_slot_map_object<T, Mut>>,
		 typename MoonAlloc = std::allocator<slot_internal::ordered_slot_map_moon<Mut>>>
struct ordered_slot_map {
private:
	typedef typename slot_internal::ordered_slot_map_moon<Mut> MoonType;

	MoonType* moon = 0;
	std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc> objs;
	std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc> store;

	friend struct ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	friend struct ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>;
	friend struct ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	friend struct slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>;

	void initMoon() {
		//set the moon
		MoonAlloc allctr;
		moon = allctr.allocate(1);
		new (moon) MoonType();
		moon->map = this;
	}
	void dtorMoon() {
		//destruct the moon if we have one
		if(moon) {
			moon->~MoonType();
			MoonAlloc allctr;
			allctr.deallocate(moon, 1);
			moon = 0;
		}
	}
	void moveMoon(ordered_slot_map& rhs) {
		//steal the moon from this, used in move copy/construct
		dtorMoon();
		moon = std::move(rhs.moon);
		rhs.moon = 0;
	}

public:
	ordered_slot_map() {
		initMoon();
	}
	ordered_slot_map(const ordered_slot_map& rhs) {
		initMoon();
		*this = rhs;
	}
	ordered_slot_map(ordered_slot_map&& rhs) {
		*this = std::move(rhs);
	}

	ordered_slot_map& operator=(const ordered_slot_map& rhs) {
		if(this == &rhs)
			return *this;
		//clear this
		clear_internal();

		//copy everything across
		for(auto it = rhs.objs.begin(); it != rhs.objs.end(); ++it) {
			ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> hdl;
			hdl.ptr = *it;
			bool owner = rhs.owns_internal(hdl);
			insert_internal((*(T*)(*it)->obj), owner);
			memset((void*)&hdl, 0, sizeof(ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>));
		}
		return *this;
	}
	ordered_slot_map& operator=(ordered_slot_map&& rhs) {
		if(this == &rhs)
			return *this;
		//clear this
		clear_internal();

		//move the moon
		moveMoon(rhs);

		//move everything across
		store = std::move(store);
		objs = std::move(objs);
		return *this;
	}

	void lock() {
		moon->mtx.lock();
	}
	void unlock() {
		moon->mtx.unlock();
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
	typedef ordered_slot_map_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc> iterator;
	typedef ordered_slot_map_const_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc> const_iterator;
	typedef ordered_slot_map_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc> reverse_iterator;
	typedef ordered_slot_map_const_reverse_iterator<T, Mut, Alloc, ObjAlloc, MoonAlloc> const_reverse_iterator;
	typedef ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> handle;
	typedef ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> weak_handle;
private:

	void destruct_internal(slot_internal::ordered_slot_map_object<T, Mut>* ptr) noexcept {
		//lock this object
		ptr->mtx.lock();
		ptr->weakcount += ptr->strongcount;
		ptr->strongcount = 0;
		//call destructor on this!
		ptr->moon = 0;
		((T*)ptr->obj)->~T();
		ptr->mtx.unlock();
	}
	void clear_internal() noexcept {
		//empty everything out, go through and  all of the
		for(unsigned i = 0; i < objs.size(); ++i)
			destruct_internal(objs[i]);

		//clear store first, this should clear objs
		store.clear();
		objs.clear();
	}
	void erase_internal(slot_internal::ordered_slot_map_object<T, Mut>* ptr) {
		{
			//find in objs
			//std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc> objs;
			typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::iterator out;
			bool found = slot_internal::binary_search(objs.begin(), objs.end(), ptr,
									 [](slot_internal::ordered_slot_map_object<T, Mut>* a,
										slot_internal::ordered_slot_map_object<T, Mut>* b) {
											if((*(T*)a->obj) < (*(T*)b->obj))
												return true;
											else if((*(T*)b->obj) < (*(T*)a->obj))
												return false;
											//the two are equal, break ties using the allocated location
											return a < b;
									 }, out);
			if(found)
				objs.erase(out);
		}

		{
			//if this is in store, delete from there too
			typename std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc>::iterator out;
			ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> tmp;
			tmp.ptr = ptr;
			bool found = slot_internal::binary_search(store.begin(), store.end(), tmp,
									 [](const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& a,
										const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& b) {
											return a.ptr < b.ptr;
									 }, out);
			//clear without destruction!
			memset((void*)&tmp, 0, sizeof(ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>));
			if(found)
				store.erase(out);
		}
	}
	void insert(slot_internal::ordered_slot_map_object<T, Mut>* ptr, bool owner) {
		{
			//add this into objs
			//std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc> objs;
			typename std::vector<slot_internal::ordered_slot_map_object<T, Mut>*, Alloc>::iterator out;
			slot_internal::binary_search(objs.begin(), objs.end(), ptr,
						 [](slot_internal::ordered_slot_map_object<T, Mut>* a,
							slot_internal::ordered_slot_map_object<T, Mut>* b) {
								if((*(T*)a->obj) < (*(T*)b->obj))
									return true;
								else if((*(T*)b->obj) < (*(T*)a->obj))
									return false;
								//the two are equal, break ties using the allocated location
								return a < b;
						 }, out);
			objs.insert(out, ptr);
		}

		if(owner) {
			//if we are to own this then added it into store too
			//std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc> store;
			typename std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc>::iterator out;
			ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> tmp;
			tmp.ptr = ptr;
			++ptr->strongcount;
			slot_internal::binary_search(store.begin(), store.end(), tmp,
							[](const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& a,
							   const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& b) {
								return a.ptr < b.ptr;
							}, out);
			store.insert(out, std::move(tmp));
		}
	}
public:
	// iterators:
	inline iterator begin() noexcept {
		return iterator(objs.begin());
	}
	inline const_iterator begin() const noexcept {
		return const_iterator(objs.cbegin());
	}
	inline iterator end() noexcept {
		return iterator(objs.end());
	}
	inline const_iterator end() const noexcept {
		return const_iterator(objs.cend());
	}

	inline reverse_iterator rbegin() noexcept {
		return reverse_iterator(objs.rbegin());
	}
	inline const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(objs.crbegin());
	}
	inline reverse_iterator rend() noexcept {
		return reverse_iterator(objs.rend());
	}
	inline const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(objs.crend());
	}

	inline const_iterator cbegin() const noexcept {
		return const_iterator(objs.cbegin());
	}
	inline const_iterator cend() const noexcept {
		return const_iterator(objs.cend());
	}
	inline const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(objs.crbegin());
	}
	inline const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(objs.crend());
	}

	// capacity:
	inline size_type size() const noexcept {
		const_cast<ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->lock();
		size_type rtn = objs.size();
		const_cast<ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->unlock();
		return rtn;
	}
	inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	inline void resize(size_type sz) {
		//do nothing
	}
	inline size_type capacity() const noexcept {
		const_cast<ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->lock();
		size_type rtn = objs.capacity();
		const_cast<ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->unlock();
		return rtn;
	}
	void reserve(size_type sz) {
		lock();
		if(sz < objs.size()) {
			unlock();
			return;
		}
		objs.reserve(sz);
		unlock();
	}
	inline bool empty() const noexcept {
		return size() == 0;
	}
	void shrink_to_fit() {
		lock();
		objs.shrink_to_fit();
		unlock();
	}

private:
	ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> insert_internal(const T& val, bool owner) {
		//allocate a new object, copy everything across
		ObjAlloc allctr;
		slot_internal::ordered_slot_map_object<T, Mut>* nw = allctr.allocate(1);
		new (nw) slot_internal::ordered_slot_map_object<T, Mut>();
		nw->strongcount = 1;
		nw->moon = moon;
		new (nw->obj) T(val);

		ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> rtn;
		rtn.ptr = nw;

		insert(nw, owner);
		return rtn;
	}
	ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> insert_internal(T&& val, bool owner) {
		//allocate a new object, copy everything across
		ObjAlloc allctr;
		slot_internal::ordered_slot_map_object<T, Mut>* nw = allctr.allocate(1);
		new (nw) slot_internal::ordered_slot_map_object<T, Mut>();
		nw->strongcount = 1;
		nw->moon = moon;
		new (nw->obj) T(std::move(val));

		ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> rtn;
		rtn.ptr = nw;

		insert(nw, owner);
		return rtn;
	}
public:
	ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> insert(const T& val, bool owner = false) {
		lock();
		ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> rtn = insert_internal(val, owner);
		unlock();
		return rtn;
	}
	ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> insert(T&& val, bool owner = false) {
		lock();
		ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc> rtn = insert_internal(std::move(val), owner);
		unlock();
		return rtn;
	}
	template<typename Itr>
	std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>> insert(Itr begin, Itr end, bool owner = false) {
		lock();
		std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>> rtn;
		for(; begin != end; ++begin)
			rtn.push_back(insert_internal(*begin, owner));
		unlock();
		return rtn;
	}

	inline bool is_valid(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return false;
		lock();
		bool rtn = obj->moon == moon;
		unlock();
		return rtn;
	}
	T* get_object(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return 0;
		lock();
		T* rtn = (obj->moon == moon ? (T*)obj->obj : 0);
		unlock();
		return rtn;
	}
	const T* get_object(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return 0;
		lock();
		const T* rtn = (obj->moon == moon ? (const T*)obj->obj : 0);
		unlock();
		return rtn;
	}

private:
	void erase(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl, bool strong) {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return;
		lock();
		if(obj->moon == moon) {
			unlock();
			if(strong)
				((ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl).del();
			else
				((ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl).del();
			return;
		}
		unlock();
	}
	bool owns_internal(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) const {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return false;
		bool found = false;
		if(obj->moon == moon) {
			typename std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc>::const_iterator out;
			found = slot_internal::binary_search(store.begin(), store.end(), (const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl,
						[](const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& a,
						   const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& b) {
							return a.ptr < b.ptr;
						}, out);
		}
		return found;
	}
public:
	void erase(ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		erase(hdl, true);
	}
	void erase(ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		erase(hdl, false);
	}

	bool owns(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) const {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return false;
		//do we own this object?
		const_cast<ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->lock();
		bool found = false;
		if(obj->moon == moon) {
			typename std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc>::const_iterator out;
			found = slot_internal::binary_search(store.begin(), store.end(), (const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl,
						[](const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& a,
						   const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& b) {
							return a.ptr < b.ptr;
						}, out);
		}
		const_cast<ordered_slot_map<T, Mut, Alloc, ObjAlloc, MoonAlloc>*>(this)->unlock();
		return found;
	}
	bool release(slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return false;
		//if we own this object then release it
		lock();
		bool found = false;
		if(obj->moon == moon) {
			typename std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc>::iterator out;
			found = slot_internal::binary_search(store.begin(), store.end(), (ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl,
						[](const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& a,
						   const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& b) {
							return a.ptr < b.ptr;
						}, out);
			if(found)
				store.erase(out);
			//TODO work out locking??? unlock incorrect here and above if(found) store.erase(out); as store.erase(out); may lock this again circular lock!!!
		}
		unlock();
		return found;
	}
private:
	bool own(const slot_internal::internal_ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl, bool strong) {
		//if we don't own this then don't take ownership
		typename std::vector<ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>, Alloc>::iterator out;
		bool found = slot_internal::binary_search(store.begin(), store.end(), (const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl,
						[](const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& a,
						   const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& b) {
							return a.ptr < b.ptr;
						}, out);
		if(!found) {
			if(strong)
				store.insert(out, (const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl);
			else
				store.insert(out, (const ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>&)hdl);
		}
		return true;
	}
public:
	bool own(const ordered_slot_map_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return false;
		lock();
		bool rslt = (obj->moon != moon ? false : own(hdl, true));
		unlock();
		return rslt;
	}
	bool own(const ordered_slot_map_weak_handle<T, Mut, Alloc, ObjAlloc, MoonAlloc>& hdl) {
		slot_internal::ordered_slot_map_object<T, Mut>* obj = hdl.get_obj_nolock();
		if(obj == 0)
			return false;
		lock();
		bool rslt = (obj->moon != moon ? false : own(hdl, false));
		unlock();
		return rslt;
	}

	void clear() noexcept {
		lock();
		clear_internal();
		unlock();
	}
	inline void defragment() noexcept {
		//do nothing
	}
	~ordered_slot_map() {
		lock();
		clear_internal();
		unlock();
		dtorMoon();
	}
};

}
