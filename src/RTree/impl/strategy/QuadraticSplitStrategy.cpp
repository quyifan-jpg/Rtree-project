#include "QuadraticSplitStrategy.h"

#include "src/RTree/impl/node/Node.h"

namespace RTree
{
    // Add explicit default constructor and destructor definitions to ensure the virtual function table is generated correctly
    QuadraticSplitStrategy::QuadraticSplitStrategy() = default;
    QuadraticSplitStrategy::~QuadraticSplitStrategy() = default;

    std::pair<std::vector<Data *>, std::vector<Data *>>
    QuadraticSplitStrategy::splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const
    {
        std::vector<Data *> group1;
        std::vector<Data *> group2;

        // Find the best two seed entries
        auto [seed1, seed2] = pickSeeds(entries);

        // Assign seed entries to two groups
        group1.push_back(entries[seed1]);
        group2.push_back(entries[seed2]);

        // Create working copy to track unassigned entries
        std::vector<Data *> remaining;
        for (size_t i = 0; i < entries.size(); ++i)
        {
            if (i != seed1 && i != seed2)
            {
                remaining.push_back(entries[i]);
            }
        }

        // Ensure each group has at least capacity/2 entries
        uint32_t minEntries = capacity / 2;

        // Calculate initial MBR
        Region mbr1 = group1[0]->getRegion();
        Region mbr2 = group2[0]->getRegion();

        // Assign remaining entries to appropriate groups
        while (!remaining.empty())
        {
            // If one group has too few entries, assign all remaining to it
            if (group1.size() + remaining.size() <= minEntries)
            {
                for (auto &entry : remaining)
                {
                    group1.push_back(entry);
                    mbr1.combine(entry->getRegion());
                }
                remaining.clear();
                break;
            }
            if (group2.size() + remaining.size() <= minEntries)
            {
                for (auto &entry : remaining)
                {
                    group2.push_back(entry);
                    mbr2.combine(entry->getRegion());
                }
                remaining.clear();
                break;
            }

            // Calculate area growth for each entry and select the one with the most growth
            double maxDiff = -std::numeric_limits<double>::max();
            size_t selectedIndex = 0;
            size_t targetGroup = 0; // 0 means group1, 1 means group2

            for (size_t i = 0; i < remaining.size(); ++i)
            {
                const Region &entryRegion = remaining[i]->getRegion();

                // Calculate area growth after assigning the entry to group1
                Region combinedMbr1 = mbr1;
                combinedMbr1.combine(entryRegion);
                double growth1 = combinedMbr1.getArea() - mbr1.getArea();

                // Calculate area growth after assigning the entry to group2
                Region combinedMbr2 = mbr2;
                combinedMbr2.combine(entryRegion);
                double growth2 = combinedMbr2.getArea() - mbr2.getArea();

                // Calculate growth difference
                double diff = std::abs(growth1 - growth2);
                if (diff > maxDiff)
                {
                    maxDiff = diff;
                    selectedIndex = i;
                    targetGroup = (growth1 < growth2) ? 0 : 1;
                }
            }

            // Assign the selected entry to the target group
            Data *selectedEntry = remaining[selectedIndex];
            if (targetGroup == 0)
            {
                group1.push_back(selectedEntry);
                mbr1.combine(selectedEntry->getRegion());
            }
            else
            {
                group2.push_back(selectedEntry);
                mbr2.combine(selectedEntry->getRegion());
            }

            // Remove the entry from the unassigned list
            remaining.erase(remaining.begin() + selectedIndex);
        }

        return {group1, group2};
    }
    //     std::pair<std::vector<Data *>, std::vector<Data *>>
    // LinearSplitStrategy::splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const
    //
    std::pair<std::vector<Node *>, std::vector<Node *>>
    QuadraticSplitStrategy::splitInternalChildren(const std::vector<Node *> &children, uint32_t capacity) const
    {
        std::vector<Node *> group1;
        std::vector<Node *> group2;

        // Find the best two seed nodes
        auto [seed1, seed2] = pickSeeds(children);

        // Assign seed nodes to two groups
        group1.push_back(children[seed1]);
        group2.push_back(children[seed2]);

        // Create working copy to track unassigned nodes
        std::vector<Node *> remaining;
        for (size_t i = 0; i < children.size(); ++i)
        {
            if (i != seed1 && i != seed2)
            {
                remaining.push_back(children[i]);
            }
        }

        // Ensure each group has at least capacity/2 child nodes
        uint32_t minChildren = capacity / 2;

        // Calculate initial MBR
        Region mbr1 = group1[0]->getMBR();
        Region mbr2 = group2[0]->getMBR();

        // Assign remaining nodes to appropriate groups
        while (!remaining.empty())
        {
            // If one group has too few nodes, assign all remaining to it
            if (group1.size() + remaining.size() <= minChildren)
            {
                for (auto &child : remaining)
                {
                    group1.push_back(child);
                    mbr1.combine(child->getMBR());
                }
                remaining.clear();
                break;
            }
            if (group2.size() + remaining.size() <= minChildren)
            {
                for (auto &child : remaining)
                {
                    group2.push_back(child);
                    mbr2.combine(child->getMBR());
                }
                remaining.clear();
                break;
            }

            // Calculate area growth for each node and select the one with the most growth
            double maxDiff = -std::numeric_limits<double>::max();
            size_t selectedIndex = 0;
            size_t targetGroup = 0; // 0 means group1, 1 means group2

            for (size_t i = 0; i < remaining.size(); ++i)
            {
                const Region &childMbr = remaining[i]->getMBR();

                // Calculate area growth after assigning the node to group1
                Region combinedMbr1 = mbr1;
                combinedMbr1.combine(childMbr);
                double growth1 = combinedMbr1.getArea() - mbr1.getArea();

                // Calculate area growth after assigning the node to group2
                Region combinedMbr2 = mbr2;
                combinedMbr2.combine(childMbr);
                double growth2 = combinedMbr2.getArea() - mbr2.getArea();

                // Calculate growth difference
                double diff = std::abs(growth1 - growth2);
                if (diff > maxDiff)
                {
                    maxDiff = diff;
                    selectedIndex = i;
                    targetGroup = (growth1 < growth2) ? 0 : 1;
                }
            }

            // Assign the selected node to the target group
            Node *selectedChild = remaining[selectedIndex];
            if (targetGroup == 0)
            {
                group1.push_back(selectedChild);
                mbr1.combine(selectedChild->getMBR());
            }
            else
            {
                group2.push_back(selectedChild);
                mbr2.combine(selectedChild->getMBR());
            }

            // Remove the node from the unassigned list
            remaining.erase(remaining.begin() + selectedIndex);
        }

        return {group1, group2};
    }

    std::pair<size_t, size_t> QuadraticSplitStrategy::pickSeeds(const std::vector<Data *> &entries) const
    {
        size_t seed1 = 0;
        size_t seed2 = 0;
        double maxWastedArea = -1.0;

        // Find the best two seed entries
        for (size_t i = 0; i < entries.size(); ++i)
        {
            const Region &region1 = entries[i]->getRegion();

            for (size_t j = i + 1; j < entries.size(); ++j)
            {
                const Region &region2 = entries[j]->getRegion();

                // Calculate merged region
                Region combinedRegion = region1;
                combinedRegion.combine(region2);

                // Calculate wasted area = merged region area - two original regions area
                double wastedArea = combinedRegion.getArea() - region1.getArea() - region2.getArea();

                if (wastedArea > maxWastedArea)
                {
                    maxWastedArea = wastedArea;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }

        return {seed1, seed2};
    }

    std::pair<size_t, size_t> QuadraticSplitStrategy::pickSeeds(const std::vector<Node *> &children) const
    {
        size_t seed1 = 0;
        size_t seed2 = 0;
        double maxWastedArea = -1.0;

        // Find the best two seed nodes
        for (size_t i = 0; i < children.size(); ++i)
        {
            const Region &region1 = children[i]->getMBR();

            for (size_t j = i + 1; j < children.size(); ++j)
            {
                const Region &region2 = children[j]->getMBR();

                // Calculate merged region
                Region combinedRegion = region1;
                combinedRegion.combine(region2);

                // Calculate wasted area = merged region area - two original regions area
                double wastedArea = combinedRegion.getArea() - region1.getArea() - region2.getArea();

                if (wastedArea > maxWastedArea)
                {
                    maxWastedArea = wastedArea;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }

        return {seed1, seed2};
    }

    double QuadraticSplitStrategy::calculateAreaEnlargement(const Region &r1, const Region &r2) const
    {
        Region combined = r1;
        combined.combine(r2);
        return combined.getArea() - r1.getArea();
    }

    std::string QuadraticSplitStrategy::getName() const
    {
        return "QuadraticSplit";
    }

} // namespace RTree