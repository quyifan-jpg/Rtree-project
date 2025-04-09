#include "InternalNode.h"
#include <algorithm>
#include <limits>

#include "LeafNode.h"
#include "src/RTree/impl/Data.h"
#include "src/RTree/impl/strategy/SplitStrategy.h"

namespace RTree
{
    class LeafNode;
    class SplitStrategy;

    InternalNode::InternalNode(uint32_t capacity,
        const SplitStrategy *splitStrategy,
        MetricManager* metric_manager)
        : Node(splitStrategy, metric_manager), m_capacity(capacity), m_mbr(0)
    {
        // Initialize MBR as invalid region
    }

    InternalNode::~InternalNode()
    {
        for (Node *child : m_children)
        {
            delete child;
        }
    }

    bool InternalNode::isLeaf() const
    {
        return false;
    }

    const Region &InternalNode::getMBR() const
    {
        return m_mbr;
    }

    void InternalNode::insert(Data *data)
    {
        // Choose the best subtree
        Node *child = chooseSubtree(data->getRegion());

        // Insert data
        child->insert(data);
        total_entries++;

        // Check if splitting is needed
        if (child->shouldSplit())
        {
            auto [original, newChild] = child->split();
            if (newChild)
            {
                addChild(newChild);
            }
        }

        // Update MBR
        recalculateMBR();
    }

    bool InternalNode::remove(id_type id, const Region &mbr)
    {
        // Find all child nodes that might contain this data
        bool found = false;
        for (Node *child : m_children)
        {
            if (child->getMBR().intersects(mbr))
            {
                if (child->remove(id, mbr))
                {
                    found = true;
                    total_entries--;
                    break; // Found and removed, no need to continue searching
                }
            }
        }

        if (found)
        {
            // Data removed, need to update MBR
            recalculateMBR();

            // Check if there are empty child nodes that can be deleted
            auto it = std::remove_if(m_children.begin(), m_children.end(),
                                     [](Node *child)
                                     {
                                         if (child->isEmpty())
                                         {
                                             delete child;
                                             return true;
                                         }
                                         return false;
                                     });

            m_children.erase(it, m_children.end());
        }

        return found;
    }

    bool InternalNode::isEmpty()
    {
        return m_children.empty();
    }

    std::vector<Node *> InternalNode::children() {
        return m_children;
    }

    unsigned long InternalNode::size() {
        return m_children.size();
    }

    std::vector<Data *> InternalNode::search(const Region &query)
    {
        std::vector<Data *> results;

        // Search all child nodes intersecting with the query region
        for (Node *child : m_children)
        {
            if (child->getMBR().intersects(query))
            {
                std::vector<Data *> childResults = child->search(query);
                results.insert(results.end(), childResults.begin(), childResults.end());
            }
        }

        return results;
    }

    bool InternalNode::shouldSplit() const
    {
        return m_children.size() > m_capacity;
    }

    std::pair<Node *, Node *> InternalNode::split() {
        auto startTime = std::chrono::high_resolution_clock::now();

        if (m_children.size() <= 2) {
            return {this, nullptr};
        }

        // Use strategy to split child nodes
        auto [group1, group2] = m_splitStrategy->splitInternalChildren(m_children, m_capacity);

        // Create new node
        auto *newNode = new InternalNode(m_capacity, m_splitStrategy, metric_manager);

        // TODO: Might remove this, since no reinsert function on internal node
        if (m_tree != nullptr)
        {
            newNode->setTree(m_tree);
        }

        // Clear current node's children (but don't delete them, as they will be reassigned)
        std::vector<Node *> originalChildren = std::move(m_children);
        m_children.clear();

        // Add first group of children to current node
        for (auto *child : group1)
        {
            m_children.push_back(child);
        }

        // Add second group of children to new node
        for (auto *child : group2)
        {
            newNode->addChild(child);
        }

        // Recalculate MBR
        recalculateMBR();
        newNode->recalculateMBR();

        const auto endTime = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                                  endTime - startTime)
                                  .count();
        this->metric_manager->record_split_time(duration);

        return {this, newNode};

    }

    void InternalNode::addChild(Node *child)
    {
        // Propagate the tree pointer to the child node
        if (m_tree != nullptr && child != nullptr)
        {
            child->setTree(m_tree);
        }

        m_children.push_back(child);
        recalculateMBR();
    }

    void InternalNode::recalculateMBR()
    {
        if (m_children.empty())
        {
            m_mbr = Region(0); // Create empty region
            return;
        }

        // Use the MBR of the first child as initial value
        m_mbr = m_children[0]->getMBR();

        // Combine MBRs of all other children
        for (size_t i = 1; i < m_children.size(); ++i)
        {
            Region mbr = m_children[i]->getMBR();
            m_mbr.combine(mbr);
        }
    }

    Node *InternalNode::chooseSubtree(const Region &mbr) const
    {
        if (m_children.empty())
        {
            return nullptr;
        }

        // Use unified selection criteria for all children
        double minEnlargement = std::numeric_limits<double>::max();
        Node *bestChild = nullptr;

        for (Node *child : m_children)
        {
            // Calculate area increase after combining regions
            Region combined = child->getMBR();
            double originalArea = combined.getArea();
            combined.combine(mbr);
            double enlargement = combined.getArea() - originalArea;

            // Primary criterion: minimum expansion
            if (bestChild == nullptr || enlargement < minEnlargement)
            {
                minEnlargement = enlargement;
                bestChild = child;
            }
            // Secondary criterion: if expansion is the same, choose the smaller area
            else if (enlargement == minEnlargement)
            {
                if (child->getMBR().getArea() < bestChild->getMBR().getArea())
                {
                    bestChild = child;
                }
            }
        }

        // If the best child is an internal node, recurse into it
        if (!bestChild->isLeaf())
        {
            return static_cast<InternalNode *>(bestChild)->chooseSubtree(mbr);
        }

        // Otherwise return the leaf node found
        return bestChild;
    }

    uint32_t InternalNode::getHeight() const
    {
        if (m_children.empty())
        {
            return 1; // Even empty internal nodes have height 1
        }

        // Internal node's height is the maximum height of its children plus 1
        uint32_t maxChildHeight = 0;
        for (const Node *child : m_children)
        {
            maxChildHeight = std::max(maxChildHeight, child->getHeight());
        }

        return maxChildHeight + 1;
    }
} // namespace RTree
