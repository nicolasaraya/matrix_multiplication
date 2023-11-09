#ifndef sdsdl_compressor
#define sdsl_compressor

#include <string.h>
#include <fstream>
#include <vector>

#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/suffix_arrays.hpp>

#include "../../biclique_extraction/Utils/Utils.hpp"

namespace w_sdsl{
    uint32_t compressInt(std::string); 
    uint32_t compressBitmap(std::string);

    sdsl::wm_int<sdsl::rrr_vector<15>> readCompressedInt(const std::string); 
    sdsl::rrr_vector<15> readCompressedBitmap(const std::string);

};

#endif