#include "sdsl_compressor.hpp"

namespace w_sdsl {
    uint32_t compressInt(std::string path)
    {
        std::cout << "path: " << path << std::endl;
        assert(validateExtension(path, "bin")); 

        sdsl::wm_int<sdsl::rrr_vector<15>> wm_int; 
        sdsl::construct(wm_int, path, 4);

        while (path.back() != '.') path.pop_back(); 
        path += "sdsl";
        std::cout << " wm file " << path  << std::endl;

        sdsl::store_to_file(wm_int, path);

        return size_in_bytes(wm_int)*8; 
    }

    uint32_t compressBitmap(std::string path)
    {
        std::cout << "path: " << path << std::endl;
        typedef bool binVar; 
        std::ifstream binFile;
        binFile.open(path, std::ifstream::in);

        assert(binFile.is_open());

        binVar* buffer = new binVar(0);
        std::vector<binVar> temp; 

        while (not binFile.eof()) {
            binFile.read((char*)buffer, sizeof(binVar));
            temp.push_back(*buffer);
        }

        delete buffer;

        size_t size = temp.size(); 
        sdsl::bit_vector b_vector(size, 0);

        for (size_t i = 0; i < size; i++) {
            if (temp.at(i) == 1) { 
                b_vector[i] = 1; 
            }
        }

        uInt bitsize = 0;
        sdsl::rrr_vector<15> rrrb(b_vector);

        while(path.back() != '.') path.pop_back();
        path.pop_back();
        path += "-rrr-64.sdsl"; 

        std::cout << "bitmap file " << path << std::endl;
        sdsl::store_to_file(rrrb, path);
        return size_in_bytes(rrrb)*8;
    }

    sdsl::wm_int<sdsl::rrr_vector<15>> readCompressedInt(const std::string path)
    {
        assert(validateExtension(path, "sdsl")); 
        sdsl::wm_int<sdsl::rrr_vector<15>> wm_int;
        sdsl::load_from_file(wm_int, path.c_str());
        std::cout << "loaded: " << path << std::endl; 
        return wm_int;
    }
    sdsl::rrr_vector<15> readCompressedBitmap(const std::string path)
    {
        assert(validateExtension(path, "sdsl")); 
        sdsl::rrr_vector<15> rrr;
        sdsl::load_from_file(rrr, path.c_str());
        std::cout << "loaded: " << path << std::endl; 
        return rrr;
    }
};