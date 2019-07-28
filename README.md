# slot-map - std like reference counted slot map implementation

Features
 - fast, constant time object lookup/erase using array indexes
 - v.fast insert, insert doesn't invalidate other references on slot_map resize
 - no memory fragmentation, only contiguose memory used
 - object, destruction notification, objects can be created/destroyed by any other object
 - iterate over list fast as contiguose only storage

MIT Licence - See Source/License file

# Example use - C++

(examples in main.cpp)

```C++
#include <iostream>
#include "slot_map.hpp"

using namespace std;

struct slot_data {
	unsigned a;
	unsigned b;
};

int main() {
	//slot_map tests
	slot_map<slot_data> map;

	slot_map<slot_data>::handle hdl1 = map.insert(slot_data{50, 85});
	if(map.is_valid(hdl1)) {
		slot_data& itm = *map.get_object(hdl1);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	slot_map<slot_data>::handle hdl2 = hdl1;
	if(map.is_valid(hdl2)) {
		slot_data& itm = *map.get_object(hdl2);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	//make some object handle, not we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{100, 90});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{200, 100});

	for(auto it = map.begin(); it != map.end(); ++it) {
		cout << "it->a : " << it->a << endl;
		cout << "it->b : " << it->b << endl;
	}

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

	return 0;
}
```

Please use and let me know what you think.

Thanks

ceorron

aka

Richard Cookman

