// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: Response.capnp

#include "Response.capnp.h"

namespace capnp {
namespace schemas {
static const ::capnp::_::AlignedData<40> b_9ec349be4999c4ca = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    202, 196, 153,  73, 190,  73, 195, 158,
     15,   0,   0,   0,   1,   0,   2,   0,
    165, 236,  65, 120, 214, 232,   6, 192,
      0,   0,   7,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 162,   0,   0,   0,
     29,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0, 119,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     82, 101, 115, 112, 111, 110, 115, 101,
     46,  99,  97, 112, 110, 112,  58,  67,
    101, 108, 108,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
      8,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     41,   0,   0,   0,  50,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     36,   0,   0,   0,   3,   0,   1,   0,
     48,   0,   0,   0,   2,   0,   1,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
    226,  97, 164, 202, 163,  31, 175, 218,
     45,   0,   0,   0,  58,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    111, 119, 110, 101, 114,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     97, 116, 116,  97,  99, 107,   0,   0, }
};
::capnp::word const* const bp_9ec349be4999c4ca = b_9ec349be4999c4ca.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_9ec349be4999c4ca[] = {
  &s_daaf1fa3caa461e2,
};
static const uint16_t m_9ec349be4999c4ca[] = {1, 0};
static const uint16_t i_9ec349be4999c4ca[] = {0, 1};
const ::capnp::_::RawSchema s_9ec349be4999c4ca = {
  0x9ec349be4999c4ca, b_9ec349be4999c4ca.words, 40, d_9ec349be4999c4ca, m_9ec349be4999c4ca,
  1, 2, i_9ec349be4999c4ca, nullptr, nullptr, { &s_9ec349be4999c4ca, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<47> b_daaf1fa3caa461e2 = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    226,  97, 164, 202, 163,  31, 175, 218,
     20,   0,   0,   0,   1,   0,   2,   0,
    202, 196, 153,  73, 190,  73, 195, 158,
      0,   0,   7,   0,   1,   0,   2,   0,
      3,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 218,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0, 119,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     82, 101, 115, 112, 111, 110, 115, 101,
     46,  99,  97, 112, 110, 112,  58,  67,
    101, 108, 108,  46,  97, 116, 116,  97,
     99, 107,   0,   0,   0,   0,   0,   0,
      8,   0,   0,   0,   3,   0,   4,   0,
      0,   0, 255, 255,  32,   0,   0,   0,
      0,   0,   1,   0,   1,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
     41,   0,   0,   0,  34,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     36,   0,   0,   0,   3,   0,   1,   0,
     48,   0,   0,   0,   2,   0,   1,   0,
      1,   0, 254, 255,   2,   0,   0,   0,
      0,   0,   1,   0,   2,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     45,   0,   0,   0,  42,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     40,   0,   0,   0,   3,   0,   1,   0,
     52,   0,   0,   0,   2,   0,   1,   0,
     99,  97, 110,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      1,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    117, 110, 105, 116,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_daaf1fa3caa461e2 = b_daaf1fa3caa461e2.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_daaf1fa3caa461e2[] = {
  &s_9ec349be4999c4ca,
};
static const uint16_t m_daaf1fa3caa461e2[] = {0, 1};
static const uint16_t i_daaf1fa3caa461e2[] = {0, 1};
const ::capnp::_::RawSchema s_daaf1fa3caa461e2 = {
  0xdaaf1fa3caa461e2, b_daaf1fa3caa461e2.words, 47, d_daaf1fa3caa461e2, m_daaf1fa3caa461e2,
  1, 2, i_daaf1fa3caa461e2, nullptr, nullptr, { &s_daaf1fa3caa461e2, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<42> b_a9b9ddaa203d4eec = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    236,  78,  61,  32, 170, 221, 185, 169,
     15,   0,   0,   0,   1,   0,   1,   0,
    165, 236,  65, 120, 214, 232,   6, 192,
      1,   0,   7,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 170,   0,   0,   0,
     29,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0, 119,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     82, 101, 115, 112, 111, 110, 115, 101,
     46,  99,  97, 112, 110, 112,  58,  69,
    110, 101, 109, 121,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
      8,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     41,   0,   0,   0,  74,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     40,   0,   0,   0,   3,   0,   1,   0,
     52,   0,   0,   0,   2,   0,   1,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
    197,   3, 137, 137,  41,  89,  70, 224,
     49,   0,   0,   0,  82,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    112, 111, 115, 105, 116, 105, 111, 110,
      0,   0,   0,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
    179,  43, 219,  13,  59,  24, 160, 236,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    100, 105, 114, 101,  99, 116, 105, 111,
    110,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_a9b9ddaa203d4eec = b_a9b9ddaa203d4eec.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_a9b9ddaa203d4eec[] = {
  &s_e0465929898903c5,
  &s_eca0183b0ddb2bb3,
};
static const uint16_t m_a9b9ddaa203d4eec[] = {1, 0};
static const uint16_t i_a9b9ddaa203d4eec[] = {0, 1};
const ::capnp::_::RawSchema s_a9b9ddaa203d4eec = {
  0xa9b9ddaa203d4eec, b_a9b9ddaa203d4eec.words, 42, d_a9b9ddaa203d4eec, m_a9b9ddaa203d4eec,
  2, 2, i_a9b9ddaa203d4eec, nullptr, nullptr, { &s_a9b9ddaa203d4eec, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<49> b_e0465929898903c5 = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    197,   3, 137, 137,  41,  89,  70, 224,
     21,   0,   0,   0,   1,   0,   1,   0,
    236,  78,  61,  32, 170, 221, 185, 169,
      1,   0,   7,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 250,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0, 119,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     82, 101, 115, 112, 111, 110, 115, 101,
     46,  99,  97, 112, 110, 112,  58,  69,
    110, 101, 109, 121,  46, 100, 105, 114,
    101,  99, 116, 105, 111, 110,   0,   0,
      8,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     41,   0,   0,   0,  74,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     40,   0,   0,   0,   3,   0,   1,   0,
     52,   0,   0,   0,   2,   0,   1,   0,
      1,   0,   0,   0,   1,   0,   0,   0,
      0,   0,   1,   0,   2,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     49,   0,   0,   0,  90,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     48,   0,   0,   0,   3,   0,   1,   0,
     60,   0,   0,   0,   2,   0,   1,   0,
    118, 101, 114, 116, 105,  99,  97, 108,
      0,   0,   0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
    153,  13, 183, 195, 216,  71, 166, 154,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    104, 111, 114, 105, 122, 111, 110, 116,
     97, 108,   0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
    153,  13, 183, 195, 216,  71, 166, 154,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_e0465929898903c5 = b_e0465929898903c5.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_e0465929898903c5[] = {
  &s_9aa647d8c3b70d99,
  &s_a9b9ddaa203d4eec,
};
static const uint16_t m_e0465929898903c5[] = {1, 0};
static const uint16_t i_e0465929898903c5[] = {0, 1};
const ::capnp::_::RawSchema s_e0465929898903c5 = {
  0xe0465929898903c5, b_e0465929898903c5.words, 49, d_e0465929898903c5, m_e0465929898903c5,
  2, 2, i_e0465929898903c5, nullptr, nullptr, { &s_e0465929898903c5, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<94> b_e3f0a29933f791f6 = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    246, 145, 247,  51, 153, 162, 240, 227,
     15,   0,   0,   0,   1,   0,   2,   0,
    165, 236,  65, 120, 214, 232,   6, 192,
      1,   0,   7,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 162,   0,   0,   0,
     29,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0,  31,   1,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     82, 101, 115, 112, 111, 110, 115, 101,
     46,  99,  97, 112, 110, 112,  58,  85,
    110, 105, 116,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
     20,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    125,   0,   0,   0,  50,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    120,   0,   0,   0,   3,   0,   1,   0,
    132,   0,   0,   0,   2,   0,   1,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    129,   0,   0,   0,  74,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    128,   0,   0,   0,   3,   0,   1,   0,
    140,   0,   0,   0,   2,   0,   1,   0,
      2,   0,   0,   0,   2,   0,   0,   0,
      0,   0,   1,   0,   2,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    137,   0,   0,   0,  82,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    136,   0,   0,   0,   3,   0,   1,   0,
    148,   0,   0,   0,   2,   0,   1,   0,
      3,   0,   0,   0,   2,   0,   0,   0,
      0,   0,   1,   0,   3,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
    145,   0,   0,   0,  58,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    140,   0,   0,   0,   3,   0,   1,   0,
    152,   0,   0,   0,   2,   0,   1,   0,
      4,   0,   0,   0,   3,   0,   0,   0,
      0,   0,   1,   0,   4,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
    149,   0,   0,   0,  58,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    144,   0,   0,   0,   3,   0,   1,   0,
    156,   0,   0,   0,   2,   0,   1,   0,
    111, 119, 110, 101, 114,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    112, 111, 115, 105, 116, 105, 111, 110,
      0,   0,   0,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
    179,  43, 219,  13,  59,  24, 160, 236,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    100, 105, 114, 101,  99, 116, 105, 111,
    110,   0,   0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
    153,  13, 183, 195, 216,  71, 166, 154,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    104, 101,  97, 108, 116, 104,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   3,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    107, 105, 108, 108, 101, 114,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   6,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_e3f0a29933f791f6 = b_e3f0a29933f791f6.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_e3f0a29933f791f6[] = {
  &s_9aa647d8c3b70d99,
  &s_eca0183b0ddb2bb3,
};
static const uint16_t m_e3f0a29933f791f6[] = {2, 3, 4, 0, 1};
static const uint16_t i_e3f0a29933f791f6[] = {0, 1, 2, 3, 4};
const ::capnp::_::RawSchema s_e3f0a29933f791f6 = {
  0xe3f0a29933f791f6, b_e3f0a29933f791f6.words, 94, d_e3f0a29933f791f6, m_e3f0a29933f791f6,
  2, 5, i_e3f0a29933f791f6, nullptr, nullptr, { &s_e3f0a29933f791f6, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<101> b_b40482839d54ca4a = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
     74, 202,  84, 157, 131, 130,   4, 180,
     15,   0,   0,   0,   1,   0,   2,   0,
    165, 236,  65, 120, 214, 232,   6, 192,
      4,   0,   7,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 194,   0,   0,   0,
     29,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0,  31,   1,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     82, 101, 115, 112, 111, 110, 115, 101,
     46,  99,  97, 112, 110, 112,  58,  82,
    101, 115, 112, 111, 110, 115, 101,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
     20,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    125,   0,   0,   0,  58,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    120,   0,   0,   0,   3,   0,   1,   0,
    132,   0,   0,   0,   2,   0,   1,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
     16,  27, 209, 144,  36,  34, 138, 163,
    129,   0,   0,   0,  42,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      2,   0,   0,   0,   1,   0,   0,   0,
      0,   0,   1,   0,   4,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    105,   0,   0,   0,  50,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    100,   0,   0,   0,   3,   0,   1,   0,
    144,   0,   0,   0,   2,   0,   1,   0,
      3,   0,   0,   0,   2,   0,   0,   0,
      0,   0,   1,   0,   5,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    141,   0,   0,   0,  66,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    136,   0,   0,   0,   3,   0,   1,   0,
    164,   0,   0,   0,   2,   0,   1,   0,
      4,   0,   0,   0,   3,   0,   0,   0,
      0,   0,   1,   0,   6,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    161,   0,   0,   0,  50,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    156,   0,   0,   0,   3,   0,   1,   0,
    184,   0,   0,   0,   2,   0,   1,   0,
    115, 116,  97, 116, 117, 115,   0,   0,
     12,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     12,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    105, 110, 102, 111,   0,   0,   0,   0,
     99, 101, 108, 108, 115,   0,   0,   0,
     14,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   3,   0,   1,   0,
     14,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   3,   0,   1,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
    202, 196, 153,  73, 190,  73, 195, 158,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     14,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    101, 110, 101, 109, 105, 101, 115,   0,
     14,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   3,   0,   1,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
    236,  78,  61,  32, 170, 221, 185, 169,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     14,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    117, 110, 105, 116, 115,   0,   0,   0,
     14,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   3,   0,   1,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
    246, 145, 247,  51, 153, 162, 240, 227,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     14,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_b40482839d54ca4a = b_b40482839d54ca4a.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_b40482839d54ca4a[] = {
  &s_9ec349be4999c4ca,
  &s_a38a222490d11b10,
  &s_a9b9ddaa203d4eec,
  &s_e3f0a29933f791f6,
};
static const uint16_t m_b40482839d54ca4a[] = {2, 3, 1, 0, 4};
static const uint16_t i_b40482839d54ca4a[] = {0, 1, 2, 3, 4};
const ::capnp::_::RawSchema s_b40482839d54ca4a = {
  0xb40482839d54ca4a, b_b40482839d54ca4a.words, 101, d_b40482839d54ca4a, m_b40482839d54ca4a,
  4, 5, i_b40482839d54ca4a, nullptr, nullptr, { &s_b40482839d54ca4a, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<62> b_a38a222490d11b10 = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
     16,  27, 209, 144,  36,  34, 138, 163,
     24,   0,   0,   0,   1,   0,   2,   0,
     74, 202,  84, 157, 131, 130,   4, 180,
      4,   0,   7,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 234,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0, 175,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     82, 101, 115, 112, 111, 110, 115, 101,
     46,  99,  97, 112, 110, 112,  58,  82,
    101, 115, 112, 111, 110, 115, 101,  46,
    105, 110, 102, 111,   0,   0,   0,   0,
     12,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     69,   0,   0,   0,  42,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     64,   0,   0,   0,   3,   0,   1,   0,
     76,   0,   0,   0,   2,   0,   1,   0,
      1,   0,   0,   0,   1,   0,   0,   0,
      0,   0,   1,   0,   2,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     73,   0,   0,   0,  50,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     68,   0,   0,   0,   3,   0,   1,   0,
     80,   0,   0,   0,   2,   0,   1,   0,
      2,   0,   0,   0,   2,   0,   0,   0,
      0,   0,   1,   0,   3,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     77,   0,   0,   0,  42,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     72,   0,   0,   0,   3,   0,   1,   0,
     84,   0,   0,   0,   2,   0,   1,   0,
    111, 119, 110, 115,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    108, 101, 118, 101, 108,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    116, 105,  99, 107,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_a38a222490d11b10 = b_a38a222490d11b10.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_a38a222490d11b10[] = {
  &s_b40482839d54ca4a,
};
static const uint16_t m_a38a222490d11b10[] = {1, 0, 2};
static const uint16_t i_a38a222490d11b10[] = {0, 1, 2};
const ::capnp::_::RawSchema s_a38a222490d11b10 = {
  0xa38a222490d11b10, b_a38a222490d11b10.words, 62, d_a38a222490d11b10, m_a38a222490d11b10,
  1, 3, i_a38a222490d11b10, nullptr, nullptr, { &s_a38a222490d11b10, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
}  // namespace schemas
}  // namespace capnp

// =======================================================================================

namespace protocol {

// Cell
constexpr uint16_t Cell::_capnpPrivate::dataWordSize;
constexpr uint16_t Cell::_capnpPrivate::pointerCount;
#if !CAPNP_LITE
constexpr ::capnp::Kind Cell::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Cell::_capnpPrivate::schema;
#endif  // !CAPNP_LITE

// Cell::Attack
constexpr uint16_t Cell::Attack::_capnpPrivate::dataWordSize;
constexpr uint16_t Cell::Attack::_capnpPrivate::pointerCount;
#if !CAPNP_LITE
constexpr ::capnp::Kind Cell::Attack::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Cell::Attack::_capnpPrivate::schema;
#endif  // !CAPNP_LITE

// Enemy
constexpr uint16_t Enemy::_capnpPrivate::dataWordSize;
constexpr uint16_t Enemy::_capnpPrivate::pointerCount;
#if !CAPNP_LITE
constexpr ::capnp::Kind Enemy::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Enemy::_capnpPrivate::schema;
#endif  // !CAPNP_LITE

// Enemy::Direction
constexpr uint16_t Enemy::Direction::_capnpPrivate::dataWordSize;
constexpr uint16_t Enemy::Direction::_capnpPrivate::pointerCount;
#if !CAPNP_LITE
constexpr ::capnp::Kind Enemy::Direction::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Enemy::Direction::_capnpPrivate::schema;
#endif  // !CAPNP_LITE

// Unit
constexpr uint16_t Unit::_capnpPrivate::dataWordSize;
constexpr uint16_t Unit::_capnpPrivate::pointerCount;
#if !CAPNP_LITE
constexpr ::capnp::Kind Unit::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Unit::_capnpPrivate::schema;
#endif  // !CAPNP_LITE

// Response
constexpr uint16_t Response::_capnpPrivate::dataWordSize;
constexpr uint16_t Response::_capnpPrivate::pointerCount;
#if !CAPNP_LITE
constexpr ::capnp::Kind Response::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Response::_capnpPrivate::schema;
#endif  // !CAPNP_LITE

// Response::Info
constexpr uint16_t Response::Info::_capnpPrivate::dataWordSize;
constexpr uint16_t Response::Info::_capnpPrivate::pointerCount;
#if !CAPNP_LITE
constexpr ::capnp::Kind Response::Info::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Response::Info::_capnpPrivate::schema;
#endif  // !CAPNP_LITE


}  // namespace

