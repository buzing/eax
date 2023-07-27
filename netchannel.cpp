#include "includes.h"

#define NET_FRAMES_BACKUP 64 // must be power of 2. 
#define NET_FRAMES_MASK ( NET_FRAMES_BACKUP - 1 )

int SetSuitableInSequence(INetChannel* channel) {
    float spike = std::max(1.f, g_menu.main.misc.fake_latency_amt.get()) / 1000.f;
    float correct = std::max(0.f, spike - g_cl.m_latency - g_cl.m_lerp);

    channel->m_in_seq -= game::TIME_TO_TICKS(correct);
    return game::TIME_TO_TICKS(correct);
}

void FakePing(INetChannel* chan, float latency) {
    for (auto& sequence : g_cl.m_sequences) {
        if (g_csgo.m_globals->m_realtime - sequence.m_time >= latency) {
            chan->m_in_rel_state = sequence.m_state;
            chan->m_in_seq = sequence.m_seq;
            break;
        }
    }
}





int Hooks::SendDatagram(void* data) {

    if (!g_csgo.m_engine->IsInGame())
        return g_hooks.m_net_channel.GetOldMethod< SendDatagram_t >(INetChannel::SENDDATAGRAM)(this, data);

    // backup netchannel.
    const auto backup1 = reinterpret_cast<INetChannel*> (this);

    // backup sequence incoming.
    const int backup2 = g_csgo.m_net->m_in_rel_state;
    const int backup3 = g_csgo.m_net->m_in_seq;

    float spike = g_aimbot.m_fake_latency ? g_menu.main.misc.fake_latency_amt.get() : g_menu.main.misc.fake_latency_amt.get();

    float correct = std::max(0.f, (spike / 1000.f) - g_cl.m_lerp);
    if (g_aimbot.m_fake_latency) {
        FakePing(backup1, correct);
    }

    const int ret = g_hooks.m_net_channel.GetOldMethod< SendDatagram_t >(INetChannel::SENDDATAGRAM)(this, data);

    g_csgo.m_net->m_in_rel_state = backup2;
    g_csgo.m_net->m_in_seq = backup3;

    return ret;
}

void Hooks::ProcessPacket(void* packet, bool header) {
    g_hooks.m_net_channel.GetOldMethod< ProcessPacket_t >(INetChannel::PROCESSPACKET)(this, packet, header);

    g_cl.UpdateIncomingSequences();

    // get this from CL_FireEvents string "Failed to execute event for classId" in engine.dll
    for (CEventInfo* it{ g_csgo.m_cl->m_events }; it != nullptr; it = it->m_next) {
        if (!it->m_class_id)
            continue;

        // set all delays to instant.
        it->m_fire_delay = 0.f;
    }

    // game events are actually fired in OnRenderStart which is WAY later after they are received
    // effective delay by lerp time, now we call them right after theyre received (all receive proxies are invoked without delay).
    g_csgo.m_engine->FireEvents();
}


