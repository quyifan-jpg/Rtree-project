//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef LINEARSPLITSTRATEGY_H
#define LINEARSPLITSTRATEGY_H
#include "SplitStrategy.h"

namespace RTree {

    class LinearSplitStrategy : public SplitStrategy
    {
    public:
        // Add explicit default constructor and destructor
        LinearSplitStrategy();
        ~LinearSplitStrategy() override;

        std::pair<std::vector<Data *>, std::vector<Data *>>
        splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const override;

        std::pair<std::vector<Node *>, std::vector<Node *>>
        splitInternalChildren(const std::vector<Node *> &children, uint32_t capacity) const override;

        std::string getName() const override;
    };


}

#endif //LINEARSPLITSTRATEGY_H
