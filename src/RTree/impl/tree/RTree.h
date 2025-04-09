//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef RTREE_H
#define RTREE_H
#include <cstdint>
#include <vector>

#include "src/RTree/impl/common.h"
#include "src/RTree/impl/metric/MetricManager.h"
#include "src/RTree/impl/strategy/LinearSplitStrategy.h"

namespace RTree
{
    class Node;
    class Point;
    class Data;
    class Region;
    class SplitStrategy;

    class RTree
    {
    public:
        RTree(uint32_t dimension, uint32_t nodeCapacity, const SplitStrategy *splitStrategy);
        ~RTree();

        void insert(const Region &mbr, id_type id);
        bool remove(const Region &mbr, id_type id);

        // Query method - Return result set without using visitor pattern
        std::vector<Data *> intersectionQuery(const Region &query);
        std::vector<Data *> containmentQuery(const Region &query);
        std::vector<Data *> pointQuery(const Point &point);

        // Helper methods
        uint32_t getDimension() const;
        uint32_t getNodeCapacity() const;
        uint32_t getHeight() const;

        // For R*-tree reinsertion
        void handleRstarReinsertion(std::vector<Data *> &dataEntries);

        void construction_finished() const;

        void print_construction_metrics(std::string name) const;

        void print_point_query_metrics(std::string name) const;

        void print_range_query_metrics(std::string name, double window) const;

    private:
        Node *m_root_node;
        uint32_t m_dimension;
        uint32_t m_nodeCapacity;
        const SplitStrategy *m_splitStrategy;

        MetricManager *metricManager = new MetricManager();

        void insertData_impl(Data *data);
    };

}

#endif // RTREE_H
