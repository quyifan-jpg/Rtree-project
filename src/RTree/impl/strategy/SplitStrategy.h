//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef SPLITSTRATEGY_H
#define SPLITSTRATEGY_H
#include <vector>

#include "src/RTree/impl/Data.h"

namespace RTree {
    class Node;

    class SplitStrategy
    {
    public:
        virtual ~SplitStrategy() = default;

        virtual std::pair<std::vector<Data *>, std::vector<Data *>>
        splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const = 0;
        virtual std::pair<std::vector<Node *>, std::vector<Node *>>
        splitInternalChildren(const std::vector<Node *> &children, uint32_t capacity) const = 0;
        virtual std::string getName() const = 0;
    };
}

#endif //SPLITSTRATEGY_H
