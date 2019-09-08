/*----------------------------------------------------------------------------------*\
 |																					|
 | main.hpp 																		|
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

#include <iostream>

#include "ordered_slot_map.hpp"
#include "basic_ordered_slot_map.hpp"
#include "slot_map.hpp"
#include "basic_slot_map.hpp"

using namespace std;

struct slot_data {
	unsigned a;
	unsigned b;

	//less than operator (needed by basic_ordered_slot_map & ordered_slot_map)
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

	//make some object handles, note we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{200, 100});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{100, 90});

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

	cout << "--------------------" << endl;

	if(map.is_valid(hdl3))
		cout << "hdl3 is valid" << endl;
	if(map.is_valid(hdl4))
		cout << "hdl4 is valid" << endl;
	if(map.is_valid(hdl5))
		cout << "hdl5 is valid" << endl;

	cout << "--------------------" << endl;

	slot_map<slot_data>::weak_handle wkhdl1 = hdl3;
	slot_map<slot_data>::weak_handle wkhdl2 = hdl4;

	if(map.is_valid(wkhdl1))
		cout << "wkhdl1 is valid" << endl;
	if(map.is_valid(wkhdl2))
		cout << "wkhdl2 is valid" << endl;

	map.erase(hdl3);
	map.erase(hdl4);

	if(!map.is_valid(wkhdl1))
		cout << "wkhdl1 is invalid" << endl;
	if(!map.is_valid(wkhdl2))
		cout << "wkhdl2 is invalid" << endl;
	cout << endl;
}

void ordered_slot_map_test() {
	cout << "--- ordered_slot_map_test ---" << endl;
	//slot_map tests
	ordered_slot_map<slot_data> map;

	//set ownership
	ordered_slot_map<slot_data>::handle hdl1 = map.insert(slot_data{50, 85}, true);
	if(map.is_valid(hdl1)) {
		slot_data& itm = *map.get_object(hdl1);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	//ownership
	if(map.owns(hdl1))
		cout << "map owns hdl1" << endl;

	map.release(hdl1);

	if(!map.owns(hdl1))
		cout << "map released hdl1" << endl;

	if(map.own(hdl1))
		cout << "map takes ownership of hdl1" << endl;

	//release for cleanup
	map.release(hdl1);

	cout << "--------------------" << endl;

	ordered_slot_map<slot_data>::handle hdl2 = hdl1;
	if(map.is_valid(hdl2)) {
		slot_data& itm = *map.get_object(hdl2);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	//make some object handles, note we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{200, 100});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{100, 90});

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

	cout << "--------------------" << endl;

	if(map.is_valid(hdl3))
		cout << "hdl3 is valid" << endl;
	if(map.is_valid(hdl4))
		cout << "hdl4 is valid" << endl;
	if(map.is_valid(hdl5))
		cout << "hdl5 is valid" << endl;

	cout << "--------------------" << endl;

	ordered_slot_map<slot_data>::weak_handle wkhdl1 = hdl3;
	ordered_slot_map<slot_data>::weak_handle wkhdl2 = hdl4;

	if(map.is_valid(wkhdl1))
		cout << "wkhdl1 is valid" << endl;
	if(map.is_valid(wkhdl2))
		cout << "wkhdl2 is valid" << endl;

	map.erase(hdl3);
	map.erase(hdl4);

	if(!map.is_valid(wkhdl1))
		cout << "wkhdl1 is invalid" << endl;
	if(!map.is_valid(wkhdl2))
		cout << "wkhdl2 is invalid" << endl;
	cout << endl;
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

	//make some object handles, note we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{200, 100});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{100, 90});

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
	cout << endl;
}

void basic_ordered_slot_map_test() {
	cout << "--- basic_ordered_slot_map_test ---" << endl;
	//slot_map tests
	basic_ordered_slot_map<slot_data> map;

	basic_ordered_slot_map<slot_data>::handle hdl1 = map.insert(slot_data{50, 85});
	if(map.is_valid(hdl1)) {
		slot_data& itm = *map.get_object(hdl1);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	basic_ordered_slot_map<slot_data>::handle hdl2 = hdl1;
	if(map.is_valid(hdl2)) {
		slot_data& itm = *map.get_object(hdl2);

		cout << "itm.a : " << itm.a << endl;
		cout << "itm.b : " << itm.b << endl;
	}

	cout << "--------------------" << endl;

	//make some object handles, note we must have a handle to an item or else the item would get instantly destroyed
	auto hdl3 = map.insert(slot_data{200, 100});
	auto hdl4 = map.insert(slot_data{150, 95});
	auto hdl5 = map.insert(slot_data{100, 90});

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

	cout << "--------------------" << endl;

	if(map.is_valid(hdl3))
		cout << "hdl3 is valid" << endl;
	if(map.is_valid(hdl4))
		cout << "hdl4 is valid" << endl;
	if(map.is_valid(hdl5))
		cout << "hdl5 is valid" << endl;

	cout << "--------------------" << endl;

	basic_ordered_slot_map<slot_data>::weak_handle wkhdl1 = hdl3;
	basic_ordered_slot_map<slot_data>::weak_handle wkhdl2 = hdl4;

	if(map.is_valid(wkhdl1))
		cout << "wkhdl1 is valid" << endl;
	if(map.is_valid(wkhdl2))
		cout << "wkhdl2 is valid" << endl;

	map.erase(hdl3);
	map.erase(hdl4);

	if(!map.is_valid(wkhdl1))
		cout << "wkhdl1 is invalid" << endl;
	if(!map.is_valid(wkhdl2))
		cout << "wkhdl2 is invalid" << endl;
	cout << endl;
}

int main() {
	//test each of the slot maps!!!
	slot_map_test();
	ordered_slot_map_test();
	basic_slot_map_test();
	basic_ordered_slot_map_test();
	return 0;
}
