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