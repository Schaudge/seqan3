// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
//
// Copyright (c) 2006-2018, Knut Reinert, FU Berlin
// Copyright (c) 2016-2018, Knut Reinert & MPI Molekulare Genetik
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================

#include <sstream>

#include <gtest/gtest.h>

#include <range/v3/algorithm/equal.hpp>
#include <range/v3/view/transform.hpp>

#include <seqan3/alphabet/quality/all.hpp>
#include <seqan3/io/sequence_file/input_format_concept.hpp>
#include <seqan3/io/sequence_file/output_format_concept.hpp>
#include <seqan3/io/sequence_file/format_embl.hpp>
#include <seqan3/range/view/convert.hpp>

using namespace seqan3;

// ----------------------------------------------------------------------------
// general
// ----------------------------------------------------------------------------

TEST(general, concepts)
{
    EXPECT_TRUE((sequence_file_input_format_concept<sequence_file_format_embl>));
    EXPECT_TRUE((sequence_file_output_format_concept<sequence_file_format_embl>));
}

// ----------------------------------------------------------------------------
// reading
// ----------------------------------------------------------------------------

struct read : public ::testing::Test
{
    std::vector<std::string> expected_ids
    {
        { "ID1" },
        { "ID2" },
        { "ID3 lala" },
    };

    std::vector<dna5_vector> expected_seqs
    {
        { "ACGTTTTTTTTTTTTTTT"_dna5 },
        { "ACGTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"_dna5 },
        { "ACGTTTA"_dna5 },
    };

    sequence_file_format_embl format;

    sequence_file_input_options<dna5, false> options;

    std::string id;
    dna5_vector seq;

    void do_read_test(std::string const & input)
    {
        std::stringstream istream{input};

        for (unsigned i = 0; i < 3; ++i)
        {
            id.clear();
            seq.clear();

            EXPECT_NO_THROW(( format.read(istream, options, seq, id, std::ignore) ));
            EXPECT_EQ(id, expected_ids[i]);
            EXPECT_EQ(seq, expected_seqs[i]);
            EXPECT_TRUE((ranges::equal(seq, expected_seqs[i])));
            EXPECT_TRUE((ranges::equal(id, expected_ids[i])));
        }
    }
};

TEST_F(read, standard)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    do_read_test(input);
}

TEST_F(read, whitespace_in_seq)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT\nTTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT\nTTTTTTTTTT\nTTTTTTTTTT\vTTTTTTTTTT\tTTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };
    do_read_test(input);
}

TEST_F(read, no_id)
{
    std::string input
    {
        "IK ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT\nTTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT\nTTTTTTTTTT\nTTTTTTTTTT\vTTTTTTTTTT\tTTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    std::stringstream istream{input};
    EXPECT_THROW(( format.read(istream, options, seq, id, std::ignore)), parse_error );
}

TEST_F(read, other_lines)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "XX\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT\nTTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT\nTTTTTTTTTT\nTTTTTTTTTT\vTTTTTTTTTT\tTTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "AC 2389ß\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };
    do_read_test(input);
}

TEST_F(read, options_truncate_ids)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    options.truncate_ids = true;
    expected_ids[2] = "ID3"; // "lala" is stripped
    do_read_test(input);
}

TEST_F(read, complete_header)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "XX\n"
        "AC   AB000263;\n"
        "XX\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    options.complete_header = true;
    expected_ids[0] = "ID ID1;\tstuff\n";
    expected_ids[1] = "ID ID2;\n";
    expected_ids[2] = "ID ID3 lala;\nXX\nAC   AB000263;\nXX\n";
    do_read_test(input);
}

TEST_F(read, complete_header2)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "XX S\n"
        "XX Q\n"
        "AC   AB000263;\n"
        "XX\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    options.complete_header = true;
    expected_ids[0] = "ID ID1;\tstuff\n";
    expected_ids[1] = "ID ID2;\n";
    expected_ids[2] = "ID ID3 lala;\nXX S\nXX Q\nAC   AB000263;\nXX\n";
    do_read_test(input);
}

TEST_F(read, only_seq)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    std::stringstream istream{input};

    for (unsigned i = 0; i < 3; ++i)
    {
        id.clear();
        seq.clear();

        format.read(istream, options, seq, std::ignore, std::ignore);

        EXPECT_TRUE((ranges::equal(seq, expected_seqs[i])));
    }
}

TEST_F(read, only_seq_multiple_lines_before)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "XX\n"
        "XX\n"
        "XX\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    std::stringstream istream{input};

    for (unsigned i = 0; i < 3; ++i)
    {
        id.clear();
        seq.clear();

        format.read(istream, options, seq, std::ignore, std::ignore);

        EXPECT_TRUE((ranges::equal(seq, expected_seqs[i])));
    }
}

TEST_F(read, only_id)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    std::stringstream istream{input};

    for (unsigned i = 0; i < 3; ++i)
    {
        id.clear();
        seq.clear();

        format.read(istream, options, std::ignore, id, std::ignore);

        EXPECT_TRUE((ranges::equal(id, expected_ids[i])));
    }
}

TEST_F(read, seq_qual)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTT        18\n"
        "//\n"
        "ID ID2;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT TTTTTTTTTT 60\n"
        "TTTTTTTTTT TTTTTTTTTT TT        82\n"
        "//\n"
        "ID ID3 lala;\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ACGTTTA        7\n"
        "//\n"
    };

    std::stringstream istream{input};
    sequence_file_input_options<dna5, true> options2;

    std::vector<qualified<dna5, phred42>> seq_qual;

    for (unsigned i = 0; i < 3; ++i)
    {
        id.clear();
        seq_qual.clear();

        format.read(istream, options2, seq_qual, id, seq_qual);

        EXPECT_TRUE((ranges::equal(id, expected_ids[i])));
        EXPECT_TRUE((ranges::equal(seq_qual | view::convert<dna5>, expected_seqs[i])));
    }
}

TEST_F(read, illegal_alphabet)
{
    std::string input
    {
        "ID ID1;\tstuff\n"
        "SQ Sequence 1859 BP; 609 A; 314 C; 355 G; 581 T; 0 other;\n"
        "  ARGTTTTTTT\nTTTTTTTT        18\n"
        "//\n"
    };

    std::stringstream istream{input};
    EXPECT_THROW(( format.read(istream, options, seq, id, std::ignore)), parse_error );
}

// ----------------------------------------------------------------------------
// writing
// ----------------------------------------------------------------------------

struct write : public ::testing::Test
{
    std::vector<dna5_vector> seqs
    {
        "ACGT"_dna5,
        "AGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGNAGGCTGN"_dna5,
        "GGAGTATAATATATATATATATAT"_dna5
    };

    std::vector<std::string> ids
    {
        "TEST 1",
        "Test2",
        "Test3"
    };

    sequence_file_format_embl format;

    sequence_file_output_options options;

    std::ostringstream ostream;

    void do_write_test()
    {
        for (unsigned i = 0; i < 3; ++i)
            EXPECT_NO_THROW(( format.write(ostream, options, seqs[i], ids[i], std::ignore) ));

        ostream.flush();
    }
};

TEST_F(write, arg_handling_id_missing)
{
    EXPECT_THROW( (format.write(ostream, options, seqs[0], std::ignore, std::ignore)),
                   std::logic_error );
}

TEST_F(write, arg_handling_id_empty)
{
    EXPECT_THROW( (format.write(ostream, options, seqs[0], std::string_view{""}, std::ignore)),
                   std::runtime_error );
}

TEST_F(write, arg_handling_seq_missing)
{
    EXPECT_THROW( (format.write(ostream, options, std::ignore, ids[0], std::ignore)),
                   std::logic_error );
}

TEST_F(write, arg_handling_seq_empty)
{
    EXPECT_THROW( (format.write(ostream, options, std::string_view{""}, ids[0], std::ignore)),
                   std::runtime_error );
}

TEST_F(write, default_options)
{
    std::string comp
    {
        "ID TEST 1; 4 BP.\n"
        "SQ Sequence 4 BP;\n"
        "ACGT                                                              4\n"
        "//\n"
        "ID Test2; 91 BP.\n"
        "SQ Sequence 91 BP;\n"
        "AGGCTGNAGG CTGNAGGCTG NAGGCTGNAG GCTGNAGGCT GNAGGCTGNA GGCTGNAGGC 60\n"  // linebreak inserted after 60 char
        "TGNAGGCTGN AGGCTGNAGG CTGNAGGCTG N                                91\n"
        "//\n"
        "ID Test3; 24 BP.\n"
        "SQ Sequence 24 BP;\n"
        "GGAGTATAAT ATATATATAT ATAT                                        24\n"
        "//\n"
    };

    do_write_test();

    EXPECT_EQ(ostream.str(), comp);
}

TEST_F(write, seq_qual)
{
    auto convert_to_qualified = ranges::view::transform([] (auto const in)
    {
        return qualified<dna5, phred42>{} = in;
    });

    for (unsigned i = 0; i < 3; ++i)
        EXPECT_NO_THROW(( format.write(ostream,
                                       options,
                                       seqs[i] | convert_to_qualified,
                                       ids[i],
                                       seqs[i] | convert_to_qualified) ));

    ostream.flush();

    std::string comp
    {
        "ID TEST 1; 4 BP.\n"
        "SQ Sequence 4 BP;\n"
        "ACGT                                                              4\n"
        "//\n"
        "ID Test2; 91 BP.\n"
        "SQ Sequence 91 BP;\n"
        "AGGCTGNAGG CTGNAGGCTG NAGGCTGNAG GCTGNAGGCT GNAGGCTGNA GGCTGNAGGC 60\n"  // linebreak inserted after 60 char
        "TGNAGGCTGN AGGCTGNAGG CTGNAGGCTG N                                91\n"
        "//\n"
        "ID Test3; 24 BP.\n"
        "SQ Sequence 24 BP;\n"
        "GGAGTATAAT ATATATATAT ATAT                                        24\n"
        "//\n"
    };

    EXPECT_EQ(ostream.str(), comp);
}

TEST_F(write, complete_header)
{
    std::string comp
    {
        "ID TEST 1; 4 BP.\n"
        "XX\n"
        "SQ Sequence 4 BP;\n"
        "ACGT                                                              4\n"
        "//\n"
        "ID Test2; 91 BP.\n"
        "XX\n"
        "SQ Sequence 91 BP;\n"
        "AGGCTGNAGG CTGNAGGCTG NAGGCTGNAG GCTGNAGGCT GNAGGCTGNA GGCTGNAGGC 60\n"  // linebreak inserted after 60 char
        "TGNAGGCTGN AGGCTGNAGG CTGNAGGCTG N                                91\n"
        "//\n"
        "ID Test3; 24 BP.\n"
        "XX\n"
        "SQ Sequence 24 BP;\n"
        "GGAGTATAAT ATATATATAT ATAT                                        24\n"
        "//\n"
    };
    options.complete_header = true;
    ids[0] = std::string{"ID TEST 1; 4 BP.\nXX\n"};
    ids[1] = std::string{"ID Test2; 91 BP.\nXX\n"};
    ids[2] = std::string{"ID Test3; 24 BP.\nXX\n"};
    do_write_test();

    EXPECT_EQ(ostream.str(), comp);
}
