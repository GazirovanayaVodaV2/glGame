#include "compression.hpp"

#include <algorithm>
#include <unordered_map>

#include "../utils/logger/logger.hpp"

static inline float getEntropy(std::span<std::uint16_t> data) {
    std::unordered_map<std::uint16_t, int> dict;

    for (const auto& value : data) {
        dict[value]++;
    }

    float h = 0.0f;

    for (const auto& [num, count] : dict) {
        float p = (float)count / data.size();
        h -= p * std::log2f(p);
    }

    return h;
}

compression::Result compression::compress(std::span<std::uint16_t> data)
{
    if (data.empty()) {
        sizeOfOgVector = 0;
        return Result::FAILED;
    }

    sizeOfOgVector = data.size();

    auto entropy = getEntropy(data);
    compressionIsPossible = entropy < sizeof(std::uint16_t) * 4;

    if (compressionIsPossible) {
        compressedDataT tempData;
        std::uint16_t currentBlock = data[0];
        int countOfBlock = 1;

        for (std::size_t i = 1; i < sizeOfOgVector; i++) {
            if (data[i] == currentBlock) {
                countOfBlock++;
            }
            else {
                tempData.emplace_back(currentBlock, countOfBlock);
                currentBlock = data[i];
                countOfBlock = 1;
            }
        }
        tempData.emplace_back(currentBlock, countOfBlock);

        m_data = std::move(tempData);

        return Result::SUCCESS;
    }
    else {
        logger::Console::print<logger::Level::DEBUG>("Entropy to high for compression!");
        m_data = data;
        return Result::FAILED;
    }
}


void compression::decompress(std::vector<std::uint16_t>& out) {
    out.reserve(sizeOfOgVector);

    if (compressionIsPossible) {
        const auto& compressed = std::get<compressedDataT>(m_data);
        for (const auto& [id, count] : compressed) {
            out.insert(out.end(), count, id);
        }
    }
    else {
        auto og_data = std::get<ogDataPtr>(m_data);
        out.assign(og_data.begin(), og_data.end());
    }
}
