//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef INTERNALNODE_H
#define INTERNALNODE_H
#include <cstdint>
#include <vector>

#include "Node.h"
#include "src/RTree/impl/common.h"
#include "src/RTree/impl/Region.h"

namespace RTree
{
    class Data;
    class Region;

    class InternalNode : public Node
    {
    public:
        InternalNode(uint32_t capacity,
            const SplitStrategy *splitStrategy,
            MetricManager* metricManager);
        ~InternalNode() override;

        bool isLeaf() const override;
        const Region &getMBR() const override;
        void insert(Data *data) override;
        bool remove(id_type id, const Region &mbr) override;
        bool isEmpty() override;
        std::vector<Node *> children() override;
        unsigned long size() override;
        std::vector<Data *> search(const Region &query) override;
        bool shouldSplit() const override;
        std::pair<Node *, Node *> split() override;
        uint32_t getHeight() const override;

        void addChild(Node *child);

    private:
        uint32_t m_capacity;
        std::vector<Node *> m_children;
        Region m_mbr;
        uint32_t total_entries = 0; // Track total entries in subtree

        void recalculateMBR();
        Node *chooseSubtree(const Region &mbr) const;

        friend class RTree;
    };

}

#endif // INTERNALNODE_H
