/*----------------------------------------------------------------------------------*\
 |																					|
 | generation_data.hpp 																|
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

namespace std {

namespace slot_internal {

template<typename T>
struct generation_data {
private:
	bool isvalid;									//is this current generation valid?
	bool isvec;										//is gens a generation of vectors?
	alignas(alignof(std::vector<T>)) char gens[sizeof(std::vector<T>)];
	T current_generation() {
		//get the current generation, don't modify
		//return the new generation, count = 1
		if(isvec) {
			std::vector<T>& vec = *((std::vector<T>*)gens);
			return vec.size() - 1;
		} else {
			//get the top most generation
			T gen = 0;
			T lgen = 0;
			char* lst = (char*)gens;
			while(lst < gens + sizeof(std::vector<T>)) {
				T crnt = *(T*)lst;

				if(crnt != 0)
					lgen = gen;

				lst += sizeof(T);
				++gen;
			}
			return lgen;
		}
	}
public:
	T new_generation() {
		isvalid = true;
		//return the new generation, count = 1
		if(isvec) {
			std::vector<T>& vec = *((std::vector<T>*)gens);
			vec.push_back(1);
			return vec.size() - 1;
		} else {
			//get the top most generation
			T gen = 0;
			T lgen = 0;
			char* lst = (char*)gens;
			while(lst < gens + sizeof(std::vector<T>)) {
				T crnt = *(T*)lst;

				if(crnt != 0)
					lgen = gen;

				lst += sizeof(T);
				++gen;
			}
			lst = gens + (sizeof(T) * (lgen + 1));

			//create a vector in place for the generation counters
			if(lst >= gens + sizeof(std::vector<T>)) {
				//swap this to the vector!
				isvec = true;

				std::vector<T> lvec;
				lvec.resize(sizeof(std::vector<T>)/sizeof(T), 0);
				memcpy(&lvec[0], gens, (sizeof(std::vector<T>)/sizeof(T)) * sizeof(T));

				new (gens) std::vector<T>(std::move(lvec));

				std::vector<T>& vec = *((std::vector<T>*)gens);
				vec.push_back(1);
				return vec.size() - 1;
			} else {
				((T*)gens)[lgen] = 1;
				return lgen;
			}
		}
	}
	T& get_generation_count(T gen) {
		//return the T count value for this generation
		if(isvec) {
			std::vector<T>& vec = *((std::vector<T>*)gens);
			return vec[gen];
		} else {
			T* lst = (T*)gens;
			return lst[gen];
		}
	}
	T increment_generation() {
		//get the current generation
		T gen = current_generation();
		//increment this
		++get_generation_count(gen);
	}
	bool match_generation(T pgen) const {
		//does the passed in generation match the current generation?
		if(isvec) {
			const std::vector<T>& vec = *((const std::vector<T>*)gens);
			return pgen == vec.size() - 1;
		} else {
			//get the top most generation
			T gen = 0;
			T lgen = 0;
			const char* lst = (const char*)gens;
			while(lst < gens + sizeof(std::vector<T>)) {
				T crnt = *(T*)lst;

				if(crnt != 0)
					lgen = gen;

				lst += sizeof(T);
				++gen;
			}
			return pgen == lgen;
		}
	}
	void decrement_generation(T pgen) {
		//decrement the given generation
		if(isvec) {
			std::vector<T>& vec = *((std::vector<T>*)gens);
			--vec[pgen];
			if(vec[pgen] == 0 && pgen == vec.size() - 1) {
				//remove any old generations
				while(vec.size() > 0 && vec.back() == 0)
					vec.pop_back();

				//move the data into the vector, small data optimisation
				if(vec.size() * sizeof(T) < sizeof(std::vector<T>)) {
					isvec = false;

					std::vector<T> lvec = std::move(*((std::vector<T>*)gens));
					memset(gens, 0, sizeof(std::vector<T>));
					memcpy(gens, &lvec[0], lvec.size() * sizeof(T));
				}
			}
		} else {
			T* lst = (T*)gens;
			--lst[pgen];
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
		using vctr = std::vector<T>;
		if(isvec) {
			vctr& vec = *((vctr*)gens);
			vec.~vctr();
		}
	}
};


}

}
