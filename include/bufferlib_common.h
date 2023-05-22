// Copyright (c) 2023 Ture Fronczek-Munter
//
// This file is part of the BufferLib library
// Please visit: https://github.com/ture-f-munter/BufferLibCpp
//
// MIT License
#pragma once

#if(_WIN32)
#define BUFFERLIB_NEWLINE "\r\n"
#endif
#if(__linux__)
#define BUFFERLIB_NEWLINE "\n"
#endif
