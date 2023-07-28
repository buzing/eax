#include "includes.h"

Chams g_chams{ };;

Chams::model_type_t Chams::GetModelType(const ModelRenderInfo_t& info) {

	std::string mdl{ info.m_model->m_name };

	if (mdl.find(XOR("player")) != std::string::npos && info.m_index >= 1 && info.m_index <= 64)
		return model_type_t::player;

	return model_type_t::invalid;
}

bool Chams::IsInViewPlane(const vec3_t& world) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix();

	w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];

	return w > 0.001f;
}

void Chams::SetColor(Color col, IMaterial* mat) {

	if (mat) {
		mat->ColorModulate(col);

		auto pVar = mat->find_var("$envmaptint");
		if (pVar)
			(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, col.r() / 255.f, col.g() / 255.f, col.b() / 255.f);
	}
	else
		g_csgo.m_render_view->SetColorModulation(col);
}

void Chams::SetAlpha(float alpha, IMaterial* mat) {
	if (mat)
		mat->AlphaModulate(alpha);

	else
		g_csgo.m_render_view->SetBlend(alpha);
}

void Chams::SetupMaterial(IMaterial* mat, Color col, bool z_flag) {
	SetColor(col, mat);

	// mat->SetFlag( MATERIAL_VAR_HALFLAMBERT, flags );
	mat->SetFlag(MATERIAL_VAR_ZNEARER, z_flag);
	mat->SetFlag(MATERIAL_VAR_NOFOG, z_flag);
	mat->SetFlag(MATERIAL_VAR_IGNOREZ, z_flag);

	g_csgo.m_studio_render->ForcedMaterialOverride(mat);
}

void Chams::init() {

	std::ofstream("csgo/materials/glow.vmt") << R"#("VertexLitGeneric" 
		{
		"$additive" "1"
		"$envmap" "models/effects/cube_white"
		"$envmaptint" "[1 1 1]"
		"$envmapfresnel" "1"
		"$envmapfresnelminmaxexp" "[0 1 2]"
		"$alpha" "0.8"
		})#";

	std::ofstream("csgo/materials/styler.vmt") << R"#("VertexLitGeneric"
     {
     				"$basetexture"				"vgui/white_additive"
					"$ignorez"					"1"
                    "$wireframe"					"1"
      })#";
    
	std::ofstream("csgo\\materials\\regular_reflective.vmt") << R"#("VertexLitGeneric" {
					"$basetexture"				"vgui/white_additive"
					"$ignorez"					"0"
					"$phong"					"1"
					"$BasemapAlphaPhongMask"    "1"
					"$phongexponent"			"15"
					"$normalmapalphaenvmask"	"1"
					"$envmap"					"env_cubemap"
					"$envmaptint"				"[0.0 0.0 0.0]"
					"$phongboost"				"[0.6 0.6 0.6]"
					"phongfresnelranges"		"[0.5 0.5 1.0]"
					"$nofog"					"1"
					"$model"					"1"
					"$nocull"					"0"
					"$selfillum"				"1"
					"$halflambert"				"1"
					"$znearer"					"0"
					"$flat"						"0"	
					"$rimlight"					"1"
					"$rimlightexponent"			"2"
					"$rimlightboost"			"0"
        })#";

	// find stupid materials.
	m_materials.push_back(g_csgo.m_material_system->FindMaterial(XOR("debug/debugambientcube"), XOR("Model textures")));
	m_materials.push_back(g_csgo.m_material_system->FindMaterial(XOR("debug/debugdrawflat"), XOR("Model textures")));
	m_materials.push_back(g_csgo.m_material_system->FindMaterial(XOR("regular_reflective"), XOR("Model textures")));
	m_materials.push_back(g_csgo.m_material_system->FindMaterial(XOR("models/inventory_items/trophy_majors/gloss"), XOR("Model textures")));
	m_materials.push_back(g_csgo.m_material_system->FindMaterial(XOR("glow"), nullptr));
	m_materials.push_back(g_csgo.m_material_system->FindMaterial(XOR("dev/glow_armsrace"), XOR("Model textures")));
	m_materials.push_back(g_csgo.m_material_system->FindMaterial(XOR("styler"), XOR("Model Textures")));

	for (int i = 0; i < m_materials.size(); i++) {
		m_materials[i]->IncrementReferenceCount();
	}
}

bool Chams::OverridePlayer(int index) {
	Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(index);
	if (!player)
		return false;

	// always skip the local player in DrawModelExecute.
	// this is because if we want to make the local player have less alpha
	// the static props are drawn after the players and it looks like aids.
	// therefore always process the local player in scene end.
	if (player->m_bIsLocalPlayer())
		return true;

	// see if this player is an enemy to us.
	bool enemy = g_cl.m_local && player->enemy(g_cl.m_local);

	// we have chams on enemies.
	if (enemy && g_menu.main.players.chams_enemy.get(0))
		return true;

	return false;
}

bool Chams::GenerateLerpedMatrix(int index, BoneArray* out) {
	LagRecord* current_record;
	AimPlayer* data;

	Player* ent = g_csgo.m_entlist->GetClientEntity< Player* >(index);
	if (!ent)
		return false;

	if (!g_aimbot.IsValidTarget(ent))
		return false;

	data = &g_aimbot.m_players[index - 1];
	if (!data || data->m_records.empty())
		return false;

	if (data->m_records.size() < 2)
		if (data->m_records.front().get()->m_broke_lc)
			return false;

	if (data->m_records.size() < 3)
		return false;

	auto* channel_info = g_csgo.m_engine->GetNetChannelInfo();
	if (!channel_info)
		return false;

	float max_unlag = g_csgo.m_cvar->FindVar(HASH("sv_maxunlag"))->GetFloat();

	// start from begin
	for (auto it = data->m_records.begin(); it != data->m_records.end(); ++it) {

		LagRecord* last_first{ nullptr };
		LagRecord* last_second{ nullptr };

		if (it->get()->valid() && it + 1 != data->m_records.end() && !(it + 1)->get()->valid() && !(it + 1)->get()->dormant()) {
			last_first = (it + 1)->get();
			last_second = (it)->get();
		}

		if (!last_first || !last_second)
			continue;

		const auto& FirstInvalid = last_first;
		const auto& LastInvalid = last_second;

		if (!LastInvalid || !FirstInvalid)
			continue;

		const auto NextOrigin = LastInvalid->m_origin;
		const auto curtime = g_csgo.m_globals->m_curtime;

		auto flDelta = 1.f - (curtime - LastInvalid->m_interp_time) / (LastInvalid->m_sim_time - FirstInvalid->m_sim_time);
		if (flDelta < 0.f || flDelta > 1.f)
			LastInvalid->m_interp_time = curtime;

		flDelta = 1.f - (curtime - LastInvalid->m_interp_time) / (LastInvalid->m_sim_time - FirstInvalid->m_sim_time);

		const auto lerp = math::Interpolate(NextOrigin, FirstInvalid->m_origin, std::clamp(flDelta, 0.f, 1.f));

		matrix3x4_t ret[128];
		std::memcpy(ret, FirstInvalid->m_bones, sizeof(ret));

		for (size_t i{ }; i < 128; ++i) {
			const auto matrix_delta = FirstInvalid->m_bones[i].GetOrigin() - FirstInvalid->m_origin;
			ret[i].SetOrigin(matrix_delta + lerp);
		}

		std::memcpy(out, ret, sizeof(ret));
		return true;
	}

	return false;
}

void Chams::RenderHistoryChams(int index) {
	AimPlayer* data;
	LagRecord* record;
	const auto freq = g_menu.main.players.rainbow_speed.get(); /// Gradient speed (curr: 100%)
	const auto real_time = g_csgo.m_globals->m_realtime * freq;

	const auto r = floor(sin(real_time + 0.f) * 27 + 198);
	const auto g = floor(sin(real_time + 2.f) * 27 + 198);
	const auto b = floor(sin(real_time + 4.f) * 27 + 198);

	Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(index);
	if (!player)
		return;

	if (!g_aimbot.IsValidTarget(player))
		return;

	bool enemy = g_cl.m_local && player->enemy(g_cl.m_local);
	if (enemy) {
		data = &g_aimbot.m_players[index - 1];
		if (!data)
			return;

		record = g_resolver.FindLastRecord(data);
		if (!record)
			return;

		if (record->m_broke_lc)
			return;

		if (!record->m_setup)
			return;

		// override blend.
		SetAlpha(g_menu.main.players.chams_enemy_history_blend.get() / 100.f);

		// set material and color.
		if (g_menu.main.players.rainbow_visuals.get(3)) {
			SetupMaterial(m_materials[g_menu.main.players.chams_enemy_history_mat.get()], Color(r, g, b), true);
		}
		else {
			SetupMaterial(m_materials[g_menu.main.players.chams_enemy_history_mat.get()], g_menu.main.players.chams_enemy_history_col.get(), true);

		}

		// was the matrix properly setup?
		BoneArray arr[128];
		if (Chams::GenerateLerpedMatrix(index, arr)) {
			// backup the bone cache before we fuck with it.
			auto backup_bones = player->m_BoneCache().m_pCachedBones;

			// replace their bone cache with our custom one.
			player->m_BoneCache().m_pCachedBones = arr;

			// manually draw the model.
			player->DrawModel();

			// reset their bone cache to the previous one.
			player->m_BoneCache().m_pCachedBones = backup_bones;
		}
	}
}

bool Chams::DrawModel(uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone) {
	// store and validate model type.
	model_type_t type = GetModelType(info);
	if (type == model_type_t::invalid)
		return true;

	// is a valid player.
	if (type == model_type_t::player) {
		// do not cancel out our own calls from SceneEnd
		// also do not cancel out calls from the glow.
		if (!m_running && !g_csgo.m_studio_render->m_pForcedMaterial && OverridePlayer(info.m_index))
			return false;
	}

	return true;
}

void Chams::SceneEnd() {
	// store and sort ents by distance.
	if (SortPlayers()) {
		// iterate each player and render them.
		for (const auto& p : m_players)
			RenderPlayer(p);
	}

	// restore.
	g_csgo.m_studio_render->ForcedMaterialOverride(nullptr);
	g_csgo.m_render_view->SetColorModulation(colors::white);
	g_csgo.m_render_view->SetBlend(1.f);
}

void Chams::RenderPlayer(Player* player) {
	// prevent recruisive model cancelation.
	m_running = true;

	const auto freq = g_menu.main.players.rainbow_speed.get(); /// Gradient speed (curr: 100%)
	const auto real_time = g_csgo.m_globals->m_realtime * freq;

	const auto r = floor(sin(real_time + 0.f) * 27 + 198);
	const auto g = floor(sin(real_time + 2.f) * 27 + 198);
	const auto b = floor(sin(real_time + 4.f) * 27 + 198);

	// restore.
	g_csgo.m_studio_render->ForcedMaterialOverride(nullptr);
	g_csgo.m_render_view->SetColorModulation(colors::white);
	g_csgo.m_render_view->SetBlend(1.f);

	// this is the local player.
	// we always draw the local player manually in drawmodel.
	if (player->m_bIsLocalPlayer()) {

		Weapon* weapon = player->GetActiveWeapon();
		if (!weapon)
			return;

		WeaponInfo* data = weapon->GetWpnData();
		if (!data)
			return;

		// do not do this on: bomb, knife and nades.
		CSWeaponType type = data->m_weapon_type;
		if (g_menu.main.players.chams_local_scope.get() && type == WEAPONTYPE_C4 || type == WEAPONTYPE_GRENADE || player->m_bIsScoped())
			SetAlpha(g_menu.main.players.chams_local_scope_blend.get() / 100.f);

		else if (g_menu.main.players.chams_local.get()) {
			// override blend.
			SetAlpha(g_menu.main.players.chams_local_blend.get() / 100.f);

			// set material and color.
			if (g_menu.main.players.chams_local_mat.get() == 4)
				SetupMaterial(m_materials[0], g_menu.main.players.chams_local_col.get(), false);
			else
				SetupMaterial(m_materials[g_menu.main.players.chams_local_mat.get()], g_menu.main.players.chams_local_col.get(), false);
			}
		player->DrawModel();
	}

	if (g_menu.main.players.chams_local_mat.get() == 4) {
		if (player->m_bIsLocalPlayer()) {

			if (g_menu.main.players.chams_local.get()) {
				// override blend.
				SetAlpha(g_menu.main.players.chams_local_blend.get() / 100.f);

				// set material and color.
				//SetupMaterial(m_materials[1], g_menu.main.players.chams_local_col.get(), false);

				SetupMaterial(m_materials[4], g_menu.main.players.chams_local2_col.get(), false);
			}


			if (g_menu.main.players.chams_local_scope.get() && player->m_bIsScoped())
				SetAlpha(g_menu.main.players.chams_local_scope_blend.get() / 100.f);

			Weapon* weapon = player->GetActiveWeapon();
			if (!weapon)
				return;

			WeaponInfo* data = weapon->GetWpnData();
			if (!data)
				return;

			// do not do this on: bomb, knife and nades.
			CSWeaponType type = data->m_weapon_type;
			if (type == WEAPONTYPE_C4 || type == WEAPONTYPE_GRENADE)
				SetAlpha(g_menu.main.players.chams_local_scope_blend.get() / 100.f);

			if (g_menu.main.players.chams_local_mat.get() == 5) {
				SetAlpha(0.f);
			}

			// manually draw the model.
			//player->DrawModel();
			player->DrawModel();
		}
	}

	if (g_menu.main.players.chams_fake.get()) {
		if (player->m_bIsLocalPlayer()) {

			if (g_menu.main.players.chams_fake.get()) {
				// override blend.
				SetAlpha(g_menu.main.players.chams_fake_blend.get() / 100.f);

				// set material and color.
				if (g_menu.main.players.chams_fake_mat.get() == 4)
					SetupMaterial(m_materials[0], g_menu.main.players.chams_fake_col.get(), false);
				else
					SetupMaterial(m_materials[g_menu.main.players.chams_fake_mat.get()], g_menu.main.players.chams_fake_col.get(), false);
			}

			if (g_menu.main.players.chams_local_scope.get() && player->m_bIsScoped())
				SetAlpha(g_menu.main.players.chams_local_scope_blend.get() / 100.f);

			Weapon* weapon = player->GetActiveWeapon();
			if (!weapon)
				return;

			WeaponInfo* data = weapon->GetWpnData();
			if (!data)
				return;

			// do not do this on: bomb, knife and nades.

			// manually draw the model.
			//player->DrawModel();
			auto old_angles = player->GetAbsAngles();
			player->SetAbsAngles({ 0.f, g_cl.m_radar.y, 0.f });
			player->DrawModel();
			player->SetAbsAngles(old_angles);

			if (g_menu.main.players.chams_fake_mat.get() == 4) {
				if (g_menu.main.players.chams_fake.get()) {
					// override blend.
					SetAlpha(g_menu.main.players.chams_fake_blend.get() / 100.f);

					// set material and color.
					SetupMaterial(m_materials[4], g_menu.main.players.chams_fake_glow_col.get(), false);
				}


				if (g_menu.main.players.chams_local_scope.get() && player->m_bIsScoped())
					SetAlpha(g_menu.main.players.chams_local_scope_blend.get() / 100.f);

				Weapon* weapon = player->GetActiveWeapon();
				if (!weapon)
					return;

				WeaponInfo* data = weapon->GetWpnData();
				if (!data)
					return;

				// do not do this on: bomb, knife and nades.
				CSWeaponType type = data->m_weapon_type;
				if (type == WEAPONTYPE_C4 || type == WEAPONTYPE_GRENADE)
					SetAlpha(g_menu.main.players.chams_local_scope_blend.get() / 100.f);

				if (g_menu.main.players.chams_local_mat.get() == 5) {
					SetAlpha(0.f);
				}

				// manually draw the model.
				//player->DrawModel();
				auto old_angles = player->GetAbsAngles();
				player->SetAbsAngles({ 0.f, g_cl.m_radar.y, 0.f });
				player->DrawModel();
				player->SetAbsAngles(old_angles);
			}
		}
	}

	// check if is an enemy.
	bool enemy = g_cl.m_local && player->enemy(g_cl.m_local);

	if (enemy && g_menu.main.players.chams_enemy_history.get()) {
		RenderHistoryChams(player->index());
	}

	if (enemy && g_menu.main.players.chams_enemy.get(0)) {
		if ((g_menu.main.players.chams_enemy_mat.get() == 2)) {

				auto var = m_materials[2]->find_var(XOR("$envmaptint"));
				if (var)
					var->set_vec_value(g_menu.main.players.chams_enemy_reflectivity.get().r() * (g_menu.main.players.chams_reflectivity.get() / 100.f),
						g_menu.main.players.chams_enemy_reflectivity.get().r() * (g_menu.main.players.chams_reflectivity.get() / 100.f),
						g_menu.main.players.chams_enemy_reflectivity.get().r() * (g_menu.main.players.chams_reflectivity.get() / 100.f));

				auto phong2 = m_materials[2]->find_var(XOR("$phongboost"));
				if (phong2)
					phong2->set_float_value(g_menu.main.players.chams_enemy_shine.get() / 100.f);

				auto rim2 = m_materials[2]->find_var(XOR("$rimlightboost"));
				if (rim2)
					rim2->set_float_value(g_menu.main.players.chams_enemy_rim.get());

		}

		if (g_menu.main.players.chams_enemy_mat.get() == 4) {
			if (g_menu.main.players.chams_enemy.get(1)) {

				SetAlpha(g_menu.main.players.chams_enemy_blend2.get() / 100.f);
				SetupMaterial(m_materials[0], g_menu.main.players.chams_enemy_invis.get(), true);

				player->DrawModel();

				SetAlpha(g_menu.main.players.chams_enemy_blend.get() / 100.f);
				SetupMaterial(m_materials[4], g_menu.main.players.chams_enemy2_invis.get(), true);

				player->DrawModel();
			}

			SetAlpha(g_menu.main.players.chams_enemy_blend.get() / 100.f);
			SetupMaterial(m_materials[0], g_menu.main.players.chams_enemy_vis.get(), false);

			player->DrawModel();

			SetAlpha(g_menu.main.players.chams_enemy_blend.get() / 100.f);
			SetupMaterial(m_materials[4], g_menu.main.players.chams_enemy2_vis.get(), false);

			player->DrawModel();
		}
		else
		{
			if (g_menu.main.players.chams_enemy.get(1)) {

				SetAlpha(g_menu.main.players.chams_enemy_blend2.get() / 100.f);
				if (g_menu.main.players.rainbow_visuals.get(2)) {
					SetupMaterial(m_materials[g_menu.main.players.chams_enemy_mat.get()], Color(r, g, b), true);
				}
				else {
					SetupMaterial(m_materials[g_menu.main.players.chams_enemy_mat.get()], g_menu.main.players.chams_enemy_invis.get(), true);
				}

				player->DrawModel();
			}

			SetAlpha(g_menu.main.players.chams_enemy_blend.get() / 100.f);
			if (g_menu.main.players.rainbow_visuals.get(1)) {
				SetupMaterial(m_materials[g_menu.main.players.chams_enemy_mat.get()], Color(r, g, b), false);
			}
			else {
				SetupMaterial(m_materials[g_menu.main.players.chams_enemy_mat.get()], g_menu.main.players.chams_enemy_vis.get(), false);
			}
			player->DrawModel();

		}
	}

	else if (!enemy && !g_cl.m_local && g_menu.main.players.chams_friendly.get(0)) {
		if (g_menu.main.players.chams_friendly.get(1)) {

			SetAlpha(g_menu.main.players.chams_friendly_blend.get() / 100.f);
			SetupMaterial(m_materials[g_menu.main.players.chams_friendly_mat.get()], g_menu.main.players.chams_friendly_invis.get(), true);

			player->DrawModel();
		}

		SetAlpha(g_menu.main.players.chams_friendly_blend.get() / 100.f);
		SetupMaterial(m_materials[g_menu.main.players.chams_friendly_mat.get()], g_menu.main.players.chams_friendly_vis.get(), false);

		player->DrawModel();
	}

	m_running = false;
}

bool Chams::SortPlayers() {
	// lambda-callback for std::sort.
	// to sort the players based on distance to the local-player.
	static auto distance_predicate = [](Entity* a, Entity* b) {
		vec3_t local = g_cl.m_local->GetAbsOrigin();

		// note - dex; using squared length to save out on sqrt calls, we don't care about it anyway.
		float len1 = (a->GetAbsOrigin() - local).length_sqr();
		float len2 = (b->GetAbsOrigin() - local).length_sqr();

		return len1 < len2;
	};

	// reset player container.
	m_players.clear();

	// find all players that should be rendered.
	for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
		// get player ptr by idx.
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

		// validate.
		if (!player || !player->IsPlayer() || !player->alive() || player->dormant())
			continue;

		// do not draw players occluded by view plane.
		if (!IsInViewPlane(player->WorldSpaceCenter()))
			continue;

		// this player was not skipped to draw later.
		// so do not add it to our render list.
		if (!OverridePlayer(i))
			continue;

		m_players.push_back(player);
	}

	// any players?
	if (m_players.empty())
		return false;

	// sorting fixes the weird weapon on back flickers.
	// and all the other problems regarding Z-layering in this shit game.
	std::sort(m_players.begin(), m_players.end(), distance_predicate);

	return true;
}