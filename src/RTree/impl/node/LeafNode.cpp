#include "LeafNode.h"

#include <algorithm>
#include <iostream>
#include "src/RTree/impl/Data.h"
#include "src/RTree/impl/strategy/SplitStrategy.h"
#include "src/RTree/impl/tree/RTree.h"

namespace RTree
{

    LeafNode::LeafNode(uint32_t capacity, const SplitStrategy *splitStrategy, MetricManager* metric_manager)
        : Node(splitStrategy, metric_manager), m_capacity(capacity), m_mbr(0) {
        // Initialize MBR as an invalid region with dimension
    }

    LeafNode::~LeafNode()
    {
        for (Data *data : m_entries)
        {
            delete data;
        }
    }

    bool LeafNode::isLeaf() const
    {
        return true;
    }

    const Region &LeafNode::getMBR() const
    {
        return m_mbr;
    }

    void LeafNode::insert(Data *data)
    {
        // Add data to this leaf
        m_entries.push_back(data);
        recalculateMBR();

        // Check if this node needs to split
        if (shouldSplit())
        {
            if (m_splitStrategy->getName() == "RStarSplit" && overflow)
            {
                // Mark that we've handled overflow once for this node
                overflow = false;
                std::vector<Data *> entriesToReinsert = reinsertForRstar();
                m_tree->handleRstarReinsertion(entriesToReinsert);
                        }
        }
    }

    bool LeafNode::remove(id_type id, const Region &mbr)
    {
        auto it = std::find_if(m_entries.begin(), m_entries.end(),
                               [id](const Data *data)
                               { return data->getIdentifier() == id; });

        if (it != m_entries.end())
        {
            delete *it;
            m_entries.erase(it);
            recalculateMBR();
            return true;
        }

        return false;
    }

    bool LeafNode::isEmpty()
    {
        return m_entries.empty();
    }

    unsigned long LeafNode::size() {
        return m_entries.size();
    }

    std::vector<Node *> LeafNode::children() {
        return {};
    }


    std::vector<Data *> LeafNode::search(const Region &query)
    {
        std::vector<Data *> results;

        for (Data *data : m_entries)
        {
            if (data->getRegion().intersects(query))
            {
                results.push_back(data);
            }
        }

        return results;
    }

    bool LeafNode::shouldSplit() const
    {
        return m_entries.size() > m_capacity;
    }

    std::pair<Node *, Node *> LeafNode::split()
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        if (m_entries.size() <= 2)
        {
            return {this, nullptr};
        }

        // Use specified split strategy or default binary split
        std::vector<Data *> group1;
        std::vector<Data *> group2;
        std::tie(group1, group2) = m_splitStrategy->splitLeafEntries(m_entries, m_capacity);
        LeafNode *newNode = new LeafNode(m_capacity, m_splitStrategy, metric_manager);

        m_entries.clear();
        for (auto *entry : group1)
        {
            m_entries.push_back(entry);
        }
        for (auto *entry : group2)
        {
            newNode->insert(entry);
        }
        recalculateMBR();
        newNode->recalculateMBR();

        const auto endTime = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                                  endTime - startTime)
                                  .count();
        this->metric_manager->record_split_time(duration);

        return {this, newNode};
    }

    void LeafNode::recalculateMBR()
    {
        if (m_entries.empty())
        {
            m_mbr = Region(0); // Create empty region
            return;
        }

        // Use the region of the first data as initial value
        m_mbr = m_entries[0]->getRegion();

        // Combine regions of all other data
        for (size_t i = 1; i < m_entries.size(); ++i)
        {
            m_mbr.combine(m_entries[i]->getRegion());
        }
    }

    uint32_t LeafNode::getHeight() const
    {
        return 1; // Leaf node's height is always 1
    }

    std::vector<Data *> LeafNode::reinsertForRstar()
    {
        const double REINSERT_PERCENTAGE = reinsertFactor; // Typically 30% of entries
        size_t numToReinsert = std::max(size_t(1), size_t(m_entries.size() * REINSERT_PERCENTAGE));

        // Calculate center of the node's MBR
        Region nodeMBR = this->getMBR();
        std::vector<double> nodeCenter(nodeMBR.getDimension());

        for (uint32_t d = 0; d < nodeMBR.getDimension(); ++d)
        {
            nodeCenter[d] = (nodeMBR.getLow(d) + nodeMBR.getHigh(d)) / 2.0;
        }

        // Calculate distances from center for each entry
        std::vector<std::pair<size_t, double>> distances;
        for (size_t i = 0; i < m_entries.size(); ++i)
        {
            Region entryMBR = m_entries[i]->getRegion();
            std::vector<double> entryCenter(entryMBR.getDimension());

            for (uint32_t d = 0; d < entryMBR.getDimension(); ++d)
            {
                entryCenter[d] = (entryMBR.getLow(d) + entryMBR.getHigh(d)) / 2.0;
            }

            // Calculate squared distance from node center
            double dist = 0.0;
            for (uint32_t d = 0; d < entryMBR.getDimension(); ++d)
            {
                double diff = entryCenter[d] - nodeCenter[d];
                dist += diff * diff;
            }

            distances.push_back({i, dist});
        }

        // Sort by distance (farthest first)
        std::sort(distances.begin(), distances.end(),
                  [](const auto &a, const auto &b)
                  { return a.second > b.second; });

        // Select entries farthest from center for reinsertion
        std::vector<Data *> entriesToReinsert;
        for (size_t i = 0; i < numToReinsert && i < distances.size(); ++i)
        {
            size_t idx = distances[i].first;
            entriesToReinsert.push_back(m_entries[idx]);
        }

        // Remove selected entries from this node
        for (auto *entry : entriesToReinsert)
        {
            auto it = std::find(m_entries.begin(), m_entries.end(), entry);
            if (it != m_entries.end())
            {
                m_entries.erase(it);
            }
        }

        // Recalculate MBR after removing entries
        recalculateMBR();

        // Return entries to be reinserted
        return entriesToReinsert;
    }

} // namespace RTree