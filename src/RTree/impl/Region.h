//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef REGION_H
#define REGION_H
#include <cstdint>

#include "pojo/Point.h"

namespace RTree {
    class Region
    {
    public:
        explicit Region(uint32_t m_dimension = 2);
        Region(const double *low, const double *high, uint32_t dimension);
        Region(const Point &low, const Point &high);
        Region(const Region &other);
        ~Region();

        Region &operator=(const Region &other);
        bool operator==(const Region &other) const;

        bool intersects(const Region &other) const;
        bool contains(const Region &other) const;
        bool contains(const Point &point) const;

        double getArea() const;
        double getMargin() const;
        double getMinDistance(const Point &point) const;
        double getMinDistance(const Region &other) const;

        void combine(const Region &other);
        void combine(const Point &point);
        void getCombinedRegion(Region &out, const Region &other) const;

        double getLow(uint32_t index) const;
        double getHigh(uint32_t index) const;
        uint32_t getDimension() const;

    private:
        uint32_t m_dimension;
        double *m_pLow;
        double *m_pHigh;
    };
}

#endif //REGION_H
