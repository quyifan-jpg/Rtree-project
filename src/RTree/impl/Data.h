//
// Created by Shengqiao Zhao on 2025-04-04.
//

#ifndef DATA_H
#define DATA_H
#include <cstdint>

#include "common.h"
#include "Region.h"

namespace RTree {
    class Data
    {
    public:
        Data(const Region &mbr, id_type id);
        ~Data() = default;

        Data *clone() const;
        id_type getIdentifier() const;
        const Region &getRegion() const;

    private:
        id_type m_id;
        Region m_region;
    };
}

#endif //DATA_H
