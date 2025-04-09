#include "Point.h"

#include <cstring>
#include <stdexcept>

namespace RTree
{
    Point::Point(const double *coords, uint32_t dimension, int id) : m_dimension(dimension), id(id)
    {
        m_pCoords = new double[dimension];
        memcpy(m_pCoords, coords, dimension * sizeof(double));
    }

    Point::Point(const Point &other) : m_dimension(other.m_dimension)
    {
        m_pCoords = new double[m_dimension];
        memcpy(m_pCoords, other.m_pCoords, m_dimension * sizeof(double));
        this->id = other.id;
    }

    Point::~Point()
    {
        delete[] m_pCoords;
    }

    bool Point::operator==(const Point &other) const
    {
        if (m_dimension != other.m_dimension)
            return false;

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            if (m_pCoords[i] != other.m_pCoords[i])
                return false;
        }

        return this-> id == other.id;
    }

    double Point::getCoordinate(uint32_t index) const
    {
        if (index >= m_dimension)
        {
            throw std::out_of_range("Index out of range");
        }
        return m_pCoords[index];
    }

    uint32_t Point::getDimension() const
    {
        return m_dimension;
    }

    int Point::getId() const {
        return id;
    }
} // namespace RTree