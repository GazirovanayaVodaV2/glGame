#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include <bit>
#include <utility>
#include <span>
#include <variant>

class compression {
    private:
        using compressedDataT = std::vector<std::pair<std::uint16_t, int>>;
        using ogDataPtr = std::span<std::uint16_t>;
        using dataVatiant = std::variant<
            compressedDataT,
            ogDataPtr
        >;
        dataVatiant m_data;
        bool compressionIsPossible = true;
        std::size_t sizeOfOgVector{};
    public:
        enum class Result {
            SUCCESS,
            FAILED,
        };

        compression() = default;
        ~compression() = default;

        Result compress(std::span<std::uint16_t> data);
        void decompress(std::vector<std::uint16_t>& out);
};