//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef LEAFNODE_H
#define LEAFNODE_H
#include <cstdint>

#include "Node.h"
#include "src/RTree/impl/Region.h"

namespace RTree
{
    class LeafNode : public Node
    {
    public:
        LeafNode(uint32_t capacity,
            const SplitStrategy *splitStrategy,
            MetricManager* metric_manager);
        ~LeafNode() override;

        bool isLeaf() const override;
        const Region &getMBR() const override;
        void insert(Data *data) override;
        bool remove(id_type id, const Region &mbr) override;
        bool isEmpty() override;
        unsigned long size() override;
        std::vector<Node *> children() override;
        std::vector<Data *> search(const Region &query) override;
        bool shouldSplit() const override;
        std::pair<Node *, Node *> split() override;
        uint32_t getHeight() const override;

        // For R*-tree forced reinsertion
        std::vector<Data *> reinsertForRstar();

    private:
        uint32_t m_capacity;
        std::vector<Data *> m_entries;
        Region m_mbr;

        void recalculateMBR();

        friend class RTree;
        friend class InternalNode;
    };

}

#endif // LEAFNODE_H
