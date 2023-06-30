#include "domain.h"

namespace transport_catalog {

    
    size_t StopsPairHasher::operator()(const std::pair<Stop*, Stop*>& p_s) const {
        size_t h1 = hasher_(p_s.first);
        size_t h2 = hasher_(p_s.second);

        return h1 * 37 + h2;
    }


}   // namespace transport_catalog