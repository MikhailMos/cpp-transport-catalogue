#define _USE_MATH_DEFINES
#include "geo.h"

#include <cmath>

namespace transport_catalog {

    namespace geo {

        
        double ComputeDistance(Coordinates from, Coordinates to) {
            using namespace std;

            const size_t RADIUS_OF_EARTH = 6371000;
            if (from == to) {
                return 0;
            }
            static const double dr = M_PI / 180.;
            return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
                * RADIUS_OF_EARTH;
        }


    }  // namespace geo

}   // namespace transport_catalog