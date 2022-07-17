#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <iostream>
#include <mutex>
#include <utility>

/*
- cppLawyer
- V2
*/

#ifdef _DEBUG
namespace safe {
	template <typename T>
	class sQueue {
		T* main_Mem = nullptr;
		T* temp_Mem = nullptr;
		uint_fast64_t size = 0;
		std::mutex sQmutex;
	public:
		sQueue<T>() = default;
		sQueue<T>(sQueue<T>& itemTocopy) {
			std::lock_guard<std::mutex> lg(itemTocopy.sQmutex);//so there is no undefined behavior
			this->size = itemTocopy.size;
			main_Mem = new T[size];
			for (uint_fast64_t i = 0; i < size; ++i)
				main_Mem[i] = itemTocopy.main_Mem[i];
		}
		sQueue<T>(sQueue<T>&& itemTocopy) = delete;
		inline void push(T value) {
			std::lock_guard<std::mutex> lg(sQmutex);
			if (size == 0) {
				main_Mem = new T[++size];
				*main_Mem = std::move(value);
			}
			else {
				temp_Mem = new T[size];
				std::memmove((void*)temp_Mem, (void*)main_Mem, sizeof(T) * size);
				delete[] main_Mem;
				main_Mem = new T[(size + 1)];
				*main_Mem = std::move(value);
				std::memmove((void*)(main_Mem + 1), (void*)temp_Mem, sizeof(T) * size);
				delete[] temp_Mem;
				++size;
			}
		}
		inline void pop() {
			std::lock_guard<std::mutex> lg(sQmutex);
			if (size == 0) {
				std::cerr << "\nPopping empty Queue\n";
				exit(-1); //popping empty Queue
			}
			temp_Mem = new T[size];
			std::memmove((void*)temp_Mem, (void*)main_Mem, sizeof(T) * size);
			delete[] main_Mem;
			main_Mem = new T[--size];
			std::memmove((void*)main_Mem, (void*)(temp_Mem + 1), sizeof(T) * size);
			delete[] temp_Mem;
		}
		inline T front() {
			std::lock_guard<std::mutex> lg(sQmutex);
			if (size == 0) {
				std::cerr << "\nNo element in Queue\n";
				exit(-1); //no element in Queue
			}
			return main_Mem[0];
		}
		inline bool empty() {
			std::lock_guard<std::mutex> lg(sQmutex);
			return (size == 0);
		}
		inline size_t sizeQ() {
			std::lock_guard<std::mutex> lg(sQmutex);
			return this->size;
		}
		~sQueue() {
			delete[] main_Mem;
		}
	};

}
#else
namespace safe {
	template <typename T>
	class sQueue {
		T* main_Mem = nullptr;
		T* temp_Mem = nullptr;
		uint_fast64_t size = 0;
		std::mutex sQmutex;
	public:
		sQueue<T>() = default;
		sQueue<T>(sQueue<T>& itemTocopy) {
			std::lock_guard<std::mutex> lg(itemTocopy.sQmutex);
			this->size = itemTocopy.size;
			main_Mem = new T[size];
			for (uint_fast64_t i = 0; i < size; ++i)
				main_Mem[i] = itemTocopy.main_Mem[i];
		}
		sQueue<T>(sQueue<T>&& itemTocopy) = delete;
		inline void push(T value) noexcept{
			std::lock_guard<std::mutex> lg(sQmutex);
			if (size == 0) {
				main_Mem = new T[++size];
				*main_Mem = std::move(value);
			}
			else {
				temp_Mem = new T[size];
				std::memmove((void*)temp_Mem, (void*)main_Mem, sizeof(T) * size);
				delete[] main_Mem;
				main_Mem = new T[(size + 1)];
				*main_Mem = std::move(value);
				std::memmove((void*)(main_Mem + 1), (void*)temp_Mem, sizeof(T) * size);
				delete[] temp_Mem;
				++size;
			}
		}
		inline void pop() noexcept{
			std::lock_guard<std::mutex> lg(sQmutex);
			temp_Mem = new T[size];
			std::memmove((void*)temp_Mem, (void*)main_Mem, sizeof(T) * size);
			delete[] main_Mem;
			main_Mem = new T[--size];
			std::memmove((void*)main_Mem, (void*)(temp_Mem + 1), sizeof(T) * size);
			delete[] temp_Mem;
		}
		inline T front() noexcept{
			std::lock_guard<std::mutex> lg(sQmutex);
			return main_Mem[0];
		}
		inline bool empty() noexcept{
			std::lock_guard<std::mutex> lg(sQmutex);
			return (size == 0);
		}
		inline size_t sizeQ() noexcept{
			std::lock_guard<std::mutex> lg(sQmutex);
			return this->size;
		}
		~sQueue() {
			delete[] main_Mem;
		}
	};
}
#endif
