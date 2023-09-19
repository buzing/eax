#include "includes.h"


#include <urlmon.h>
#include <mmsystem.h>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment( lib, "Iphlpapi.lib" )

Shots g_shots{ };

std::string TranslateResolverMode(int iMode) {
	/*
	RESOLVE_NONE = 0,
		RESOLVE_WALK,
		RESOLVE_STAND,
		RESOLVE_STAND1,
		RESOLVE_STAND2,
		RESOLVE_AIR,
		RESOLVE_BODY,
		RESOLVE_SAFE,
		RESOLVE_STOPPED_MOVING,
		RESOLVE_OVERRIDE,
		RESOLVE_LASTMOVE,
		RESOLVE_UNKNOWM,
		RESOLVE_BRUTEFORCE,
	*/
	switch (iMode) {
	case Resolver::Modes::RESOLVE_NONE:
		return "none";
		break;
	case Resolver::Modes::RESOLVE_WALK:
		return "walk";
		break;
	case Resolver::Modes::RESOLVE_AIR:
		return "airbrute";
		break;
	case Resolver::Modes::RESOLVE_AIR_BACK:
		return "airback";
		break;
	case Resolver::Modes::RESOLVE_AIR_LBY:
		return "air lastmove";
		break;
	case Resolver::Modes::RESOLVE_STAND:
		return "stand";
		break;
	case Resolver::Modes::RESOLVE_STAND2:
		return "stand2";
		break;
	case Resolver::Modes::RESOLVE_BACK:
		return "back";
		break;
	case Resolver::Modes::RESOLVE_BODY:
		return "lby";
		break;
	case Resolver::Modes::RESOLVE_UNSAFELBY:
		return "unsafe lby";
		break;
	case Resolver::Modes::RESOLVE_UNSAFELM:
		return "unsafe lastmove";
		break;
	case Resolver::Modes::RESOLVE_SEXBACK:
		return "safe 165 back";
		break;
	case Resolver::Modes::RESOLVE_SBACK:
		return "safe 180 back";
		break;
	case Resolver::Modes::RESOLVE_SFS:
		return "safe rfs";
		break;
	case Resolver::Modes::RESOLVE_LBY:
		return "lby";
		break;
	case Resolver::Modes::RESOLVE_NETWORK:
		return "networked";
		break;
	case Resolver::Modes::RESOLVE_EXBACK:
		return "165 back";
		break;
	case Resolver::Modes::RESOLVE_REVERSEFS:
		return "freestand";
		break;
	case Resolver::Modes::RESOLVE_FAKEWALK:
		return "fakewalk";
		break;
	case Resolver::Modes::RESOLVE_LASTMOVE:
		return "lastmoving lby";
		break;
	case Resolver::Modes::RESOLVE_BRUTEFORCE:
		return "bruteforce";
		break;
	case Resolver::Modes::RESOLVE_OVERRIDE:
		return "supermansolver";
		break;
	case Resolver::Modes::RESOLVE_LAST_UPD_LBY:
		return "last_upd_lby";
		break;
	default:
		return "unk";
		break;
	}
}


void Shots::OnShotFire(Player* target, float damage, int bullets, LagRecord* record, int hitbox, int hitgroup) {

	// iterate all bullets in this shot.
	for (int i{ }; i < bullets; ++i) {
		// setup new shot data.
		ShotRecord shot;
		shot.m_target = target;
		shot.m_record = record;
		shot.m_time = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());
		shot.m_lat = g_cl.m_latency;
		shot.m_damage = damage;
		shot.m_pos = g_cl.m_local->GetShootPosition();
		shot.m_pos = g_cl.m_shoot_pos;
		shot.m_hurt = false;
		shot.m_impacted = false;
		shot.m_matched = false;
		shot.m_range = g_cl.m_weapon_info->m_range;
		shot.m_hitgroup = hitgroup;

		// we are not shooting manually.
		// and this is the first bullet, only do this once.
		if (target && record) {
			// increment total shots on this player.
			AimPlayer* data = &g_aimbot.m_players[target->index() - 1];
			const int history_ticks = game::TIME_TO_TICKS(target->m_flSimulationTime() - record->m_player->m_flSimulationTime());
			player_info_t info;
			g_csgo.m_engine->GetPlayerInfo(target->index(), &info);
			shot.m_had_pred_error = history_ticks < 0;
			if (data)
				++data->m_shots;
			//	g_notify.add(tfm::format("n: %s[%i], m: %s, bt: %i,  p: %s\n", info.m_name, target->index(), TranslateResolverMode(record->m_mode), game::TIME_TO_TICKS(data->m_records.front().get()->m_sim_time - record->m_sim_time), m_groups[hitgroup]));
		}

		// add to tracks.
		m_shots.push_front(shot);
	}

	// no need to keep an insane amount of shots.
	while (m_shots.size() > 128)
		m_shots.pop_back();
}

void Shots::OnImpact(IGameEvent* evt) {
	int        attacker;
	vec3_t     pos, dir, start, end;
	float      time;
	CGameTrace trace;

	// screw this.
	if (!evt)
		return;

	// get attacker, if its not us, screw it.
	attacker = g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	//if ( attacker != g_csgo.m_engine->GetLocalPlayer( ) )
	//	return;
	// 
	// decode impact coordinates and convert to vec3.
	pos = {
		evt->m_keys->FindKey(HASH("x"))->GetFloat(),
		evt->m_keys->FindKey(HASH("y"))->GetFloat(),
		evt->m_keys->FindKey(HASH("z"))->GetFloat()
	};

	// get prediction time at this point.
	time = g_csgo.m_globals->m_curtime;

	// add to visual impacts if we have features that rely on it enabled.
	// todo - dex; need to match shots for this to have proper GetShootPosition, don't really care to do it anymore.


	if (!g_cl.m_local)
		return;



	// we did not take a shot yet.
	if (m_shots.empty())
		return;

	struct ShotMatch_t { float delta; ShotRecord* shot; };
	ShotMatch_t match;
	match.delta = std::numeric_limits< float >::max();
	match.shot = nullptr;

	// iterate all shots.
	for (auto& s : m_shots) {

		// this shot was already matched
		// with a 'bullet_impact' event.
		if (s.m_impacted)
			continue;

		// add the latency to the time when we shot.
		// to predict when we would receive this event.
		float predicted = s.m_time + s.m_lat;

		// get the delta between the current time
		// and the predicted arrival time of the shot.
		float delta = std::abs(time - predicted);

		// fuck this.
		if (delta > 1.f)
			continue;

		// store this shot as being the best for now.
		if (delta < match.delta) {
			match.delta = delta;
			match.shot = &s;
		}
	}

	// no valid shotrecord was found.
	ShotRecord* shot = match.shot;
	if (!shot)
		return;

	// this shot was matched.
	shot->m_impacted = true;
	shot->m_impact_pos = pos;
}

void Shots::OnHurt(IGameEvent* evt) {
	int         attacker, victim, group, hp;
	float       damage, time;
	std::string name;

	if (!evt || !g_cl.m_local)
		return;

	attacker = g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("attacker"))->GetInt());
	victim = g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());

	// skip invalid player indexes.
	// should never happen? world entity could be attacker, or a nade that hits you.
	if (attacker < 1 || attacker > 64 || victim < 1 || victim > 64)
		return;

	// we were not the attacker or we hurt ourselves.
	else if (attacker != g_csgo.m_engine->GetLocalPlayer() || victim == g_csgo.m_engine->GetLocalPlayer())
		return;

	// get hitgroup.
	// players that get naded ( DMG_BLAST ) or stabbed seem to be put as HITGROUP_GENERIC.
	group = evt->m_keys->FindKey(HASH("hitgroup"))->GetInt();

	// invalid hitgroups ( note - dex; HITGROUP_GEAR isn't really invalid, seems to be set for hands and stuff? ).
	if (group == HITGROUP_GEAR)
		return;

	// get the player that was hurt.
	Player* target = g_csgo.m_entlist->GetClientEntity< Player* >(victim);
	if (!target)
		return;

	// get player info.
	player_info_t info;
	if (!g_csgo.m_engine->GetPlayerInfo(victim, &info))
		return;

	// get player name;
	name = std::string(info.m_name).substr(0, 24);

	// get damage reported by the server.
	damage = (float)evt->m_keys->FindKey(HASH("dmg_health"))->GetInt();

	// get remaining hp.
	hp = evt->m_keys->FindKey(HASH("health"))->GetInt();

	// get prediction time at this point.
	time = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());

	// setup headshot marker
	if (group == HITGROUP_HEAD)
		iHeadshot = true;
	else
		iHeadshot = false;

	// hitmarker.
	if (g_menu.main.players.hitmarker.get()) {
		g_visuals.m_hit_duration = 1.f;
		g_visuals.m_hit_start = g_csgo.m_globals->m_curtime;
		g_visuals.m_hit_end = g_visuals.m_hit_start + g_visuals.m_hit_duration;

		// bind to draw
		iHitDmg = damage;

		// get interpolated origin.
		iPlayerOrigin = target->GetAbsOrigin();

		// get hitbox bounds.
		// hehe boy
		target->ComputeHitboxSurroundingBox(&iPlayermins, &iPlayermaxs);

		// correct x and y coordinates.
		iPlayermins = { iPlayerOrigin.x, iPlayerOrigin.y, iPlayermins.z };
		iPlayermaxs = { iPlayerOrigin.x, iPlayerOrigin.y, iPlayermaxs.z + 8.f };
	}

	auto modify_volume = [&](BYTE* bytes) {
		int offset = 0;
		for (int i = 0; i < sizeof(bytes) / 2; i++) {
			if (bytes[i] == 'd' && bytes[i + 1] == 'a'
				&& bytes[i + 2] == 't' && bytes[i + 3] == 'a')
			{
				offset = i;
				break;
			}
		}

		if (!offset)
			return;

		BYTE* pDataOffset = (bytes + offset);
		DWORD dwNumSampleBytes = *(DWORD*)(pDataOffset + 4);
		DWORD dwNumSamples = dwNumSampleBytes / 2;

		SHORT* pSample = (SHORT*)(pDataOffset + 8);
		for (DWORD dwIndex = 0; dwIndex < dwNumSamples; dwIndex++)
		{
			SHORT shSample = *pSample;
			shSample = (SHORT)(shSample * 0.09);
			*pSample = shSample;
			pSample++;
			if (((BYTE*)pSample) >= (bytes + sizeof(bytes) - 1))
				break;
		}
	};

	
	g_csgo.m_sound->EmitAmbientSound(XOR("buttons/arena_switch_press_02.wav"), 2.f);
		

	if (group == HITGROUP_GENERIC)
		return;

	// if we hit a player, mark vis impacts.
	if (!m_vis_impacts.empty()) {
		for (auto& i : m_vis_impacts) {
			if (i.m_tickbase == g_cl.m_local->m_nTickBase())
				i.m_hit_player = true;
		}
	}

	struct ShotMatch_t { float delta; ShotRecord* shot; };
	ShotMatch_t match;
	match.delta = std::numeric_limits< float >::max();
	match.shot = nullptr;

	// iterate all shots.
	for (auto& s : m_shots) {

		// this shot was already matched
		// with a 'player_hurt' event.
		if (s.m_hurt)
			continue;

		// add the latency to the time when we shot.
		// to predict when we would receive this event.
		float predicted = s.m_time + s.m_lat;

		// get the delta between the current time
		// and the predicted arrival time of the shot.
		float delta = std::abs(time - predicted);

		// fuck this.
		if (delta > 1.f)
			continue;

		// store this shot as being the best for now.
		if (delta < match.delta) {
			match.delta = delta;
			match.shot = &s;
		}
	}

	// no valid shotrecord was found.
	ShotRecord* shot = match.shot;
	if (!shot)
		return;

	
	std::string out = tfm::format(XOR("name: %s in: %s dmg: %i (%i ramins)\n"), name, m_groups[group], (int)damage, hp);
	g_notify.add(out);
	

	// this shot was matched.
	shot->m_hurt = true;
}

void Shots::OnWeaponFire(IGameEvent* evt) {
	int        attacker;

	// screw this.
	if (!evt || !g_cl.m_local)
		return;

	// get attacker, if its not us, screw it.
	attacker = g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (attacker != g_csgo.m_engine->GetLocalPlayer())
		return;

	struct ShotMatch_t { float delta; ShotRecord* shot; };
	ShotMatch_t match;
	match.delta = std::numeric_limits< float >::max();
	match.shot = nullptr;

	// iterate all shots.
	for (auto& s : m_shots) {

		// this shot was already matched
		// with a 'weapon_fire' event.
		if (s.m_matched)
			continue;

		// add the latency to the time when we shot.
		// to predict when we would receive this event.
		float predicted = s.m_time + s.m_lat;

		// get the delta between the current time
		// and the predicted arrival time of the shot.
		float delta = std::abs(g_csgo.m_globals->m_curtime - predicted);

		// fuck this.
		if (delta > 1.f)
			continue;

		// store this shot as being the best for now.
		if (delta < match.delta) {
			match.delta = delta;
			match.shot = &s;
		}
	}

	// no valid shotrecord was found.
	ShotRecord* shot = match.shot;
	if (!shot)
		return;

	// this shot was matched.
	shot->m_matched = true;
	shot->m_record_valid = shot->m_record->valid();
}

void Shots::OnShotMiss(ShotRecord& shot) {
	vec3_t     pos, dir, start, end;
	CGameTrace trace;

	// shots we fire manually won't have a record.
	if (!shot.m_record)
		return;

	// nospread mode.
	if (g_menu.main.aimbot.nospread.get())
		return;

	// not in nospread mode, see if the shot missed due to spread.
	Player* target = shot.m_target;
	if (!target)
		return;

	// not gonna bother anymore.
	if (!target->alive())
		return;

	AimPlayer* data = &g_aimbot.m_players[target->index() - 1];
	if (!data)
		return;

	// this record was deleted already.
	if (!shot.m_record->m_bones)
		return;

	// we are going to alter this player.
	// store all his og data.
	BackupRecord backup;
	backup.store(target);

	// write historical matrix of the time that we shot
	// into the games bone cache, so we can trace against it.
	shot.m_record->cache();

	// start position of trace is where we took the shot.
	start = shot.m_pos;

	// where our shot landed at.
	pos = shot.m_impact_pos;

	// the impact pos contains the spread from the server
	// which is generated with the server seed, so this is where the bullet
	// actually went, compute the direction of this from where the shot landed
	// and from where we actually took the shot.
	dir = (pos - start).normalized();

	// get end pos by extending direction forward.
	end = start + (dir * shot.m_range);

	// intersect our historical matrix with the path the shot took.
	g_csgo.m_engine_trace->ClipRayToEntity(Ray(start, end), MASK_SHOT, target, &trace);

	bool is_resolver_issue = g_aimbot.CanHit(start, end, shot.m_record, shot.m_hitgroup, true, shot.m_record->m_bones);

	// handle miss logs here
	const float point_dist = shot.m_pos.dist_to(end);
	const float impact_dist = shot.m_pos.dist_to(vec3_t(g_visuals.m_impacts.back().x, g_visuals.m_impacts.back().y, g_visuals.m_impacts.back().z));
	// not gonna bother anymore.
	if (!target->alive()) {
		g_notify.add(XOR("missed shot due to neverlose LEL\n"));
		return;
	}


	
	if (!trace.m_entity || !trace.m_entity->IsPlayer() || trace.m_entity != target && g_cl.m_local->GetActiveWeapon()->GetInaccuracy() > 0.005f) {
		g_notify.add(XOR("Missed shot due to crack\n"));
	}
	else if (shot.m_aim_point.dist_to(pos) < trace.m_endpos.dist_to(start) /*point_dist > impact_dist*/) {
		g_notify.add(XOR("Missed shot due to ecstasy\n"));
	}
	else if (shot.m_had_pred_error)
	{
		g_notify.add(XOR("Missed shot due to weed error\n"));
	}
	else if ( /*is_resolver_issue*/ trace.m_entity == target && shot.m_record->m_mode != Resolver::Modes::RESOLVE_NONE) {
		// we should have 100% hit this player..
		// this is a miss due to wrong angles.
		size_t mode = shot.m_record->m_mode;
		if (mode == Resolver::Modes::RESOLVE_BODY) {
			++data->m_body_index;
		}

		else if (mode == Resolver::Modes::RESOLVE_SEXBACK)
		{
			++data->m_sexback_idx;
		}

		else if (mode == Resolver::Modes::RESOLVE_SBACK)
		{
			++data->m_sback_idx;
		}

		else if (mode == Resolver::Modes::RESOLVE_SFS)
		{
			++data->m_sfs_idx;
		}

		else if (mode == Resolver::Modes::RESOLVE_STAND) {
			++data->m_stand_index;
		}

		else if (mode == Resolver::Modes::RESOLVE_STAND2) {
			++data->m_stand_index2;
		}

		else if (mode == Resolver::Modes::RESOLVE_BACK)
		{
			++data->m_backwards_idx;
		}

		else if (mode == Resolver::Modes::RESOLVE_STAND3)
		{
			++data->m_stand_index3;
		}

		else if (mode == Resolver::Modes::RESOLVE_LBY)
		{
			++data->m_lby_idx;
		}


		else if (mode == Resolver::Modes::RESOLVE_AIR)
		{
			++data->m_air_brute_index;
		}

		else if (mode == Resolver::Modes::RESOLVE_REVERSEFS) {
			++data->m_reverse_fs;
		}

		else if (mode == Resolver::Modes::RESOLVE_FAKEWALK) {
			++data->m_fake_walk_idx;
		}

		else if (mode == Resolver::Modes::RESOLVE_EXBACK) {
			++data->m_exback_idx;
		}

		else if (mode == Resolver::Modes::RESOLVE_LASTMOVE)
		{
			++data->m_lastmove_idx;
		}
		++data->m_missed_shots;

		player_info_t info;
		g_csgo.m_engine->GetPlayerInfo(shot.m_target->index(), &info);

		
			if (mode != Resolver::Modes::RESOLVE_BODY && !g_resolver.iPlayers[shot.m_record->m_player->index()])
			{
				g_notify.add(tfm::format("Missed shot due to paysus\n"));
			}
			else {
				// print resolver debug.
				g_notify.add(tfm::format("Missed shot due to grass\n"));
			}
	}

	// restore player to his original state.
	backup.restore(target);
}


void Shots::Think() {
	if (!g_cl.m_processing || m_shots.empty()) {
		// we're dead, we won't need this data anymore.
		if (!m_shots.empty())
			m_shots.clear();

		// we don't handle shots if we're dead or if there are none to handle.
		return;
	}

	// iterate all shots.
	for (auto it = m_shots.begin(); it != m_shots.end(); ) {
		// too much time has passed, we don't need this anymore.
		if (it->m_time + 1.f < g_csgo.m_globals->m_curtime)
			// remove it.
			it = m_shots.erase(it);
		else
			it = next(it);
	}

	// iterate all shots.
	for (auto it = m_shots.begin(); it != m_shots.end(); ) {
		// our shot impacted, and it was confirmed, but we didn't damage anyone. we missed.
		if (it->m_matched && it->m_impacted) {

			if( !it->m_hurt ) {
				// handle the shot.
				OnShotMiss(*it);
			}
			// since we've handled this shot, we won't need it anymore.
			it = m_shots.erase(it);
		}
		else
			it = next(it);
	}
}