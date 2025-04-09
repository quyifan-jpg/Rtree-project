//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef QUADRATICSPLITSTRATEGY_H
#define QUADRATICSPLITSTRATEGY_H
#include "SplitStrategy.h"

namespace RTree {

    class QuadraticSplitStrategy : public SplitStrategy
    {
    public:
        // Add explicit default constructor and destructor
        QuadraticSplitStrategy();
        ~QuadraticSplitStrategy() override;

        std::pair<std::vector<Data *>, std::vector<Data *>>
        splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const override;

        std::pair<std::vector<Node *>, std::vector<Node *>>
        splitInternalChildren(const std::vector<Node *> &children, uint32_t capacity) const override;

        std::string getName() const override;

    private:
        std::pair<size_t, size_t> pickSeeds(const std::vector<Data *> &entries) const;
        std::pair<size_t, size_t> pickSeeds(const std::vector<Node *> &children) const;
        double calculateAreaEnlargement(const Region &r1, const Region &r2) const;
    };


}

#endif //QUADRATICSPLITSTRATEGY_H
