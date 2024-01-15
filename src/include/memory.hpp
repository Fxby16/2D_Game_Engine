#pragma once

void *AllocateMemory(size_t size,std::source_location src=std::source_location::current());
void FreeMemory(void *ptr,std::source_location src=std::source_location::current());