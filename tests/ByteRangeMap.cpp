/* Reverse Engineer's Hex Editor
 * Copyright (C) 2020-2025 Daniel Collins <solemnwarning@solemnwarning.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "../src/platform.hpp"
#include <gtest/gtest.h>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <string>

#include "../src/ByteRangeMap.hpp"

using namespace REHex;

#define EXPECT_RANGES(...) \
{ \
	std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > ranges = { __VA_ARGS__ }; \
	EXPECT_EQ(brm.get_ranges(), ranges); \
}

#define EXPECT_BIT_RANGES(...) \
{ \
	std::vector< std::pair<BitRangeMap<std::string>::Range, std::string> > ranges = { __VA_ARGS__ }; \
	EXPECT_EQ(brm.get_ranges(), ranges); \
}

/* Used by Google Test to print out Range values. */
std::ostream& operator<<(std::ostream& os, const ByteRangeMap<std::string>::Range &range)
{
	char buf[128];
	snprintf(buf, sizeof(buf), "{ offset = %jd, length = %jd }", (intmax_t)(range.offset), (intmax_t)(range.length));
	
	return os << buf;
}

TEST(ByteRangeMap, EmptyMap)
{
	ByteRangeMap<std::string> brm;
	
	EXPECT_RANGES();
}

TEST(ByteRangeMap, GetRange)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "veil"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  1), "suggest"),
		std::make_pair(ByteRangeMap<std::string>::Range(42,  1), "rebel"),
	};
	
	ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	EXPECT_EQ(brm.get_range(9),  brm.end());
	EXPECT_EQ(brm.get_range(10), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range(29), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range(30), brm.end());
	EXPECT_EQ(brm.get_range(39), brm.end());
	EXPECT_EQ(brm.get_range(40), std::next(brm.begin(), 1));
	EXPECT_EQ(brm.get_range(41), brm.end());
	EXPECT_EQ(brm.get_range(42), std::next(brm.begin(), 2));
	EXPECT_EQ(brm.get_range(43), brm.end());
}

TEST(ByteRangeMap, GetRangeEmptyMap)
{
	ByteRangeMap<std::string> brm;
	
	EXPECT_EQ(brm.get_range(0),  brm.end());
}

TEST(ByteRangeMap, GetRangeIn)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "fumbling"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "false"),
		std::make_pair(ByteRangeMap<std::string>::Range(60, 10), "oval"),
	};
	
	const ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	EXPECT_EQ(brm.get_range_in( 0, 10), brm.end());
	EXPECT_EQ(brm.get_range_in( 0, 15), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range_in( 0, 35), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range_in(10,  0), brm.end());
	EXPECT_EQ(brm.get_range_in(10,  1), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range_in(10, 10), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range_in(10, 20), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range_in(29, 20), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range_in(30, 10), brm.end());
	EXPECT_EQ(brm.get_range_in(30, 20), std::next(brm.begin(), 1));
	EXPECT_EQ(brm.get_range_in(30, 50), std::next(brm.begin(), 1));
	EXPECT_EQ(brm.get_range_in(50, 10), brm.end());
	EXPECT_EQ(brm.get_range_in(50, 11), std::next(brm.begin(), 2));
	EXPECT_EQ(brm.get_range_in(69,  1), std::next(brm.begin(), 2));
	EXPECT_EQ(brm.get_range_in(70, 10), brm.end());
}

TEST(ByteRangeMap, GetRangeInEmptyMap)
{
	const ByteRangeMap<std::string> brm;
	
	EXPECT_EQ(brm.get_range_in(0, 10), brm.end());
}

TEST(ByteRangeMap, GetRangeInOverflow)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "fumbling"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "false"),
		std::make_pair(ByteRangeMap<std::string>::Range(60, 10), "oval"),
	};
	
	const ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	const off_t MAX = std::numeric_limits<off_t>::max();
	
	EXPECT_EQ(brm.get_range_in(10, MAX), std::next(brm.begin(), 0));
	EXPECT_EQ(brm.get_range_in(30, MAX), std::next(brm.begin(), 1));
	EXPECT_EQ(brm.get_range_in(50, MAX), std::next(brm.begin(), 2));
}

TEST(ByteRangeMap, SetOneRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "toad");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "toad"),
	);
}

TEST(ByteRangeMap, AddExclusiveRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "pop");
	brm.set_range(80, 40, "shock");
	brm.set_range(40, 30, "sheep");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "pop"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 30), "sheep"),
		std::make_pair(ByteRangeMap<std::string>::Range(80, 40), "shock"),
	);
}

TEST(ByteRangeMap, AddSameRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "umbrella");
	brm.set_range(10, 20, "bouncy");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "bouncy"),
	);
}

TEST(ByteRangeMap, SetRangeAtStartOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "can");
	brm.set_range(10,  1, "plot");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10,  1), "plot"),
		std::make_pair(ByteRangeMap<std::string>::Range(11, 19), "can"),
	);
}

TEST(ByteRangeMap, SetRangeAtEndOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "stream");
	brm.set_range(29,  1, "wanting");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 19), "stream"),
		std::make_pair(ByteRangeMap<std::string>::Range(29,  1), "wanting"),
	);
}

TEST(ByteRangeMap, SetRangeBeforeRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "intelligent");
	brm.set_range( 5,  5, "buzz");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 5,  5), "buzz"),
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "intelligent"),
	);
}

TEST(ByteRangeMap, SetRangeAfterRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "different");
	brm.set_range(30, 50, "filthy");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "different"),
		std::make_pair(ByteRangeMap<std::string>::Range(30, 50), "filthy"),
	);
}

TEST(ByteRangeMap, SetRangeBetweenRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(5,   1, "alike");
	brm.set_range(10, 20, "gruesome");
	brm.set_range(40, 20, "friction");
	brm.set_range(99,  1, "null");
	
	brm.set_range(30, 10, "cracker");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 5,  1), "alike"),
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "gruesome"),
		std::make_pair(ByteRangeMap<std::string>::Range(30, 10), "cracker"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 20), "friction"),
		std::make_pair(ByteRangeMap<std::string>::Range(99,  1), "null"),
	);
}

TEST(ByteRangeMap, SetRangeOverlappingStartOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range( 1,  1, "thick");
	brm.set_range(10, 20, "chemical");
	
	brm.set_range( 5, 10, "rare");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 1,  1), "thick"),
		std::make_pair(ByteRangeMap<std::string>::Range( 5, 10), "rare"),
		std::make_pair(ByteRangeMap<std::string>::Range(15, 15), "chemical"),
	);
}

TEST(ByteRangeMap, SetRangeOverlappingStartOfRangeSameValue)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range( 1,  1, "roll");
	brm.set_range(10, 20, "flat");
	
	brm.set_range( 5, 10, "flat");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 1,  1), "roll"),
		std::make_pair(ByteRangeMap<std::string>::Range( 5, 25), "flat"),
	);
}

TEST(ByteRangeMap, SetRangeOverlappingEndOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "strong");
	brm.set_range(50, 20, "recess");
	
	brm.set_range(25, 10, "cushion");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 15), "strong"),
		std::make_pair(ByteRangeMap<std::string>::Range(25, 10), "cushion"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 20), "recess"),
	);
}

TEST(ByteRangeMap, SetRangeOverlappingEndOfRangeSameValue)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "misty");
	brm.set_range(50, 20, "taste");
	
	brm.set_range(25, 10, "misty");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 25), "misty"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 20), "taste"),
	);
}

TEST(ByteRangeMap, SetRangeOverlappingSeveralRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range( 5,  1, "insect");
	brm.set_range(10, 20, "fold");
	brm.set_range(40, 10, "magenta");
	brm.set_range(60,  5, "money");
	brm.set_range(70, 10, "internal");
	
	brm.set_range(15, 47, "drink");
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 5,  1), "insect"),
		std::make_pair(ByteRangeMap<std::string>::Range(10,  5), "fold"),
		std::make_pair(ByteRangeMap<std::string>::Range(15, 47), "drink"),
		std::make_pair(ByteRangeMap<std::string>::Range(62,  3), "money"),
		std::make_pair(ByteRangeMap<std::string>::Range(70, 10), "internal"),
	);
}

TEST(ByteRangeMap, AddZeroLengthRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 0, "discreet");
	
	EXPECT_RANGES();
}

TEST(ByteRangeMap, ClearExactRange)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "homorganic"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "redeceiving"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "antisophistication"),
	};
	
	ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	brm.clear_range(10, 20);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "redeceiving"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "antisophistication"),
	);
}

TEST(ByteRangeMap, ClearStartOfRange)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "gliomata"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "pereirine"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "condyle"),
	};
	
	ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	brm.clear_range(45, 10);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "gliomata"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "pereirine"),
		std::make_pair(ByteRangeMap<std::string>::Range(55,  5), "condyle"),
	);
}

TEST(ByteRangeMap, ClearEndOfRange)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "ninon"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "zoophyte"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "recruit"),
	};
	
	ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	brm.clear_range(42, 3);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "ninon"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  2), "zoophyte"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "recruit"),
	);
}

TEST(ByteRangeMap, ClearMiddleOfRange)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "boundary"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "worry"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "silly"),
	};
	
	ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	brm.clear_range(15, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10,  5), "boundary"),
		std::make_pair(ByteRangeMap<std::string>::Range(20, 10), "boundary"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "worry"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "silly"),
	);
}

TEST(ByteRangeMap, ClearMultipleRanges)
{
	const std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > RANGES = {
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "vivacious"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "snotty"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "nervous"),
	};
	
	ByteRangeMap<std::string> brm(RANGES.begin(), RANGES.end());
	
	brm.clear_range(5, 50);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(55, 5), "nervous"),
	);
}

TEST(ByteRangeMap, DataInsertedBeforeRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_inserted(9, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(15, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(45, 10), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(65, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataInsertedAfterRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_inserted(70, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(60, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataInsertedBetweenRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_inserted(35, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(45, 10), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(65, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataInsertedAtStartOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_inserted(10, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(15, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(45, 10), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(65, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataInsertedAtEndOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_inserted(29, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 19), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(34,  1), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(45, 10), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(65, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataInsertedIntoRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_inserted(44, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  4), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(49,  6), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(65, 10), "steep"),
	);
}

/* Tests ByteRangeMap::data_inserted() on a large enough set to use threading. */
TEST(ByteRangeMap, DataInsertedParallel)
{
	/* Populate a reference vector with double the ranges necessary to trigger necessary to
	 * trigger threading, then some change to check the remainder is handled properly.
	*/
	
	const size_t N_RANGES = ByteRangeMap<std::string>::DATA_INSERTED_THREAD_MIN * 2 + 5;
	size_t next_range = 0;
	
	std::vector< std::pair<ByteRangeMap<std::string>::Range, std::string> > ranges;
	ranges.reserve(N_RANGES);
	
	for(size_t i = 0; i < N_RANGES; ++i)
	{
		ranges.push_back(std::make_pair(ByteRangeMap<std::string>::Range(next_range, 5), "XXX"));
		next_range += 10;
	}
	
	/* Populate ByteRangeSet with reference data. */
	
	ByteRangeMap<std::string> brm(ranges.begin(), ranges.end());
	EXPECT_TRUE(brm.get_ranges() == ranges) << "ByteRangeSet initial data populated correctly";
	
	/* Insert some data within the 10th range. */
	
	brm.data_inserted(102, 5);
	EXPECT_TRUE(brm.get_ranges() != ranges) << "ByteRangeSet::data_inserted() modified ranges";
	
	/* Munge our data to reflect what should be the new reality... */
	
	ranges[10].first.length = 2;
	ranges.insert(std::next(ranges.begin(), 11), std::make_pair(ByteRangeMap<std::string>::Range(107, 3), "XXX"));
	
	for(size_t i = 12; i < ranges.size(); ++i)
	{
		ranges[i].first.offset += 5;
	}
	
	/* ...and check it matches. */
	EXPECT_TRUE(brm.get_ranges() == ranges) << "ByteRangeSet::data_inserted() correctly modified ranges";
}

TEST(ByteRangeMap, DataErasedBeforeRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_erased(5, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 5, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(35, 10), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(55, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataErasedAfterRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_erased(70, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(60, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataErasedMatchingRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_erased(40, 10);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataErasedOverlappingStartOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_erased(35, 10);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(35,  5), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataErasedOverlappingEndOfRange)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 20, "zippy");
	brm.set_range(40, 10, "borrow");
	brm.set_range(60, 10, "steep");
	
	brm.data_erased(45, 10);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 20), "zippy"),
		std::make_pair(ByteRangeMap<std::string>::Range(40,  5), "borrow"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 10), "steep"),
	);
}

TEST(ByteRangeMap, DataErasedOverlappingMultipleRanges)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range( 5,  2, "cowardly");
	brm.set_range(10, 20, "wilderness");
	brm.set_range(40, 10, "island");
	brm.set_range(60, 10, "reproduce");
	brm.set_range(80,  5, "upset");
	
	brm.data_erased(15, 50);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 5, 2), "cowardly"),
		std::make_pair(ByteRangeMap<std::string>::Range(10, 5), "wilderness"),
		std::make_pair(ByteRangeMap<std::string>::Range(15, 5), "reproduce"),
		std::make_pair(ByteRangeMap<std::string>::Range(30, 5), "upset"),
	);
}

TEST(ByteRangeMap, DataErasedOverlappingMultipleRangesMerge)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range( 5,  2, "summer");
	brm.set_range(10, 20, "spotty");
	brm.set_range(40, 10, "broken");
	brm.set_range(60, 10, "spotty");
	brm.set_range(80,  5, "rinse");
	
	brm.data_erased(15, 50);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range( 5,  2), "summer"),
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "spotty"),
		std::make_pair(ByteRangeMap<std::string>::Range(30,  5), "rinse"),
	);
}

TEST(ByteRangeMap, GetSliceNoMatch)
{
	ByteRangeMap<std::string> source;
	
	source.set_range(10, 10, "clear");
	source.set_range(30,  5, "request");
	source.set_range(35,  5, "pear");
	source.set_range(50, 10, "limping");
	
	ByteRangeMap<std::string> brm = source.get_slice(20, 10);
	
	EXPECT_RANGES(
		/* Empty set */
	);
}

TEST(ByteRangeMap, GetSliceMatchOneExact)
{
	ByteRangeMap<std::string> source;
	
	source.set_range(10, 10, "clear");
	source.set_range(30,  5, "request");
	source.set_range(35,  5, "pear");
	source.set_range(50, 10, "limping");
	
	ByteRangeMap<std::string> brm = source.get_slice(10, 10);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "clear"),
	);
}

TEST(ByteRangeMap, GetSliceMatchOneInsideRange)
{
	ByteRangeMap<std::string> source;
	
	source.set_range(10, 10, "clear");
	source.set_range(30,  5, "request");
	source.set_range(35,  5, "pear");
	source.set_range(50, 10, "limping");
	
	ByteRangeMap<std::string> brm = source.get_slice(5, 20);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "clear"),
	);
}

TEST(ByteRangeMap, GetSliceMatchOneClamped)
{
	ByteRangeMap<std::string> source;
	
	source.set_range(10, 10, "clear");
	source.set_range(30,  5, "request");
	source.set_range(35,  5, "pear");
	source.set_range(50, 10, "limping");
	
	ByteRangeMap<std::string> brm = source.get_slice(12, 5);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(12, 5), "clear"),
	);
}

TEST(ByteRangeMap, GetSliceMatchMulti)
{
	ByteRangeMap<std::string> source;
	
	source.set_range(10, 10, "clear");
	source.set_range(30,  5, "request");
	source.set_range(35,  5, "pear");
	source.set_range(50, 10, "limping");
	
	ByteRangeMap<std::string> brm = source.get_slice(30, 10);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(30, 5), "request"),
		std::make_pair(ByteRangeMap<std::string>::Range(35, 5), "pear"),
	);
}

TEST(ByteRangeMap, GetSliceMatchMultiClamp)
{
	ByteRangeMap<std::string> source;
	
	source.set_range(10, 10, "clear");
	source.set_range(30,  5, "request");
	source.set_range(35,  5, "pear");
	source.set_range(50, 10, "limping");
	
	ByteRangeMap<std::string> brm = source.get_slice(32, 20);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(32, 3), "request"),
		std::make_pair(ByteRangeMap<std::string>::Range(35, 5), "pear"),
		std::make_pair(ByteRangeMap<std::string>::Range(50, 2), "limping"),
	);
}

TEST(ByteRangeMap, SetSliceEmptySource)
{
	ByteRangeMap<std::string> source, brm;
	
	brm.set_range(10, 10, "kindhearted");
	brm.set_range(20, 10, "wire");
	
	brm.set_slice(source);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "kindhearted"),
		std::make_pair(ByteRangeMap<std::string>::Range(20, 10), "wire"),
	);
}

TEST(ByteRangeMap, SetSliceEmptyDest)
{
	ByteRangeMap<std::string> source, brm;
	
	source.set_range(10, 10, "sulky");
	source.set_range(20, 10, "rough");
	
	brm.set_slice(source);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "sulky"),
		std::make_pair(ByteRangeMap<std::string>::Range(20, 10), "rough"),
	);
}

TEST(ByteRangeMap, SetSliceNoCollisions)
{
	ByteRangeMap<std::string> source, brm;
	
	brm.set_range(30, 10, "bell");
	brm.set_range(40, 10, "decorate");
	
	source.set_range(10, 10, "park");
	source.set_range(20, 10, "warlike");
	
	brm.set_slice(source);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "park"),
		std::make_pair(ByteRangeMap<std::string>::Range(20, 10), "warlike"),
		std::make_pair(ByteRangeMap<std::string>::Range(30, 10), "bell"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "decorate"),
	);
}

TEST(ByteRangeMap, SetSliceExactCollision)
{
	ByteRangeMap<std::string> source, brm;
	
	brm.set_range(20, 10, "snake");
	brm.set_range(40, 10, "book");
	
	source.set_range(10, 10, "economic");
	source.set_range(20, 10, "noiseless");
	
	brm.set_slice(source);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "economic"),
		std::make_pair(ByteRangeMap<std::string>::Range(20, 10), "noiseless"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "book"),
	);
}

TEST(ByteRangeMap, SetSliceSpliceCollision)
{
	ByteRangeMap<std::string> source, brm;
	
	brm.set_range(20, 10, "vase");
	brm.set_range(40, 10, "succinct");
	
	source.set_range(25, 20, "verdant");
	source.set_range(48,  1, "spiteful");
	
	brm.set_slice(source);
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(20,  5), "vase"),
		std::make_pair(ByteRangeMap<std::string>::Range(25, 20), "verdant"),
		std::make_pair(ByteRangeMap<std::string>::Range(45,  3), "succinct"),
		std::make_pair(ByteRangeMap<std::string>::Range(48,  1), "spiteful"),
		std::make_pair(ByteRangeMap<std::string>::Range(49,  1), "succinct"),
	);
}

TEST(ByteRangeMap, Transform)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 10, "front");
	brm.set_range(20, 10, "wretched");
	
	brm.transform([](const std::string &value) { return value + "!"; });
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "front!"),
		std::make_pair(ByteRangeMap<std::string>::Range(20, 10), "wretched!"),
	);
}

TEST(ByteRangeMap, SetBulk)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_bulk({
		{ ByteRangeMap<std::string>::Range(10, 10), "seemly" },
		{ ByteRangeMap<std::string>::Range(20, 10), "approval" },
	});
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "seemly"),
		std::make_pair(ByteRangeMap<std::string>::Range(20, 10), "approval"),
	);
}

TEST(ByteRangeMap, SetBulkMerge)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_range(10, 10, "grandfather");
	brm.set_range(30, 10, "tidy");
	
	brm.set_bulk({
		{ ByteRangeMap<std::string>::Range(10, 10), "food" },
		{ ByteRangeMap<std::string>::Range(40, 10), "beef" },
	});
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "food"),
		std::make_pair(ByteRangeMap<std::string>::Range(30, 10), "tidy"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "beef"),
	);
}

TEST(ByteRangeMap, SetBulkOverlap)
{
	ByteRangeMap<std::string> brm;
	
	brm.set_bulk({
		{ ByteRangeMap<std::string>::Range(15, 10), "redundant" },
		{ ByteRangeMap<std::string>::Range(10, 10), "disturbed" },
		{ ByteRangeMap<std::string>::Range(40, 10), "cobweb" },
	});
	
	EXPECT_RANGES(
		std::make_pair(ByteRangeMap<std::string>::Range(10, 10), "disturbed"),
		std::make_pair(ByteRangeMap<std::string>::Range(20,  5), "redundant"),
		std::make_pair(ByteRangeMap<std::string>::Range(40, 10), "cobweb"),
	);
}

TEST(BitRangeMap, SetRange)
{
	BitRangeMap<std::string> brm;
	
	brm.set_range(BitOffset(10, 2), BitOffset(20, 0), "toothbrush");
	brm.set_range(BitOffset(40, 0), BitOffset( 6, 4), "heap");
	
	EXPECT_BIT_RANGES(
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(10, 2), BitOffset(20, 0)), "toothbrush"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(40, 0), BitOffset( 6, 4)), "heap"),
	);
	
	brm.set_range(BitOffset(20, 4), BitOffset(10, 0), "store");
	brm.set_range(BitOffset(38, 0), BitOffset( 4, 0), "comfortable");
	
	EXPECT_BIT_RANGES(
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(10, 2), BitOffset(10, 2)), "toothbrush"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(20, 4), BitOffset(10, 0)), "store"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(38, 0), BitOffset( 4, 0)), "comfortable"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(42, 0), BitOffset( 4, 4)), "heap"),
	);
	
	brm.set_range(BitOffset(20, 4), BitOffset(5, 0), "toothbrush");
	
	EXPECT_BIT_RANGES(
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(10, 2), BitOffset(15, 2)), "toothbrush"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(25, 4), BitOffset( 5, 0)), "store"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(38, 0), BitOffset( 4, 0)), "comfortable"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(42, 0), BitOffset( 4, 4)), "heap"),
	);
}

TEST(BitRangeMap, ClearRange)
{
	BitRangeMap<std::string> brm;
	
	brm.set_range(BitOffset(10, 2), BitOffset(20, 0), "mind");
	brm.set_range(BitOffset(40, 0), BitOffset( 6, 4), "whisper");
	brm.set_range(BitOffset(50, 4), BitOffset(10, 2), "applaud");
	
	brm.clear_range(BitOffset(20, 4), BitOffset(32, 1));
	
	EXPECT_BIT_RANGES(
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(10, 2), BitOffset(10, 2)), "mind"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(52, 5), BitOffset( 8, 1)), "applaud"),
	);
}

TEST(BitRangeMap, DataInserted)
{
	BitRangeMap<std::string> brm;
	
	brm.set_range(BitOffset(10, 2), BitOffset(20, 0), "pathetic");
	brm.set_range(BitOffset(40, 0), BitOffset( 6, 4), "quack");
	brm.set_range(BitOffset(60, 0), BitOffset(10, 2), "skillful");
	
	brm.data_inserted(44, 10);
	
	EXPECT_BIT_RANGES(
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(10, 2), BitOffset(20, 0)), "pathetic"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(40, 0), BitOffset( 4, 0)), "quack"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(54, 0), BitOffset( 2, 4)), "quack"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(70, 0), BitOffset(10, 2)), "skillful"),
	);
}

TEST(BitRangeMap, DataErased)
{
	BitRangeMap<std::string> brm;
	
	brm.set_range(BitOffset(10, 2), BitOffset(20, 0), "achiever");
	brm.set_range(BitOffset(40, 0), BitOffset(6,  4), "chunky");
	brm.set_range(BitOffset(50, 0), BitOffset(8,  0), "impossible");
	brm.set_range(BitOffset(60, 0), BitOffset(10, 2), "mourn");
	
	brm.data_erased(44, 20);
	
	EXPECT_BIT_RANGES(
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(10, 2), BitOffset(20, 0)), "achiever"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(40, 0), BitOffset( 4, 0)), "chunky"),
		std::make_pair(BitRangeMap<std::string>::Range(BitOffset(44, 0), BitOffset( 6, 2)), "mourn"),
	);
}
