#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include <bit>
#include <utility>
#include <span>

class compression {
    private:
        /*std::map<std::size_t, std::uint16_t> m_alphabet;
        std::byte m_bitsPerEl = sizeof(std::uint16_t) * 8;
        std::vector<std::uint32_t> m_bitset;*/

        std::vector<std::pair<std::uint16_t, int>> m_data;
        std::size_t sizeOfOgVector{};
    public:
        compression() = default;
        ~compression() = default;

        void compress(std::span<std::uint16_t> data);
        
        //int operator[](std::size_t i);
        void decompress(std::vector<std::uint16_t>& out);
};