#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H
//dependencies
#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <utility>
//typedef for easier use
using mX = std::mutex;
using cV = std::condition_variable;
namespace safe {
	template <typename G>
	class sQueue {
	 
	 volatile G* main_Mem = nullptr; //pointer to current and new memory // volatile to protect from compiler optimization
	 volatile G* temp_Mem = nullptr; //temporary pointer to old memory  // volatile to protect from compiler optimization
	 volatile size_t size = 0; //size of the queue  // volatile to protect from compiler optimization
	 mX sQmutex; // mutex to solve thread races and undefined behavior
	 cV cv_sQ; // make sure that the thread wait an proceeds at the right time
	 std::atomic<bool> rdy_Ent; //checks if possible to proceed
	 
	 //memory order can be relaxed for performance benefit but its not for sure that every thread sees the data
	 
	 public:
		sQueue<G>() {
			rdy_Ent.store(true, std::memory_order_release);
		}
		inline void push(G value) {
		 std::unique_lock<mX> p_ul(sQmutex);
		 cv_sQ.wait(p_ul, [&]() { return rdy_Ent.load(std::memory_order_consume); });
		 rdy_Ent.store(false, std::memory_order_release);
		 //operation
		 if (size == 0) {
			 ++size;
			 main_Mem = new G[size];
			 *main_Mem = std::move(value);
		 }
		 else {
			 temp_Mem = new G[size];
			 std::memcpy((void*)temp_Mem, (void*)main_Mem, sizeof(G) * size);
			 delete[] main_Mem;
			 main_Mem = nullptr;
			 main_Mem = new G[(size + 1)];

			 *(main_Mem + 0) = std::move(value);
			 std::memcpy((void*)(main_Mem + 1),(void*)temp_Mem,sizeof(G) * size);
			 delete[] temp_Mem;
			 temp_Mem = nullptr;
			 ++size;

		 }
		  rdy_Ent.store(true, std::memory_order_release);
		  
		
		}

		inline void pop() {
			std::unique_lock<mX> pop_ul(sQmutex);
			cv_sQ.wait(pop_ul, [&]() {return rdy_Ent.load(std::memory_order_consume); });
			rdy_Ent.store(false, std::memory_order_release);
			if (size == 0) {
				if (temp_Mem != nullptr) {
					delete[] temp_Mem;
					temp_Mem = nullptr;
				}
				else {
					std::cerr << "popping empty queue\n";
					return;
				}
			}
			temp_Mem = new G[size];
			std::memcpy((void*)temp_Mem, (void*)main_Mem, sizeof(G) * size);
			delete[] main_Mem;
			main_Mem = nullptr;
			temp_Mem = new G[size - 1];
			--size;
			for (uint_fast16_t i = 0; i < size; ++i) {
				//setting new mem
				main_Mem[i] = temp_Mem[i + 1];
			}
			std::memcpy((void*)main_Mem, (void*)(temp_Mem + 1), sizeof(G) * size);
			delete[] temp_Mem;
			temp_Mem = nullptr;
			rdy_Ent.store(true, std::memory_order_release);
			





		}

		inline G front() {
			std::unique_lock<mX> front_ul(sQmutex);
			cv_sQ.wait(front_ul, [&]() { return rdy_Ent.load(std::memory_order_consume); });
			rdy_Ent.store(false, std::memory_order_release);
			if (main_Mem == nullptr) {
				std::cerr << "\nNo element in sQUEUE, No Value To Return From Front() Function / Error at Line: " << __LINE__ << " inside File: " << __FILE__ << "\n";
				terminate();

			}
			G temp = std::move(main_Mem[0]);
			rdy_Ent.store(true, std::memory_order_release);
		
			return std::move(temp);
		}

		inline bool empty() {
			std::unique_lock<mX> empty_ul(sQmutex);
			cv_sQ.wait(empty_ul, [&]() { return rdy_Ent.load(std::memory_order_consume); });
			rdy_Ent.store(false, std::memory_order_release);
			size_t temp_size = std::move(size);
			rdy_Ent.store(true, std::memory_order_release);
			return std::move(temp_size <= 0);
		}
		
		~sQueue() {
			delete[] main_Mem;
			main_Mem = nullptr;
		}
	};

}
#endif
