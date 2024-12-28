#pragma once

#include <cstdint>

namespace iqdb {

using imageId = uint32_t;
using postId = uint32_t; // An external (Danbooru) post ID.
using iqdbId = uint32_t; // An internal IQDB image ID.

// The type used for calculating similarity scores during queries, and for
// storing `avgl` values in the `m_info` array.
using Score = float;

}
