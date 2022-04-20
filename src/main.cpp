#include <iostream>
#include <thread>

void print_hello(int thread_num)
{
    std::cout << "Hello from thread " << thread_num << std::endl;
}

int main()
{
    std::thread thread_obj(print_hello, 1);
    std::cout << "Hello world!" << std::endl;
    thread_obj.join();
    return 0;
}
