#include "RStarSplitStrategy.h"

#include "src/RTree/impl/node/Node.h"

namespace RTree
{
    // Add explicit default constructor and destructor definitions
    RStarSplitStrategy::RStarSplitStrategy() = default;
    RStarSplitStrategy::~RStarSplitStrategy() = default;

    std::pair<std::vector<Data *>, std::vector<Data *>>
    RStarSplitStrategy::splitLeafEntries(const std::vector<Data *> &entries, uint32_t capacity) const{
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
    std::pair<std::vector<Node *>, std::vector<Node *>>
    RStarSplitStrategy::splitInternalChildren(const std::vector<Node *> &children, uint32_t capacity) const
    {
        std::vector<Node *> group1;
        std::vector<Node *> group2;

        // Get number of children
        size_t numChildren = children.size();
        if (numChildren <= 2)
        {
            if (numChildren == 2)
            {
                group1.push_back(children[0]);
                group2.push_back(children[1]);
            }
            else if (numChildren == 1)
            {
                group1.push_back(children[0]);
            }
            return {group1, group2};
        }

        // Calculate distribution factor (usually around 0.4)
        double splitDistributionFactor = 0.4;
        uint32_t nodeSPF = static_cast<uint32_t>(std::floor(numChildren * splitDistributionFactor));
        uint32_t splitDistribution = numChildren - (2 * nodeSPF) + 2;

        // Create vectors of indices for sorting
        std::vector<size_t> indices(numChildren);
        for (size_t i = 0; i < numChildren; ++i) {
            indices[i] = i;
        }

        // Find the best split axis and distribution
        double minimumMargin = std::numeric_limits<double>::max();
        uint32_t splitAxis = 0;
        bool useLowBound = true;
        uint32_t splitPoint = 0;

        // Get dimension from first child
        uint32_t dimension = children[0]->getMBR().getDimension();

        // Choose the split axis by minimizing the sum of perimeters

        // step 1: choose the axis perpendicular to which the split is performed
        for (uint32_t dim = 0; dim < dimension; ++dim)
        {
            // Sort indices by lower bound
            std::vector<size_t> lowIndices = indices;
            std::sort(lowIndices.begin(), lowIndices.end(), 
                [&children, dim](size_t a, size_t b) {
                    return children[a]->getMBR().getLow(dim) < children[b]->getMBR().getLow(dim);
                });

            // Sort indices by upper bound
            std::vector<size_t> highIndices = indices;
            std::sort(highIndices.begin(), highIndices.end(), 
                [&children, dim](size_t a, size_t b) {
                    return children[a]->getMBR().getHigh(dim) < children[b]->getMBR().getHigh(dim);
                });

            if (lowIndices == highIndices) {
                continue;
            }

            // Calculate margin sum for all possible distributions
            double marginLow = 0.0;
            double marginHigh = 0.0;

            // step 2: choose the best distribution into two groups
            for (uint32_t k = 1; k <= splitDistribution; ++k)
            {
                uint32_t l = nodeSPF - 1 + k;

                // Create temporary regions for margin calculation
                Region regionLow1(dimension);
                Region regionLow2(dimension);
                Region regionHigh1(dimension);
                Region regionHigh2(dimension);

                // Initialize with first children
                regionLow1 = children[lowIndices[0]]->getMBR();
                regionHigh1 = children[highIndices[0]]->getMBR();

                // Combine regions for first group
                for (size_t i = 1; i < l; ++i)
                {
                    regionLow1.combine(children[lowIndices[i]]->getMBR());
                    regionHigh1.combine(children[highIndices[i]]->getMBR());
                }

                // Initialize second group regions
                regionLow2 = children[lowIndices[l]]->getMBR();
                regionHigh2 = children[highIndices[l]]->getMBR();

                // Combine regions for second group
                for (size_t i = l + 1; i < numChildren; ++i)
                {
                    regionLow2.combine(children[lowIndices[i]]->getMBR());
                    regionHigh2.combine(children[highIndices[i]]->getMBR());
                }

                // Sum up margins
                marginLow += regionLow1.getMargin() + regionLow2.getMargin();
                marginHigh += regionHigh1.getMargin() + regionHigh2.getMargin();
            }

            // Keep the dimension with minimum margin
            double margin = std::min(marginLow, marginHigh);
            if (margin < minimumMargin)
            {
                minimumMargin = margin;
                splitAxis = dim;
                useLowBound = (marginLow < marginHigh);
            }
        }

        // Sort children by the best axis found
        std::vector<size_t> sortedIndices = indices;
        if (useLowBound) {
            std::sort(sortedIndices.begin(), sortedIndices.end(), 
                [&children, splitAxis](size_t a, size_t b) {
                    return children[a]->getMBR().getLow(splitAxis) < children[b]->getMBR().getLow(splitAxis);
                });
        } else {
            std::sort(sortedIndices.begin(), sortedIndices.end(), 
                [&children, splitAxis](size_t a, size_t b) {
                    return children[a]->getMBR().getHigh(splitAxis) < children[b]->getMBR().getHigh(splitAxis);
                });
        }

        // Now choose the best distribution by minimizing overlap
        double minOverlap = std::numeric_limits<double>::max();
        double minArea = std::numeric_limits<double>::max();

        for (uint32_t k = 1; k <= splitDistribution; ++k)
        {
            uint32_t l = nodeSPF - 1 + k;

            // Create regions for the two groups
            Region region1(dimension);
            Region region2(dimension);

            // Initialize with first child
            region1 = children[sortedIndices[0]]->getMBR();

            // Combine regions for first group
            for (size_t i = 1; i < l; ++i)
            {
                region1.combine(children[sortedIndices[i]]->getMBR());
            }

            // Initialize second group region
            region2 = children[sortedIndices[l]]->getMBR();

            // Combine regions for second group
            for (size_t i = l + 1; i < numChildren; ++i)
            {
                region2.combine(children[sortedIndices[i]]->getMBR());
            }

            // Calculate overlap area
            double overlap = calculateOverlapArea(region1, region2, dimension);

            // Calculate total area
            double area = region1.getArea() + region2.getArea();

            // Update best split point if this is better
            if (overlap < minOverlap || (overlap == minOverlap && area < minArea))
            {
                splitPoint = k;
                minOverlap = overlap;
                minArea = area;
            }
        }

        // Get final split point
        uint32_t l = nodeSPF - 1 + splitPoint;

        // Assign children to groups
        for (size_t i = 0; i < l; ++i)
        {
            group1.push_back(children[sortedIndices[i]]);
        }

        for (size_t i = l; i < numChildren; ++i)
        {
            group2.push_back(children[sortedIndices[i]]);
        }

        return {group1, group2};
    }

    std::string RStarSplitStrategy::getName() const
    {
        return "RStarSplit";
    }

    // Calculate overlap area
    double RStarSplitStrategy::calculateOverlapArea(const Region &region1, const Region &region2, uint32_t dimension) const
    {
        if (!region1.intersects(region2))
            return 0.0;

        double overlap = 1.0;

        for (uint32_t d = 0; d < dimension; ++d)
        {
            double overlapLow = std::max(region1.getLow(d), region2.getLow(d));
            double overlapHigh = std::min(region1.getHigh(d), region2.getHigh(d));

            if (overlapLow >= overlapHigh)
                return 0.0;

            overlap *= (overlapHigh - overlapLow);
        }
        return overlap;
    }

    std::pair<size_t, size_t> RStarSplitStrategy::pickSeeds(const std::vector<Data *> &entries) const
    {
        size_t seed1 = 0;
        size_t seed2 = 0;
        double maxWastedArea = -1.0;
        for (size_t i = 0; i < entries.size(); ++i)
        {
            const Region &region1 = entries[i]->getRegion();
            for (size_t j = i + 1; j < entries.size(); ++j)
            {
                const Region &region2 = entries[j]->getRegion();
                Region combinedRegion = region1;
                combinedRegion.combine(region2);
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

} // namespace RTree