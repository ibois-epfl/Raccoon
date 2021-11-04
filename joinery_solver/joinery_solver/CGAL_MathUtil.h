#pragma once
//#include "compas.h"

namespace CGAL_MathUtil {


    inline uint64_t unique_from_two_int(int& low, int& high) {

        if (high > low) {
            return (uint64_t)high << 32 | low;
        }
        else {
            return (uint64_t)low << 32 | high;
        }


    }

}