#include "Data.h"

#include <cstring>

namespace RTree
{

    Data::Data(const Region &mbr, id_type id)
        : m_id(id), m_region(mbr)
    {
    }

    Data *Data::clone() const
    {
        return new Data(m_region, m_id);
    }

    id_type Data::getIdentifier() const
    {
        return m_id;
    }

    const Region &Data::getRegion() const
    {
        return m_region;
    }

} // namespace RTree