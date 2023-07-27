#pragma once

class NetData {
private:
    class StoredData_t {
    public:
        int    m_tickbase;
        ang_t  m_punch, m_punch_vel, m_view_punch;
        vec3_t m_view_offset, m_velocity, m_origin;

    public:
        __forceinline StoredData_t() : m_tickbase{}, m_punch{}, m_punch_vel{}, m_view_punch{}, m_view_offset{}, m_velocity{}, m_origin{} {};
    };

    std::array< StoredData_t, MULTIPLAYER_BACKUP > m_data;

public:
    void store();
    void apply();
    void reset();
};

extern NetData g_netdata;