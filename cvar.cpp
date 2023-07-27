#include "includes.h"

int Hooks::DebugSpreadGetInt( ) {
	Stack stack;

	static Address DrawCrosshair{ pattern::find( g_csgo.m_client_dll, XOR( "85 C0 0F 84 ? ? ? ? 8B 4C 24 1C" ) ) };

	if( g_menu.main.visuals.force_xhair.get( ) && stack.ReturnAddress( ) == DrawCrosshair )
		return 3;

	return g_hooks.m_debug_spread.GetOldMethod< GetInt_t >( ConVar::GETINT )( this );
}

bool Hooks::NetShowFragmentsGetBool() {
	return g_hooks.m_net_show_fragments.GetOldMethod< GetBool_t >(ConVar::GETBOOL)(this);
}