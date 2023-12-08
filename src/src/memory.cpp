#include <source_location>
#include <cstdio>

void *AllocateMemory(size_t size,std::source_location src=std::source_location::current()){
    printf("Allocating %zu bytes from %s:%d\n",size,src.file_name(),src.line());
    void *ptr=operator new(size);
    return ptr;
}

void FreeMemory(void *ptr,std::source_location src=std::source_location::current()){
    printf("Freeing memory from %s:%d\n",src.file_name(),src.line());
    operator delete(ptr);
}