#include <iostream>
#include "sdsl_compressor.hpp"

int main(int argc, char *argv[])
{
    if (argc != 5) {
        std::cerr << "error" << std::endl;
    } else {
        w_sdsl::compressInt(argv[1]);
        w_sdsl::compressBitmap(argv[2]);
        w_sdsl::compressInt(argv[3]);
        w_sdsl::compressBitmap(argv[4]);
    }
    return 0;
}


//g++ 