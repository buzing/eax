#include "includes.h"

Aimbot g_aimbot{ };;

bool GetAimMatrix(ang_t angle, BoneArray* bones) {
	float backup_pose;

	backup_pose = g_cl.m_local->m_flPoseParameter()[12];

	g_cl.m_local->m_flPoseParameter()[12] = (angle.x + 90.f) / 180.f;
	const auto ret = g_bones.SetupBones(g_cl.m_local, bones, 128, 0x0007FF00, g_csgo.m_globals->m_curtime, 0);

	g_cl.m_local->m_flPoseParameter()[12] = backup_pose;

	return ret;
}

ang_t look(vec3_t from, vec3_t target) {
	target -= from;
	ang_t angles;
	if (target.y == 0.0f && target.x == 0.0f) {
		angles.x = (target.z > 0.0f) ? 270.0f : 90.0f;
		angles.y = 0.0f;
	}
	else {
		angles.x = static_cast<float>(-atan2(-target.z, target.length_2d()) * -180.f / 3.14159265358979323846);
		angles.y = static_cast<float>(atan2(target.y, target.x) * 180.f / 3.14159265358979323846);

		//if ( angles.y > 90 )
		//	angles.y -= 180;
		//else if ( angles.y < 90 )
		//	angles.y += 180;
		//else if ( angles.y == 90 )
		//	angles.y = 0;
	}

	angles.z = 0.0f;
	return angles;
}

void CatchGround(LagRecord* current, LagRecord* previous, Player* entry) {
	if (current->m_flags & FL_ONGROUND) {
		if (current->m_layers[5].m_weight > 0.f
			&& previous->m_layers[5].m_weight <= 0.f
			&& !(previous->m_flags & FL_ONGROUND)) {
			const auto& land_seq = current->m_layers[5].m_sequence;

			if (land_seq >= 2) {
				const auto& land_act = entry->GetSequenceActivity(land_seq);

				if (land_act == 988
					|| land_act == 989) {
					const auto& cur_cycle = current->m_layers[5].m_cycle;
					const auto& cur_rate = current->m_layers[5].m_playback_rate;
					if (cur_cycle != 0.f &&
						cur_rate != 0.f) {

						const auto land_time = cur_cycle / cur_rate;
						if (land_time != 0.f) {
							current->m_unfixed_on_ground = true;
							current->m_act_time = current->m_anim_time - land_time;
						}
					}
				}
			}
		}

		current->m_anim_velocity.z = 0.f;
	}
	else {
		const auto jump_seq = current->m_layers[4].m_sequence;

		if (!(previous->m_flags & FL_ONGROUND)) {
			if (jump_seq >= 2) {
				const auto jump_act = entry->GetSequenceActivity(jump_seq);

				if (jump_act == 985) {
					const auto& cur_cycle = current->m_layers[4].m_cycle;
					const auto& cur_rate = current->m_layers[4].m_playback_rate;

					if (cur_cycle != 0.f &&
						cur_rate != 0.f) {

						const auto jump_time = cur_cycle / cur_rate;
						if (jump_time != 0.f) {
							current->m_unfixed_on_ground = false;
							current->m_act_time = current->m_anim_time - jump_time;
						}
					}
				}
			}
		}

		if (current->m_layers[4].m_weight > 0.f
			&& current->m_layers[4].m_playback_rate > 0.f
			&& entry->GetSequenceActivity(jump_seq) == 985) {
			const auto jump_time = (((current->m_layers[4].m_cycle / current->m_layers[4].m_playback_rate)
				/ g_csgo.m_globals->m_interval) + 0.5f) * g_csgo.m_globals->m_interval;
			const auto update_time = (game::TIME_TO_TICKS(((current->m_anim_time))) * g_csgo.m_globals->m_interval) - ((((
				current->m_layers[4].m_cycle / current->m_layers[4].m_playback_rate) / g_csgo.m_globals->m_interval
				) + 0.5f
				) * g_csgo.m_globals->m_interval);

			if (entry->m_fFlags() & FL_ONGROUND) {
				if (update_time > entry->m_PlayerAnimState()->m_last_update_time) {
					entry->m_PlayerAnimState()->m_on_ground = false;
					entry->m_flPoseParameter()[6] = 0.f;
					entry->m_PlayerAnimState()->m_time_since_in_air = 0.f;
					entry->m_PlayerAnimState()->m_last_update_time = update_time;
				}
			}

			static auto sv_gravity = g_csgo.m_cvar->FindVar(HASH("sv_gravity"));
			static auto sv_jump_impulse = g_csgo.m_cvar->FindVar(HASH("sv_jump_impulse"));

			current->m_anim_velocity.z = sv_jump_impulse->GetFloat() - sv_gravity->GetFloat() * jump_time;
		}
	}

	if (current->m_unfixed_on_ground.has_value()) {
		const auto anim_tick = game::TIME_TO_TICKS(current->m_anim_time) - current->m_lag;

		if (!current->m_unfixed_on_ground.value()) {
			const auto& jump_tick = game::TIME_TO_TICKS(current->m_act_time) + 1;

			if (jump_tick == anim_tick) {
				entry->m_fFlags() |= FL_ONGROUND;
			}
			else if (jump_tick == anim_tick + 1) {
				entry->m_AnimOverlay()[4].m_playback_rate = current->m_layers[4].m_playback_rate;
				entry->m_AnimOverlay()[4].m_sequence = current->m_layers[4].m_sequence;
				entry->m_AnimOverlay()[4].m_cycle = 0.f;
				entry->m_AnimOverlay()[4].m_weight = 0.f;
				entry->m_fFlags() &= ~FL_ONGROUND;
			}
		}
		else {
			const auto land_tick = game::TIME_TO_TICKS(current->m_act_time) + 1;
			if (land_tick == anim_tick) {
				entry->m_fFlags() &= ~FL_ONGROUND;
			}
			else if (land_tick == anim_tick + 1) {
				entry->m_AnimOverlay()[5].m_playback_rate = current->m_layers[5].m_playback_rate;
				entry->m_AnimOverlay()[5].m_sequence = current->m_layers[5].m_sequence;
				entry->m_AnimOverlay()[5].m_cycle = 0.f;
				entry->m_AnimOverlay()[5].m_weight = 0.f;
				entry->m_fFlags() |= FL_ONGROUND;
			}
		}
	}

	if (!(current->m_flags & FL_ONGROUND)) {
		if (current->m_layers[4].m_weight != 0.f
			&& current->m_layers[4].m_playback_rate != 0.f) {
			const auto& cur_seq = current->m_layers[4].m_sequence;

			if (entry->GetSequenceActivity(cur_seq) == 985) {
				const auto& cur_cycle = current->m_layers[4].m_cycle;
				const auto& previous_cycle = previous->m_layers[4].m_cycle;

				const auto previous_seq = previous->m_layers[4].m_sequence;

				if ((cur_cycle != previous_cycle || previous_seq != cur_seq)
					&& previous_cycle > cur_cycle) {
					entry->m_flPoseParameter()[6] = 0.f;
					entry->m_PlayerAnimState()->m_time_since_in_air = cur_cycle / current->m_layers[4].m_playback_rate;
				}
			}
		}
	}
}

void AimPlayer::UpdateAnimations(LagRecord* record) {
	if (m_player->m_PlayerAnimState()->m_last_update_frame >= g_csgo.m_globals->m_frame)
		m_player->m_PlayerAnimState()->m_last_update_frame -= 1;
	LagRecord* previous_record = this->m_records.size() > 1 ? this->m_records[1].get()->dormant() ? nullptr : this->m_records[1].get() : nullptr;
	auto origin = m_player->m_vecOrigin();
	auto velocity = m_player->m_vecVelocity();
	auto abs_velocity = m_player->m_vecAbsVelocity();
	auto ie_flags = m_player->m_iEFlags();
	auto flags = m_player->m_fFlags();
	auto duck_amt = m_player->m_flDuckAmount();
	auto lby = m_player->m_flLowerBodyYawTarget();

	record->simulate(previous_record, m_player);

	m_player->SetAbsOrigin(record->m_origin);

	if (previous_record) {
		m_player->SetAnimLayers(previous_record->m_layers);
		m_player->m_PlayerAnimState()->m_move_weight = previous_record->m_layers[6].m_weight;
		m_player->m_PlayerAnimState()->m_primary_cycle = previous_record->m_layers[6].m_cycle;

		m_player->m_PlayerAnimState()->m_strafe_weight = previous_record->m_layers[7].m_weight;
		m_player->m_PlayerAnimState()->m_strafe_sequence = previous_record->m_layers[7].m_sequence;
		m_player->m_PlayerAnimState()->m_strafe_cycle = previous_record->m_layers[7].m_cycle;
		m_player->m_PlayerAnimState()->m_acceleration_weight = previous_record->m_layers[12].m_weight;

		m_player->m_PlayerAnimState()->m_foot_yaw = previous_record->m_foot_yaw;
		m_player->m_PlayerAnimState()->m_move_yaw = previous_record->m_move_yaw;
		m_player->m_PlayerAnimState()->m_move_yaw_cur_to_ideal = previous_record->m_move_yaw_cur_to_ideal;
		m_player->m_PlayerAnimState()->m_move_yaw_ideal = previous_record->m_move_yaw_ideal;
		m_player->m_PlayerAnimState()->m_move_weight_smoothed = previous_record->m_move_weight_smoothed;

		CatchGround(record, previous_record, m_player);

		if (m_player->m_fFlags() & FL_ONGROUND
			&& (record->m_anim_velocity.length() < 0.1f) || record->m_fake_walk
			&& std::abs(int(previous_record->m_body - record->m_body)) <= 0.00001f) {
			m_player->m_PlayerAnimState()->m_foot_yaw = previous_record->m_body;
		}
	}
	else {
		m_player->SetAnimLayers(record->m_layers);

		if (record->m_flags & FL_ONGROUND) {
			m_player->m_PlayerAnimState()->m_on_ground = true;
			m_player->m_PlayerAnimState()->m_landing = false;
		}

		m_player->m_PlayerAnimState()->m_primary_cycle = record->m_layers[6].m_cycle;
		m_player->m_PlayerAnimState()->m_move_weight = record->m_layers[6].m_weight;
		m_player->m_PlayerAnimState()->m_strafe_weight = record->m_layers[7].m_weight;
		m_player->m_PlayerAnimState()->m_strafe_sequence = record->m_layers[7].m_sequence;
		m_player->m_PlayerAnimState()->m_strafe_cycle = record->m_layers[7].m_cycle;
		m_player->m_PlayerAnimState()->m_acceleration_weight = record->m_layers[12].m_weight;

		m_player->m_PlayerAnimState()->m_last_update_time = record->m_sim_time - g_csgo.m_globals->m_interval;
	}

	if (record->m_lag >= 2
		&& previous_record) {
		const auto duck_delta = (record->m_duck - previous_record->m_duck) / record->m_lag;
		const auto vel_delta = (record->m_anim_velocity - previous_record->m_anim_velocity) / record->m_lag;

		const auto interpolate_velocity =
			record->m_layers[6].m_playback_rate == 0.f || previous_record->m_layers[6].m_playback_rate == 0.f
			|| ((record->m_anim_velocity.length() >= 1.1f) && (previous_record->m_anim_velocity.length() >= 1.1f));

		if (interpolate_velocity) {
			m_player->m_vecAbsVelocity() = m_player->m_vecVelocity() = previous_record->m_anim_velocity + vel_delta;
		}
		else {
			m_player->m_vecAbsVelocity() = m_player->m_vecVelocity() = { 1.1f, 0.f, 0.f };
		}

		m_player->m_flDuckAmount() = previous_record->m_duck + duck_delta;
	}
	else {
		m_player->m_flDuckAmount() = record->m_duck;
		m_player->m_vecAbsVelocity() = m_player->m_vecVelocity() = record->m_anim_velocity;
	}

	if ((((1 / g_csgo.m_globals->m_interval) * record->m_shot_time) + 0.5f) ==
		(((1 / g_csgo.m_globals->m_interval) * record->m_sim_time) + 0.5f)) {

	}
	else {
		auto shot_tick = (((1 / g_csgo.m_globals->m_interval) * record->m_shot_time) + 0.5);
		if (shot_tick >= (((1 / g_csgo.m_globals->m_interval) * (record->m_anim_time)) + 0.5) && shot_tick <= (((1 / g_csgo.m_globals->m_interval) * record->m_sim_time) + 0.5)) {
			if (!previous_record
				|| record->m_lag <= 1)
				record->m_eye_angles.x = 89.f;
			else {
				record->m_eye_angles.x = previous_record->m_eye_angles.x;
			}
		}
	}

	// is player a bot?
	bool bot = game::IsFakePlayer(m_player->index());

	if (!bot) {
		g_resolver.ResolveAngles(m_player, record);
	}
	else {
		record->m_mode = 0;
		record->m_resolver_mode = "none";
	}

	if (record->m_lag > 2
		&& record->m_layers[6].m_weight == 0.0f
		|| record->m_layers[6].m_playback_rate == 0.0f
		&& record->m_velocity.length() > 0.1f
		&& record->m_velocity.length() < 25.f
		&& m_player->m_fFlags() & FL_ONGROUND) {
		record->m_fake_walk = true;
		record->m_anim_velocity = { };
		record->m_velocity = { };
	}

	m_player->m_vecOrigin() = record->m_origin;
	m_player->m_flLowerBodyYawTarget() = record->m_body;
	m_player->m_iEFlags() &= ~(EFL_DIRTY_ABSVELOCITY | EFL_DIRTY_ABSTRANSFORM);
	m_player->m_angEyeAngles() = record->m_eye_angles;

	const auto frame_count = g_csgo.m_globals->m_frame;
	const auto real_time = g_csgo.m_globals->m_realtime;
	const auto cur_time = g_csgo.m_globals->m_curtime;
	const auto frame_time = g_csgo.m_globals->m_frametime;
	const auto abs_frame_time = g_csgo.m_globals->m_abs_frametime;
	const auto interp_amt = g_csgo.m_globals->m_interp_amt;
	const auto tick_count = g_csgo.m_globals->m_tick_count;

	const auto v76 = (record->m_old_sim_time + g_csgo.m_globals->m_interval) / g_csgo.m_globals->m_interval;
	const int v77 = v76 + 0.5f;
	g_csgo.m_globals->m_realtime = record->m_old_sim_time + g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_curtime = record->m_old_sim_time + g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_abs_frametime = g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_frame = v77;
	g_csgo.m_globals->m_tick_count = v77;
	g_csgo.m_globals->m_interp_amt = 0.0f;

	m_player->m_bClientSideAnimation() = true;
	m_player->UpdateClientSideAnimation();
	m_player->m_bClientSideAnimation() = false;

	std::memcpy(m_player->m_flPoseParameter(), record->m_poses, sizeof(float(24u)));

	m_player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);

	m_player->SetAnimLayers(record->m_layers);

	record->m_foot_yaw = m_player->m_PlayerAnimState()->m_foot_yaw;
	record->m_move_yaw = m_player->m_PlayerAnimState()->m_move_yaw;
	record->m_move_yaw_cur_to_ideal = m_player->m_PlayerAnimState()->m_move_yaw_cur_to_ideal;
	record->m_move_yaw_ideal = m_player->m_PlayerAnimState()->m_move_yaw_ideal;
	record->m_move_weight_smoothed = m_player->m_PlayerAnimState()->m_move_weight_smoothed;

	g_bones.SetupBones(m_player, record->m_bones, 128, BONE_USED_BY_ANYTHING & ~BONE_USED_BY_BONE_MERGE, record->m_sim_time, 2);
	record->m_setup = record->m_bones;

	g_csgo.m_globals->m_realtime = real_time;
	g_csgo.m_globals->m_curtime = cur_time;
	g_csgo.m_globals->m_frametime = frame_time;
	g_csgo.m_globals->m_abs_frametime = abs_frame_time;
	g_csgo.m_globals->m_interp_amt = interp_amt;
	g_csgo.m_globals->m_frame = frame_count;
	g_csgo.m_globals->m_tick_count = tick_count;

	m_player->m_vecOrigin() = origin;
	m_player->m_vecVelocity() = velocity;
	m_player->m_vecAbsVelocity() = abs_velocity;
	m_player->m_fFlags() = flags;
	m_player->m_iEFlags() = ie_flags;
	m_player->m_flDuckAmount() = duck_amt;
	m_player->m_flLowerBodyYawTarget() = lby;
}

void AimPlayer::OnNetUpdate(Player* player) {
	bool reset = (!g_menu.main.aimbot.enable.get() || player->m_lifeState() == LIFE_DEAD || !player->enemy(g_cl.m_local));

	// if this happens, delete all the lagrecords.
	if (reset) {
		m_stand_index = 0;
		m_stand_index2 = 0;
		m_stand_index3 = 0;
		m_last_move = 0;
		m_body_index = 0;
		m_lby_idx = 0;
		m_edge_index = 0;
		m_bruteforce_idx = 0;
		m_backwards_idx = 0;
		m_reverse_fs = 0;
		m_anti_index = 0;
		m_freestanding_index = 0;
		m_exback_idx = 0;
		m_fake_walk_idx = 0;
		m_air_brute_index = 0;
		m_lastmove_idx = 0;
		m_moved = false;
		m_has_body_updated = false;
		m_has_distort = false;
		m_valid_last_move = false;
		player->m_bClientSideAnimation() = true;
		m_records.clear();
		return;
	}
	// update player ptr if required.
	// reset player if changed.
	if (m_player != player)
		m_records.clear();

	// update player ptr.
	m_player = player;

	// indicate that this player has been out of pvs.
	// insert dummy record to separate records
	// to fix stuff like animation and prediction.
	if (player->dormant()) {
		m_stand_index = 0;
		m_stand_index2 = 0;
		m_stand_index3 = 0;
		m_last_move = 0;
		m_body_index = 0;
		m_lby_idx = 0;
		m_edge_index = 0;
		m_bruteforce_idx = 0;
		m_exback_idx = 0;
		m_backwards_idx = 0;
		m_reverse_fs = 0;
		m_anti_index = 0;
		m_freestanding_index = 0;
		m_fake_walk_idx = 0;
		m_air_brute_index = 0;
		m_lastmove_idx = 0;
		m_moved = false;
		m_has_body_updated = false;
		m_has_distort = false;
		m_valid_last_move = false;
		bool insert = true;

		// we have any records already?
		if (!m_records.empty()) {

			LagRecord* front = m_records.front().get();

			// we already have a dormancy separator.
			if (front->dormant())
				insert = false;
		}

		if (insert) {
			// add new record.
			m_records.emplace_front(std::make_shared< LagRecord >(player));

			// get reference to newly added record.
			LagRecord* current = m_records.front().get();

			// mark as dormant.
			current->m_dormant = player->dormant();
		}
		return;
	}


	// player->m_flSimulationTime() > player->m_flOldSimulationTime( )
	// player->m_flSimulationTime() > m_records.front().get()->m_sim_time

	bool update = (m_records.empty() || player->m_flSimulationTime() > m_records.front().get()->m_sim_time);

	//if (!update && !player->dormant()) {
	//	if (player->m_vecOrigin() != player->m_vecOldOrigin())
	//		update = true;
	//
	//	// fix data.
	//	player->m_flSimulationTime() = game::TICKS_TO_TIME(g_csgo.m_cl->m_server_tick);
	//}


	// this is the first data update we are receving
	// OR we received data with a newer simulation context.

	if (update) {
		// add new record.
		m_records.emplace_front(std::make_shared< LagRecord >(player));

		// get reference to newly added record.
		LagRecord* current = m_records.front().get();

		// mark as non dormant.
		current->m_dormant = false;

		// update animations on current record.
		// call resolver.
		UpdateAnimations(current);
	}

	// no need to store insane amt of data.
	while (m_records.size() > int(1.0f / g_csgo.m_globals->m_interval)) {
		m_records.pop_back();
	}

	// we can't backtrack break lc.
	while ((m_records.size() > 0 && m_records.front().get()->m_broke_lc)) {
		m_records.pop_back();
	}
}

void AimPlayer::OnRoundStart(Player* player) {
	m_player = player;
	m_walk_record = LagRecord{ };
	m_shots = 0;
	m_missed_shots = 0;

	// reset stand and body index.
	m_stand_index = 0;
	m_stand_index2 = 0;
	m_stand_index3 = 0;
	m_last_move = 0;
	m_body_index = 0;
	m_lby_idx = 0;
	m_edge_index = 0;
	m_bruteforce_idx = 0;
	m_exback_idx = 0;
	m_backwards_idx = 0;
	m_reverse_fs = 0;
	m_anti_index = 0;
	m_freestanding_index = 0;
	m_fake_walk_idx = 0;
	m_air_brute_index = 0;
	m_lastmove_idx = 0;
	m_moved = false;
	m_has_body_updated = false;
	m_has_distort = false;
	m_valid_last_move = false;

	m_records.clear();
	m_hitboxes.clear();

	// IMPORTANT: DO NOT CLEAR LAST HIT SHIT.
}

void AimPlayer::SetupHitboxes(LagRecord* record, bool history) {
	// reset hitboxes.
	m_hitboxes.clear();
	m_prefer_body = false;

	if (g_cl.m_weapon_id == ZEUS) {
		// hitboxes for the zeus.
		m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER });
		return;
	}

	m_prefer_body = g_menu.main.aimbot.prefer_baim.get();

	if (m_prefer_body) {

		if (g_menu.main.aimbot.prefer_baim_disablers.get(0) && record->m_mode == Resolver::Modes::RESOLVE_BODY)
			m_prefer_body = false;

		if (g_menu.main.aimbot.prefer_baim_disablers.get(1) && record->m_mode == Resolver::Modes::RESOLVE_WALK)
			m_prefer_body = false;

		if (g_menu.main.aimbot.prefer_baim_disablers.get(2) && (m_is_kaaba || m_is_cheese_crack))
			m_prefer_body = false;

		if (g_menu.main.aimbot.prefer_baim_disablers.get(3) && record->m_mode == Resolver::Modes::RESOLVE_LBY)
			m_prefer_body = false;

		//for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
		//	Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);
		//
		//	if (!g_aimbot.IsValidTarget(player))
		//		continue;

		if (g_menu.main.aimbot.prefer_baim_disablers.get(4) && m_player->m_iHealth() <= 72)
			m_prefer_body = false;
		//}
	}


	// only, on key.
	auto hitbox{ g_menu.main.aimbot.hitbox };

	bool ignore_limbs = record->m_velocity.length_2d() > record->m_max_speed * 0.34f && g_menu.main.aimbot.ignore_limbs.get();

	// head
	if (hitbox.get(0) && !g_input.GetKeyState(g_menu.main.aimbot.baim_key.get())) {
		m_hitboxes.push_back({ HITBOX_HEAD });
	}

	// neck
	if (hitbox.get(1) && !g_input.GetKeyState(g_menu.main.aimbot.baim_key.get())) {
		m_hitboxes.push_back({ HITBOX_NECK });
	}

	// chest
	if (hitbox.get(2)) {
		m_hitboxes.push_back({ HITBOX_UPPER_CHEST });
		m_hitboxes.push_back({ HITBOX_CHEST });
	}

	// stomach
	if (hitbox.get(3)) {
		m_hitboxes.push_back({ HITBOX_BODY });
	}

	// arms
	if (hitbox.get(4)) {
		m_hitboxes.push_back({ HITBOX_L_UPPER_ARM });
		m_hitboxes.push_back({ HITBOX_R_UPPER_ARM });

		m_hitboxes.push_back({ HITBOX_L_FOREARM });
		m_hitboxes.push_back({ HITBOX_R_FOREARM });

	}

	// legs.
	if (hitbox.get(5)) {
		m_hitboxes.push_back({ HITBOX_L_CALF });
		m_hitboxes.push_back({ HITBOX_R_CALF });

		m_hitboxes.push_back({ HITBOX_L_THIGH });
		m_hitboxes.push_back({ HITBOX_R_THIGH });
	}

	// feet.
	if (hitbox.get(3) && !ignore_limbs) {
		m_hitboxes.push_back({ HITBOX_L_FOOT });
		m_hitboxes.push_back({ HITBOX_R_FOOT });
	}
}

void Aimbot::init() {
	// clear old targets.
	m_targets.clear();

	m_target = nullptr;
	m_aim = vec3_t{ };
	m_angle = ang_t{ };
	m_damage = 0.f;
	m_record = nullptr;
	m_stop = false;

	m_best_dist = std::numeric_limits< float >::max();
	m_best_fov = 180.f + 1.f;
	m_best_damage = 0.f;
	m_best_hp = 100 + 1;
	m_best_lag = std::numeric_limits< float >::max();
	m_best_height = std::numeric_limits< float >::max();
}

void Aimbot::StripAttack() {
	if (!g_cl.m_weapon)
		return;

	if (g_cl.m_weapon_id == REVOLVER)
		g_cl.m_cmd->m_buttons &= ~IN_ATTACK2;

	else
		g_cl.m_cmd->m_buttons &= ~IN_ATTACK;
}

void Aimbot::think() {
	// do all startup routines.
	init();

	// sanity.
	if (!g_cl.m_weapon
		|| !g_menu.main.aimbot.enable.get()
		|| g_cl.m_weapon_type == WEAPONTYPE_GRENADE
		|| g_cl.m_weapon_type == WEAPONTYPE_C4)
		return;

	// setup bones for all valid targets.
	for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

		if (!IsValidTarget(player))
			continue;

		AimPlayer* data = &m_players[i - 1];
		if (!data)
			continue;

		if (g_menu.main.aimbot.whitelist.get()) {
			if (data->m_is_eax)
				continue;
		}

		// store player as potential target this tick.
		m_targets.emplace_back(data);
	}

	// dont run ragebot if we have no targets.
	if (m_targets.empty() || m_targets.size() <= 0)
		return;

	// run knifebot.
	if (g_cl.m_weapon_type == WEAPONTYPE_KNIFE
		&& g_cl.m_weapon_id != ZEUS && g_cl.m_weapon_fire) {

		return knife();
	}

	// scan available targets... if we even have any.
	find();

	// animation silent aim, prevent the ticks with the shot in it to become the tick that gets processed.
	// we can do this by always choking the tick before we are able to shoot.
	bool revolver = g_cl.m_weapon_id == REVOLVER && g_cl.m_revolver_cock != 0;

	// one tick before being able to shoot.
	if (revolver && g_cl.m_revolver_cock > 0 && g_cl.m_revolver_cock == g_cl.m_revolver_query) {
		*g_cl.m_packet = false;
		return;
	}

	// we have a normal weapon or a non cocking revolver
	// choke if its the processing tick.
	if (g_cl.m_weapon_fire && !g_cl.m_lag && !revolver) {
		*g_cl.m_packet = false;
		return StripAttack();
	}

	// no point in aimbotting if we cannot fire this tick.
	if (!g_cl.m_weapon_fire)
		return StripAttack();

	// finally set data when shooting.
	apply();
}

void Aimbot::find() {
	struct BestTarget_t { Player* player; vec3_t pos; float damage; LagRecord* record; };

	vec3_t       tmp_pos;
	float        tmp_damage;
	BestTarget_t best;
	best.player = nullptr;
	best.damage = -1.f;
	best.pos = vec3_t{ };
	best.record = nullptr;

	if (m_targets.empty())
		return;

	// iterate all targets.
	for (const auto& t : m_targets) {
		if (t->m_records.empty())
			continue;

		// this player broke lagcomp.
		// his bones have been resetup by our lagcomp.
		// therfore now only the front record is valid.
		if (g_lagcomp.StartPrediction(t)) {

			if (t->m_delayed)
				continue;

			LagRecord* front = t->m_records.front().get(); // change to g_resolver.FindFirstRecord(); if working

			t->SetupHitboxes(front, false);
			if (t->m_hitboxes.empty())
				continue;

			// rip something went wrong..
			if (t->GetBestAimPosition(tmp_pos, tmp_damage, front) && SelectTarget(front, tmp_pos, tmp_damage)) {

				// if we made it so far, set shit.
				best.player = t->m_player;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				best.record = front;
			}
		}

		// player did not break lagcomp.
		// history aim is possible at this point.
		else {
			// ideal record
			LagRecord* ideal = g_resolver.FindIdealRecord(t);
			if (!ideal)
				continue;



			// no point in trying to backtrack someone breaking lc
			if (t->m_records.front().get()->m_broke_lc)
				return;

			t->SetupHitboxes(ideal, false);
			if (t->m_hitboxes.empty())
				continue;

			// try to select best record as target.
			if (t->GetBestAimPosition(tmp_pos, tmp_damage, ideal) && SelectTarget(ideal, tmp_pos, tmp_damage)) {
				// if we made it so far, set shit.
				best.player = t->m_player;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				best.record = ideal;
			}

			// last record
			LagRecord* last = g_resolver.FindLastRecord(t);
			if (!last || last == ideal)
				continue;

			t->SetupHitboxes(last, true);
			if (t->m_hitboxes.empty())
				continue;

			// rip something went wrong..
			if (t->GetBestAimPosition(tmp_pos, tmp_damage, last) && SelectTarget(last, tmp_pos, tmp_damage)) {
				// if we made it so far, set shit.
				best.player = t->m_player;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				best.record = last;
			}
		}
	}

	// verify our target and set needed data.
	if (best.player && best.record) {

		// calculate aim angle.

		vec3_t eye = g_cl.m_shoot_pos;
		auto looked = look(g_cl.m_shoot_pos, best.pos);
		if (GetAimMatrix((looked - g_cl.m_local->m_aimPunchAngle() * 2), g_cl.m_matrix)) {
			g_cl.m_local->ModifyEyePosition(g_cl.m_local->m_PlayerAnimState(), &eye);
			g_cl.m_shoot_pos = eye;
		}

		math::VectorAngles(best.pos - g_cl.m_shoot_pos, m_angle);

		// set member vars.
		m_target = best.player;
		m_aim = best.pos;
		m_damage = best.damage;
		m_record = best.record;

		// save fps
		if (m_damage > 0.f) {

			// write data, needed for traces / etc.
			m_record->cache();

			const bool on_land = !(g_cl.m_flags & FL_ONGROUND) && g_cl.m_local->m_fFlags() & FL_ONGROUND;



			// set autostop shit.
			if ((g_cl.m_local->m_fFlags() & FL_ONGROUND) && !on_land) {
				if ((g_cl.m_weapon_id != ZEUS || g_cl.m_weapon_type != WEAPONTYPE_KNIFE)) {
					m_stop = true;
				}
			}

			g_movement.AutoStop();

			bool on = g_menu.main.aimbot.hitchance.get() && !g_menu.main.aimbot.nospread.get();
			bool hit = on && CheckHitchance(m_target, m_hitbox, m_angle);


			// if we can scope.
			bool can_scope = !g_cl.m_local->m_bIsScoped() && (g_cl.m_weapon_id == AUG || g_cl.m_weapon_id == SG553 || g_cl.m_weapon_type == WEAPONTYPE_SNIPER_RIFLE);

			if (can_scope) {
				// always.
				if (g_menu.main.aimbot.zoom.get() == 1) {
					g_cl.m_cmd->m_buttons |= IN_ATTACK2;
					return;
				}

				// hitchance fail.
				else if (g_menu.main.aimbot.zoom.get() == 2 && on && !hit) {
					g_cl.m_cmd->m_buttons |= IN_ATTACK2;
					return;
				}
			}

			if (hit || !on) {
				// right click attack.
				if (g_menu.main.aimbot.nospread.get() && g_cl.m_weapon_id == REVOLVER)
					g_cl.m_cmd->m_buttons |= IN_ATTACK2;

				// left click attack.
				else
					g_cl.m_cmd->m_buttons |= IN_ATTACK;
			}
		}
	}
}

bool Aimbot::CheckHitchance(Player* player, int hitbox, const ang_t& angle) {
	float chance = g_menu.main.aimbot.hitchance_in_air.get() && !(g_cl.m_local->m_fFlags() & FL_ONGROUND) ? g_menu.main.aimbot.in_air_hitchance.get() : g_menu.main.aimbot.hitchance_amount.get();

	constexpr float HITCHANCE_MAX = 100.f;

	m_auto_walls_hit = m_auto_walls_done = 0;

	vec3_t     start{ g_cl.m_shoot_pos }, end{ 0, 0, 0 }, fwd{ 0, 0, 0 },
		right{ 0, 0, 0 }, up{ 0, 0, 0 }, dir{ 0, 0, 0 }, wep_spread{ 0, 0, 0 };

	float      inaccuracy{ 0 }, spread{ 0 };
	CGameTrace tr{ };

	int     total_hits{ }, needed_hits{ int(float(chance / HITCHANCE_MAX) * k_max_seeds) };

	// get needed directional vectors.
	math::AngleVectors(angle, &fwd, &right, &up);


	if (g_cl.m_weapon_id == WEAPON_ZEUS) {
		chance = g_menu.main.aimbot.zeusbot_hc.get();
	}

	// store off inaccuracy / spread ( these functions are quite intensive and we only need them once ).
	inaccuracy = g_cl.m_weapon->GetInaccuracy();
	spread = g_cl.m_weapon->GetSpread();

	// iterate all possible seeds.
	for (int i{ }; i < k_max_seeds; ++i) {

		penetration::PenetrationInput_t in;

		in.m_damage = 1.f;
		in.m_damage_pen = 1.f;
		in.m_can_pen = true;
		in.m_target = player;
		in.m_from = g_cl.m_local;

		penetration::PenetrationOutput_t out;

		// get spread.
		wep_spread = g_cl.m_weapon->CalculateSpread(m_static_seeds[i], inaccuracy, spread);

		// get spread direction.
		dir = (fwd + (right * wep_spread.x) + (up * wep_spread.y)).normalized();

		// get end of trace.
		end = start + (dir * g_cl.m_weapon_info->m_range);

		// setup ray and trace.
		g_csgo.m_engine_trace->ClipRayToEntity(Ray(start, end), MASK_SHOT | CONTENTS_HITBOX, player, &tr);

		// check if we hit a valid player / hitgroup on the player and increment total hits.
		if (tr.m_entity == player && game::IsValidHitgroup(tr.m_hitgroup)) {
			in.m_pos = end;
			++total_hits;

			if (penetration::run(&in, &out)) {
				++m_auto_walls_done;
				if (out.m_damage >= 1.f)
					++m_auto_walls_hit;
			}
		}

		// we cant make it anymore.
		if ((k_max_seeds - i + total_hits) < needed_hits)
			break;
	}

	if (total_hits >= needed_hits) {
		if (m_auto_walls_hit >= ((chance / HITCHANCE_MAX) * m_auto_walls_done))
			return true;
	}

	return false;
}

bool AimPlayer::SetupHitboxPoints(LagRecord* record, BoneArray* bones, int index, std::vector< vec3_t >& points) {
	// reset points.
	points.clear();

	const model_t* model = m_player->GetModel();
	if (!model)
		return false;

	studiohdr_t* hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return false;

	mstudiohitboxset_t* set = hdr->GetHitboxSet(m_player->m_nHitboxSet());
	if (!set)
		return false;

	mstudiobbox_t* bbox = set->GetHitbox(index);
	if (!bbox)
		return false;

	// get hitbox scales.
	float scale = g_menu.main.aimbot.scale.get() / 100.f;

	// big inair fix.
	if (!(record->m_pred_flags & FL_ONGROUND) && scale > 0.7f)
		scale = g_menu.main.aimbot.scale.get() > 0.7f ? 0.7f : g_menu.main.aimbot.scale.get() / 100.f;

	float bscale = g_menu.main.aimbot.body_scale.get() / 100.f;





	// these indexes represent boxes.
	if (bbox->m_radius <= 0.f) {
		// references: 
		//      https://developer.valvesoftware.com/wiki/Rotation_Tutorial
		//      CBaseAnimating::GetHitboxBonePosition
		//      CBaseAnimating::DrawServerHitboxes

		// convert rotation angle to a matrix.
		matrix3x4_t rot_matrix;
		g_csgo.AngleMatrix(bbox->m_angle, rot_matrix);

		// apply the rotation to the entity input space ( local ).
		matrix3x4_t matrix;
		math::ConcatTransforms(bones[bbox->m_bone], rot_matrix, matrix);

		// extract origin from matrix.
		vec3_t origin = matrix.GetOrigin();

		// compute raw center point.
		vec3_t center = (bbox->m_mins + bbox->m_maxs) / 2.f;

		// the feet hiboxes have a side, heel and the toe.
		if (index == HITBOX_R_FOOT || index == HITBOX_L_FOOT && g_menu.main.aimbot.multipoint.get(4)) {
			float d1 = (bbox->m_mins.z - center.z) * 0.875f;

			// invert.
			if (index == HITBOX_L_FOOT)
				d1 *= -1.f;

			// side is more optimal then center.
			points.push_back({ center.x, center.y, center.z + d1 });

			// get point offset relative to center point
			// and factor in hitbox scale.
			float d2 = (bbox->m_mins.x - center.x) * scale;
			float d3 = (bbox->m_maxs.x - center.x) * scale;

			// heel.
			points.push_back({ center.x + d2, center.y, center.z });

			// toe.
			points.push_back({ center.x + d3, center.y, center.z });
		}

		// nothing to do here we are done.
		if (points.empty())
			return false;

		// rotate our bbox points by their correct angle
		// and convert our points to world space.
		for (auto& p : points) {
			// VectorRotate.
			// rotate point by angle stored in matrix.
			p = { p.dot(matrix[0]), p.dot(matrix[1]), p.dot(matrix[2]) };

			// transform point to world space.
			p += origin;
		}
	}

	// these hitboxes are capsules.
	else {
		// factor in the pointscale.
		float r = bbox->m_radius * scale;
		float br = bbox->m_radius * bscale;

		// compute raw center point.
		vec3_t center = (bbox->m_mins + bbox->m_maxs) / 2.f;

		// head has 5 points.
		if (index == HITBOX_HEAD && g_menu.main.aimbot.multipoint.get(0)) {
			// add center.
			points.push_back(center);

			// rotation matrix 45 degrees.
			// https://math.stackexchange.com/questions/383321/rotating-x-y-points-45-degrees
			// std::cos( deg_to_rad( 45.f ) )
			constexpr float rotation = 0.70710678f;

			// top/back 45 deg.
			// this is the best spot to shoot at.
			points.push_back({ bbox->m_maxs.x + (rotation * r), bbox->m_maxs.y + (-rotation * r), bbox->m_maxs.z });

			// right.
			points.push_back({ bbox->m_maxs.x, bbox->m_maxs.y, bbox->m_maxs.z + r });

			// left.
			points.push_back({ bbox->m_maxs.x, bbox->m_maxs.y, bbox->m_maxs.z - r });

			// back.
			points.push_back({ bbox->m_maxs.x, bbox->m_maxs.y - r, bbox->m_maxs.z });

			// get animstate ptr.
			CCSGOPlayerAnimState* state = record->m_player->m_PlayerAnimState();

			// add this point only under really specific circumstances.
			// if we are standing still and have the lowest possible pitch pose.
			if (state && record->m_anim_velocity.length() <= 0.1f && record->m_eye_angles.x <= state->m_aim_pitch_min) {

				// bottom point.
				points.push_back({ bbox->m_maxs.x - r, bbox->m_maxs.y, bbox->m_maxs.z });
			}
		}

		// body has 4 points
		// center + back + left + right
		else if (index == HITBOX_PELVIS || index == HITBOX_UPPER_CHEST && g_menu.main.aimbot.multipoint.get(1)) {
			points.push_back({ center.x, center.y, bbox->m_maxs.z + br });
			points.push_back({ center.x, center.y, bbox->m_mins.z - br });
			points.push_back({ center.x, bbox->m_maxs.y - br, center.z });
		}
		// exact same as pelvis but inverted ( god knows what theyre doing at valve )
		else if (index == HITBOX_BODY && g_menu.main.aimbot.multipoint.get(2)) {
			points.push_back(center);

			points.push_back({ center.x, center.y, bbox->m_maxs.z - br });
			points.push_back({ center.x, center.y, bbox->m_mins.z + br });
			points.push_back({ center.x, bbox->m_maxs.y - br, center.z });
		}

		// other stomach/chest hitboxes have 3 points.
		else if (index == HITBOX_THORAX || index == HITBOX_CHEST && g_menu.main.aimbot.multipoint.get(1)) {
			// add center.
			points.push_back(center);

			// add extra point on back.
			if (!g_menu.main.aimbot.center.get()) {

				br = bbox->m_radius * (bscale * 0.9f);

				points.push_back({ center.x, center.y, bbox->m_maxs.z + br });

				points.push_back({ center.x, center.y, bbox->m_mins.z - br });

				points.push_back({ center.x, bbox->m_maxs.y - br, center.z });
			}
		}

		else if (index == HITBOX_R_CALF || index == HITBOX_L_CALF && g_menu.main.aimbot.multipoint.get(3)) {
			// add center.
			points.push_back(center);

			// half bottom.
			if (!g_menu.main.aimbot.center.get())
				points.push_back({ bbox->m_maxs.x - (bbox->m_radius / 2.f), bbox->m_maxs.y, bbox->m_maxs.z });
		}

		else if (index == HITBOX_R_THIGH || index == HITBOX_L_THIGH && g_menu.main.aimbot.multipoint.get(4)) {
			// add center.
			points.push_back(center);
		}

		// arms get only one point.
		else if (index == HITBOX_R_UPPER_ARM || index == HITBOX_L_UPPER_ARM && g_menu.main.aimbot.multipoint.get(4)) {
			// elbow.
			points.push_back({ bbox->m_maxs.x + bbox->m_radius, center.y, center.z });
		}

		// nothing left to do here.
		if (points.empty())
			return false;

		// transform capsule points.`
		for (auto& p : points)
			math::VectorTransform(p, bones[bbox->m_bone], p);
	}

	return true;
}

bool Aimbot::CanHit(const vec3_t start, const vec3_t end, LagRecord* record, int box, bool in_shot, BoneArray* bones)
{
	if (!record || !record->m_player)
		return false;

	// always try to use our aimbot matrix first.
	auto matrix = record->m_bones;

	// this is basically for using a custom matrix.
	if (in_shot)
		matrix = bones;

	if (!matrix)
		return false;

	const model_t* model = record->m_player->GetModel();
	if (!model)
		return false;

	studiohdr_t* hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return false;

	mstudiohitboxset_t* set = hdr->GetHitboxSet(record->m_player->m_nHitboxSet());
	if (!set)
		return false;

	mstudiobbox_t* bbox = set->GetHitbox(box);
	if (!bbox)
		return false;

	vec3_t min, max;
	const auto IsCapsule = bbox->m_radius != -1.f;

	if (IsCapsule) {
		math::VectorTransform(bbox->m_mins, matrix[bbox->m_bone], min);
		math::VectorTransform(bbox->m_maxs, matrix[bbox->m_bone], max);
		const auto dist = math::SegmentToSegment(start, end, min, max);

		if (dist < bbox->m_radius) {
			return true;
		}
	}
	else {
		CGameTrace tr;

		// setup ray and trace.
		g_csgo.m_engine_trace->ClipRayToEntity(Ray(start, end), CS_MASK_SHOOT, record->m_player, &tr);

		// check if we hit a valid player / hitgroup on the player and increment total hits.
		if (tr.m_entity == record->m_player && game::IsValidHitgroup(tr.m_hitgroup))
			return true;
	}

	return false;
}


/*
bool AimPlayer::SetupHitboxPoints(LagRecord* record, BoneArray* bones, int index, std::vector< vec3_t >& points) { // maxwell ty lol bruh
	// reset points.
	points.clear();

	const model_t* model = m_player->GetModel();
	if (!model)
		return false;

	studiohdr_t* hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return false;

	mstudiohitboxset_t* set = hdr->GetHitboxSet(m_player->m_nHitboxSet());
	if (!set)
		return false;

	mstudiobbox_t* bbox = set->GetHitbox(index);
	if (!bbox)
		return false;

	// get hitbox scales.
	float scale = g_menu.main.aimbot.scale.get() / 100.f;
	float bodyscale = g_menu.main.aimbot.body_scale.get() / 100.f;

	// big inair fix.
	if (!(record->m_pred_flags) & FL_ONGROUND)
		scale = 0.7f;

	float mod = bbox->m_radius != -1.f ? bbox->m_radius : 0.f;

	vec3_t max, min;
	math::VectorTransform(bbox->m_mins, bones[bbox->m_bone], min);
	math::VectorTransform(bbox->m_maxs, bones[bbox->m_bone], max);
	auto center = (min + max) / 2.f;

	points.push_back(center);

	ang_t angle;
	math::VectorAngles(center - g_cl.m_shoot_pos, angle);

	vec3_t forward;
	math::AngleVectors(angle, &forward);

	float rs = bbox->m_radius * scale;
	float bs = bbox->m_radius * bodyscale;

	vec3_t right = forward.cross(vec3_t(0, 0, 1));
	vec3_t left = vec3_t(-right.x, -right.y, right.z);

	vec3_t top = vec3_t(0, 0, 1);
	vec3_t bot = vec3_t(0, 0, -1);

	if (bbox->m_radius > 0.f) {
		//// pill.
		if (index == HITBOX_HEAD)
			points.push_back(center + top * rs);

		if (index == HITBOX_BODY || index == HITBOX_PELVIS) {
			points.push_back(center + right * bs);
			points.push_back(center + left * bs);
		}
		else {
			points.push_back(center + right * rs);
			points.push_back(center + left * rs);
		}

	}

	return true;
}
*/

bool AimPlayer::GetBestAimPosition(vec3_t& aim, float& damage, LagRecord* record) {
	bool                  done, pen;
	float                 dmg, pendmg;
	HitscanData_t         scan;
	std::vector< vec3_t > points;

	// get player hp.
	int hp = std::min(100, m_player->m_iHealth());
	//float add = g_menu.main.aimbot.minimal_damage.get() > 100 ? g_menu.main.aimbot.minimal_damage.get() - 100 : 0;

	if (g_menu.main.aimbot.delay_shot.get() && record->broke_lc())
		return false;

	if (g_cl.m_weapon_id == ZEUS) {
		dmg = pendmg = hp;
		pen = false;
	}

	else {
		pen = true;
		if (g_aimbot.m_damage_toggle) {
			dmg = pendmg = g_menu.main.aimbot.override_dmg_value.get();
		}
		else {
			dmg = g_menu.main.aimbot.minimal_damage.get();

			if (dmg >= 100)
				dmg = pendmg = hp;


		}
	}

	// write all data of this record.
	record->cache();

	// iterate hitboxes.
	for (const auto& it : m_hitboxes) {
		done = false;

		// setup points on hitbox.
		if (!SetupHitboxPoints(record, record->m_bones, it.m_index, points))
			continue;

		// iterate points on hitbox.
		for (const auto& point : points) {
			penetration::PenetrationInput_t in;

			in.m_damage = dmg;
			in.m_damage_pen = pendmg;
			in.m_can_pen = pen;
			in.m_target = m_player;
			in.m_from = g_cl.m_local;
			in.m_pos = point;

			// ignore mindmg.
			//if (it.m_mode == HitscanMode::LETHAL || it.m_mode == HitscanMode::LETHAL2)
			//	in.m_damage = in.m_damage_pen = 1.f;

			penetration::PenetrationOutput_t out;

			// we can hit p!
			if (penetration::run(&in, &out)) {

				if ((out.m_damage < dmg || out.m_damage < pendmg))
					continue;

				// prefered hitbox, just stop now.
				if ((out.m_damage > dmg || out.m_damage > pendmg) && it.m_mode == HitscanMode::PREFER)
					done = true;

				// this hitbox requires lethality to get selected, if that is the case.
				// we are done, stop now.
				else if (it.m_mode == HitscanMode::LETHAL && (out.m_damage >= hp))
					done = true;

				// this hitbox has normal selection, it needs to have more damage.
				else if (it.m_mode == HitscanMode::NORMAL) {
					// we did more damage.
					if (out.m_damage >= scan.m_damage) {
						// save new best data.
						scan.m_damage = out.m_damage;
						scan.m_pos = point;

						// if the first point is lethal
						// screw the other ones.
						if (point == points.front() && out.m_damage >= hp)
							break;
					}
				}

				// we found a preferred / lethal hitbox.
				if (done) {
					// save new best data.
					scan.m_damage = out.m_damage;
					scan.m_pos = point;
					break;
				}
			}
		}

		// ghetto break out of outer loop.
		if (done) {
			g_aimbot.m_hitbox = it.m_index;
			break;
		}
	}

	// we found something that we can damage.
	// set out vars.
	if (scan.m_damage > 0.f) {
		aim = scan.m_pos;
		damage = scan.m_damage;
		return true;
	}

	return false;
}

bool Aimbot::SelectTarget(LagRecord* record, const vec3_t& aim, float damage) {
	float dist, fov, height;
	int   hp;


	dist = (record->m_pred_origin - g_cl.m_shoot_pos).length();

	if (dist < m_best_dist) {
		m_best_dist = dist;
		return true;
	}

	return false;
}



void Aimbot::apply() {
	bool attack, attack2;

	// attack states.
	attack = (g_cl.m_cmd->m_buttons & IN_ATTACK);
	attack2 = (g_cl.m_weapon_id == REVOLVER && g_cl.m_cmd->m_buttons & IN_ATTACK2);

	// ensure we're attacking.
	if (attack || attack2) {
		// choke every shot.
		*g_cl.m_packet = false;

		if (m_target) {
			// make sure to aim at un-interpolated data.
			// do this so BacktrackEntity selects the exact record.
			if (m_record)
				g_cl.m_cmd->m_tick = game::TIME_TO_TICKS(m_record->m_sim_time + g_cl.m_lerp);

			// set angles to target.
			g_cl.m_cmd->m_view_angles = m_angle;

			// if not silent aim, apply the viewangles.
			if (!g_menu.main.aimbot.silent.get())
				g_csgo.m_engine->SetViewAngles(m_angle);

		}

		// nospread.
		if (g_menu.main.aimbot.nospread.get())
			NoSpread();

		// norecoil.
		if (g_menu.main.aimbot.norecoil.get())
			g_cl.m_cmd->m_view_angles -= g_cl.m_local->m_aimPunchAngle() * g_csgo.weapon_recoil_scale->GetFloat();

		// store fired shot.
		g_shots.OnShotFire(m_target ? m_target : nullptr, m_target ? m_damage : -1.f, g_cl.m_weapon_info->m_bullets, m_target ? m_record : nullptr, m_hitbox, m_hitgroup);

		// set that we fired.
		g_cl.m_shot = true;
	}
}

void Aimbot::NoSpread() {
	bool    attack2;
	vec3_t  spread, forward, right, up, dir;

	// revolver state.
	attack2 = (g_cl.m_weapon_id == REVOLVER && (g_cl.m_cmd->m_buttons & IN_ATTACK2));

	// get spread.
	spread = g_cl.m_weapon->CalculateSpread(g_cl.m_cmd->m_random_seed, attack2);

	// compensate.
	g_cl.m_cmd->m_view_angles -= { -math::rad_to_deg(std::atan(spread.length_2d())), 0.f, math::rad_to_deg(std::atan2(spread.x, spread.y)) };
}