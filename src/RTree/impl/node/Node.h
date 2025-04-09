//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef NODE_H
#define NODE_H
#include <vector>

#include "src/RTree/impl/common.h"
#include "src/RTree/impl/metric/MetricManager.h"

namespace RTree
{
    class Data;
    class Region;
    class SplitStrategy;
    class RTree; // Forward declaration

    class Node
    {
    public:
        Node(const SplitStrategy* strategy, MetricManager* metric_manager) :
        metric_manager(metric_manager), m_splitStrategy(strategy) {}

        virtual ~Node() = default;
        virtual bool isLeaf() const = 0;
        virtual const Region &getMBR() const = 0;
        virtual void insert(Data *data) = 0;
        virtual bool remove(id_type id, const Region &mbr) = 0;
        virtual bool isEmpty() = 0;
        virtual unsigned long size() = 0;
        virtual std::vector<Node *> children() = 0;
        virtual std::vector<Data *> search(const Region &query) = 0;
        virtual bool shouldSplit() const = 0;
        virtual std::pair<Node *, Node *> split() = 0;
        virtual uint32_t getHeight() const = 0;
        virtual Node *chooseSubtree(const Region &mbr)
        {
            return this;
        };

        // Set tree pointer
        void setTree(RTree *tree)
        {
            m_tree = tree;
        }

        RTree *getTree() const
        {
            return m_tree;
        }

    protected:
        MetricManager *metric_manager;
        const SplitStrategy *m_splitStrategy = nullptr;
        bool overflow = true;        // Flag to track if this is the first overflow at this level
        double reinsertFactor = 0.3; // Percentage of entries to reinsert (30%)
        RTree *m_tree = nullptr;     // Pointer to parent tree
    };
}

#endif // NODE_H
