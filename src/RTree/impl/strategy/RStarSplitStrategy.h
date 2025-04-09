//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef RSTARSPLITSTRATEGY_H
#define RSTARSPLITSTRATEGY_H
#include "SplitStrategy.h"

namespace RTree {
    class RStarSplitStrategy : public SplitStrategy
    {
    public:
        // Add explicit default constructor and destructor
        RStarSplitStrategy();
        ~RStarSplitStrategy() override;

        std::pair<std::vector<Data *>, std::vector<Data *>>
        splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const override;

        std::pair<std::vector<Node *>, std::vector<Node *>>
        splitInternalChildren(const std::vector<Node *> &children, uint32_t capacity) const override;

        std::string getName() const override;

    private:
        std::pair<size_t, size_t> pickSeeds(const std::vector<Data *> &entries) const;
        // Helper method to calculate overlap area between two regions
        double calculateOverlapArea(const Region &region1, const Region &region2, uint32_t dimension) const;
    };



}

#endif //RSTARSPLITSTRATEGY_H
