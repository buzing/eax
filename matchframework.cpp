#include "includes.h"

CMatchSessionOnlineHost* Hooks::GetMatchSession() {
	Stack       stack;
	Address     kv;
	std::string map;

	// ClientJob_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve::BYieldingRunJobFromMsg
	static Address BYieldingRunJobFromMsg{ pattern::find(g_csgo.m_client_dll, XOR("8B D8 85 DB 0F 84 ? ? ? ? 8B 13 8B CB 68 ? ? ? ? 68")) };

	// NOTE;
	// string xref "game/mmqueue" above that string is a virtual call which is g_pMatchFramework->GetMatchSession( )
	// string xref "Matchmaking reservation confirmed: %llx/%s\n"

	// call og.
	return g_hooks.m_match_framework.GetOldMethod< GetMatchSession_t >(CMatchFramework::GETMATCHSESSION)(this);
}