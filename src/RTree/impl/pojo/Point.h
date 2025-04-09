//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef POINT_H
#define POINT_H

#include <cstdint>

namespace RTree {
    class Point
    {
    public:
        Point(const double *coords, uint32_t dimension, int id);
        Point(const Point &other);
        ~Point();

        bool operator==(const Point &other) const;

        double getCoordinate(uint32_t index) const;
        uint32_t getDimension() const;
        int getId() const;

    private:
        uint32_t m_dimension;
        double *m_pCoords;
        int id;
    };
}

#endif //POINT_H
