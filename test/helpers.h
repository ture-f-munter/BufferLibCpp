// Copyright (c) 2023 Ture Fronczek-Munter
//
// This file is part of the BufferLib library
// Please visit: https://github.com/ture-f-munter/BufferLibCpp
//
// MIT License

#pragma once

#include "array_buffer.h"

template<typename TStorageDataType, int TCapacity>
void printbuffer(BufferLib::array_buffer<TStorageDataType, TCapacity>& buf, const std::string& heading)
{
	std::cout << std::endl;
	std::cout << "=== " << heading << " ===" << std::endl;
	std::cout << "Buf: capacity = " << std::to_string(buf.capacity()) 
		<< ", size = " << std::to_string(buf.size()) 
		<< ", free = " << std::to_string(buf.free_size()) << std::endl;

	std::cout << buf << std::endl;
}
