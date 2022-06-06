# Safe-Optimized-High-Performance-Queue-Datastructure-For-Multithreading-C-C++


- For C++17 C++20

- For Multithreading, prevents data races
- Very easy to use
- Optimized low level
- Thread Safe
- High Performance

How to Use:

safe::sQueue<YOUR_DATA_TYPE> Given_Name;


Example:

```
#include <iostream>
#include "safeQueue.h"

int main(){

safe::sQueue<int> example;
example.front();
example.empty();
example.push(5);
example.pop();
example.sizeQ();

return 0;
}

```
