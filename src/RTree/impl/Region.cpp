
#include "Region.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace RTree
{

    Region::Region(uint32_t dimension) : m_dimension(dimension)
    {

        m_pLow = new double[dimension];
        m_pHigh = new double[dimension];

        for (uint32_t i = 0; i < dimension; ++i)
        {
            m_pLow[i] = std::numeric_limits<double>::max();
            m_pHigh[i] = std::numeric_limits<double>::lowest();
        }
    }

    Region::Region(const double *low, const double *high, uint32_t dimension) : m_dimension(dimension)
    {
        m_pLow = new double[dimension];
        m_pHigh = new double[dimension];

        memcpy(m_pLow, low, dimension * sizeof(double));
        memcpy(m_pHigh, high, dimension * sizeof(double));
    }

    Region::Region(const Point &low, const Point &high) : m_dimension(low.getDimension())
    {
        if (low.getDimension() != high.getDimension())
        {
            throw std::invalid_argument("Points must have the same dimension");
        }

        m_pLow = new double[m_dimension];
        m_pHigh = new double[m_dimension];

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            m_pLow[i] = low.getCoordinate(i);
            m_pHigh[i] = high.getCoordinate(i);
        }
    }

    Region::Region(const Region &other) : m_dimension(other.m_dimension)
    {
        m_pLow = new double[m_dimension];
        m_pHigh = new double[m_dimension];

        memcpy(m_pLow, other.m_pLow, m_dimension * sizeof(double));
        memcpy(m_pHigh, other.m_pHigh, m_dimension * sizeof(double));
    }

    Region::~Region()
    {
        delete[] m_pLow;
        delete[] m_pHigh;
    }

    Region &Region::operator=(const Region &other)
    {
        if (this != &other)
        {
            if (m_dimension != other.m_dimension)
            {
                delete[] m_pLow;
                delete[] m_pHigh;
                m_dimension = other.m_dimension;
                m_pLow = new double[m_dimension];
                m_pHigh = new double[m_dimension];
            }

            memcpy(m_pLow, other.m_pLow, m_dimension * sizeof(double));
            memcpy(m_pHigh, other.m_pHigh, m_dimension * sizeof(double));
        }
        return *this;
    }

    bool Region::operator==(const Region &other) const
    {
        if (m_dimension != other.m_dimension)
            return false;

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            if (m_pLow[i] != other.m_pLow[i] || m_pHigh[i] != other.m_pHigh[i])
            {
                return false;
            }
        }
        return true;
    }

    bool Region::intersects(const Region &other) const
    {
        if (m_dimension != other.m_dimension)
            return false;

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            if (m_pLow[i] > other.m_pHigh[i] || m_pHigh[i] < other.m_pLow[i])
            {
                return false;
            }
        }
        return true;
    }

    bool Region::contains(const Region &other) const
    {
        if (m_dimension != other.m_dimension)
            return false;

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            if (m_pLow[i] > other.m_pLow[i] || m_pHigh[i] < other.m_pHigh[i])
            {
                return false;
            }
        }
        return true;
    }

    bool Region::contains(const Point &point) const
    {
        if (m_dimension != point.getDimension())
            return false;

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            double coordinate = point.getCoordinate(i);
            if (coordinate < m_pLow[i] || coordinate > m_pHigh[i])
            {
                return false;
            }
        }
        return true;
    }

    double Region::getArea() const
    {
        double area = 1.0;
        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            area *= (m_pHigh[i] - m_pLow[i]);
        }
        return area;
    }

    double Region::getMargin() const
    // margin is the sum of the lengths of all the sides of the region
    {
        double margin = 0.0;
        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            margin += (m_pHigh[i] - m_pLow[i]);
        
        }
        return margin * 2.0; // Length of each dimension needs to be counted twice (opposite edges)
    }

    double Region::getMinDistance(const Point &point) const
    {
        if (m_dimension != point.getDimension())
        {
            throw std::invalid_argument("Dimensions do not match");
        }

        double dist = 0.0;
        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            double coord = point.getCoordinate(i);
            if (coord < m_pLow[i])
            {
                double d = m_pLow[i] - coord;
                dist += d * d;
            }
            else if (coord > m_pHigh[i])
            {
                double d = coord - m_pHigh[i];
                dist += d * d;
            }
        }
        return std::sqrt(dist);
    }

    double Region::getMinDistance(const Region &other) const
    {
        if (m_dimension != other.m_dimension)
        {
            throw std::invalid_argument("Dimensions do not match");
        }

        double dist = 0.0;
        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            if (m_pHigh[i] < other.m_pLow[i])
            {
                double d = other.m_pLow[i] - m_pHigh[i];
                dist += d * d;
            }
            else if (m_pLow[i] > other.m_pHigh[i])
            {
                double d = m_pLow[i] - other.m_pHigh[i];
                dist += d * d;
            }
        }
        return std::sqrt(dist);
    }

    void Region::combine(const Region &other)
    {
        // empty region
        if (other.m_dimension == 0)
        {
            return;
        }

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            m_pLow[i] = std::min(m_pLow[i], other.m_pLow[i]);
            m_pHigh[i] = std::max(m_pHigh[i], other.m_pHigh[i]);
        }
    }

    void Region::combine(const Point &point)
    {
        if (m_dimension != point.getDimension())
        {
            throw std::invalid_argument("Dimensions do not match");
        }

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            double coord = point.getCoordinate(i);
            m_pLow[i] = std::min(m_pLow[i], coord);
            m_pHigh[i] = std::max(m_pHigh[i], coord);
        }
    }

    void Region::getCombinedRegion(Region &out, const Region &other) const
    {
        if (m_dimension != other.m_dimension)
        {
            throw std::invalid_argument("Dimensions do not match");
        }

        if (out.m_dimension != m_dimension)
        {
            // Adjust the dimension of out
            out = Region(m_dimension);
        }

        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            out.m_pLow[i] = std::min(m_pLow[i], other.m_pLow[i]);
            out.m_pHigh[i] = std::max(m_pHigh[i], other.m_pHigh[i]);
        }
    }

    double Region::getLow(uint32_t index) const
    {
        if (index >= m_dimension)
        {
            throw std::out_of_range("Index out of range");
        }
        return m_pLow[index];
    }

    double Region::getHigh(uint32_t index) const
    {
        if (index >= m_dimension)
        {
            throw std::out_of_range("Index out of range");
        }
        return m_pHigh[index];
    }

    uint32_t Region::getDimension() const
    {
        return m_dimension;
    }

} // namespace RTree