#include "includes.h"

void Hooks::LevelInitPreEntity( const char* map ) {
	float rate{ 1.f / g_csgo.m_globals->m_interval };

	// set rates when joining a server.
	g_csgo.cl_updaterate->SetValue( rate );
	g_csgo.cl_cmdrate->SetValue( rate );

	g_aimbot.reset( );
	g_visuals.m_hit_start = g_visuals.m_hit_end = g_visuals.m_hit_duration = 0.f;

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPreEntity_t >( CHLClient::LEVELINITPREENTITY )( this, map );
}

void Hooks::LevelInitPostEntity( ) {
	g_cl.OnMapload( );

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPostEntity_t >( CHLClient::LEVELINITPOSTENTITY )( this );
}

void Hooks::LevelShutdown( ) {
	g_aimbot.reset( );

	g_cl.m_local       = nullptr;
	g_cl.m_weapon      = nullptr;
	g_cl.m_processing  = false;
	g_cl.m_weapon_info = nullptr;
	g_cl.m_round_end   = false;

	g_cl.m_sequences.clear( );

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelShutdown_t >( CHLClient::LEVELSHUTDOWN )( this );
}




struct var_map_entry_t {
public:
	unsigned short type;
	unsigned short need_interp;
	void* data;
	void* watcher;
};

struct varmapping_t {
	varmapping_t() {
		interp_entries = 0;
	}

	var_map_entry_t* entries;
	int interp_entries;
	float last_interp_time;
};

varmapping_t* get_var_map(void* base_ent) {
	return reinterpret_cast<varmapping_t*>((DWORD)base_ent + 0x24); //0x4C );
}

void disable_interpolation(Player* ent) {

	varmapping_t* map = get_var_map(ent);

	if (!map)
		return;

	for (int i = 0; i < map->interp_entries; i++) {
		var_map_entry_t* e = &map->entries[i];
		e->need_interp = g_menu.main.misc.interpolation.get();
	}
}

void Hooks::FrameStageNotify( Stage_t stage ) {
	// save stage.
	if( stage != FRAME_START )
		g_cl.m_stage = stage;

	// damn son.
	g_cl.m_local = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );

	if (stage == FRAME_RENDER_START) {
		// apply local player animation fix.
		g_cl.UpdateAnimations();

		g_cl.ThirdPersonFSN();

		g_visuals.ImpactData();

		g_shots.Think();
	}
	// disable interpolation on all players
	if (g_cl.m_local && g_cl.m_processing)
	{
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);
			if (!player || player->m_bIsLocalPlayer() || player->index() == g_cl.m_local->index())
				continue;

			disable_interpolation(player);
		}
	}

	static bool turnedon = false;
	if (g_menu.main.visuals.world.get(2)) {
		Color clr = g_menu.main.visuals.ambient_color.get();
		turnedon = false;
		if (g_csgo.mat_ambient_light_r->GetFloat() != clr.r())
			g_csgo.mat_ambient_light_r->SetValue(clr.r() / g_menu.main.visuals.ambient_alpha.get());

		if (g_csgo.mat_ambient_light_g->GetFloat() != clr.g())
			g_csgo.mat_ambient_light_g->SetValue(clr.g() / g_menu.main.visuals.ambient_alpha.get());

		if (g_csgo.mat_ambient_light_b->GetFloat() != clr.b())
			g_csgo.mat_ambient_light_b->SetValue(clr.b() / g_menu.main.visuals.ambient_alpha.get());
	}
	else {
		if (!turnedon) {
			g_csgo.mat_ambient_light_r->SetValue(0.f);
			g_csgo.mat_ambient_light_g->SetValue(0.f);
			g_csgo.mat_ambient_light_b->SetValue(0.f);
			turnedon = true;
		}
	}
	

	// call og.
	g_hooks.m_client.GetOldMethod< FrameStageNotify_t >( CHLClient::FRAMESTAGENOTIFY )( this, stage );

	if( stage == FRAME_RENDER_START ) {
		// ...

		if (!g_cl.m_local)
			return;

		bool bFinalImpact = false;
		for (size_t i{ 0 }; i < g_visuals.bulletImpactInfo.size(); i++) {
			auto& currentImpact = g_visuals.bulletImpactInfo.at(i);

			if (std::abs(g_csgo.m_globals->m_realtime - currentImpact.m_flExpTime) > 3.f) {
				g_visuals.bulletImpactInfo.erase(g_visuals.bulletImpactInfo.begin() + i);
				continue;
			}

			if (currentImpact.ignore)
				continue;

			if (currentImpact.m_bRing) {
				BeamInfo_t beamInfo;
				beamInfo.m_nType = TE_BEAMRINGPOINT;

				if (g_menu.main.misc.esp_style.get() == 0) {
					beamInfo.m_pszModelName = XOR("sprites/purplelaser1.vmt");
					beamInfo.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/purplelaser1.vmt"));
					beamInfo.m_pszHaloName = XOR("sprites/purplelaser1.vmt");
					beamInfo.m_nHaloIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/purplelaser1.vmt"));
				}

				else if (g_menu.main.misc.esp_style.get() == 1) {
					beamInfo.m_pszModelName = XOR("sprites/white.vmt");
					beamInfo.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/white.vmt"));
					beamInfo.m_pszHaloName = XOR("sprites/white.vmt");
					beamInfo.m_nHaloIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/white.vmt"));
				}
				beamInfo.m_flHaloScale = 5.f;
				beamInfo.m_flLife = 1.0f;
				if (g_menu.main.misc.esp_style.get() == 0) {
				beamInfo.m_flWidth = 6.0f;
				beamInfo.m_flEndWidth = 6.0f;
			}
				else if (g_menu.main.misc.esp_style.get() == 1) {
					beamInfo.m_flWidth = 1.0f;
					beamInfo.m_flEndWidth = 1.0f;
				}
				beamInfo.m_flFadeLength = 0.0f;
				beamInfo.m_flAmplitude = 0.0f;//2.f
				beamInfo.m_flBrightness = currentImpact.m_cColor.a();
				beamInfo.m_flSpeed = 5.0f;
				beamInfo.m_flSpeed = 5.0f;
				beamInfo.m_nStartFrame = 0;
				beamInfo.m_flFrameRate = 0.f;
				beamInfo.m_flRed = currentImpact.m_cColor.r();
				beamInfo.m_flGreen = currentImpact.m_cColor.g();
				beamInfo.m_flBlue = currentImpact.m_cColor.b();
				beamInfo.m_nSegments = 1;
				beamInfo.m_bRenderable = true;
				beamInfo.m_nFlags = 0;
				beamInfo.m_vecCenter = currentImpact.m_vecStartPos + vec3_t(0, 0, 5);
				beamInfo.m_flStartRadius = 1;
				if (g_menu.main.misc.esp_style.get() == 0) { // bad way to do it ig ? 
					beamInfo.m_flEndRadius = 550;
				}
				else if (g_menu.main.misc.esp_style.get() == 1) {
					beamInfo.m_flEndRadius = 300;
				}

				auto beam = g_csgo.m_beams->CreateBeamRingPoint(beamInfo);
				if (beam) {
					g_csgo.m_beams->DrawBeam(beam);
				}

				currentImpact.ignore = true;
				continue;
			}

			// is this the final impact?
			// last impact in the vector, it's the final impact.
			if (i == (g_visuals.bulletImpactInfo.size() - 1))
				bFinalImpact = true;

			// the current impact's tickbase is different than the next, it's the final impact.
			else if ((i + 1) < g_visuals.bulletImpactInfo.size() && currentImpact.m_nTickBase != g_visuals.bulletImpactInfo.operator[ ](i + 1).m_nTickBase)
				bFinalImpact = true;

			else
				bFinalImpact = false;

			if (bFinalImpact || currentImpact.m_nIndex != g_cl.m_local->index()) {
				vec3_t start = currentImpact.m_vecStartPos;
				vec3_t end = currentImpact.m_vecHitPos;
				BeamInfo_t beamInfo;
				beamInfo.m_vecStart = start;
				beamInfo.m_vecEnd = end;
				beamInfo.m_nType = 0;
				beamInfo.m_pszModelName = ("sprites/purplelaser1.vmt");
				beamInfo.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(("sprites/purplelaser1.vmt"));
				beamInfo.m_flHaloScale = 0.0f;
				beamInfo.m_flLife = 3.f;
				beamInfo.m_flWidth = 4.0f;
				beamInfo.m_flEndWidth = 4.0f;
				beamInfo.m_flFadeLength = 0.0f;
				beamInfo.m_flAmplitude = 2.0f;
				beamInfo.m_flBrightness = currentImpact.m_cColor.a();
				beamInfo.m_flSpeed = 0.2f;
				beamInfo.m_nStartFrame = 0;
				beamInfo.m_flFrameRate = 0.f;
				beamInfo.m_flRed = currentImpact.m_cColor.r();
				beamInfo.m_flGreen = currentImpact.m_cColor.g();
				beamInfo.m_flBlue = currentImpact.m_cColor.b();
				beamInfo.m_nSegments = 2;
				beamInfo.m_bRenderable = true;
				beamInfo.m_nFlags = 0x100 | 0x200 | 0x8000;

				Beam_t* beam = g_csgo.m_beams->CreateBeamPoints(beamInfo);
				if (beam) {
					g_csgo.m_beams->DrawBeam(beam);

					currentImpact.ignore = true;
				}
			}
		}
	}

	else if( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
		// restore non-compressed netvars.
		// g_netdata.apply( );

		g_skins.think( );
	}

	else if( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END ) {
		g_visuals.NoSmoke( );
	}

	else if( stage == FRAME_NET_UPDATE_END ) {
        // restore non-compressed netvars.
		g_netdata.apply( );

		g_cl.UpdateLocal();

		// update all players.
		for( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );
			if( !player || player->m_bIsLocalPlayer( ) )
				continue;

			AimPlayer* data = &g_aimbot.m_players[ i - 1 ];
			data->OnNetUpdate( player );
		}
	}
}