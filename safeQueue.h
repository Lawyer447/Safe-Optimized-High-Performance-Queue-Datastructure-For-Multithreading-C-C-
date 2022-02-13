#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H
//dependencies
#include <iostream>
#include <mutex>
#include <utility>
//typedef for easier use
using mX = std::mutex;
using lG = std::lock_guard<mX>;

namespace safe {
	template <typename G>
	class sQueue {

		volatile G* main_Mem = nullptr; //pointer to current and new memory // volatile to protect from compiler optimization
		volatile G* temp_Mem = nullptr; //temporary pointer to old memory  // volatile to protect from compiler optimization
		volatile size_t size = 0; //size of the queue  // volatile to protect from compiler optimization
		mX sQmutex; // mutex to solve thread races and undefined behavior

	 public:
		sQueue<G>() = default;

		inline void push(G value) {
			lG puh1(sQmutex);
			//operation
			if (size == 0) {
				++size;
				main_Mem = new G[size];
				*main_Mem = std::move(value);
			}
			else {
				temp_Mem = new G[size];
				std::memmove((void*)temp_Mem, (void*)main_Mem, sizeof(G) * size);
				delete[] main_Mem;
				main_Mem = nullptr;
				main_Mem = new G[(size + 1)];

				*(main_Mem + 0) = std::move(value);
				std::memmove((void*)(main_Mem + 1), (void*)temp_Mem, sizeof(G) * size);
				delete[] temp_Mem;
				temp_Mem = nullptr;
				++size;

			}



		}
		inline void pop() {
			lG po_p(sQmutex);
			if (size == 0) {
				if (main_Mem != nullptr) {
					delete[] main_Mem;
					main_Mem = nullptr;
				}
				else {
					std::cerr << "\nPopping empty queue / Error at Line : " << __LINE__ << " inside File : " << __FILE__ << "\n";
					terminate();
				}
			}
			temp_Mem = new G[size];
			std::memmove((void*)temp_Mem, (void*)main_Mem, sizeof(G) * size);
			delete[] main_Mem;
			main_Mem = nullptr;
			main_Mem = new G[size - 1];
			--size;
			std::memmove((void*)main_Mem, (void*)(temp_Mem + 1), sizeof(G) * size);
			delete[] temp_Mem;
			temp_Mem = nullptr;
			
		}
		inline G front() {
			lG fr(sQmutex);
			if (main_Mem == nullptr) {
				std::cerr << "\nNo element in sQUEUE, No Value To Return From Front() Function / Error at Line: " << __LINE__ << " inside File: " << __FILE__ << "\n";
				terminate();

			}
			
			return std::move(main_Mem[0]);
		}
		inline bool empty() {
			lG em(sQmutex);
			return std::move(size <= 0);
		}
		inline size_t sizeQ() {
			lG sizlg(sQmutex);
			return std::move(this->size);
		}

		~sQueue() {
			delete[] main_Mem;
			main_Mem = nullptr;
		}
	};

}
#endif
