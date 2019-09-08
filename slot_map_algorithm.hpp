/*----------------------------------------------------------------------------------*\
 |																					|
 | slot_map_algorithm.hpp 															|
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

namespace std {

namespace slot_internal {

template<typename U>
inline ptrdiff_t dist(U* first, U* last) {
	return last - first;
}
inline int midpoint(unsigned imin, unsigned imax) {
	return (imin + imax) >> 1;
}
//basic binary search
template<typename Itr, typename T, typename Less>
bool binary_search(Itr beg, Itr end, const T& item,
				   Less comp, Itr& out) {
	//binary search return the insertion point, in both the found and not found case
	int sze = std::distance(beg, end);
	if(sze == 0) {
		out = end;
		return false;
	}

	int imin = 0;
	int imax = sze - 1;
	int imid = 0;

	// continue searching while [imin,imax] is not empty
	while(imin <= imax) {
		// calculate the midpoint for roughly equal partition
		imid = midpoint(imin, imax);
		// determine which subarray to search
		if (comp(*(beg + imid), item))
			// change min index to search upper subarray
			imin = imid + 1;
		else if(comp(item, *(beg + imid)))
			// change max index to search lower subarray
			imax = imid - 1;
		else {
			out = (beg + imid);
			return true;
		}
	}
	// item was not found return the insertion point
	out = (beg + imin);
	return false;
}

}

}
