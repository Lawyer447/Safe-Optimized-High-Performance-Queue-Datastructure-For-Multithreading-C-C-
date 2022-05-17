#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H
/*
-- cppLawyer --

GitHub: https://github.com/cppLawyer/

safe::sQueue --version <1.1>

*/

//dependencies
#include <iostream>
#include <mutex>
#include <utility>


//typedef for easier use
using lg = std::lock_guard<std::mutex>;
using spInt = uint_fast64_t;

namespace safe {
	template <typename T>
	class sQueue {

		T* main_Mem = nullptr;
		T* temp_Mem = nullptr;
		spInt size = 0;
		std::mutex sQmutex;

	public:
		sQueue<T>() = default;
		sQueue<T>(sQueue<T>& itemTocopy) {
			lg p1(itemTocopy.sQmutex);//so there is no undefined behavior
			this->size = itemTocopy.size;
			main_Mem[size];
			for (spInt i = 0; i < size; ++i) {
				main_Mem[i] = itemTocopy.main_Mem[i]; //deep copy
			}
		}//copy constructor 

		sQueue<T>(sQueue<T>&& itemTocopy) = delete;

		inline void push(T value) {
			lg puh1(sQmutex);
			if (size == 0) {
				main_Mem = new T[++size];
				*main_Mem = std::move(value);
			}
			else {
				temp_Mem = new T[size];
				std::memcpy((void*)temp_Mem, (void*)main_Mem, sizeof(T) * size);
				delete[] main_Mem;
				main_Mem = new T[(size + 1)];
				*main_Mem = std::move(value);
				std::memcpy((void*)(main_Mem + 1), (void*)temp_Mem, sizeof(T) * size);
				delete[] temp_Mem;
				++size;

			}



		}
		inline void pop() {
			lg po_p(sQmutex);
			if (size == 0) {
				std::cerr << "\nPopping empty Queue\n";
				exit(-1); //popping empty Queue
			}

			temp_Mem = new T[size];
			std::memcpy((void*)temp_Mem, (void*)main_Mem, sizeof(T) * size);
			delete[] main_Mem;
			main_Mem = new T[--size];
			std::memcpy((void*)main_Mem, (void*)(temp_Mem + 1), sizeof(T) * size);
			delete[] temp_Mem;
		}
		inline T front() {
			lg fr(sQmutex);
			if (size == 0) {
				std::cerr << "\nNo element in Queue\n";
				exit(-1); //no element in Queue
			}
			return main_Mem[0];
		}
		FORCEINLINE bool empty() {
			lg em(sQmutex);
			return (size == 0);
		}
		FORCEINLINE size_t sizeQ() {
			lg sizlg(sQmutex);
			return this->size;
		}

		~sQueue() {
			delete[] main_Mem;
		}
	};

}
#endif
