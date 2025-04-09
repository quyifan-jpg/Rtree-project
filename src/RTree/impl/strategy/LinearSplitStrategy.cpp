#include "LinearSplitStrategy.h"

namespace RTree
{
    // Add explicit default constructor and destructor definitions to ensure the virtual function table is generated correctly
    LinearSplitStrategy::LinearSplitStrategy() = default;
    LinearSplitStrategy::~LinearSplitStrategy() = default;

    std::pair<std::vector<Data *>, std::vector<Data *>>
    LinearSplitStrategy::splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const
    {
        std::vector<Data *> group1;
        std::vector<Data *> group2;

        // Simple linear split - Distribute entries evenly between two groups
        size_t midpoint = entries.size() / 2;

        // Ensure each group has at least capacity/2 entries
        uint32_t minEntries = capacity / 2;

        for (size_t i = 0; i < entries.size(); ++i)
        {
            if (i < midpoint)
            {
                group1.push_back(entries[i]);
            }
            else
            {
                group2.push_back(entries[i]);
            }
        }

        // Ensure both groups meet the minimum entry requirement
        if (group1.size() < minEntries)
        {
            // Move entries from group2 to group1
            while (group1.size() < minEntries && group2.size() > minEntries)
            {
                group1.push_back(group2.front());
                group2.erase(group2.begin());
            }
        }
        else if (group2.size() < minEntries)
        {
            // Move entries from group1 to group2
            while (group2.size() < minEntries && group1.size() > minEntries)
            {
                group2.insert(group2.begin(), group1.back());
                group1.pop_back();
            }
        }

        return {group1, group2};
    }

    std::pair<std::vector<Node *>, std::vector<Node *>>
    LinearSplitStrategy::splitInternalChildren(const std::vector<Node *> &children, uint32_t capacity) const
    {
        std::vector<Node *> group1;
        std::vector<Node *> group2;

        // Simple linear split - Distribute child nodes evenly between two groups
        size_t midpoint = children.size() / 2;

        // Ensure each group has at least capacity/2 child nodes
        uint32_t minChildren = capacity / 2;

        for (size_t i = 0; i < children.size(); ++i)
        {
            if (i < midpoint)
            {
                group1.push_back(children[i]);
            }
            else
            {
                group2.push_back(children[i]);
            }
        }

        // Ensure both groups meet the minimum child node requirement
        if (group1.size() < minChildren)
        {
            // Move child nodes from group2 to group1
            while (group1.size() < minChildren && group2.size() > minChildren)
            {
                group1.push_back(group2.front());
                group2.erase(group2.begin());
            }
        }
        else if (group2.size() < minChildren)
        {
            // Move child nodes from group1 to group2
            while (group2.size() < minChildren && group1.size() > minChildren)
            {
                group2.insert(group2.begin(), group1.back());
                group1.pop_back();
            }
        }

        return {group1, group2};
    }

    std::string LinearSplitStrategy::getName() const
    {
        return "LinearSplit";
    }

} // namespace RTree