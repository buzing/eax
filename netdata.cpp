#include "includes.h"

NetData g_netdata{};;

void NetData::store() {
    int          tickbase;
    StoredData_t* data;

    if (!g_cl.m_processing) {
        reset();
        return;
    }

    tickbase = g_cl.m_local->m_nTickBase();

    // get current record and store data.
    data = &m_data[tickbase % MULTIPLAYER_BACKUP];

    data->m_tickbase = tickbase;
    data->m_punch = g_cl.m_local->m_aimPunchAngle();
    data->m_punch_vel = g_cl.m_local->m_aimPunchAngleVel();
    data->m_view_punch = g_cl.m_local->m_viewPunchAngle();
    data->m_view_offset = g_cl.m_local->m_vecViewOffset();
    data->m_origin = g_cl.m_local->m_vecOrigin();
    data->m_velocity = g_cl.m_local->m_vecVelocity();

}

void NetData::apply() {
    int          tickbase;
    StoredData_t* data;
    ang_t        punch_delta, punch_vel_delta, view_punch;
    vec3_t       view_delta, velocity_delta, origin_delta;

    if (!g_cl.m_processing) {
        reset();
        return;
    }

    tickbase = g_cl.m_local->m_nTickBase();

    // get current record and validate.
    data = &m_data[tickbase % MULTIPLAYER_BACKUP];

    if (g_cl.m_local->m_nTickBase() != data->m_tickbase)
        return;

    // get deltas.
    // note - dex;  before, when you stop shooting, punch values would sit around 0.03125 and then goto 0 next update.
    //              with this fix applied, values slowly decay under 0.03125.
    punch_delta = g_cl.m_local->m_aimPunchAngle() - data->m_punch;
    punch_vel_delta = g_cl.m_local->m_aimPunchAngleVel() - data->m_punch_vel;
    view_delta = g_cl.m_local->m_vecViewOffset() - data->m_view_offset;
    velocity_delta = g_cl.m_local->m_vecVelocity() - data->m_velocity;
    origin_delta = g_cl.m_local->m_vecOrigin() - data->m_origin;
    view_punch = g_cl.m_local->m_viewPunchAngle() - data->m_view_punch;

    // set data.
    if (std::abs(punch_delta.x) < 0.03125f &&
        std::abs(punch_delta.y) < 0.03125f &&
        std::abs(punch_delta.z) < 0.03125f)
        g_cl.m_local->m_aimPunchAngle() = data->m_punch;

    if (std::abs(punch_vel_delta.x) < 0.03125f &&
        std::abs(punch_vel_delta.y) < 0.03125f &&
        std::abs(punch_vel_delta.z) < 0.03125f)
        g_cl.m_local->m_aimPunchAngleVel() = data->m_punch_vel;

    if (std::abs(view_punch.x) <= 0.03125f
        && std::abs(view_punch.y) <= 0.03125f
        && std::abs(view_punch.z) <= 0.03125f)
        g_cl.m_local->m_viewPunchAngle() = data->m_view_punch;

    if (std::abs(view_delta.x) < 0.03125f &&
        std::abs(view_delta.y) < 0.03125f &&
        std::abs(view_delta.z) < 0.03125f)
        g_cl.m_local->m_vecViewOffset() = data->m_view_offset;

    if (std::abs(velocity_delta.x) <= 0.5f
        && std::abs(velocity_delta.y) <= 0.5f
        && std::abs(velocity_delta.z) <= 0.5f)
        g_cl.m_local->m_vecVelocity() = data->m_velocity;

    if (std::abs(origin_delta.x) <= 0.1f
        && std::abs(origin_delta.y) <= 0.1f
        && std::abs(origin_delta.z) <= 0.1f)
        g_cl.m_local->m_vecOrigin() = data->m_origin;
}

void NetData::reset() {
    m_data.fill(StoredData_t());
}