# slot-map - std like reference counted slot map implementation

Features [all slot maps]
 - fast, constant time object lookup/erase/insert O(1) using array indexes, minimal overhead
 - v.fast insert, insert doesn't invalidate other handles on slot_map resize
 - object erase, erase doesn't invalidate other handles on slot_map
 - no memory fragmentation, only contiguous memory used
 - iterate over full map, fast as contiguous only storage
 - basic_slot_map replaced by slot_map (faster, smaller memory usage), kept as faster in certain cases

Features [ordered slot map only]
 - ordered_slot_map keeps a vector of ordered items, slower O(log n) insert/erase
 - iteration happens over ordered list of items using, less-than operator or given comparison function
 - insert/erase invalidates raw pointers/iterators but not other handles (like other slot maps)

MIT Licence - See Source/License file

# TODO
 - [#] weak and strong handles on slot_map/ordered_slot_map
 - [#] change generation_data to remove 0 generations

# Example use - C++

(examples in main.cpp)

```C++
#include <iostream>

#include "slot_map.hpp"
#include "ordered_slot_map.hpp"
#include "basic_slot_map.hpp"

using namespace std;

struct slot_data {
	unsigned a;
	unsigned b;

	//less than operator (needed by ordered_slot_map)
	bool operator<(const slot_data& rhs) const {
		if(a < rhs.a)
			return true;
		if(a > rhs.a)
			return false;

		if(b < rhs.b)
			return true;
		if(b > rhs.b)
			return false;
		return false;
	}
};

void slot_map_test() {
	cout << "--- slot_map_test ---" << endl;
	//slot_map tests
	slot_map<slot_data> map;

	slot_map<slot_data>::handle hdl1 = map.insert(slot_data{50, 85});
	if(map.is_valid(hdl1)) {
		slot_data& itm = *map.get_object(hdl1);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	slot_map<slot_data>::handle hdl2 = hdl1;
	if(map.is_valid(hdl2)) {
		slot_data& itm = *map.get_object(hdl2);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	//make some object handle, not we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{100, 90});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{200, 100});

	for(auto it = map.begin(); it != map.end(); ++it) {
		cout << "it->a : " << it->a << endl;
		cout << "it->b : " << it->b << endl;
	}

	cout << "--------------------" << endl;

	map.erase(hdl1);

	if(!map.is_valid(hdl1))
		cout << "hdl1 is invalid" << endl;
	if(!map.is_valid(hdl2))
		cout << "hdl2 is invalid" << endl;

	if(map.is_valid(hdl3))
		cout << "hdl3 is valid" << endl;
	if(map.is_valid(hdl4))
		cout << "hdl4 is valid" << endl;
	if(map.is_valid(hdl5))
		cout << "hdl5 is valid" << endl;
}

void ordered_slot_map_test() {
	cout << "--- ordered_slot_map_test ---" << endl;
	//slot_map tests
	ordered_slot_map<slot_data> map;

	ordered_slot_map<slot_data>::handle hdl1 = map.insert(slot_data{50, 85});
	if(map.is_valid(hdl1)) {
		slot_data& itm = *map.get_object(hdl1);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	ordered_slot_map<slot_data>::handle hdl2 = hdl1;
	if(map.is_valid(hdl2)) {
		slot_data& itm = *map.get_object(hdl2);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	//make some object handle, not we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{100, 90});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{200, 100});

	for(auto it = map.begin(); it != map.end(); ++it) {
		cout << "it->a : " << it->a << endl;
		cout << "it->b : " << it->b << endl;
	}

	cout << "--------------------" << endl;

	map.erase(hdl1);

	if(!map.is_valid(hdl1))
		cout << "hdl1 is invalid" << endl;
	if(!map.is_valid(hdl2))
		cout << "hdl2 is invalid" << endl;

	if(map.is_valid(hdl3))
		cout << "hdl3 is valid" << endl;
	if(map.is_valid(hdl4))
		cout << "hdl4 is valid" << endl;
	if(map.is_valid(hdl5))
		cout << "hdl5 is valid" << endl;
}

void basic_slot_map_test() {
	cout << "--- basic_slot_map_test ---" << endl;
	//slot_map tests
	basic_slot_map<slot_data> map;

	basic_slot_map<slot_data>::handle hdl1 = map.insert(slot_data{50, 85});
	if(map.is_valid(hdl1)) {
		slot_data& itm = *map.get_object(hdl1);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	basic_slot_map<slot_data>::handle hdl2 = hdl1;
	if(map.is_valid(hdl2)) {
		slot_data& itm = *map.get_object(hdl2);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	//make some object handle, not we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{100, 90});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{200, 100});

	for(auto it = map.begin(); it != map.end(); ++it) {
		cout << "it->a : " << it->a << endl;
		cout << "it->b : " << it->b << endl;
	}

	cout << "--------------------" << endl;

	map.erase(hdl1);

	if(!map.is_valid(hdl1))
		cout << "hdl1 is invalid" << endl;
	if(!map.is_valid(hdl2))
		cout << "hdl2 is invalid" << endl;

	if(map.is_valid(hdl3))
		cout << "hdl3 is valid" << endl;
	if(map.is_valid(hdl4))
		cout << "hdl4 is valid" << endl;
	if(map.is_valid(hdl5))
		cout << "hdl5 is valid" << endl;
}

int main() {
	//test each of the slot maps!!!
	slot_map_test();
	ordered_slot_map_test();
	basic_slot_map_test();
	return 0;
}
```

Please use and let me know what you think.

Thanks

ceorron

aka

Richard Cookman

