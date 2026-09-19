#include "compression.hpp"
#include <algorithm>

void compression::compress(std::span<std::uint16_t> data)
{
    if (data.size() == 0) {
        return;
    }
    /*struct firstStageCompression_t {
        std::uint16_t id;
        int count;
    }
    std::uint16_t currentBlock = data[0];
    int countOfBlock;
    std::vector<firstStageCompression_t> firstStageCompression;
    firstStageCompression.reserve(data.size());
    for (std::size_t i{}; i < data.size(); i++) {
        if (currentBlock == data[i]) {
            countOfBlock++;
        } else {
            firstStageCompression.emplace_back(firstStageCompression_t{currentBlock, countOfBlock});
            countOfBlock = 0;
            currentBlock = data[i];
        }
    }*/

    
    m_data.clear();
    if (data.empty()) {
        sizeOfOgVector = 0;
        return;
    }

    sizeOfOgVector = data.size();
    std::uint16_t currentBlock = data[0];
    int countOfBlock = 1;

    for (std::size_t i = 1; i < sizeOfOgVector; i++) { 
        if (data[i] == currentBlock) {
            countOfBlock++;
        } else {
            m_data.emplace_back(currentBlock, countOfBlock);
            currentBlock = data[i];
            countOfBlock = 1; 
        }
    }
    m_data.emplace_back(currentBlock, countOfBlock);
    /*auto max_value = std::max_element(
        data.begin(),
        data.end()
    );

    auto firstBitinMax = std::countr_zero(max_value);
    m_bitsPerEl = m_bitsPerEl - firstBitinMax; */

}

/*std::uint16_t compression::operator[](std::size_t i) 
{
    return 0; 

}*/

void compression::decompress(std::vector<std::uint16_t>& out) {
    out.reserve(sizeOfOgVector);

    for (auto compressedValIt = m_data.begin(); compressedValIt < m_data.end(); compressedValIt++)
    {
        auto id = (*compressedValIt).first;
        auto count = (*compressedValIt).second;
        for (std::size_t i{}; i < count; i++) {
            out.push_back(id);
        }
    }
}
