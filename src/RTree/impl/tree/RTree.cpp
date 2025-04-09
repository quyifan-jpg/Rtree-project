#include "RTree.h"
#include <algorithm>
#include <stack>

#include "src/RTree/impl/node/InternalNode.h"
#include "src/RTree/impl/node/LeafNode.h"
#include "src/RTree/impl/strategy/LinearSplitStrategy.h"

namespace RTree
{
    // Create global static LinearSplitStrategy instance
    RTree::RTree(const uint32_t dimension, const uint32_t nodeCapacity, const SplitStrategy *splitStrategy)
        : m_dimension(dimension), m_nodeCapacity(nodeCapacity), m_splitStrategy(splitStrategy)
    {
        m_root_node = new LeafNode(nodeCapacity, m_splitStrategy, metricManager);
        m_root_node->setTree(this); // Set tree pointer for the root node
    }

    RTree::~RTree()
    {
        delete m_root_node;
        delete metricManager;
    }

    void RTree::insert(const Region &mbr, id_type id)
    {
        auto insertStartTime = std::chrono::high_resolution_clock::now();
        // Create data object
        Data *data = new Data(mbr, id);
        // Insert data
        insertData_impl(data);
        auto insertEndTime = std::chrono::high_resolution_clock::now();
        auto insertDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                                  insertEndTime - insertStartTime)
                                  .count();
        metricManager->record_insertion_time(insertDuration);
    }

    bool RTree::remove(const Region &mbr, id_type id)
    {
        return m_root_node->remove(id, mbr);
    }

    std::vector<Data *> RTree::intersectionQuery(const Region &query)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto result = m_root_node->search(query);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                                  endTime - startTime)
                                  .count();
        metricManager->record_range_query_time(!result.empty(), duration);

        return result;
    }

    std::vector<Data *> RTree::containmentQuery(const Region &query)
    {
        std::vector<Data *> intersectedResults = m_root_node->search(query);
        std::vector<Data *> containedResults;

        // Filter out results that are fully contained
        for (Data *data : intersectedResults)
        {
            if (query.contains(data->getRegion()))
            {
                containedResults.push_back(data);
            }
        }

        return containedResults;
    }

    std::vector<Data *> RTree::pointQuery(const Point &point)
    {
        // Create a tiny region for point query
        double coords[m_dimension * 2];
        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            coords[i] = point.getCoordinate(i);
            coords[i + m_dimension] = point.getCoordinate(i);
        }

        Region pointRegion(coords, coords + m_dimension, m_dimension);

        auto startTime = std::chrono::high_resolution_clock::now();
        const std::vector<Data *> intersectedResults = m_root_node->search(pointRegion);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                                  endTime - startTime)
                                  .count();

        std::vector<Data *> pointResults;

        // Filter out results that contain the point
        for (Data *data : intersectedResults)
        {
            if (data->getRegion().contains(point))
            {
                pointResults.push_back(data);
            }
        }

        metricManager->record_point_query_time(!pointResults.empty(), duration);

        return pointResults;
    }

    uint32_t RTree::getDimension() const
    {
        return m_dimension;
    }

    uint32_t RTree::getNodeCapacity() const
    {
        return m_nodeCapacity;
    }

    uint32_t RTree::getHeight() const
    {
        if (!m_root_node)
        {
            return 0; // Empty tree has height 0
        }

        return m_root_node->getHeight();
    }

    void RTree::construction_finished() const {
        std::vector<double> node_capacity_percent = {};
        // capacity -> average, mean

        std::stack<Node*> s;
        s.push(m_root_node);

        if(m_root_node->isLeaf()) {
            node_capacity_percent.push_back(static_cast<double>(m_root_node->size()) /static_cast<double>(m_nodeCapacity));
        } else {
            while (!s.empty()) {
                const auto node = s.top();
                s.pop();
                for (auto child : node->children()) {
                    if(child->isLeaf()) {
                        double percent = static_cast<double>(child->size()) / static_cast<double>(m_nodeCapacity);
                        node_capacity_percent.push_back(percent);
                    } else {
                        s.push(child);
                    }
                }
            }
        }

        uint32_t height = getHeight();

        metricManager->record_post_construction_metrics(height, node_capacity_percent);
    }

    void RTree::print_construction_metrics(std::string name) const {
        metricManager->print_construction_metrics(name);
    }

    void RTree::print_point_query_metrics(std::string name) const {
        metricManager->print_point_query_metrics(name);
    }

    void RTree::print_range_query_metrics(std::string name, double window) const {
        metricManager->print_range_query_metrics(name, window);
    }

    void RTree::insertData_impl(Data *data) {
        // Insert data into the root node
        m_root_node->insert(data);

        // If the root node splits, need to create a new root node
        if (m_root_node->shouldSplit())
        {
            auto [original, newNode] = m_root_node->split();

            if (newNode)
            {
                // Set tree pointer for new node
                newNode->setTree(this);

                // Create a new internal node as root
                auto *newRoot = new InternalNode(m_nodeCapacity, m_splitStrategy, metricManager);
                newRoot->setTree(this); // Set tree pointer for new root
                newRoot->addChild(original);
                newRoot->addChild(newNode);

                m_root_node = newRoot;
            }
        }
    }

    void RTree::handleRstarReinsertion(std::vector<Data *> &dataEntries)
    {
        // So far just call inseart again
        for (Data *data : dataEntries)
        {
            Data *dataClone = data->clone();
            insertData_impl(dataClone);
        }
    }
} // namespace RTree