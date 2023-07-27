#include "includes.h"

Visuals g_visuals{ };;

void Visuals::ModulateWorld() {
	std::vector< IMaterial* > world, props;

	// iterate material handles.
	for (uint16_t h{ g_csgo.m_material_system->FirstMaterial() }; h != g_csgo.m_material_system->InvalidMaterial(); h = g_csgo.m_material_system->NextMaterial(h)) {
		// get material from handle.
		IMaterial* mat = g_csgo.m_material_system->GetMaterial(h);
		if (!mat)
			continue;

		// store world materials.
		if (FNV1a::get(mat->GetTextureGroupName()) == HASH("World textures"))
			world.push_back(mat);

		// store props.
		else if (FNV1a::get(mat->GetTextureGroupName()) == HASH("StaticProp textures"))
			props.push_back(mat);
	}

	// night

	Color col = g_menu.main.visuals.nightcolor.get();
	Color col2 = g_menu.main.visuals.propscolor.get();

	if (g_menu.main.visuals.world.get(0)) {
		for (const auto& w : world)

			w->ColorModulate(col.r() / 255.f, col.g() / 255.f, col.b() / 255.f);

		// IsUsingStaticPropDebugModes my nigga
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != 0) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(0);
		}

		for (const auto& p : props)
			p->ColorModulate(col2.r() / 255.f, col2.g() / 255.f, col2.b() / 255.f);

		g_csgo.LoadNamedSky(XOR("sky_l4d_rural02")); 
	}

	// disable night.
	else {
		for (const auto& w : world)
			w->ColorModulate(1.f, 1.f, 1.f);

		// restore r_DrawSpecificStaticProp.
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != -1) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(-1);
		}

		for (const auto& p : props)
			p->ColorModulate(1.f, 1.f, 1.f);
	}

	// transparent props.
		float alpha2 = g_menu.main.visuals.walls_amount.get() / 100;
		for (const auto& w : world)
			w->AlphaModulate(alpha2);

		float alpha = g_menu.main.visuals.transparent_props_amount.get() / 100;
		for (const auto& p : props)
			p->AlphaModulate(alpha);

		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != 0) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(0);
		}
}

void Visuals::ThirdpersonThink( ) {
	ang_t                          offset;
	vec3_t                         origin, forward;
	static CTraceFilterSimple_game filter{ };
	CGameTrace                     tr;

	// for whatever reason overrideview also gets called from the main menu.
	if( !g_csgo.m_engine->IsInGame( ) )
		return;

	// check if we have a local player and he is alive.
	bool alive = g_cl.m_local && g_cl.m_local->alive( );

	// camera should be in thirdperson.
	if( m_thirdperson ) {

		// if alive and not in thirdperson already switch to thirdperson.
		if( alive && !g_csgo.m_input->CAM_IsThirdPerson( ) )
			g_csgo.m_input->CAM_ToThirdPerson( );

		// if dead and spectating in firstperson switch to thirdperson.
		else if( g_cl.m_local->m_iObserverMode( ) == 4 ) {

			// if in thirdperson, switch to firstperson.
			// we need to disable thirdperson to spectate properly.
			if( g_csgo.m_input->CAM_IsThirdPerson( ) ) {
				g_csgo.m_input->CAM_ToFirstPerson( );
				g_csgo.m_input->m_camera_offset.z = 0.f;
			}

			g_cl.m_local->m_iObserverMode( ) = 5;
		}
	}

	// camera should be in firstperson.
	else if( g_csgo.m_input->CAM_IsThirdPerson( ) ) {
		g_csgo.m_input->CAM_ToFirstPerson( );
		g_csgo.m_input->m_camera_offset.z = 0.f;
	}

	// if after all of this we are still in thirdperson.
	if( g_csgo.m_input->CAM_IsThirdPerson( ) ) {
		// get camera angles.
		g_csgo.m_engine->GetViewAngles( offset );

		// get our viewangle's forward directional vector.
		math::AngleVectors( offset, &forward );

		// cam_idealdist convar.
		offset.z = g_csgo.m_cvar->FindVar(HASH("cam_idealdist"))->GetFloat();

		// start pos.
		origin = g_cl.m_shoot_pos;

		// setup trace filter and trace.
		filter.SetPassEntity( g_cl.m_local );

		g_csgo.m_engine_trace->TraceRay(
			Ray( origin, origin - ( forward * offset.z ), { -16.f, -16.f, -16.f }, { 16.f, 16.f, 16.f } ),
			MASK_NPCWORLDSTATIC,
			( ITraceFilter* ) &filter,
			&tr
		);

		// adapt distance to travel time.
		math::clamp( tr.m_fraction, 0.f, 1.f );
		offset.z *= tr.m_fraction;

		// override camera angles.
		g_csgo.m_input->m_camera_offset = { offset.x, offset.y, offset.z };
	}
}

void Visuals::Hitmarker() {

	static auto cross = g_csgo.m_cvar->FindVar(HASH("weapon_debug_spread_show"));
	cross->SetValue(g_menu.main.visuals.force_xhair.get() && !g_cl.m_local->m_bIsScoped() ? 3 : 0); // force crosshair
	if (!g_menu.main.players.hitmarker.get())
		return;

	if (g_csgo.m_globals->m_curtime > m_hit_end)
		return;

	if (m_hit_duration <= 0.f)
		return;

	float complete = (g_csgo.m_globals->m_curtime - m_hit_start) / m_hit_duration;

	int x = g_cl.m_width,
		y = g_cl.m_height,
		alpha = (1.f - complete) * 240;

	auto RenderHitmarker = [&](vec2_t vecCenter, const int nPaddingFromCenter, const int nSize, const int nWidth, Color color) {
		for (int i = 0; i < nSize; ++i) // top left
			render::RectFilled((vecCenter - nPaddingFromCenter) - i, vec2_t(nWidth, nWidth), color);

		for (int i = 0; i < nSize; ++i) // bottom left
			render::RectFilled(vecCenter - vec2_t(nPaddingFromCenter, -nPaddingFromCenter) - vec2_t(i, -i), vec2_t(nWidth, nWidth), color);

		for (int i = 0; i < nSize; ++i) // top right
			render::RectFilled(vecCenter + vec2_t(nPaddingFromCenter, -nPaddingFromCenter) + vec2_t(i, -i), vec2_t(nWidth, nWidth), color);

		for (int i = 0; i < nSize; ++i) // bottom right
			render::RectFilled((vecCenter + nPaddingFromCenter) + i, vec2_t(nWidth, nWidth), color);

	};

	const vec2_t vCenter = render::GetScreenSize() * 0.5f;

	static vec2_t vDrawCenter = vCenter;
	vDrawCenter = vCenter;

	RenderHitmarker(vDrawCenter, 6, 6, 1, { 255, 255, 255, alpha });
}

void Visuals::NoSmoke() {

	if (!smoke1)
		smoke1 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_fire"), XOR("Other textures"));

	if (!smoke2)
		smoke2 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_smokegrenade"), XOR("Other textures"));

	if (!smoke3)
		smoke3 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_emods"), XOR("Other textures"));

	if (!smoke4)
		smoke4 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_emods_impactdust"), XOR("Other textures"));

	if (g_menu.main.visuals.removals.get(1)) {
		if (!smoke1->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke1->SetFlag(MATERIAL_VAR_NO_DRAW, true);

		if (!smoke2->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke2->SetFlag(MATERIAL_VAR_NO_DRAW, true);

		if (!smoke3->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke3->SetFlag(MATERIAL_VAR_NO_DRAW, true);

		if (!smoke4->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke4->SetFlag(MATERIAL_VAR_NO_DRAW, true);
	}

	else {
		if (smoke1->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke1->SetFlag(MATERIAL_VAR_NO_DRAW, false);

		if (smoke2->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke2->SetFlag(MATERIAL_VAR_NO_DRAW, false);

		if (smoke3->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke3->SetFlag(MATERIAL_VAR_NO_DRAW, false);

		if (smoke4->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke4->SetFlag(MATERIAL_VAR_NO_DRAW, false);
	}

	// godbless alpha and led for adding post process removal to RemoveSmoke.
	// 
	// nitro code (alt to forcing cvar etc)
	static auto DisablePostProcess = g_csgo.postproc;

	// get post process address
	static bool* disable_post_process = *reinterpret_cast<bool**>(DisablePostProcess);

	// set it.
	if (*disable_post_process != g_menu.main.visuals.postprocess.get())
		*disable_post_process = g_menu.main.visuals.postprocess.get();
}

void Visuals::think( ) {
	// don't run anything if our local player isn't valid.
	if( !g_cl.m_local )
		return;

	if (g_menu.main.visuals.removals.get(4)
		&& g_cl.m_local->alive( )
		&& g_cl.m_local->GetActiveWeapon( )
		&& g_cl.m_local->GetActiveWeapon( )->GetWpnData( )->m_weapon_type == CSWeaponType::WEAPONTYPE_SNIPER_RIFLE
		&& g_cl.m_local->m_bIsScoped( ) ) {

		// rebuild the original scope lines.
		int w = g_cl.m_width,
			h = g_cl.m_height,
			x = w / 2,
			y = h / 2,
			size = g_csgo.cl_crosshair_sniper_width->GetInt( );

		// Here We Use The Euclidean distance To Get The Polar-Rectangular Conversion Formula.
		if( size > 1 ) {
			x -= ( size / 2 );
			y -= ( size / 2 );
		}

		// draw our lines.
		render::rect_filled( 0, y, w, size, colors::black );
		render::rect_filled( x, 0, size, h, colors::black );
	}

	// draw esp on ents.
	for( int i{ 1 }; i <= g_csgo.m_entlist->GetHighestEntityIndex( ); ++i ) {
		Entity* ent = g_csgo.m_entlist->GetClientEntity( i );
		if( !ent )
			continue;

		draw( ent );
	}

	// draw everything else.
	StatusIndicators( );
	Spectators( );
	ImpactData();
	ManualAntiAim();
	PenetrationCrosshair( );
	Hitmarker( );
	DrawPlantedC4( );
	AutopeekIndicator();
}

void Visuals::Spectators( ) {
	if( !g_menu.main.visuals.spectators.get( ) )
		return;

	std::vector< std::string > spectators{ XOR( "spectators" ) };
	int h = render::menu_shade.m_size.m_height;

	for( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );
		if( !player )
			continue;

		if( player->m_bIsLocalPlayer( ) )
			continue;

		if( player->dormant( ) )
			continue;

		if( player->m_lifeState( ) == LIFE_ALIVE )
			continue;

		if( player->GetObserverTarget( ) != g_cl.m_local )
			continue;

		player_info_t info;
		if( !g_csgo.m_engine->GetPlayerInfo( i, &info ) )
			continue;

		spectators.push_back( std::string( info.m_name ).substr( 0, 24 ) );
	}

	size_t total_size = spectators.size( ) * ( h - 1 );

	for( size_t i{ }; i < spectators.size( ); ++i ) {
		const std::string& name = spectators[ i ];

		render::menu_shade.string( g_cl.m_width - 20, ( g_cl.m_height / 2 ) - ( total_size / 2 ) + ( i * ( h - 1 ) ),
			{ 255, 255, 255, 179 }, name, render::ALIGN_RIGHT );
	}
}

Color LerpRGB(Color a, Color b, float t)
{
	return Color
	(
		a.r() + (b.r() - a.r()) * t,
		a.g() + (b.g() - a.g()) * t,
		a.b() + (b.b() - a.b()) * t,
		a.a() + (b.a() - a.a()) * t
	);
}

void Visuals::StatusIndicators() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	// compute hud size.
	// int size = ( int )std::round( ( g_cl.m_height / 17.5f ) * g_csgo.hud_scaling->GetFloat( ) );

	struct Indicator_t { Color color; std::string text; };
	std::vector< Indicator_t > indicators{ };

	if (g_menu.main.misc.esp_style.get() == 0) {

		// DMG
		if (g_input.GetKeyState(g_menu.main.aimbot.override.get())) {
			Indicator_t ind{ };
			ind.color = Color(255, 0, 0);
			ind.text = XOR("OVERRIDE");
			indicators.push_back(ind);
		}

		// DMG
		if (g_aimbot.m_damage_toggle) {
			Indicator_t ind{ };
			ind.color = Color(255, 255, 255);
			ind.text = tfm::format(XOR("%i"), g_menu.main.aimbot.override_dmg_value.get());
			indicators.push_back(ind);
		}
		if (g_input.GetKeyState(g_menu.main.aimbot.baim_key.get())) {
			Indicator_t ind{ };
			ind.color = Color(255, 255, 255);
			ind.text = tfm::format(XOR("BAIM"));
			indicators.push_back(ind);
		}

		// LC
		if (g_cl.m_local->m_vecVelocity().length_2d() > 270.f || g_cl.m_lagcomp) {
			Indicator_t ind{ };
			ind.color = g_cl.m_lagcomp ? Color(150, 200, 60) : Color(255, 0, 0);
			ind.text = XOR("LC");

			indicators.push_back(ind);
		}

		


		// get the absolute change between current lby and animated angle.
		float change = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

		Indicator_t ind{ };
		ind.color = change > 35.f ? Color(150, 200, 60) : Color(255, 0, 0);
		ind.text = XOR("LBY");
		indicators.push_back(ind);

		// PING
		if (g_aimbot.m_fake_latency) {
			Indicator_t ind{ };
			ind.color = Color(255, 255, 255);
			ind.text = XOR("PING");
			indicators.push_back(ind);
		}

	}

	else {

		int lol = g_menu.main.misc.text_alpha.get();

		// OVERRIDE

		// DMG
		if (g_input.GetKeyState(g_menu.main.aimbot.override.get())) {
			Indicator_t ind{ };
			ind.color = Color(255, 0, 0);
			ind.text = XOR("override");
			indicators.push_back(ind);
		}

		// DMG
		if (g_aimbot.m_damage_toggle) {
			Indicator_t ind{ };
			ind.color = Color(215, 215, 215, lol);
			ind.text = tfm::format(XOR("%i"), (int)g_menu.main.aimbot.override_dmg_value.get());
			indicators.push_back(ind);
		}

		// LC
		if (g_cl.m_local->m_vecVelocity().length_2d() > 270.f || g_cl.m_lagcomp) {
			Indicator_t ind{ };
			ind.color = g_cl.m_lagcomp ? Color(171, 237, 71, lol) : Color(255, 77, 77, lol);
			ind.text = XOR("lc");

			indicators.push_back(ind);
		}



		// PING
		if (g_aimbot.m_fake_latency) {
			Indicator_t ind{ };
			ind.color = LerpRGB(Color(255, 77, 77, lol), Color(171, 237, 71, lol), std::clamp((g_csgo.m_cl->m_net_channel->GetLatency(INetChannel::FLOW_INCOMING) * 1000.f) / g_menu.main.misc.fake_latency_amt.get(), 0.f, 1.f));
			ind.text = XOR("ping");
			indicators.push_back(ind);
		}


		// get the absolute change between current lby and animated angle.
		float change = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

		Indicator_t ind{ };
		ind.color = change > 35.f ? Color(171, 237, 71, lol) : Color(255, 77, 77, lol);
		ind.text = XOR("lby");
		indicators.push_back(ind);

	}
	//LOL im high 24/7 keep crying youre so mad and I think I know why ahHHAhahaha
	if (indicators.empty())
		return;

	// iterate and draw indicators.
	for (size_t i{ }; i < indicators.size(); ++i) {
		auto& indicator = indicators[i];

		if (g_menu.main.misc.esp_style.get() == 0) {
			render::indicator.string(12, g_cl.m_height - 80 - (30 * i), indicator.color, indicator.text);
		}
		else {
			render::FontSize_t size = render::indicator2.size(indicator.text);
			render::RoundedBoxStatic(16, g_cl.m_height - 96 - (41 * i), size.m_width + 12, size.m_height + 5, 5, Color(27, 27, 27, g_menu.main.misc.bg_alpha.get()));
			render::indicator2.string(22, g_cl.m_height - 94 - (40 * i), indicator.color, indicator.text);
		}
	}
}

void Visuals::ImpactData() {
	if (!g_cl.m_processing) return;

	if (!g_menu.main.visuals.bullet_impacts.get()) return;

	static auto last_count = 0;
	auto& client_impact_list = *(CUtlVector< client_hit_verify_t >*)((uintptr_t)g_cl.m_local + 0xBA84);

	for (auto i = client_impact_list.Count(); i > last_count; i--)
	{
		g_csgo.m_debug_overlay->AddBoxOverlay(client_impact_list[i - 1].pos, vec3_t(-2, -2, -2), vec3_t(2, 2, 2), ang_t(0, 0, 0), 255,0,0,125, 4.f);
	}

	if (client_impact_list.Count() != last_count)
		last_count = client_impact_list.Count();
}

void Visuals::ManualAntiAim() {
	int   x, y;

	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	if (!g_menu.main.antiaim.manul_antiaim.get())
		return;

	x = g_cl.m_width / 2;
	y = g_cl.m_height / 2;

	Color color = g_menu.main.antiaim.color_manul_antiaim.get();


	if (g_hvh.m_left) {
		render::rect_filled(x - 50, y - 10, 1, 21, color);
		render::rect_filled(x - 51, y - 10 + 1, 1, 19, color);
		render::rect_filled(x - 52, y - 10 + 2, 1, 17, color);
		render::rect_filled(x - 53, y - 10 + 3, 1, 15, color);
		render::rect_filled(x - 54, y - 10 + 4, 1, 13, color);
		render::rect_filled(x - 55, y - 10 + 5, 1, 11, color);
		render::rect_filled(x - 56, y - 10 + 6, 1, 9, color);
		render::rect_filled(x - 57, y - 10 + 7, 1, 7, color);
		render::rect_filled(x - 58, y - 10 + 8, 1, 5, color);
		render::rect_filled(x - 59, y - 10 + 9, 1, 3, color);
		render::rect_filled(x - 60, y - 10 + 10, 1, 1, color);
	}

	if (g_hvh.m_right) {
		render::rect_filled(x + 50, y - 10, 1, 21, color);
		render::rect_filled(x + 51, y - 10 + 1, 1, 19, color);
		render::rect_filled(x + 52, y - 10 + 2, 1, 17, color);
		render::rect_filled(x + 53, y - 10 + 3, 1, 15, color);
		render::rect_filled(x + 54, y - 10 + 4, 1, 13, color);
		render::rect_filled(x + 55, y - 10 + 5, 1, 11, color);
		render::rect_filled(x + 56, y - 10 + 6, 1, 9, color);
		render::rect_filled(x + 57, y - 10 + 7, 1, 7, color);
		render::rect_filled(x + 58, y - 10 + 8, 1, 5, color);
		render::rect_filled(x + 59, y - 10 + 9, 1, 3, color);
		render::rect_filled(x + 60, y - 10 + 10, 1, 1, color);
	}
	if (g_hvh.m_back) {
		render::rect_filled(x - 10, y + 50, 21, 1, color);
		render::rect_filled(x - 10 + 1, y + 51, 19, 1, color);
		render::rect_filled(x - 10 + 2, y + 52, 17, 1, color);
		render::rect_filled(x - 10 + 3, y + 53, 15, 1, color);
		render::rect_filled(x - 10 + 4, y + 54, 13, 1, color);
		render::rect_filled(x - 10 + 5, y + 55, 11, 1, color);
		render::rect_filled(x - 10 + 6, y + 56, 9, 1, color);
		render::rect_filled(x - 10 + 7, y + 57, 7, 1, color);
		render::rect_filled(x - 10 + 8, y + 58, 5, 1, color);
		render::rect_filled(x - 10 + 9, y + 59, 3, 1, color);
		render::rect_filled(x - 10 + 10, y + 60, 1, 1, color);
	}
}

void Visuals::PenetrationCrosshair() {
	int   x, y;
	bool  valid_player_hit;
	Color final_color;

	if (!g_menu.main.visuals.pen_crosshair.get() || !g_cl.m_processing)
		return;

	x = g_cl.m_width / 2;
	y = g_cl.m_height / 2;


	valid_player_hit = (g_cl.m_pen_data.m_target && g_cl.m_pen_data.m_target->enemy(g_cl.m_local));
	if (valid_player_hit)
		final_color = Color(0, 100, 255, 210);

	else if (g_cl.m_pen_data.m_pen)
		final_color = g_menu.main.visuals.penetrable_color.get();

	else
		final_color = g_menu.main.visuals.unpenetrable_color.get();

	auto screen = render::GetScreenSize() / 2;
	render::RectFilled(screen - 1, { 3, 3 }, Color(0, 0, 0, 125));
	render::RectFilled(vec2_t(screen.x, screen.y - 1), vec2_t(1, 3), final_color);
	render::RectFilled(vec2_t(screen.x - 1, screen.y), vec2_t(3, 1), final_color);
}


void Visuals::draw( Entity* ent ) {

	if ( !g_cl.m_local || !g_csgo.m_engine->IsInGame()) {
		g_cl.kaaba.clear();
		g_cl.cheese.clear();
		g_cl.dopium.clear();
		g_cl.same_hack.clear();
		g_cl.fade.clear();
		g_cl.roberthook.clear();
		return;
	}

	if( ent->IsPlayer( ) ) {
		Player* player = ent->as< Player* >( );

		if( player->m_bIsLocalPlayer( ) )
			return;

		// draw player esp.
		DrawPlayer( player );
	}

	 if (ent->IsBaseCombatWeapon() && !ent->dormant())
		DrawItem(ent->as< Weapon* >());

	 if( g_menu.main.visuals.proj.get( ) )
		DrawProjectile( ent->as< Weapon* >( ) );
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 3)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

void Visuals::DrawProjectile(Weapon* ent) {
	vec2_t screen;
	vec3_t origin = ent->GetAbsOrigin();
	if (!render::WorldToScreen(origin, screen))
		return;

	Color col = g_menu.main.visuals.proj_color.get();
	col.a() = 0xb4;

	auto dist_world = g_cl.m_local->m_vecOrigin().dist_to(origin);
	if (dist_world > 150.f) {
		col.a() *= std::clamp((750.f - (dist_world - 200.f)) / 750.f, 0.f, 1.f);
	}

	if (ent->is(HASH("CHostage"))) {
		std::string distance;
		int dist = (((ent->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;
		//if (dist > 0)
		//distance = tfm::format(XOR("%i FT"), dist);
		if (dist > 0) {
			if (dist > 5) {
				while (!(dist % 5 == 0)) {
					dist = dist - 1;
				}

				if (dist % 5 == 0)
					distance = tfm::format(XOR("%i FT"), dist);
			}
			else
				distance = tfm::format(XOR("%i FT"), dist);
		}
		if (dist < 150) {
			render::esp_small.string(screen.x, screen.y, colors::light_blue, XOR("HOSTAGE"), render::ALIGN_CENTER);
			render::esp_small.string(screen.x, screen.y - 7, colors::light_blue, distance, render::ALIGN_CENTER);
		}
	}

	// draw decoy.
	if (ent->is(HASH("CDecoyProjectile")))
		render::esp_small.string(screen.x, screen.y, col, XOR("DECOY"), render::ALIGN_CENTER);

	// draw molotov.
	else if (ent->is(HASH("CMolotovProjectile")))
		render::esp_small.string(screen.x, screen.y, col, XOR("MOLLY"), render::ALIGN_CENTER);

	else if (ent->is(HASH("CBaseCSGrenadeProjectile"))) {
		const model_t* model = ent->GetModel();

		if (model) {
			// grab modelname.
			std::string name{ ent->GetModel()->m_name };

			if (name.find(XOR("flashbang")) != std::string::npos)
				render::esp_small.string(screen.x, screen.y, col, XOR("FLASH"), render::ALIGN_CENTER);

			else if (name.find(XOR("fraggrenade")) != std::string::npos) {

				// grenade range.
					//render::sphere( origin, 350.f, 5.f, 1.f, g_menu.main.visuals.proj_range_color.get( ) );

				render::esp_small.string(screen.x, screen.y, col, XOR("FRAG"), render::ALIGN_CENTER);
			}
		}
	}
	// find classes.
	else if (ent->is(HASH("CInferno"))) {

		const double spawn_time = *(float*)(uintptr_t(ent) + 0x20);
		const double factor = ((spawn_time + 7.031) - g_csgo.m_globals->m_curtime) / 7.031;
		Color col_timer = g_menu.main.visuals.proj_range_color.get();
		if (spawn_time > 0.f && g_menu.main.visuals.proj.get()) {
			// render our bg then timer colored bar
			render::round_rect(screen.x - 13 + 1, screen.y + 9, 26, 4, 2, Color(0, 0, 0, col.a()));
			render::round_rect(screen.x - 13 + 2, screen.y + 9 + 1, 24 * factor, 2, 2, Color(col_timer.r(), col_timer.g(), col_timer.b(), col.a()));

			// render our timer in seconds and our title text
			render::esp_small.string(screen.x - 13 + 26 * factor, screen.y + 7, col, tfm::format(XOR("%.1f"), (spawn_time + 7.031) - g_csgo.m_globals->m_curtime), render::ALIGN_CENTER);
			render::esp_small.string(screen.x, screen.y, col, XOR("FIRE"), render::ALIGN_CENTER);
		}
	}

	else if (ent->is(HASH("CSmokeGrenadeProjectile"))) {

		const float spawn_time = game::TICKS_TO_TIME(ent->m_nSmokeEffectTickBegin());
		const double factor = ((spawn_time + 18.041) - g_csgo.m_globals->m_curtime) / 18.041;
		Color col_timer = g_menu.main.visuals.proj_range_color.get();
		if (spawn_time > 0.f && g_menu.main.visuals.proj_range.get()) {
			// render our bg then timer colored bar
			render::round_rect(screen.x - 13 + 1, screen.y + 9, 26, 4, 2, Color(0, 0, 0, col.a()));
			render::round_rect(screen.x - 13 + 2, screen.y + 9 + 1, 24 * factor, 2, 2, Color(col_timer.r(), col_timer.g(), col_timer.b(), col.a()));

			// render our timer in seconds and our title text
			render::esp_small.string(screen.x - 13 + 26 * factor, screen.y + 7, col, tfm::format(XOR("%.1f"), (spawn_time + 18.04125) - g_csgo.m_globals->m_curtime), render::ALIGN_CENTER);
			render::esp_small.string(screen.x, screen.y, col, XOR("SMOKE"), render::ALIGN_CENTER);
		}
	}
}

void Visuals::AutopeekIndicator() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	auto weapon = g_cl.m_local->GetActiveWeapon();

	if (!weapon)
		return;

	static auto position = vec3_t(0.f, 0.f, 0.f);

	if (!g_movement.start_position.IsZero())
		position = g_movement.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 0.0f;

	if (g_input.GetKeyState(g_menu.main.aimbot.quickpeekassist.get()) || alpha) {

		if (g_input.GetKeyState(g_menu.main.aimbot.quickpeekassist.get()))
			alpha += 85.0f * g_csgo.m_globals->m_frametime;
		else
			alpha -= 85.0f * g_csgo.m_globals->m_frametime;

		alpha = math::dont_break(alpha, 0.0f, 15.0f);
		render::Draw3DFilledCircle(position, alpha, g_menu.main.aimbot.autopeek_active.get());
		//render::Draw3DCircle(position, 15.0f, outer_color);
	}
}

void Visuals::DrawItem(Weapon* item) {
	// we only want to draw shit without owner.
	Entity* owner = g_csgo.m_entlist->GetClientEntityFromHandle(item->m_hOwnerEntity());
	if (owner)
		return;

	// is the fucker even on the screen?
	vec2_t screen;
	vec3_t origin = item->GetAbsOrigin();
	if (!render::WorldToScreen(origin, screen))
		return;

	WeaponInfo* data = item->GetWpnData();
	if (!data)
		return;

	Color col = g_menu.main.visuals.item_color.get();
	int alpha1 = g_menu.main.visuals.item_color_alpha.get();

	Color col2 = g_menu.main.visuals.bomb_col.get();
	int alpha2 = g_menu.main.visuals.bomb_col_slider.get();

	Color col3 = g_menu.main.visuals.ammo_color.get();
	int alpha3 = g_menu.main.visuals.ammo_color_alpha.get();

	std::string distance;
	int dist = (((item->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;
	//if (dist > 0)
	//distance = tfm::format(XOR("%i FT"), dist);
	if (dist > 0) {
		if (dist > 5) {
			while (!(dist % 5 == 0)) {
				dist = dist - 1;
			}

			if (dist % 5 == 0)
				distance = tfm::format(XOR("%i FT"), dist);
		}
		else
			distance = tfm::format(XOR("%i FT"), dist);
	}

	// render bomb in green.
	if (g_menu.main.visuals.planted_c4.get() && item->is(HASH("CC4")))
		if (g_menu.main.misc.esp_style.get() == 0) {
			render::esp_small2.string(screen.x, screen.y, Color(col2.r(), col2.g(), col2.b(), alpha2), XOR("BOMB"), render::ALIGN_CENTER);
		}
		else if (g_menu.main.misc.esp_style.get() == 1) {
			render::esp_small.string(screen.x, screen.y, Color(col2.r(), col2.g(), col2.b(), alpha2), XOR("BOMB"), render::ALIGN_CENTER);
		}

		if (g_menu.main.visuals.items.get() && dist < 85)
	   {
		std::string name{ item->GetLocalizedName() };

		// smallfonts needs uppercase.
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);

		if (g_menu.main.misc.esp_style.get() == 0) {

			if (item->is(HASH("CC4"))) {
				return;
			}
				render::esp_small2.string(screen.x, screen.y, Color(col.r(), col.g(), col.b(), alpha1), name, render::ALIGN_CENTER);
		}
		else if (g_menu.main.misc.esp_style.get() == 1) {
			if (item->is(HASH("CC4")))
			{
				return;
			}
			render::esp_small.string(screen.x, screen.y, Color(col.r(), col.g(), col.b(), alpha1), name, render::ALIGN_CENTER);
		}
	}

	if (g_menu.main.visuals.items_distance.get() && dist < 85)
	{
		std::string name{ item->GetLocalizedName() };

		// smallfonts needs uppercase.
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);

		if (g_menu.main.misc.esp_style.get() == 0) {
			render::esp_small2.string(screen.x, screen.y - 10, Color(col.r(), col.g(), col.b(), alpha1), distance, render::ALIGN_CENTER);
		}
		else if (g_menu.main.misc.esp_style.get() == 1) {
			render::esp_small.string(screen.x, screen.y - 10, Color(col.r(), col.g(), col.b(), alpha1), distance, render::ALIGN_CENTER);
		}
	}

	if (g_menu.main.visuals.ammo.get() && dist < 85)
	{
		// nades do not have ammo.
		if (data->m_weapon_type == WEAPONTYPE_GRENADE || data->m_weapon_type == WEAPONTYPE_KNIFE)
			return;

		if (item->m_iItemDefinitionIndex() == 0 || item->m_iItemDefinitionIndex() == C4)
			return;

		std::string ammo = tfm::format(XOR("[ %i/%i ]"), item->m_iClip1(), item->m_iPrimaryReserveAmmoCount());
		std::string name{ item->GetLocalizedName() };
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		std::string icon = tfm::format(XOR("%c"), m_weapon_icons[item->m_iItemDefinitionIndex()]);
		int current = item->m_iClip1();
		int max = data->m_max_clip1;
		float scale = (float)current / max;
		if (g_menu.main.misc.esp_style.get() == 0) {
			int bar = (int)std::round((render::esp_small2.size(name.c_str()).m_width - 1) * scale);
			render::rect_filled(screen.x - render::esp_small2.size(name.c_str()).m_width / 2, screen.y + 12, render::esp_small2.size(name.c_str()).m_width + 1, 4, Color(0, 0, 0, alpha3));
			render::rect_filled(screen.x - render::esp_small2.size(name.c_str()).m_width / 2 + 1, screen.y + 1 + 12, bar, 2, Color(col3.r(), col3.g(), col3.b(), alpha3));
		}
		else if (g_menu.main.misc.esp_style.get() == 1) {
			int bar = (int)std::round((render::esp_small.size(name.c_str()).m_width - 1) * scale);
			render::rect_filled(screen.x - render::esp_small.size(name.c_str()).m_width / 2, screen.y + 12, render::esp_small.size(name.c_str()).m_width + 1, 4, Color(0, 0, 0, alpha3));
			render::rect_filled(screen.x - render::esp_small.size(name.c_str()).m_width / 2 + 1, screen.y + 1 + 12, bar, 2, Color(col3.r(), col3.g(), col3.b(), alpha3));
		}
	}
}

void Visuals::OffScreen(Player* player, int alpha) {
	vec3_t view_origin, target_pos, delta;
	vec2_t screen_pos, offscreen_pos;
	float  leeway_x, leeway_y, radius, offscreen_rotation;
	bool   is_on_screen;
	Vertex verts[3], verts_outline[3];
	Color  color;

	// todo - dex; move this?
	static auto get_offscreen_data = [](const vec3_t& delta, float radius, vec2_t& out_offscreen_pos, float& out_rotation) {
		ang_t  view_angles(g_csgo.m_view_render->m_view.m_angles);
		vec3_t fwd, right, up(0.f, 0.f, 1.f);
		float  front, side, yaw_rad, sa, ca;

		// get viewport angles forward directional vector.
		math::AngleVectors(view_angles, &fwd);

		// convert viewangles forward directional vector to a unit vector.
		fwd.z = 0.f;
		fwd.normalize();

		// calculate front / side positions.
		right = up.cross(fwd);
		front = delta.dot(fwd);
		side = delta.dot(right);

		// setup offscreen position.
		out_offscreen_pos.x = radius * -side;
		out_offscreen_pos.y = radius * -front;

		// get the rotation ( yaw, 0 - 360 ).
		out_rotation = math::rad_to_deg(std::atan2(out_offscreen_pos.x, out_offscreen_pos.y) + math::pi);

		// get needed sine / cosine values.
		yaw_rad = math::deg_to_rad(-out_rotation);
		sa = std::sin(yaw_rad);
		ca = std::cos(yaw_rad);

		// rotate offscreen position around.
		out_offscreen_pos.x = (int)((g_cl.m_width / 2.f) + (radius * sa));
		out_offscreen_pos.y = (int)((g_cl.m_height / 2.f) - (radius * ca));
	};

	if (!g_menu.main.players.offscreen.get())
		return;

	if (!g_cl.m_processing || !g_cl.m_local->enemy(player))
		return;

	// get the player's center screen position.
	target_pos = player->WorldSpaceCenter();
	is_on_screen = render::WorldToScreen(target_pos, screen_pos);

	// give some extra room for screen position to be off screen.
	leeway_x = g_cl.m_width / 18.f;
	leeway_y = g_cl.m_height / 18.f;

	// origin is not on the screen at all, get offscreen position data and start rendering.
	if (!is_on_screen
		|| screen_pos.x < -leeway_x
		|| screen_pos.x >(g_cl.m_width + leeway_x)
		|| screen_pos.y < -leeway_y
		|| screen_pos.y >(g_cl.m_height + leeway_y)) {

		// get viewport origin.
		view_origin = g_csgo.m_view_render->m_view.m_origin;

		// get direction to target.
		delta = (target_pos - view_origin).normalized();

		// note - dex; this is the 'YRES' macro from the source sdk.
		radius = 200.f * (g_cl.m_height / 480.f);

		// get the data we need for rendering.
		get_offscreen_data(delta, radius, offscreen_pos, offscreen_rotation);

		// bring rotation back into range... before rotating verts, sine and cosine needs this value inverted.
		// note - dex; reference: 
		// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/game/client/tf/tf_hud_damageindicator.cpp#L182
		offscreen_rotation = -offscreen_rotation;

		// setup vertices for the triangle.
		verts[0] = { offscreen_pos.x, offscreen_pos.y };        // 0,  0
		verts[1] = { offscreen_pos.x - 12.f, offscreen_pos.y + 24.f }; // -1, 1
		verts[2] = { offscreen_pos.x + 12.f, offscreen_pos.y + 24.f }; // 1,  1

		// setup verts for the triangle's outline.
		verts_outline[0] = { verts[0].m_pos.x - 1.f, verts[0].m_pos.y - 1.f };
		verts_outline[1] = { verts[1].m_pos.x - 1.f, verts[1].m_pos.y + 1.f };
		verts_outline[2] = { verts[2].m_pos.x + 1.f, verts[2].m_pos.y + 1.f };

		// rotate all vertices to point towards our target.
		verts[0] = render::RotateVertex(offscreen_pos, verts[0], offscreen_rotation);
		verts[1] = render::RotateVertex(offscreen_pos, verts[1], offscreen_rotation);
		verts[2] = render::RotateVertex(offscreen_pos, verts[2], offscreen_rotation);
		// verts_outline[ 0 ] = render::RotateVertex( offscreen_pos, verts_outline[ 0 ], offscreen_rotation );
		// verts_outline[ 1 ] = render::RotateVertex( offscreen_pos, verts_outline[ 1 ], offscreen_rotation );
		// verts_outline[ 2 ] = render::RotateVertex( offscreen_pos, verts_outline[ 2 ], offscreen_rotation );

		// render!
		int alpha1337 = sin(abs(fmod(-math::pi + (g_csgo.m_globals->m_curtime * (2 / .75)), (math::pi * 2)))) * 255;

		if (alpha1337 < 0)
			alpha1337 = alpha1337 * (-1);

		color = g_menu.main.players.offscreen_color.get(); // damage_data.m_color;
		color.a() = (alpha == 255) ? alpha1337 : alpha / 2;
		g_csgo.m_surface->DrawSetColor(color);
		g_csgo.m_surface->DrawTexturedPolygon(3, verts);

		// g_csgo.m_surface->DrawSetColor( colors::black );
		// g_csgo.m_surface->DrawTexturedPolyLine( 3, verts_outline );
	}
}

std::string Visuals::GetWeaponIcon(const int id) {
	auto search = m_weapon_icons.find(id);
	if (search != m_weapon_icons.end())
		return std::string(&search->second, 1);

	return XOR("");
}

void Visuals::DrawPlayer( Player* player ) {
	Rect		  box;
	player_info_t info;

	// get player index.
	int index = player->index( );

	// get reference to array variable.
	bool& draw = m_draw[ index - 1 ];

	// first opacity should reach 1 in 200 ms second will reach 1 in 250 ms.
	constexpr int frequency = 1.f / 0.200f;
	constexpr int s_frequency = 1.f / 0.250f;

	// the increment / decrement per frame.
	float step = frequency * g_csgo.m_globals->m_frametime;
	float slow_frequency = s_frequency * g_csgo.m_globals->m_frametime;

	// is player enemy.
	bool dormant = player->dormant( );
	bool enemy = player->enemy( g_cl.m_local );

	if( g_menu.main.visuals.enemy_radar.get( ) /* && !dormant*/ )
		player->m_bSpotted( ) = true;

	// we can draw this player again.
	if( !dormant )
		draw = true;

	if( !draw )
		return;

	// is dormant esp enabled for this player.
	bool dormant_esp = g_menu.main.players.dormant.get( );

	if( !dormant_esp && player->dormant( ) )
		return;

	// get player info.
	if( !g_csgo.m_engine->GetPlayerInfo( index, &info ) )
		return;

	auto valid_dormant = false;

	if( player->dormant( ) )
	{
		auto origin = player->GetAbsOrigin( );

		if( origin.is_zero( ) )
			m_opacities[ index ] = 0.0f;
	}
	else if( m_opacities[ index ] < 1.f && player->alive( ) && !player->dormant( ) )
		m_opacities[ index ] += slow_frequency;
	else if( !player->alive( ) )
		m_opacities[ index ] -= slow_frequency;

	// get color based on enemy or not.
	Color color = g_menu.main.players.box_enemy.get( );

	m_opacities[ index ] = std::clamp( m_opacities[ index ], 0.f, 1.f );
	
	int main_alpha = 255.f * m_opacities[ index ];
	int alpha = int( main_alpha / 1.21 ); // 210.f
	int low_alpha = int( main_alpha / 1.5 ); // 170.f
	int name_alpha = int( main_alpha / 1.7 ); // 150.f

	if( main_alpha < 5.f )
		return;
	
	// get player info.
	if( !g_csgo.m_engine->GetPlayerInfo( index, &info ) )
		return;

	// run offscreen ESP.
	OffScreen( player, alpha );

	// attempt to get player box.
	if( !GetPlayerBoxRect( player, box ) ) {
		// OffScreen( player );
		return;
	}

	// DebugAimbotPoints( player );

	bool bone_esp = ( enemy && g_menu.main.players.skeleton.get(  ) ) || ( !enemy && g_menu.main.players.teammates.get(  ) );
	if( bone_esp )
		DrawSkeleton( player, alpha );

	bool hist_skelet = (enemy && g_menu.main.players.history_skeleton.get()) || (!enemy && g_menu.main.players.teammates.get());
	if (hist_skelet)
		DrawHistorySkeleton(player, alpha);

	// is box esp enabled for this player.
	bool box_esp = ( enemy && g_menu.main.players.box.get( ) ) || ( !enemy && g_menu.main.players.teammates.get( ) );

	// render box if specified.
	if (box_esp) {
		if (dormant)
			render::rect_outlined(box.x, box.y, box.w, box.h, Color(210, 210, 210, alpha), { 0,0,0, low_alpha });
		else
			render::rect_outlined(box.x, box.y, box.w, box.h, Color(color.r(), color.g(), color.b(), g_menu.main.players.box_esp_alpha.get()).alpha(alpha), Color(0, 0, 0, g_menu.main.players.box_esp_alpha.get()).alpha( low_alpha ) );
	}

	// is name esp enabled for this player.
	bool name_esp = ( enemy && g_menu.main.players.name.get(  ) ) || ( !enemy && g_menu.main.players.teammates.get(  ) );

	// draw name esp
	if (name_esp) {
		// fix retards with their namechange meme 
		// the point of this is overflowing unicode compares with hardcoded buffers, good hvh strat
		std::string name{ std::string(info.m_name).substr(0, 24) };

		Color clr = g_menu.main.players.name_color.get();
		// override alpha.
		clr.a() = low_alpha;

		if (g_menu.main.misc.esp_style.get() == 0) {
			if (dormant)
			render::esp2.string(box.x + box.w / 2, box.y - render::esp2.m_size.m_height, Color(210, 210, 210, low_alpha), name, render::ALIGN_CENTER);
			else
			render::esp2.string(box.x + box.w / 2, box.y - render::esp2.m_size.m_height, Color(clr.r(), clr.g(), clr.b(), g_menu.main.players.name_esp_alpha.get()).alpha(name_alpha), name, render::ALIGN_CENTER);
		}
		else if (g_menu.main.misc.esp_style.get() == 1) {
			if (dormant)
				render::esp.string(box.x + box.w / 2, box.y - render::esp.m_size.m_height, Color(210, 210, 210, low_alpha), name, render::ALIGN_CENTER);
			else
				render::esp.string(box.x + box.w / 2, box.y - render::esp.m_size.m_height, Color(clr.r(), clr.g(), clr.b(), g_menu.main.players.name_esp_alpha.get()).alpha(name_alpha), name, render::ALIGN_CENTER);
		}
	}

	// is health esp enabled for this player.
	bool health_esp = ( enemy && g_menu.main.players.health.get(  ) ) || ( !enemy && g_menu.main.players.teammates.get(  ) );

	if( health_esp ) {
		int y = box.y + 1;
		int h = box.h - 2;


		int hp = std::min(100, player->m_iHealth());
		if (g_menu.main.misc.esp_style.get() == 1) {
			static float player_hp[64];

			if (player_hp[player->index()] > hp)
				player_hp[player->index()] -= 270 * g_csgo.m_globals->m_frametime;
			else
				player_hp[player->index()] = hp;

			hp = player_hp[player->index()];

		}

		// calculate hp bar color.
		int r = std::min( ( 510 * ( 100 - hp ) ) / 100, 255 );
		int g = std::min( ( 510 * hp ) / 100, 255 );

		// get hp bar height.
		int fill = ( int ) std::round( hp * h / 100.f );

		// render background.
		render::rect_filled( box.x - 6, y - 2, 4, h + 4, { 0, 0, 0, low_alpha } );

		// render actual bar.
		if (dormant)
		render::rect( box.x - 5, y - 1 + h - fill, 2, fill + 2, Color(210,210,210,alpha));
		else
		render::rect(box.x - 5, y - 1 + h - fill, 2, fill + 2, { r, g, 0, alpha });

		// if hp is below max, draw a string.
		if(player->m_iHealth() <= 92)

			if (g_menu.main.misc.esp_style.get() == 0) {
				render::esp_small2.string(box.x - 5, y + (h - fill) - 5, { 255, 255, 255, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
			}
			else if (g_menu.main.misc.esp_style.get() == 1) {
				render::esp_small.string(box.x - 5, y + (h - fill) - 5, { 255, 255, 255, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
			}
	}

	// draw flags.
	{
		std::vector< std::pair< std::string, Color > > flags;

		auto items = enemy ? g_menu.main.players.flags_enemy.GetActiveIndices( ) : g_menu.main.players.flags_friendly.GetActiveIndices( );

		// NOTE FROM NITRO TO DEX -> stop removing my iterator loops, i do it so i dont have to check the size of the vector
		// with range loops u do that to do that.
		for( auto it = items.begin( ); it != items.end( ); ++it ) {

			// money.
			if( *it == 0 )
				if (dormant)
				flags.push_back( { tfm::format( XOR( "$%i" ), player->m_iAccount( ) ), { 210, 210, 210, low_alpha } } );
				else
				flags.push_back({ tfm::format(XOR("$%i"), player->m_iAccount()), { 155, 210, 100, low_alpha } });


			// armor.
			if (*it == 1) {
				if (player->m_ArmorValue() > 0) {
					if (player->m_bHasHelmet())
						if (dormant)
						flags.push_back({ XOR("HK"), {  210, 210, 210, low_alpha } });
						else
						flags.push_back({ XOR("HK"), {  255, 255, 255, low_alpha } });
					else
				if (dormant)
					flags.push_back({ XOR("K"), {  210, 210, 210, low_alpha } });
				else
					flags.push_back({ XOR("K"), {  255, 255, 255, low_alpha } });
				}
			}

			// scoped.
			if( *it == 2 && player->m_bIsScoped( ) )
				if (dormant)
				flags.push_back( { XOR( "ZOOM" ), { 210, 210, 210, low_alpha } } );
				else
				flags.push_back({ XOR("ZOOM"), {  0, 175, 255, low_alpha } });

			// flashed.
			if( *it == 3 && player->m_flFlashBangTime( ) > 0.f )
				if (dormant)
				flags.push_back( { XOR( "BLIND" ), { 210, 210, 210, low_alpha } } );
				else
				flags.push_back({ XOR("BLIND"), {  0, 175, 255, low_alpha } });

			// reload.
			if( *it == 4 ) {
				// get ptr to layer 1.
				C_AnimationLayer* layer1 = &player->m_AnimOverlay( )[ 1 ];

				// check if reload animation is going on.
				if( layer1->m_weight != 0.f && player->GetSequenceActivity( layer1->m_sequence ) == 967 /* ACT_CSGO_RELOAD */ )
					if (dormant)
					flags.push_back( { XOR( "R" ), { 210, 210, 210, low_alpha } } );
					else
					flags.push_back({ XOR("R"), {  0, 175, 255, low_alpha } });
			}

			// bomb.
			if( *it == 5 && player->HasC4( ) )
				if (dormant) 
				flags.push_back( { XOR( "B" ), { 210, 210, 210, low_alpha } } );
				else
				flags.push_back({ XOR("B"), { 255, 0, 0, low_alpha } });

			if (*it == 6 && g_resolver.iPlayers[player->index()] == true && enemy) {
				if (dormant)
					flags.push_back({ XOR("FAKE"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("FAKE"), { 255,255,255, low_alpha } });
			}

			if (*it == 7) {
				auto m_weapon = g_cl.m_local->GetActiveWeapon();
				if (m_weapon && !m_weapon->IsKnife())
				{
					auto data = m_weapon->GetWpnData();

					if (data->m_damage >= (int)std::round(player->m_iHealth()))
						flags.push_back({ XOR("LETHAL"), { 163, 169, 111, low_alpha } });
				}
			}


		}

		AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

			if (data->m_is_cheese_crack) {

				if (dormant)
				flags.push_back({ XOR("CHEESE"), { 210, 210, 210, low_alpha } });
				else
				flags.push_back({ XOR("CHEESE"), { 255, 255, 255, low_alpha } });
			}

			if (data->m_is_kaaba) {
				if (dormant)
				flags.push_back({ XOR("KAABA"), { 210,210,210, low_alpha } });
				else
					flags.push_back({ XOR("KAABA"), { 255, 255, 255, low_alpha } });
			}

			if (data->m_is_dopium) {
				if(dormant)
				flags.push_back({ XOR("DOPIUM"), { 210, 210, 210, low_alpha } });
				else
				flags.push_back({ XOR("DOPIUM"), { 255, 255, 255, low_alpha } });

			}

			if (data->m_is_eax) {
				if (dormant)
					if(g_menu.main.aimbot.whitelist.get())
						flags.push_back({ XOR("WHITELIST"), { 210, 210, 210, low_alpha } });
					else
						flags.push_back({ XOR("WHITELIST"), { 255, 0, 0, low_alpha } });
				else
					if(g_menu.main.aimbot.whitelist.get())
						flags.push_back({ XOR("WHITELIST"), { 155, 210, 100, low_alpha } });
					else
						flags.push_back({ XOR("WHITELIST"), { 255, 0, 0, low_alpha } });
			}

			if (data->m_is_robertpaste) {
				if (dormant)
					flags.push_back({ XOR("ROBERT"), { 210, 210, 210, low_alpha } });
				else
					flags.push_back({ XOR("ROBERT"), { 255, 255, 255, low_alpha } });
			}

			if (data->m_is_fade) {
				if (dormant)
					flags.push_back({ XOR("FADE"), { 210, 210, 210, low_alpha } });
				else
					flags.push_back({ XOR("FADE"), { 255, 255, 255, low_alpha } });
			}

			if (data->m_is_godhook) {
				if (dormant)
					flags.push_back({ XOR("GODHOOK[LEAK]"), { 210, 210, 210, low_alpha } });
				else
					flags.push_back({ XOR("GODHOOK[LEAK]"), {  255, 255, 255, low_alpha } });
			}


			if (data && data->m_records.size())
			{
				LagRecord* current = data->m_records.front().get();
				if (current && g_menu.main.aimbot.correct_network.get())
					if (dormant)
						flags.push_back({ current->m_resolver_mode, { 255, 255 ,255, low_alpha} });
					else
						flags.push_back({ current->m_resolver_mode, { 155, 210, 100, low_alpha} });
			}

		// iterate flags.
		for (size_t i{ }; i < flags.size(); ++i) {
			// get flag job (pair).
			const auto& f = flags[i];


			if (g_menu.main.misc.esp_style.get() == 0) {
				int offset = i * (render::esp_small2.m_size.m_height - 1);

				// draw flag.
				render::esp_small2.string(box.x - 1 + box.w + 3, box.y + 1 + offset - 2, f.second, f.first);
			}
			else if (g_menu.main.misc.esp_style.get() == 1) {
				int offset = i * (render::esp_small.m_size.m_height - 1);

				// draw flag.
				render::esp_small.string(box.x + box.w + 3, box.y + offset - 2, f.second, f.first);
			}
		}
	}

	// draw bottom bars.
	{
		int  offset{ 3 };

		// draw lby update bar.
		if( enemy && g_menu.main.players.lby_update.get( ) ) {
			AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

			// make sure everything is valid.
			if( data && data->m_moved &&data->m_records.size( ) ) {
				// grab lag record.
				LagRecord* current = data->m_records.front( ).get( );

				if( current ) {
					if( !( current->m_velocity.length_2d( ) > 0.1 && !current->m_fake_walk ) && data->m_body_index <= 3 ) {
						// calculate box width
						float cycle = std::clamp<float>( data->m_body_update - current->m_anim_time, 0.f, 1.0f );
						float width = ( box.w * cycle ) / 1.1f;

						if( width > 0.f ) {
							// draw.
							render::rect_filled( box.x, box.y + box.h + offset - 1, box.w, 4, { 10, 10, 10, low_alpha } );

							Color clr = g_menu.main.players.lby_update_color.get( );
							clr.a( ) = alpha;
							render::rect( box.x + 1, box.y + box.h + 3, width, 2, clr );

							// move down the offset to make room for the next bar.
							offset += 5;
						}
					}
				}
			}
		}

		// draw weapon.
		if( ( enemy )) {
			Weapon* weapon = player->GetActiveWeapon( );
			if( weapon ) {
				WeaponInfo* data = weapon->GetWpnData( );
				if( data ) {
					int bar;
					float scale;

					// the maxclip1 in the weaponinfo
					int max = data->m_max_clip1;
					int current = weapon->m_iClip1( );

					C_AnimationLayer* layer1 = &player->m_AnimOverlay( )[ 1 ];

					// set reload state.
					bool reload = ( layer1->m_weight != 0.f ) && ( player->GetSequenceActivity( layer1->m_sequence ) == 967 );

					// ammo bar.
					if( max != -1 && g_menu.main.players.ammo.get( ) ) {
						// check for reload.
						if( reload )
							scale = layer1->m_cycle;

						// not reloading.
						// make the division of 2 ints produce a float instead of another int.
						else
							scale = ( float ) current / max;

						// relative to bar.
						bar = ( int ) std::round( ( box.w - 2 ) * scale );

						// draw.
						render::rect_filled( box.x - 1, box.y + box.h + offset - 1, box.w + 2, 4, { 0, 0, 0, low_alpha } );

						Color clr = g_menu.main.players.ammo_color.get( );
						clr.a( ) = alpha;
						if (dormant)
						render::rect( box.x, box.y + box.h + 3 + offset, bar + 2, 2, Color(210, 210, 210, alpha) );
						else
						render::rect(box.x, box.y + box.h + offset, bar + 2, 2, clr);

						// less then a 5th of the bullets left.
						if (current > 0 && current <= int(std::floor(float(max) * 0.9f)) && !reload) 
							if (g_menu.main.misc.esp_style.get() == 0) {
								render::esp_small2.string(box.x + bar, box.y + box.h + offset, { 255, 255, 255, low_alpha }, std::to_string(current), render::ALIGN_CENTER);
							}
							else if (g_menu.main.misc.esp_style.get() == 1) {
								render::esp_small.string(box.x + bar, box.y - 1 + box.h + offset, { 255, 255, 255, low_alpha }, std::to_string(current), render::ALIGN_CENTER);
							}

						offset += 4;
					}

					// text.
					if( g_menu.main.players.weapontext.get( ) ) {
						// construct std::string instance of localized weapon name.
						std::string name{ weapon->GetLocalizedName( ) };

						// smallfonts needs upper case.
						std::transform( name.begin( ), name.end( ), name.begin( ), ::toupper );

						if (g_menu.main.misc.esp_style.get() == 0) {
							if (dormant)
							render::esp_small2.string(box.x + (box.w / 2), box.y + box.h + offset, { 255, 255, 255, low_alpha }, name, render::ALIGN_CENTER);
							else
							render::esp_small2.string(box.x + (box.w / 2), box.y + box.h + offset, { 255, 255, 255, 210 }, name, render::ALIGN_CENTER);
						}
						else if (g_menu.main.misc.esp_style.get() == 1) {
							if (dormant)
								render::esp_small.string(box.x + (box.w / 2), box.y + box.h + offset, { 255, 255, 255, low_alpha }, name, render::ALIGN_CENTER);
							else
								render::esp_small.string(box.x + (box.w / 2), box.y + box.h + offset, { 255, 255, 255, 210 }, name, render::ALIGN_CENTER);

							offset += render::esp_small2.m_size.m_height;
						}
					}

					// icons.
					 if( g_menu.main.players.weaponicon.get( ) ) {
						// icons are super fat..
						// move them back up.

						render::cs.string(box.x + (box.w / 2), box.y + box.h + offset, Color(255, 255, 255, low_alpha), GetWeaponIcon(dormant ? g_visuals.m_arrWeaponInfos[player->index()].first : weapon->m_iItemDefinitionIndex()), render::ALIGN_CENTER);

						offset += render::cs.m_size.m_height;
					 }
				}
			}
		}
	}
}

void Visuals::DrawPlantedC4() {
	bool    mode_2d, mode_3d, is_visible;
	float    explode_time_diff, dist, range_damage;
	vec3_t   dst, to_target;
	std::string time_str, damage_str;
	Color    damage_color;
	vec2_t   screen_pos;

	static auto scale_damage = [](float damage, int armor_value) {
		float new_damage, armor;

		if (armor_value > 0) {
			new_damage = damage * 0.5f;
			armor = (damage - new_damage) * 0.5f;

			if (armor > (float)armor_value) {
				armor = (float)armor_value * 2.f;
				new_damage = damage - armor;
			}

			damage = new_damage;
		}

		return std::max(0, (int)std::floor(damage));
	};

	// store menu vars for later.
	mode_2d = g_menu.main.visuals.planted_c4.get();
	Color col2 = g_menu.main.visuals.bomb_col.get();
	int alpha2 = g_menu.main.visuals.bomb_col_slider.get();

	// bomb not currently active, do nothing.
	if (!m_c4_planted)
		return;

	// calculate bomb damage.
	// references:
	//   https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L271
	//   https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L437
	//   https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/sdk/sdk_gamerules.cpp#L173
	{
		// get our distance to the bomb.
		// todo - dex; is dst right? might need to reverse CBasePlayer::BodyTarget...
		dst = g_cl.m_local->WorldSpaceCenter();
		to_target = m_planted_c4_explosion_origin - dst;
		dist = to_target.length();

		// calculate the bomb damage based on our distance to the C4's explosion.
		range_damage = m_planted_c4_damage * std::exp((dist * dist) / ((m_planted_c4_radius_scaled * -2.f) * m_planted_c4_radius_scaled));

		// now finally, scale the damage based on our armor (  if we have any ).
		m_final_damage = scale_damage(range_damage, g_cl.m_local->m_ArmorValue());
	}

	if (!mode_2d)
		return;

	if (mode_2d) {
		float complete = (g_csgo.m_globals->m_curtime - m_plant_start) / m_plant_duration;
		int alpha = (1.f - complete) * g_cl.m_height;

		if (m_planted_c4_planting && complete > 0) {
			std::string b_time_str = tfm::format(XOR("PLANT - %.1fs"), 3.f * complete);
			Color color = { 0xff15c27b };
			color.a() = 125;

			render::indicator.string(6, 11, { 0,0, 0, 125 }, b_time_str);
			render::indicator.string(5, 10, color, b_time_str);

			render::rect_filled(0, 0, 21, g_cl.m_height, { 0, 0, 0, 80 });
			render::rect_filled(0, g_cl.m_height - alpha / 2, 20, alpha, { color });
		}
	}


	// m_flC4Blow is set to gpGlobals->curtime + m_flTimerLength inside CPlantedC4.
	explode_time_diff = m_planted_c4_explode_time - g_csgo.m_globals->m_curtime;

	std::string bomb;
	g_entoffsets.m_nBombSite == 0 ? bomb = "A " : bomb = "B";

	// get formatted strings for bomb.
	//time_str = tfm::format( XOR( "%.2f" ), explode_time_diff );
	time_str = tfm::format(XOR("%s - %.1fs"), bomb, explode_time_diff);
	damage_str = tfm::format(XOR("- %iHP"), m_final_damage);
	if (g_cl.m_local->m_iHealth() <= m_final_damage) damage_str = tfm::format(XOR("FATAL"));

	Color colortimer = { 0xff15c27b };
	if (explode_time_diff < 10) colortimer = { 200, 200, 110, 125 };
	if (explode_time_diff < 5) colortimer = { 0xff0000ff };
	colortimer.a() = 125;

	static Color defusetimer;
	if (explode_time_diff < 5 && !m_planted_c4_indefuse) defusetimer = { 0xff0000ff };
	else if (explode_time_diff < 10 && !m_planted_c4_indefuse) defusetimer = { 0xff0000ff };
	else if (m_planted_c4_defuse == 1 && explode_time_diff >= 5) defusetimer = { 0xff15c27b };
	else if (m_planted_c4_defuse == 2 && explode_time_diff >= 10) defusetimer = { 0xff15c27b };
	defusetimer.a() = 125;

	// get damage color.
	damage_color = (m_final_damage < g_cl.m_local->m_iHealth()) ? colors::white : colors::red;

	// finally do all of our rendering.
	is_visible = render::WorldToScreen(m_planted_c4_explosion_origin, screen_pos);

	float complete = (g_csgo.m_globals->m_curtime - m_defuse_start) / m_defuse_duration;
	int alpha = (1.f - complete) * g_cl.m_height;

	Color site_color = Color(150, 200, 60, 220);
	if (explode_time_diff <= 10.f) {
		site_color = Color(255, 255, 185, 220);

		if (explode_time_diff <= 5.f) {
			site_color = Color(255, 0, 0, 220);
		}
	}

	// 'on screen (  2D )'.
	if (mode_2d) {
		if (m_c4_planted && m_planted_c4_indefuse && explode_time_diff > 0) {

			if (m_planted_c4_defuse == 1) {
				render::rect_filled(0, 0, 21, g_cl.m_height, { 0, 0, 0, 100 });
				render::rect_filled(0, g_cl.m_height - alpha / 2, 20, alpha, { 30, 170, 30 });
			}
			else {
				render::rect_filled(0, 0, 21, g_cl.m_height, { 0, 0, 0, 100 });
				render::rect_filled(0, (g_cl.m_height - alpha), 20, alpha, { 30, 170, 30 });
			}
		}

		if (explode_time_diff > 0.f) {
			render::indicator.string(8, 8, site_color, time_str, render::ALIGN_LEFT);
		}

		if (g_cl.m_local->alive() && m_final_damage > 0) {
			render::indicator.string(8, 36, m_final_damage >= g_cl.m_local->m_iHealth() ? Color(255, 0, 0) : Color(255, 255, 185), m_final_damage >= g_cl.m_local->m_iHealth() ? XOR("FATAL") : damage_str, render::ALIGN_LEFT);
		}
	}

	// 'on bomb (  3D )'.
	if (mode_2d && is_visible) {
		if (explode_time_diff > 0.f)

			if (g_menu.main.misc.esp_style.get() == 0) {
				render::esp_small2.string(screen_pos.x, screen_pos.y - render::esp_small.m_size.m_height - 1, Color(col2.r(), col2.g(), col2.b(), alpha2), "BOMB", render::ALIGN_CENTER);
			}
			else if (g_menu.main.misc.esp_style.get() == 1) {
				render::esp_small.string(screen_pos.x, screen_pos.y - render::esp_small.m_size.m_height - 1, Color(col2.r(), col2.g(), col2.b(), alpha2), "BOMB", render::ALIGN_CENTER);
			}
	}
}

bool Visuals::GetPlayerBoxRect(Player* player, Rect& box) {
	vec3_t pos{ player->GetAbsOrigin() };
	vec3_t top = pos + vec3_t(0, 0, player->GetCollideable()->OBBMaxs().z);

	vec2_t pos_screen, top_screen;

	if (!render::WorldToScreen(pos, pos_screen) ||
		!render::WorldToScreen(top, top_screen))
		return false;

	box.x = int(top_screen.x - ((pos_screen.y - top_screen.y) / 2) / 2);
	box.y = int(top_screen.y);

	box.w = int(((pos_screen.y - top_screen.y)) / 2);
	box.h = int((pos_screen.y - top_screen.y));

	const bool out_of_fov = pos_screen.x + box.w + 20 < 0 || pos_screen.x - box.w - 20 > g_cl.m_width || pos_screen.y + 20 < 0 || pos_screen.y - box.h - 20 > g_cl.m_height;

	return !out_of_fov;
}

void Visuals::AddMatrix(Player* player, matrix3x4_t* bones) {
	auto& hit = m_hit_matrix.emplace_back();

	std::memcpy(hit.pBoneToWorld, bones, player->bone_cache().count() * sizeof(matrix3x4_t));

	float time = g_menu.main.players.chams_shot_fadetime.get();

	hit.time = g_csgo.m_globals->m_realtime + time;

	static int m_nSkin = 0xA1C;
	static int m_nBody = 0xA20;

	hit.info.m_origin = player->GetAbsOrigin();
	hit.info.m_angles = player->GetAbsAngles();

	auto renderable = player->renderable();
	if (!renderable)
		return;

	auto model = player->GetModel();
	if (!model)
		return;

	auto hdr = *(studiohdr_t**)(player->GetModelPtr());
	if (!hdr)
		return;

	hit.state.m_pStudioHdr = hdr;
	hit.state.m_pStudioHWData = g_csgo.m_model_cache->GetHardwareData(model->m_studio);
	hit.state.m_pRenderable = renderable;
	hit.state.m_drawFlags = 0;

	hit.info.m_renderable = renderable;
	hit.info.m_model = model;
	hit.info.m_lighting_offset = nullptr;
	hit.info.m_lighting_origin = nullptr;
	hit.info.m_hitboxset = player->m_nHitboxSet();
	hit.info.m_skin = (int)(uintptr_t(player) + m_nSkin);
	hit.info.m_body = (int)(uintptr_t(player) + m_nBody);
	hit.info.m_index = player->index();
	hit.info.m_instance = util::get_method<ModelInstanceHandle_t(__thiscall*)(void*) >(renderable, 30u)(renderable);
	hit.info.m_flags = 0x1;

	hit.info.m_model_to_world = &hit.model_to_world;
	hit.state.m_pModelToWorld = &hit.model_to_world;

	math::angle_matrix(hit.info.m_angles, hit.info.m_origin, hit.model_to_world);
}

void Visuals::override_material(bool ignoreZ, bool use_env, Color& color, IMaterial* material) {
	material->SetFlag(MATERIAL_VAR_IGNOREZ, ignoreZ);
	material->IncrementReferenceCount();

	bool found;
	auto var = material->FindVar("$envmaptint", &found);

	if (found)
		var->set_vec_value(color.r(), color.g(), color.b());

	g_csgo.m_studio_render->ForcedMaterialOverride(material);
}

void Visuals::on_post_screen_effects() {
	if (!g_cl.m_processing)
		return;

	const auto freq = g_menu.main.players.rainbow_speed.get(); /// Gradient speed (curr: 100%)
	const auto real_time = g_csgo.m_globals->m_realtime * freq;

	const auto r = floor(sin(real_time + 0.f) * 27 + 198);
	const auto g = floor(sin(real_time + 2.f) * 27 + 198);
	const auto b = floor(sin(real_time + 4.f) * 27 + 198);

	const auto local = g_cl.m_local;
	if (!local || !g_menu.main.players.chams_shot.get() || !g_csgo.m_engine->IsInGame())
		m_hit_matrix.clear();

	if (m_hit_matrix.empty() || !g_csgo.m_model_render)
		return;

	auto ctx = g_csgo.m_material_system->get_render_context();
	if (!ctx)
		return;

	auto it = m_hit_matrix.begin();

	while (it != m_hit_matrix.end()) {
		if (!it->state.m_pModelToWorld || !it->state.m_pRenderable || !it->state.m_pStudioHdr || !it->state.m_pStudioHWData ||
			!it->info.m_renderable || !it->info.m_model_to_world || !it->info.m_model) {
			++it;
			continue;
		}

		auto alpha = 1.0f;
		auto delta = g_csgo.m_globals->m_realtime - it->time;

		if (delta > 0.0f) {
			alpha -= delta;

			if (delta > 1.0f) {
				it = m_hit_matrix.erase(it);
				continue;
			}
		}

		auto alpha_color = (float)g_menu.main.players.chams_shot_blend.get() / 255.f;

		Color ghost_color = g_menu.main.players.chams_shot_col.get();

		g_csgo.m_render_view->SetBlend(alpha_color * alpha);
		if (g_menu.main.players.rainbow_visuals.get(4)) {
			g_chams.SetupMaterial(g_chams.m_materials[g_menu.main.players.chams_shot_mat.get()], Color(r, g, b), true);
		}
		else {
			g_chams.SetupMaterial(g_chams.m_materials[g_menu.main.players.chams_shot_mat.get()], ghost_color, true);
		}
		g_csgo.m_model_render->DrawModelExecute(ctx, it->state, it->info, it->pBoneToWorld);
		g_csgo.m_model_render->ForceMat(nullptr);

		++it;
	}
}

void Visuals::DrawSkeleton( Player* player, int opacity ) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	int           parent;
	BoneArray     matrix[ 128 ];
	vec3_t        bone_pos, parent_pos;
	vec2_t        bone_pos_screen, parent_pos_screen;
	constexpr float MAX_DORMANT_TIME = 10.f;
	constexpr float DORMANT_FADE_TIME = MAX_DORMANT_TIME / 2.f;

	Rect		  box;
	player_info_t info;
	Color		  color;

	// get player's model.
	model = player->GetModel( );
	if( !model )
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel( model );
	if( !hdr )
		return;

	// get bone matrix.
	if( !player->SetupBones( matrix, 128, BONE_USED_BY_ANYTHING, g_csgo.m_globals->m_curtime ) )
		return;

	for( int i{ }; i < hdr->m_num_bones; ++i ) {
		// get bone.
		bone = hdr->GetBone( i );
		if( !bone || !( bone->m_flags & BONE_USED_BY_HITBOX ) )
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if( parent == -1 )
			continue;

		// resolve main bone and parent bone positions.
		matrix->get_bone( bone_pos, i );
		matrix->get_bone( parent_pos, parent );

		Color clr = player->enemy( g_cl.m_local ) ? g_menu.main.players.skeleton_enemy.get( ) : g_menu.main.players.skeleton_enemy.get( );

		// get player index.
		int index = player->index();

		// get reference to array variable.
		float& opacity = m_opacities[index - 1];
		bool& draw = m_draw[index - 1];

		// opacity should reach 1 in 300 milliseconds.
		constexpr int frequency = 1.f / 0.3f;

		// the increment / decrement per frame.
		float step = frequency * g_csgo.m_globals->m_frametime;

		// is player enemy.
		bool enemy = player->enemy(g_cl.m_local);
		bool dormant = player->dormant();

		if (g_menu.main.visuals.enemy_radar.get() && enemy && !dormant)
			player->m_bSpotted() = true;

		// we can draw this player again.
		if (!dormant)
			draw = true;

		if (!draw)
			return;

		// if non-dormant	-> increment
	    // if dormant		-> decrement
		dormant ? opacity -= step : opacity += step;

		// is dormant esp enabled for this player.
		bool dormant_esp = enemy && g_menu.main.players.dormant.get();

		// clamp the opacity.
		math::clamp(opacity, 0.f, 1.f);
		if (!opacity && !dormant_esp)
			return;

		// stay for x seconds max.
		float dt = g_csgo.m_globals->m_curtime - player->m_flSimulationTime();
		if (dormant && dt > MAX_DORMANT_TIME)
			return;

		// calculate alpha channels.
		int alpha = (int)(210.f * opacity);
		int low_alpha = (int)(180.f * opacity);

		// get color based on enemy or not.
		color = enemy ? g_menu.main.players.box_enemy.get() : g_menu.main.players.box_enemy.get();

		if (dormant && dormant_esp) {
			alpha = 112;
			low_alpha = 80;

			// fade.
			if (dt > DORMANT_FADE_TIME) {
				// for how long have we been fading?
				float faded = (dt - DORMANT_FADE_TIME);
				float scale = 1.f - (faded / DORMANT_FADE_TIME);

				alpha *= scale;
				low_alpha *= scale;
			}

			// override color.
			color = { 210, 210, 210 };
		}

		// override alpha.
		color.a() = alpha;

		// world to screen both the bone parent bone then draw.
		if( render::WorldToScreen( bone_pos, bone_pos_screen ) && render::WorldToScreen( parent_pos, parent_pos_screen ) )
			if (dormant)
			render::line( bone_pos_screen.x, bone_pos_screen.y, parent_pos_screen.x, parent_pos_screen.y, Color(210,210,210,low_alpha));
			else
			render::line(bone_pos_screen.x, bone_pos_screen.y, parent_pos_screen.x, parent_pos_screen.y, Color(clr.r(), clr.g(), clr.b(), g_menu.main.players.skeleton_alpha.get()));
	}
}

bool is_grenade(const int id)
{
	return id == 9 || id == 98 || id == 134;
}

bool isc4(const int id)
{
	return id == 29 || id == 108;
}

void Visuals::RenderGlow() {
	Color   color;
	Player* player;
	const auto freq = g_menu.main.players.rainbow_speed.get(); /// Gradient speed (curr: 100%)
	const auto real_time = g_csgo.m_globals->m_realtime * freq;

	if (!g_cl.m_local)
		return;

	if (!g_csgo.m_glow->m_object_definitions.Count())
		return;

	float blend = g_menu.main.players.glow_blend.get() / 100.f;

	for (int i{ }; i < g_csgo.m_glow->m_object_definitions.Count(); ++i) {
		GlowObjectDefinition_t* obj = &g_csgo.m_glow->m_object_definitions[i];

		if (obj->IsUnused() || !obj->m_entity)
			continue;

		const auto classid = obj->m_entity->GetClientClass()->m_ClassID;

		if (is_grenade(classid) && g_menu.main.visuals.proj.get()) {
			color = g_menu.main.visuals.proj_col.get();

			float blend = g_menu.main.visuals.proj_col_slider.get() / 100.f;

			obj->m_render_occluded = true;
			obj->m_render_unoccluded = false;
			obj->m_render_full_bloom = false;
			obj->m_color = { (float)color.r() / 255.f, (float)color.g() / 255.f, (float)color.b() / 255.f };
			obj->m_alpha = blend;
			obj->m_bloom_amount = 1.f;
		}

		if (isc4(classid) && g_menu.main.visuals.planted_c4.get()) {
			color = g_menu.main.visuals.bomb_col.get();

			float blend = g_menu.main.visuals.bomb_col_glow_slider.get() / 100.f;

			obj->m_render_occluded = true;
			obj->m_render_unoccluded = false;
			obj->m_render_full_bloom = false;
			obj->m_color = { (float)color.r() / 255.f, (float)color.g() / 255.f, (float)color.b() / 255.f };
			obj->m_alpha = blend;
			obj->m_bloom_amount = 1.f;
		}

		if (obj->m_entity->IsBaseCombatWeapon() && g_menu.main.visuals.itemsglow.get()) {
			color = g_menu.main.visuals.item_color.get();
			float blend = g_menu.main.visuals.glow_color_alpha.get() / 100.f;

			obj->m_render_occluded = true;
			obj->m_render_unoccluded = false;
			obj->m_render_full_bloom = false;
			obj->m_color = { (float)color.r() / 255.f, (float)color.g() / 255.f, (float)color.b() / 255.f };
			obj->m_alpha = blend;
			obj->m_bloom_amount = 1.f;
		}

		if (obj->m_entity->IsPlayer()) {

			// skip non-players.
			if (!obj->m_entity || !obj->m_entity->IsPlayer())
				continue;

			// get player ptr.
			player = obj->m_entity->as< Player* >();

			if (player->m_bIsLocalPlayer())
				continue;

			// get reference to array variable.
			float& opacity = m_opacities[player->index()];

			bool enemy = player->enemy(g_cl.m_local);

			if (enemy && !g_menu.main.players.glow.get())
				continue;

			if (!enemy && !g_menu.main.players.teammates.get())
				continue;

			// enemy color
			if (enemy)
				color = g_menu.main.players.glow_enemy.get();

			// friendly color
			else
				color = g_menu.main.players.glow_enemy.get();

			const auto r = floor(sin(real_time + 0.f) * 16 + 88);
			const auto g = floor(sin(real_time + 2.f) * 16 + 88);
			const auto b = floor(sin(real_time + 4.f) * 16 + 88);

			obj->m_render_occluded = true;
			obj->m_render_unoccluded = false;
			obj->m_render_full_bloom = false;
			if (g_menu.main.players.rainbow_visuals.get(0)) {
				obj->m_color = { r / 255.f, g / 255.f, b / 255.f };
			}
			else {
				obj->m_color = { (float)color.r() / 255.f, (float)color.g() / 255.f, (float)color.b() / 255.f };
			}
			obj->m_alpha = opacity * blend;
		}
	}
}
/*
void Visuals::DrawBeam() {
	const auto pLocal = g_cl.m_local;
	if (!pLocal)
		return;

	bool bFinalImpact = false;
	for (size_t i{ 0 }; i < bulletImpactInfo.size(); i++) {
		auto& currentImpact = bulletImpactInfo.at(i);

		if (std::abs(g_csgo.m_globals->m_realtime - currentImpact.m_flExpTime) > 3.f) {
			bulletImpactInfo.erase(bulletImpactInfo.begin() + i);
			continue;
		}

		if (currentImpact.ignore)
			continue;

		if (currentImpact.m_bRing) {
			BeamInfo_t beamInfo;
			beamInfo.m_nType = TE_BEAMRINGPOINT;
			beamInfo.m_pszModelName = XOR("sprites/purplelaser1.vmt");
			beamInfo.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/purplelaser1.vmt"));
			beamInfo.m_pszHaloName = XOR("sprites/purplelaser1.vmt");
			beamInfo.m_nHaloIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/purplelaser1.vmt"));
			beamInfo.m_flHaloScale = 5.f;
			beamInfo.m_flLife = 1.0f;
			beamInfo.m_flWidth = 6.0f;
			beamInfo.m_flEndWidth = 6.0f;
			beamInfo.m_flFadeLength = 0.0f;
			beamInfo.m_flAmplitude = 0.0f;//2.f
			beamInfo.m_flBrightness = currentImpact.m_cColor.a();
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
			beamInfo.m_flEndRadius = 550;

			auto beam = g_csgo.m_beams->CreateBeamRingPoint(beamInfo);
			if (beam) {
				g_csgo.m_beams->DrawBeam(beam);
			}

			currentImpact.ignore = true;
			continue;
		}

		if ((i + 1) < bulletImpactInfo.size()) {
			if (currentImpact.m_nTickBase != bulletImpactInfo.operator[ ](i + 1).m_nTickBase) {
				bFinalImpact = true;
			}
		}
		else if ((i == (bulletImpactInfo.size() - 1))) {
			bFinalImpact = true;
		}
		else {
			bFinalImpact = false;
		}

		if (bFinalImpact || currentImpact.m_nIndex != pLocal->index()) {
			vec3_t start = currentImpact.m_vecStartPos;
			vec3_t end = currentImpact.m_vecHitPos;

			BeamInfo_t beamInfo;
			beamInfo.m_vecStart = start;
			beamInfo.m_vecEnd = end;
			beamInfo.m_nType = 0;
			beamInfo.m_pszModelName = XOR("sprites/purplelaser1.vmt");
			beamInfo.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/purplelaser1.vmt"));
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

			const auto beam = g_csgo.m_beams->CreateBeamPoints(beamInfo);

			if (beam) {
				g_csgo.m_beams->DrawBeam(beam);
			}

			currentImpact.ignore = true;
		}
	}
}
*/
void Visuals::Add(BulletImpactInfo beamEffect) {
	bulletImpactInfo.push_back(beamEffect);
}	

void Visuals::DrawHistorySkeleton(Player* player, int opacity) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	AimPlayer* data;
	LagRecord* record;
	int           parent;
	vec3_t        bone_pos, parent_pos;
	vec2_t        bone_pos_screen, parent_pos_screen;

	if (!g_menu.main.misc.fake_latency.get())
		return;

	// get player's model.
	model = player->GetModel();
	if (!model)
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	data = &g_aimbot.m_players[player->index() - 1];
	if (!data)
		return;

	record = g_resolver.FindLastRecord(data);
	if (!record)
		return;

	for (int i{ }; i < hdr->m_num_bones; ++i) {
		// get bone.
		bone = hdr->GetBone(i);
		if (!bone || !(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if (parent == -1)
			continue;

		// resolve main bone and parent bone positions.
		record->m_bones->get_bone(bone_pos, i);
		record->m_bones->get_bone(parent_pos, parent);

		Color clr = player->enemy(g_cl.m_local) ? g_menu.main.players.skeleton_enemy.get() : g_menu.main.players.skeleton_enemy.get();
		clr.a() = opacity;

		// world to screen both the bone parent bone then draw.
		if (render::WorldToScreen(bone_pos, bone_pos_screen) && render::WorldToScreen(parent_pos, parent_pos_screen))
			render::line(bone_pos_screen.x, bone_pos_screen.y, parent_pos_screen.x, parent_pos_screen.y, clr);
	}
}