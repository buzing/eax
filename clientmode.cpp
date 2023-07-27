#include "includes.h"

bool Hooks::ShouldDrawParticles( ) {
	return g_hooks.m_client_mode.GetOldMethod< ShouldDrawParticles_t >( IClientMode::SHOULDDRAWPARTICLES )( this );
}

bool Hooks::ShouldDrawFog( ) {
	// remove fog.
	if( g_menu.main.visuals.removals.get( 2 ) )
		return false;

	return g_hooks.m_client_mode.GetOldMethod< ShouldDrawFog_t >( IClientMode::SHOULDDRAWFOG )( this );
}

void Hooks::OverrideView(CViewSetup* view) {
	// damn son.
	g_cl.m_local = g_csgo.m_entlist->GetClientEntity< Player* >(g_csgo.m_engine->GetLocalPlayer());

	if( g_cl.m_local && g_cl.m_local->alive( ) && g_cl.m_weapon ) {
		if( g_cl.m_local->m_bIsScoped( ) ) {
			if( g_menu.main.visuals.fov_scoped.get( ) ) {
				if( g_cl.m_weapon->m_zoomLevel( ) != 2 ) {
					g_csgo.m_view_render->m_view.m_fov = g_menu.main.misc.fov_amt.get( );
				}
				else {
					g_csgo.m_view_render->m_view.m_fov += 45.f;
				}
			}
		}

		else g_csgo.m_view_render->m_view.m_fov = g_menu.main.misc.fov_amt.get( );
	}

	// g_grenades.think( );
	g_visuals.ThirdpersonThink();

	// call original.
	g_hooks.m_client_mode.GetOldMethod< OverrideView_t >(IClientMode::OVERRIDEVIEW)(this, view);
}

bool Hooks::CreateMove( float time, CUserCmd* cmd ) {
	Stack   stack;
	bool    ret;

	// let original run first.
	ret = g_hooks.m_client_mode.GetOldMethod< CreateMove_t >( IClientMode::CREATEMOVE )( this, time, cmd );

	// called from CInput::ExtraMouseSample -> return original.
	if( !cmd->m_command_number )
		return ret;

	// if we arrived here, called from -> CInput::CreateMove
	// call EngineClient::SetViewAngles according to what the original returns.
	if( ret )
		g_csgo.m_engine->SetViewAngles( cmd->m_view_angles );

	if (g_csgo.cl_csm_shadows && g_csgo.cl_csm_shadows->GetInt() != 0)
		g_csgo.cl_csm_shadows->SetValue(0);

	// random_seed isn't generated in ClientMode::CreateMove yet, we must set generate it ourselves.
	cmd->m_random_seed = g_csgo.MD5_PseudoRandom( cmd->m_command_number ) & 0x7fffffff;

	// get bSendPacket off the stack.
	g_cl.m_packet = stack.next( ).local( 0x1c ).as< bool* >( );

	// get bFinalTick off the stack.
	g_cl.m_final_packet = stack.next( ).local( 0x1b ).as< bool* >( );

	// invoke move function.
	g_cl.OnTick( cmd );

	return false;
}

bool Hooks::DoPostScreenSpaceEffects( CViewSetup* setup ) {
	g_visuals.RenderGlow( );

	g_visuals.on_post_screen_effects();

	return g_hooks.m_client_mode.GetOldMethod< DoPostScreenSpaceEffects_t >( IClientMode::DOPOSTSPACESCREENEFFECTS )( this, setup );
}