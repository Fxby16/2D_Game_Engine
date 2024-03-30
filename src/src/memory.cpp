#include <pch.hpp>

void *AllocateMemory(size_t size,std::source_location src){
    #ifdef DEBUG
        printf("Allocating %zu bytes from %s:%d\n",size,src.file_name(),src.line());
    #endif
    void *ptr=operator new(size);
    return ptr;
}

void FreeMemory(void *ptr,std::source_location src){
    #ifdef DEBUG
        printf("Freeing memory from %s:%d\n",src.file_name(),src.line());
    #endif
    if(ptr!=nullptr)
        operator delete(ptr);
}