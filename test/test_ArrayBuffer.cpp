// Copyright (c) 2023 Ture Fronczek-Munter
//
// This file is part of the BufferLib library
// Please visit: https://github.com/ture-f-munter/BufferLibCpp
//
// MIT License
#include "array_buffer.h"
#include "helpers.h"

#include <gtest/gtest.h>

TEST(ArrayBuffer, testCreate)
{
	constexpr int buffer_size = 64;

	BufferLib::array_buffer<64> buf;

	EXPECT_EQ(buffer_size, buf.capacity());   // Total capacity of buffer
	EXPECT_EQ(buffer_size, buf.free_size());  // Number of bytes free for writing in buffer
	EXPECT_EQ(0, buf.size());   // Number of bytes ready for reading in buffer

	EXPECT_EQ(buf.read_ptr(), buf.write_ptr());
	EXPECT_EQ(buf.read_ptr(), buf.raw_ptr());

	// Check that all elements are zero
	for(int i=0; i<buf.capacity(); ++i)
	{
		EXPECT_EQ(0, buf.raw_ptr()[i]);
	}
}

TEST(ArrayBuffer, testCommitAndConsume)
{
	BufferLib::array_buffer<64> buf;
	buf.set_min_free(24);
	
	// ***** Commit elements
	const int n_elems_committed = 42;
	for (auto i = 0; i < n_elems_committed; ++i)
	{
		buf.raw_ptr()[i] = i;
	}
	buf.commit(n_elems_committed);

	ASSERT_EQ(n_elems_committed, buf.capacity() - buf.free_size());
	ASSERT_EQ(n_elems_committed, buf.size());
	ASSERT_EQ(42, buf.size());
	ASSERT_EQ(22, buf.free_size());
	ASSERT_EQ(buf.raw_ptr(), buf.read_ptr());
	ASSERT_EQ(buf.raw_ptr() + n_elems_committed, buf.write_ptr());
	for (auto i = 0; i < n_elems_committed; ++i)
	{
		// Check that all bytes from start to write_ptr have a value as expected
		EXPECT_EQ(i, buf.raw_ptr()[i]);
		EXPECT_EQ(i, buf.read_ptr()[i]);
	}
	for (auto i = n_elems_committed; i < buf.capacity(); ++i)
	{
		// Check that all bytes *after* the write_ptr are 0
		EXPECT_EQ(0, buf.raw_ptr()[i]);
		ASSERT_GE(buf.raw_ptr() + i, buf.write_ptr());
	}

	// ***** Consume 5 elements
	const auto n_elems_consumed_1 = 5ull;
	int total_elems_consumed = n_elems_consumed_1;
	buf.consume(n_elems_consumed_1);      // A relocation is triggered, so buffer starts with the value 0x05

	const int n_elems_available_1 = n_elems_committed - n_elems_consumed_1;
	ASSERT_EQ(n_elems_available_1, buf.capacity() - buf.free_size());
	ASSERT_EQ(n_elems_available_1, buf.size());
	ASSERT_EQ(37, buf.size());
	ASSERT_EQ(27, buf.free_size());
	ASSERT_EQ(buf.raw_ptr(), buf.read_ptr());
	ASSERT_EQ(buf.raw_ptr() + n_elems_available_1, buf.write_ptr());
	for (auto i = 0; i < n_elems_available_1; ++i)
	{
		// Check that all bytes from start to write_ptr have a value as expected
		EXPECT_EQ(i+ total_elems_consumed, buf.raw_ptr()[i]);
		EXPECT_EQ(i+ total_elems_consumed, buf.read_ptr()[i]);
	}
	for (auto i = n_elems_available_1; i < buf.capacity(); ++i)
	{
		// Check that all bytes *after* the write_ptr are 0
		EXPECT_EQ(0, buf.raw_ptr()[i]);
		ASSERT_GE(buf.raw_ptr() + i, buf.write_ptr());
	}


	// ***** Consume 13 elements
	const auto n_elems_consumed_2 = 13;
	buf.consume(n_elems_consumed_2);

	const int n_elems_available_2 = 24;  // 42 - 5 - 13
	ASSERT_EQ(24, buf.size());
	ASSERT_EQ(27, buf.free_size());
	ASSERT_EQ(buf.raw_ptr() + n_elems_consumed_2, buf.read_ptr());
	ASSERT_EQ(buf.raw_ptr() + n_elems_consumed_2 + n_elems_available_2, buf.write_ptr());
	for (auto i = 0; i < 36; ++i)
	{
		// Check that all bytes from start to write_ptr have a value as expected
		EXPECT_EQ(i + 5, buf.raw_ptr()[i]);
	}
	for (auto i = 0; i < 24; ++i)
	{
		// Check that all bytes from read_ptr to write_ptr have a value as expected
		EXPECT_EQ(i + 18, buf.read_ptr()[i]);
	}
	for (auto i = 37; i < buf.capacity(); ++i)
	{
		// Check that all bytes *after* the write_ptr are 0
		EXPECT_EQ(0, buf.raw_ptr()[i]);
		ASSERT_GE(buf.raw_ptr() + i, buf.write_ptr());
	}

	// ***** Consume 19 elements
	const std::uint64_t n_elems_consumed_3 = 19;
	buf.consume(n_elems_consumed_3);

	ASSERT_EQ(5, buf.size());
	ASSERT_EQ(27, buf.free_size());
	ASSERT_EQ(buf.raw_ptr() + 32, buf.read_ptr());
	ASSERT_EQ(buf.raw_ptr() + 37, buf.write_ptr());
	for (auto i = 0; i < 36; ++i)
	{
		// Check that all bytes from start to write_ptr have a value as expected
		EXPECT_EQ(i + 5, buf.raw_ptr()[i]);
	}
	for (auto i = 0; i < 5; ++i)
	{
		// Check that all bytes from read_ptr to write_ptr have a value as expected
		EXPECT_EQ(i + 37, buf.read_ptr()[i]);
	}
	for (auto i = 37; i < buf.capacity(); ++i)
	{
		// Check that all bytes *after* the write_ptr are 0
		EXPECT_EQ(0, buf.raw_ptr()[i]);
		ASSERT_GE(buf.raw_ptr() + i, buf.write_ptr());
	}
}


TEST(ArrayBuffer, testPrintout)
{
	BufferLib::array_buffer<64> buf;
	buf.set_min_free(24);

	printbuffer(buf, "BEFORE");

	// Commit elements
	const int n_elems_committed = 42;
	for (auto i = 0; i < n_elems_committed; ++i)
	{
		buf.raw_ptr()[i] = i;
	}
	buf.commit(n_elems_committed);

	std::stringstream ss;
	ss << std::to_string(n_elems_committed) << " ELEMENTS COMMITTED";
	printbuffer(buf, ss.str());

	// Consume elements
	const auto n_elems_consumed_1 = 5ull;
	buf.consume(n_elems_consumed_1);
	ss.str(""); // Clear stringstream
	ss << std::to_string(n_elems_consumed_1) << " ELEMENTS CONSUMED";
	printbuffer(buf, ss.str());

	// Consume elements
	const auto n_elems_consumed_2 = buf.free_size() / 2;
	buf.consume(n_elems_consumed_2);
	ss.str(""); // Clear stringstream
	ss << std::to_string(n_elems_consumed_2) << " ELEMENTS CONSUMED";
	printbuffer(buf, ss.str());

	// Consume elements
	const std::uint64_t n_elems_consumed_3 = buf.size() - 5;
	buf.consume(n_elems_consumed_3);
	ss.str(""); // Clear stringstream
	ss << std::to_string(n_elems_consumed_3) << " ELEMENTS CONSUMED";
	printbuffer(buf, ss.str());
}