#include "includes.h"

Resolver g_resolver{};;

void Resolver::FindBestAngle(LagRecord* record, AimPlayer* data) {
	if (!data->m_player
		|| data->m_records.empty())
		return;

	if (!data->m_player->alive()
		|| data->m_player->m_iTeamNum() == g_cl.m_local->m_iTeamNum()
		|| data->m_player->dormant())
		return;

	const auto at_target_angle = math::CalcAngle(g_cl.m_local->m_vecOrigin(), data->m_player->m_vecOrigin());

	vec3_t left_dir{}, right_dir{};
	math::AngleVectors(ang_t(0.f, at_target_angle.y - 90.f, 0.f), &left_dir);
	math::AngleVectors(ang_t(0.f, at_target_angle.y + 90.f, 0.f), &right_dir);

	const auto eye_pos = data->m_player->GetShootPosition();
	auto left_eye_pos = eye_pos + (left_dir * 16.f);
	auto right_eye_pos = eye_pos + (right_dir * 16.f);

	auto shoot_pos = g_cl.m_shoot_pos + (g_cl.m_local->m_vecVelocity() * g_csgo.m_globals->m_interval) * 1;

	auto left_pen_data = g_auto_wall->get(shoot_pos, left_eye_pos, data->m_player);
	auto right_pen_data = g_auto_wall->get(shoot_pos, right_eye_pos, data->m_player);

	if (left_pen_data.m_dmg == -1
		&& right_pen_data.m_dmg == -1) {

		Ray ray{};
		CGameTrace trace{};
		CTraceFilterWorldOnly filter{};

		ray = { left_eye_pos, eye_pos };
		g_csgo.m_engine_trace->TraceRay(ray, 0xFFFFFFFF, &filter, &trace);
		const auto left_frac = trace.m_fraction;

		ray = { right_eye_pos, eye_pos };
		g_csgo.m_engine_trace->TraceRay(ray, 0xFFFFFFFF, &filter, &trace);
		const auto right_frac = trace.m_fraction;

		if (left_frac > right_frac) {
			data->m_best_angle = at_target_angle.y + 90.f;
		}
		else if (right_frac > left_frac) {
			data->m_best_angle = at_target_angle.y - 90.f;
		}
		else
			data->m_has_freestand = false;

		return;
	}

	if (left_pen_data.m_dmg == right_pen_data.m_dmg) {
		data->m_has_freestand = false;
		return;
	}

	if (left_pen_data.m_dmg > right_pen_data.m_dmg) {
		data->m_best_angle = at_target_angle.y + 90.f;
	}
	else if (right_pen_data.m_dmg > left_pen_data.m_dmg) {
		data->m_best_angle = at_target_angle.y - 90.f;
	}
	data->m_has_freestand = true;
}

LagRecord* Resolver::FindIdealRecord(AimPlayer* data) {
	LagRecord* first_valid, * current;

	if (data->m_records.empty())
		return nullptr;

	// dont backtrack if we have almost no data
	if (data->m_records.size() <= 3)
		return data->m_records.front().get();

	LagRecord* front = data->m_records.front().get();

	// dont backtrack if first record invalid or breaking lagcomp
	if (!front || front->dormant() || front->immune())
		return nullptr;

	if (front->broke_lc())
		return data->m_records.front().get();

	first_valid = nullptr;

	// iterate records.
	for (const auto& it : data->m_records) {
		if (it->dormant() || it->immune() || !it->valid() || it->broke_lc() || !it->m_setup)
			continue;

		// get current record.
		current = it.get();

		// more than 1s delay between front and this record, skip it
		if (game::TIME_TO_TICKS(fabsf(front->m_sim_time - current->m_sim_time)) > 64)
			continue;

		// first record that was valid, store it for later.
		if (!first_valid)
			first_valid = current;


	}

	// none found above, return the first valid record if possible.
	return (first_valid) ? first_valid : nullptr;
}

LagRecord* Resolver::FindLastRecord(AimPlayer* data) {
	LagRecord* current;

	if (data->m_records.empty())
		return nullptr;

	// dont backtrack if we have almost no data
	if (data->m_records.size() <= 3)
		return nullptr;

	LagRecord* front = data->m_records.front().get();

	// dont backtrack if first record invalid or breaking lagcomp
	if (!front || front->dormant() || front->immune() || front->m_broke_lc || !front->m_setup)
		return nullptr;

	// iterate records in reverse.
	for (auto it = data->m_records.crbegin(); it != data->m_records.crend(); ++it) {

		current = it->get();

		// more than 1s delay between front and this record, skip it
		if (game::TIME_TO_TICKS(fabsf(front->m_sim_time - current->m_sim_time)) > 64)
			continue;

		// if this record is valid.
		// we are done since we iterated in reverse.
		if (current->valid() && !current->immune() && !current->dormant() && !current->broke_lc())
			return current;
	}

	return nullptr;
}

void Resolver::OnBodyUpdate(Player* player, float value) {
	AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

	if (player->m_vecVelocity().length_2d() > 0.1f || !(player->m_fFlags() & FL_ONGROUND)) {
		data->m_body_proxy_updated = false;
		data->m_body_proxy_old = value;
		data->m_body_proxy = value;
		return;
	}

	// lol
	if (fabsf(math::AngleDiff(value, data->m_body_proxy)) >= 35.f) {
		data->m_body_proxy_old = data->m_body_proxy;
		data->m_body_proxy = value;
		data->m_body_proxy_updated = true;
	}
}

float Resolver::GetAwayAngle(LagRecord* record) {
	float  delta{ std::numeric_limits< float >::max() };
	vec3_t pos;
	ang_t  away;

	if (g_cl.m_net_pos.empty()) {
		math::VectorAngles(g_cl.m_local->m_vecOrigin() - record->m_pred_origin, away);
		return away.y;
	}

	float owd = (g_cl.m_latency / 2.f);

	float target = record->m_pred_time;

	// iterate all.
	for (const auto& net : g_cl.m_net_pos) {
		float dt = std::abs(target - net.m_time);

		// the best origin.
		if (dt < delta) {
			delta = dt;
			pos = net.m_pos;
		}
	}

	math::VectorAngles(pos - record->m_pred_origin, away);
	return away.y;
}

void Resolver::MatchShot(AimPlayer* data, LagRecord* record) {
	// do not attempt to do this in nospread mode.
	if (g_menu.main.aimbot.nospread.get())
		return;

	float shoot_time = -1.f;

	Weapon* weapon = data->m_player->GetActiveWeapon();
	if (weapon) {
		// with logging this time was always one tick behind.
		// so add one tick to the last shoot time. (maybe?)
		shoot_time = weapon->m_fLastShotTime(); //+ g_csgo.m_globals->m_interval;
	}

	// this record has a shot on it.
	if (game::TIME_TO_TICKS(shoot_time) - game::TIME_TO_TICKS(record->m_sim_time) == 1) {
		if (record->m_lag < 2)
			record->m_shot = true;

		// more than 1 choke, cant hit pitch, apply prev pitch.
		else if (data->m_records.size() >= 2) {
			//LagRecord* previous = data->m_records[1].get();
			LagRecord* previous = data->m_records[1].get();

			if (previous && !previous->dormant())
				record->m_eye_angles.x = previous->m_eye_angles.x;
		}
	}

}

void Resolver::SetMode(LagRecord* record) {
	// the resolver has 3 modes to chose from.
// these modes will vary more under the hood depending on what data we have about the player
// and what kind of hack vs. hack we are playing (mm/nospread).

	float speed = record->m_anim_velocity.length();


	// if on ground, moving, and not fakewalking.
	if ((record->m_flags & FL_ONGROUND) && speed > 0.1f && !record->m_fake_walk)
		record->m_mode = Modes::RESOLVE_WALK;


	// if on ground, not moving or fakewalking.
	else if ((record->m_flags & FL_ONGROUND) && (speed <= 0.1f || record->m_fake_walk) && !g_input.GetKeyState(g_menu.main.aimbot.override.get()))
		record->m_mode = Modes::RESOLVE_STAND;

	// if on ground, not moving or fakewalking.
	if ((record->m_flags & FL_ONGROUND) && (speed <= 0.1f || record->m_fake_walk) && (g_input.GetKeyState(g_menu.main.aimbot.override.get())))
		record->m_mode = Modes::RESOLVE_OVERRIDE;

	// if not on ground.
	else if (!(record->m_flags & FL_ONGROUND))
		record->m_mode = Modes::RESOLVE_AIR;
}

void Resolver::ResolveAngles(Player* player, LagRecord* record) {
	AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

	// next up mark this record with a resolver mode that will be used.
	SetMode(record);

	// if we are in nospread mode, force all players pitches to down.
	// TODO; we should check thei actual pitch and up too, since those are the other 2 possible angles.
	// this should be somehow combined into some iteration that matches with the air angle iteration.
	if (g_menu.main.aimbot.nospread.get())
		record->m_eye_angles.x = 90.f;

	// we arrived here we can do the acutal resolve.
	if (record->m_mode == Modes::RESOLVE_WALK)
		ResolveWalk(data, record, player);

	else if (record->m_mode == Modes::RESOLVE_STAND)
		ResolveStand(data, record, player);

	else if (record->m_mode == Modes::RESOLVE_AIR)
		ResolveAir(data, record, player);

	else if (record->m_mode == Modes::RESOLVE_OVERRIDE)
		ResolveOverride(data, record, player);

	// normalize the eye angles, doesn't really matter but its clean.
	math::NormalizeAngle(record->m_eye_angles.y);
}

void Resolver::ResolveWalk(AimPlayer* data, LagRecord* record, Player* player) {

	// apply lby to eyeangles.
	record->m_eye_angles.y = record->m_body;

	// delay body update.
	data->m_body_update = record->m_anim_time + 0.22f;

	float speed = record->m_anim_velocity.length_2d();
	Weapon* weapon = data->m_player->GetActiveWeapon();
	iPlayers[record->m_player->index()] = false;
	if (weapon) {
		WeaponInfo* weapon_data = weapon->GetWpnData();

		if (weapon_data) {

			float max_speed = record->m_player->m_bIsScoped() ? weapon_data->m_max_player_speed_alt : weapon_data->m_max_player_speed;

			if (speed >= max_speed * 0.20f && (record->m_flags & FL_ONGROUND))
				data->m_valid_run = true;
		}
	}

	LagRecord* previous = data->m_records.size() > 1 ? data->m_records[1].get() : nullptr;


	record->m_resolver_mode = "MOVE";

	// reset stand and body index.
	//if( speed >= 5.f ) {
	data->m_stand_index = 0;
	data->m_stand_index2 = 0;
	data->m_stand_index3 = 0;
	data->m_last_move = 0;
	data->m_body_index = 0;
	data->m_lby_idx = 0;
	data->m_edge_index = 0;
	data->m_bruteforce_idx = 0;
	data->m_backwards_idx = 0;
	data->m_reverse_fs = 0;
	data->m_anti_index = 0;
	data->m_exback_idx = 0;
	data->m_freestanding_index = 0;
	data->m_fake_walk_idx = 0;
	data->m_air_brute_index = 0;
	data->m_lastmove_idx = 0;
	data->m_moved = false;
	data->m_has_body_updated = false;
	data->m_has_distort = false;
	data->m_valid_last_move = false;
	data->m_air_brute_index = 0;

	// copy the last record that this player was walking
	// we need it later on because it gives us crucial data.
	std::memcpy(&data->m_walk_record, record, sizeof(LagRecord));
}

bool Resolver::IsYawSideways(Player* entity, float yaw, bool smth)
{
	const float delta =
		fabs(math::NormalizedAngle((math::CalcAngle(g_cl.m_local->m_vecOrigin(),
			entity->m_vecOrigin()).y)
			- yaw));

	return smth ? (delta > 24.f && delta < 165.f) : (delta >= 40.f && delta < 150.f);
}

bool Resolver::IsYawDistorting(AimPlayer* data, LagRecord* record, LagRecord* previous_record) {

	if (data->m_records.size() < 3)
		return false;

	LagRecord* pre_previous = data->m_records[2].get();
	LagRecord* move = &data->m_walk_record;

	if (record == previous_record)
		return false;

	if (
		math::AngleDiff(record->m_body, pre_previous->m_body) < 45.f /* real */ ||
		(data->m_has_body_updated) ||
		record->m_body == pre_previous->m_body &&
		(record->m_body != previous_record->m_body &&
			previous_record->m_body != pre_previous->m_body && pre_previous->m_body != move->m_body)) {
		return true;
	}

	return false;
}

void Resolver::ResolveStand(AimPlayer* data, LagRecord* record, Player* player) {
	// for no-spread call a seperate resolver.
	if (g_menu.main.aimbot.nospread.get()) {
		StandNS(data, record);
		return;
	}

	if (g_menu.main.aimbot.correct_network1.get() && (data->m_is_cheese_crack || data->m_is_kaaba) && data->m_network_index <= 1) {
		record->m_eye_angles.y = data->m_networked_angle;
		record->m_resolver_color = { 155, 210, 100 };
		record->m_resolver_mode = "NETWORKED";
		record->m_mode = Modes::RESOLVE_NETWORK;
		return;
	}

	// pointer for easy access.
	LagRecord* move = &data->m_walk_record;
	float m_time_since_moving = FLT_MAX;
	float last_move_delta = FLT_MAX;

	// get predicted away angle for the player.
	float away = GetAwayAngle(record);
	C_AnimationLayer* curr = &record->m_layers[3];
	int act = data->m_player->GetSequenceActivity(curr->m_sequence);

	// we have a valid moving record.
	if (move->m_sim_time > 0.f) {
		vec3_t delta = move->m_origin - record->m_origin;

		// check if moving record is close.
		if (delta.length() <= 128.f) {
			// indicate that we are using the moving lby.
			data->m_moved = true;
			m_time_since_moving = move->m_anim_time - record->m_anim_time;
			last_move_delta = fabsf(record->m_body - move->m_body);
		}
	}

	if (data->m_moved) {
		LagRecord* previous = data->m_records.size() > 1 ? data->m_records[1].get()->dormant() ? nullptr : data->m_records[1].get() : nullptr;

		data->m_valid_last_move = false;

		if (previous && !record->m_dormant
			&& previous->m_dormant
			&& (record->m_origin - previous->m_origin).length_2d() > 16.0f)
		{
			data->m_valid_run = false;
		}




		float lower_body_yaw = record->m_body;
		float forward_diff = fabsf(math::AngleDiff(lower_body_yaw, away));
		float back_diff = fabsf(math::AngleDiff(lower_body_yaw, away + 180.f));
		bool valid_last_move = data->m_lastmove_idx < 1 && data->m_valid_run;
		bool can_backwards = fabsf(back_diff) <= 3.f;
		bool can_forwards = fabsf(forward_diff) <= 3.f;
		bool can_lby = !data->m_has_body_updated && m_time_since_moving < 0.22;
		if (record->m_fake_walk) {
			record->m_mode = Modes::RESOLVE_FAKEWALK;
			record->m_resolver_color = colors::transparent_red;
			record->m_resolver_mode = "FW-LBY";
			switch (data->m_fake_walk_idx % 2)
			{
			case 0:
				record->m_eye_angles.y = record->m_body;
				record->m_resolver_mode += "[LBY]";

				break;
			case 1:
				record->m_eye_angles.y = move->m_body;
				record->m_resolver_mode += "[LM]";

				break;
			default:
				break;
			}


		}
		else {
			if (fabsf(math::AngleDiff(lower_body_yaw, away + 165.f)) <= 3.f && data->m_exback_idx < 1) {
				record->m_mode = Modes::RESOLVE_EXBACK;
				record->m_eye_angles.y = away + 165.f;
				record->m_resolver_mode = "F:EXBACK";
				record->m_resolver_color = colors::transparent_yellow;
			}
			else if (can_backwards && data->m_backwards_idx < 1 && !data->m_has_freestand) {
				record->m_mode = Modes::RESOLVE_BACK;
				record->m_resolver_mode = "F:BW";
				record->m_resolver_color = colors::transparent_green;
				record->m_eye_angles.y = away + 180.0f/*record->m_body*/;
			}

			else if (math::AngleDiff(lower_body_yaw, data->m_best_angle) <= 3.f && data->m_reverse_fs < 2 && data->m_has_freestand) {
				record->m_mode = Modes::RESOLVE_REVERSEFS;
				record->m_eye_angles.y = data->m_best_angle;
				record->m_resolver_mode = "F:FS";
				record->m_resolver_color = colors::transparent_green;
			}
			else if (IsYawSideways(data->m_player, move->m_body, false) && last_move_delta <= 12.5f && valid_last_move)
			{
				record->m_mode = Modes::RESOLVE_LASTMOVE;
				record->m_resolver_mode = "S:LAST";
				record->m_resolver_color = colors::transparent_green;
				record->m_eye_angles.y = move->m_body;
				data->m_valid_last_move = true;
			}
			else if (last_move_delta <= 15.0f && valid_last_move) {
				record->m_mode = Modes::RESOLVE_LASTMOVE;
				record->m_resolver_mode = "S:SLAST";
				record->m_resolver_color = colors::transparent_green;
				record->m_eye_angles.y = move->m_body;
				data->m_valid_last_move = true;
			}
			else if (data->m_has_freestand && IsYawSideways(data->m_player, lower_body_yaw, false) && data->m_sfs_idx < 2) { // method 1, semi-safe
				record->m_mode = Modes::RESOLVE_SFS;
				record->m_eye_angles.y = data->m_best_angle;
				record->m_resolver_mode = "S:FS";
				record->m_resolver_color = colors::transparent_yellow;
			}
			else if (!IsYawSideways(data->m_player, lower_body_yaw, true) && fabsf(lower_body_yaw - away + 165.f) <= 40.f && data->m_sexback_idx < 1) { // method 1 back 165, semi-safe
				record->m_mode = Modes::RESOLVE_SEXBACK;
				record->m_eye_angles.y = away + 165.f;
				record->m_resolver_mode = "S:EX:BACK";
				record->m_resolver_color = colors::transparent_yellow;
			}
			else if (!IsYawSideways(data->m_player, lower_body_yaw, false) && data->m_sback_idx < 1) { // method 1 back, semi-safe
				record->m_mode = Modes::RESOLVE_SBACK;
				record->m_eye_angles.y = away + 180.f;
				record->m_resolver_mode = "S:BACK";
				record->m_resolver_color = colors::transparent_yellow;
			}
			else {
				record->m_mode = Modes::RESOLVE_UNSAFELM;
				record->m_eye_angles.y = move->m_body;
				record->m_resolver_mode = "U:LM";
				record->m_resolver_color = colors::transparent_yellow;
			}
		}

		if (previous && data->m_body_index <= 2) {
			// if proxy updated and we have a timer update
			// or anim lby changed	
			bool timer_update = (data->m_body_proxy_updated && data->m_body_update <= record->m_anim_time);
			bool body_update = fabsf(math::AngleDiff(record->m_body, previous->m_body)) >= 35.f;

			// body updated.
			if (body_update || timer_update) {
				record->m_mode = Modes::RESOLVE_BODY;

				// set the resolve mode.
				record->m_resolver_mode = "L:UPD";
				record->m_resolver_color = colors::green;

				// set angles to current LBY.
				record->m_eye_angles.y = record->m_body;
				iPlayers[data->m_player->index()] = false;

				// delay body update.
				data->m_body_update = record->m_anim_time + 1.1f;
				data->m_has_body_updated = true;

				// exit out of the resolver, thats it.
				return;
			}
		}
	}
	else {
		auto local_player = g_cl.m_local;
		//record->m_mode = Modes::RESOLVE_BRUTEFORCE;
		float away = GetAwayAngle(record);
		float lower_body_yaw = record->m_body;
		float forward_diff = fabsf(math::AngleDiff(lower_body_yaw, away));
		float back_diff = fabsf(math::AngleDiff(lower_body_yaw, away + 180.f));
		bool can_backwards = fabsf(back_diff) <= 3.f;
		bool can_forwards = fabsf(forward_diff) <= 5.f;


		if (data->m_has_freestand && math::AngleDiff(lower_body_yaw, data->m_best_angle) <= 3.f && data->m_reverse_fs < 2) { // fully resolved
			record->m_mode = Modes::RESOLVE_REVERSEFS;
			record->m_eye_angles.y = data->m_best_angle;
			record->m_resolver_mode = "F:FS";
			record->m_resolver_color = colors::transparent_yellow;
		}
		else if (can_backwards && data->m_backwards_idx < 1) { // fully resolved
			record->m_mode = Modes::RESOLVE_BACK;
			record->m_eye_angles.y = away + 180.f;
			record->m_resolver_mode = "F:BACK";
			record->m_resolver_color = colors::transparent_yellow;
		}
		else if (fabsf(math::AngleDiff(lower_body_yaw, away + 165.f)) <= 3.f && data->m_exback_idx < 1) { // fully resolved
			record->m_mode = Modes::RESOLVE_EXBACK;
			record->m_eye_angles.y = away + 165.f;
			record->m_resolver_mode = "F:EX:BACK";
			record->m_resolver_color = colors::transparent_yellow;
		}

		else if (can_forwards && data->m_edge_index < 1 /* == 0 */) {
			record->m_mode = Modes::RESOLVE_EDGE;
			record->m_resolver_mode = "F:EDGE";
			record->m_resolver_color = colors::transparent_yellow;
			record->m_eye_angles.y = away;
		}
		// the fuckery begins
		else if (data->m_has_freestand && IsYawSideways(data->m_player, lower_body_yaw, false) && data->m_sfs_idx < 2) { // method 1, semi-safe
			record->m_mode = Modes::RESOLVE_SFS;
			record->m_eye_angles.y = data->m_best_angle;
			record->m_resolver_mode = "S:FS";
			record->m_resolver_color = colors::transparent_yellow;
		}
		else if (!IsYawSideways(data->m_player, lower_body_yaw, false) && data->m_sback_idx < 1) { // method 1 back, semi-safe
			record->m_mode = Modes::RESOLVE_SBACK;
			record->m_eye_angles.y = away + 180.f;
			record->m_resolver_mode = "S:BACK";
			record->m_resolver_color = colors::transparent_yellow;
		}
		else if (!IsYawSideways(data->m_player, lower_body_yaw, false) && fabsf(lower_body_yaw - away + 165.f) <= 40.f && data->m_sexback_idx < 1) { // fully resolved
			record->m_mode = Modes::RESOLVE_SEXBACK;
			record->m_eye_angles.y = away + 165.f;
			record->m_resolver_mode = "S:EX:BACK";
			record->m_resolver_color = colors::transparent_yellow;
		}
		else {
				record->m_mode = Modes::RESOLVE_UNSAFELBY;
				record->m_eye_angles.y = record->m_body;
				record->m_resolver_mode = "U:LBY";
				record->m_resolver_color = colors::red;
		}

	}
}

void Resolver::StandNS(AimPlayer* data, LagRecord* record) {
	// get away angles.
	float away = GetAwayAngle(record);

	switch (data->m_shots % 8) {
	case 0:
		record->m_eye_angles.y = away + 180.f;
		break;

	case 1:
		record->m_eye_angles.y = away + 90.f;
		break;
	case 2:
		record->m_eye_angles.y = away - 90.f;
		break;

	case 3:
		record->m_eye_angles.y = away + 45.f;
		break;
	case 4:
		record->m_eye_angles.y = away - 45.f;
		break;

	case 5:
		record->m_eye_angles.y = away + 135.f;
		break;
	case 6:
		record->m_eye_angles.y = away - 135.f;
		break;

	case 7:
		record->m_eye_angles.y = away;
		break;

	default:
		break;
	}

	// force LBY to not fuck any pose and do a true bruteforce.
	record->m_body = record->m_eye_angles.y;
}

void Resolver::ResolveOverride(AimPlayer* data, LagRecord* record, Player* player) {
	if (g_input.GetKeyState(g_menu.main.aimbot.override.get())) {
		ang_t       viewangles;
		g_csgo.m_engine->GetViewAngles(viewangles);

		//auto yaw = math::clamp (  g_cl.m_local->GetAbsOrigin(   ), Player->origin(   )  ).y;
		const float at_target_yaw = math::CalcAngle(g_cl.m_local->m_vecOrigin(), record->m_origin).y;

		if (fabs(math::NormalizedAngle(viewangles.y - at_target_yaw)) > 30.f)
			return ResolveStand(data, record, nullptr); // dc


		if ((math::NormalizedAngle(viewangles.y - at_target_yaw) > 0 && math::NormalizedAngle(viewangles.y - at_target_yaw) <= 2.f) || (math::NormalizedAngle(viewangles.y - at_target_yaw) < 0 && math::NormalizedAngle(viewangles.y - at_target_yaw) > -2.f)) {
			record->m_eye_angles.y = at_target_yaw;
		}
		else if (math::NormalizedAngle(viewangles.y - at_target_yaw) < 0) {
			record->m_eye_angles.y = at_target_yaw - 90.f;
		}
		else if (math::NormalizedAngle(viewangles.y - at_target_yaw) > 0) {
			record->m_eye_angles.y = at_target_yaw + 90.f;
		}

		// set the resolve mode.
		record->m_resolver_mode = XOR("OVERRIDE");
		record->m_mode = Modes::RESOLVE_OVERRIDE;
	}

	if (record->m_anim_time > data->m_body_update && g_menu.main.aimbot.correct.get()) {
		// only shoot the LBY flick 3 times.
		// if we happen to miss then we most likely mispredicted.
		if (data->m_body_index <= 0) {
			// set angles to current LBY.
			record->m_eye_angles.y = data->m_body;

			// predict next body update.
			data->m_body_update = record->m_anim_time + 1.1f;

			// set the resolve mode.
			record->m_resolver_mode = XOR("LBYUPD");
			record->m_mode = Modes::RESOLVE_BODY;

			return;
		}
		else if (data->m_body != data->m_old_body && !(data->m_body_index <= 0) && g_menu.main.aimbot.correct.get()) {
			// set angles to current LBY.
			record->m_eye_angles.y = data->m_body;

			// predict next body update.
			data->m_body_update = record->m_anim_time + 1.1f;

			// reset misses.
			data->m_body_index = 0;

			// set the resolve mode.
			record->m_resolver_mode = XOR("LBYPRED");
			record->m_mode = Modes::RESOLVE_BODY;

			return;
		}
	}
}

void Resolver::ResolveAir(AimPlayer* data, LagRecord* record, Player* player) {
	// for no-spread call a seperate resolver.
	if (g_menu.main.aimbot.nospread.get()) {
		AirNS(data, record);
		return;
	}


	if (g_menu.main.aimbot.correct_network1.get() && (data->m_is_cheese_crack || data->m_is_kaaba) && data->m_network_index <= 1) {
		record->m_eye_angles.y = data->m_networked_angle;
		record->m_resolver_color = { 155, 210, 100 };
		record->m_resolver_mode = "NETWORKED";
		record->m_mode = Modes::RESOLVE_NETWORK;
		return;
	}

	// else run our matchmaking air resolver.

	// try to predict the direction of the player based on his velocity direction.
	// this should be a rough estimation of where he is looking.
	float velyaw = math::rad_to_deg(std::atan2(record->m_anim_velocity.y, record->m_anim_velocity.x));

	// pointer for easy access.
	LagRecord* move = &data->m_walk_record;
	LagRecord* previous = data->m_records.size() > 1 ? data->m_records[1].get()->dormant() ? nullptr : data->m_records[1].get() : nullptr;

	iPlayers[player->index()] = true;

	float back_diff = fabsf(velyaw + 180.0f - record->m_body);

	record->m_mode = Modes::RESOLVE_AIR;
	bool can_last_move_air = move->m_anim_time > 0.f &&
		fabsf(move->m_body - record->m_body) < 12.5f
		&& data->m_air_brute_index < 1;

	if (back_diff <= 18.f && data->m_air_brute_index < 2) {
		record->m_resolver_mode = XOR("AIR-BACK");
		record->m_resolver_color = colors::transparent_green;
		record->m_eye_angles.y = velyaw + 180.f;
	}
	else if (can_last_move_air)
	{
		record->m_resolver_mode = XOR("AIR-MOVE");
		record->m_resolver_color = colors::transparent_green;
		record->m_eye_angles.y = move->m_body;
	}
	else
	{
		switch (data->m_air_brute_index % 3) {
		case 0:
			record->m_resolver_mode = XOR("BAIR-BACK");
			record->m_resolver_color = colors::transparent_green;
			record->m_eye_angles.y = velyaw + 180.f;
			break;
		case 1:
			record->m_resolver_mode = XOR("BAIR-LBYPOS");
			record->m_resolver_color = colors::red;
			record->m_eye_angles.y = record->m_body + 35.f;
			break;
		case 2:
			record->m_resolver_mode = XOR("BAIR-LBYNEG");
			record->m_resolver_color = colors::red;
			record->m_eye_angles.y = record->m_body - 35.f;
			break;
		default:
			break;
		}
	}
}


void Resolver::AirNS(AimPlayer* data, LagRecord* record) {
	// get away angles.
	float away = GetAwayAngle(record);

	switch (data->m_shots % 9) {
	case 0:
		record->m_eye_angles.y = away + 180.f;
		break;

	case 1:
		record->m_eye_angles.y = away + 150.f;
		break;
	case 2:
		record->m_eye_angles.y = away - 150.f;
		break;

	case 3:
		record->m_eye_angles.y = away + 165.f;
		break;
	case 4:
		record->m_eye_angles.y = away - 165.f;
		break;

	case 5:
		record->m_eye_angles.y = away + 135.f;
		break;
	case 6:
		record->m_eye_angles.y = away - 135.f;
		break;

	case 7:
		record->m_eye_angles.y = away + 90.f;
		break;
	case 8:
		record->m_eye_angles.y = away - 90.f;
		break;

	default:
		break;
	}
}