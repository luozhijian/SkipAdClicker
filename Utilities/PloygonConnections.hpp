#ifndef AUTOMATIOTEST_PLOYGONCONNECTIONS_HPP
#define AUTOMATIOTEST_PLOYGONCONNECTIONS_HPP

#include "PloygonConnection.hpp"

#include <cstddef>
#include <vector>

namespace automationtest::utilities {

template <typename T>
class PloygonConnections {
public:
    void AddConnection(T* polygon, int connection_type)
    {
        connections_.emplace_back(polygon, connection_type);
    }

    [[nodiscard]] std::size_t Count() const noexcept
    {
        return connections_.size();
    }

    [[nodiscard]] const std::vector<PloygonConnection<T>>& Items() const noexcept
    {
        return connections_;
    }

private:
    std::vector<PloygonConnection<T>> connections_ {};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_PLOYGONCONNECTIONS_HPP
