/*----------------------------------------------------------------------------------*\
 |																					|
 | generation_data.hpp 																|
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

namespace std {

namespace slot_internal {

template<typename T>
struct generation_data {
private:
	struct counts {
		T weakcount;
		T strongcount;

		inline bool is_zero() const {
			return weakcount == 0 && strongcount == 0;
		}
	};
	bool isvalid;									//is this current generation valid?
	bool isvec;										//is gens a generation of vectors?
	T base;											//the algorithms remove the 0'th element, this is the number removed
													//we add base to the generation returned and sub base when looking up the current counts
													//this optimisation keeps gens small even as generations get large (also prevents generations from getting large)
	alignas(alignof(std::vector<counts>)) char gens[sizeof(std::vector<counts>)];
	void get_current_gen(T& lgen, const char*& lst) const {
		T gen = 0;
		lgen = 0;
		lst = (const char*)gens;
		while(lst < gens + sizeof(std::vector<counts>)) {
			counts crnt = *(counts*)lst;

			if(!crnt.is_zero())
				lgen = gen;

			lst += sizeof(counts);
			++gen;
		}

		//calculate the next list position
		gen = 0;
		if(lgen != 0 || !((counts*)gens)[0].is_zero()) gen = lgen + 1;
		lst = gens + (sizeof(counts) * gen);
	}
	T current_generation() {
		//get the current generation, don't modify
		//return the new generation, count = 1
		if(isvec) {
			std::vector<counts>& vec = *((std::vector<counts>*)gens);
			return (vec.size() - 1) + base;
		} else {
			//get the top most generation
			T lgen = 0;
			char* lst = (char*)gens;
			get_current_gen(lgen, lst);
			return lgen + base;
		}
	}
public:
	T new_generation() {
		isvalid = true;
		//return the new generation, count = 1
		if(isvec) {
			std::vector<counts>& vec = *((std::vector<counts>*)gens);
			vec.push_back(counts{0, 1});
			return (vec.size() - 1) + base;
		} else {
			//get the top most generation
			T lgen = 0;
			const char* lst = (char*)gens;
			get_current_gen(lgen, lst);

			//create a vector in place for the generation counters
			if(lst >= gens + sizeof(std::vector<counts>)) {
				//swap this to the vector!
				isvec = true;

				std::vector<counts> lvec;
				lvec.resize(sizeof(std::vector<counts>)/sizeof(counts), counts{0, 0});
				memcpy(&lvec[0], gens, (sizeof(std::vector<counts>)/sizeof(counts)) * sizeof(counts));

				new (gens) std::vector<counts>(std::move(lvec));

				std::vector<counts>& vec = *((std::vector<counts>*)gens);
				vec.push_back(counts{0, 1});
				return vec.size() - 1;
			} else {
				if(lgen != 0 || !((counts*)gens)[0].is_zero())
					lgen += 1;
				((counts*)gens)[lgen] = counts{0, 1};
				return lgen + base;
			}
		}
	}
	counts& get_generation_count(T gen) {
		//return the T count value for this generation
		if(isvec) {
			std::vector<counts>& vec = *((std::vector<counts>*)gens);
			return vec[gen - base];
		} else {
			counts* lst = (counts*)gens;
			return lst[gen - base];
		}
	}
	T increment_generation(bool weak) {
		//get the current generation
		T gen = current_generation();
		//increment this
		counts& tmp = get_generation_count(gen);
		if(weak)
			++tmp.weakcount;
		else
			++tmp.strongcount;
		return gen;
	}
	bool match_generation(T pgen, bool weak) const {
		//does the passed in generation match the current generation?
		if(isvec) {
			const std::vector<counts>& vec = *((const std::vector<counts>*)gens);
			return (pgen - base) == vec.size() - 1;
		} else {
			//get the top most generation
			T lgen = 0;
			const char* lst = (const char*)gens;
			get_current_gen(lgen, lst);
			return (pgen - base) == lgen;
		}
	}
	void decrement_generation(T pgen, bool weak) {
		//decrement the given generation
		pgen -= base;
		if(isvec) {
			std::vector<counts>& vec = *((std::vector<counts>*)gens);
			if(weak)
				--vec[pgen].weakcount;
			else
				--vec[pgen].strongcount;
			if(vec[pgen].is_zero()) {
				//remove any old generations
				if(pgen == vec.size() - 1) {
					while(vec.size() > 0 && vec.back().is_zero())
						vec.pop_back();
				} else if(pgen == 0) {
					while(vec.size() > 0 && vec.front().is_zero()) {
						vec.erase(vec.begin());
						++base;
					}
					//no outstanding handles to this, safe to zero base
					if(vec.size() == 0) base = 0;
				}

				//move the data into the vector, small data optimisation
				if(vec.size() * sizeof(counts) < sizeof(std::vector<counts>)) {
					isvec = false;

					std::vector<counts> lvec = std::move(*((std::vector<counts>*)gens));
					memset(gens, 0, sizeof(std::vector<counts>));
					memcpy(gens, &lvec[0], lvec.size() * sizeof(counts));
				}
			}
		} else {
			counts* lst = (counts*)gens;
			if(weak)
				--lst[pgen].weakcount;
			else
				--lst[pgen].strongcount;
			//de-base this
			if(pgen == 0 && lst[0].is_zero()) {
				T lgen = 0;
				const char* lst = (const char*)gens;
				get_current_gen(lgen, lst);

				if(lgen == 0)
					//no outstanding handles to this, safe to zero base
					base = 0;
				else {
					base -= lgen;

					//move the data by lgen (pop_front)
					counts* lst = (counts*)gens;
					size_t i = 0;
					size_t k = lgen;
					//do copy
					for(; i <= lgen; ++i, ++k)
						lst[i] = lst[k];
					//zero the remaining data
					for(; k * sizeof(counts) < sizeof(std::vector<counts>); ++k)
						lst[k] = counts{0, 0};
				}
			}
		}
	}
	inline void set_invalid() {
		isvalid = false;
	}
	inline bool is_valid() const {
		return isvalid;
	}
	~generation_data() {
		//do final destruction
		using vctr = std::vector<counts>;
		if(isvec) {
			vctr& vec = *((vctr*)gens);
			vec.~vctr();
		}
	}
};


}

}
