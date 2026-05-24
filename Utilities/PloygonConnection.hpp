#ifndef AUTOMATIOTEST_PLOYGONCONNECTION_HPP
#define AUTOMATIOTEST_PLOYGONCONNECTION_HPP

namespace automationtest::utilities {

template <typename T>
struct PloygonConnection {
    T* ploygon {nullptr};
    int connection_type {0};

    PloygonConnection() = default;
    PloygonConnection(T* polygon_value, int connection_type_value)
        : ploygon(polygon_value)
        , connection_type(connection_type_value)
    {
    }
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_PLOYGONCONNECTION_HPP
