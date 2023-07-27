#pragma once
#include <optional>

// pre-declare.
class LagRecord;

class BackupRecord {
public:
	BoneArray* m_bones;
	int        m_bone_count;
	vec3_t     m_origin, m_abs_origin;
	vec3_t     m_mins;
	vec3_t     m_maxs;
	ang_t      m_abs_ang;

public:
	__forceinline void store( Player* player ) {
		// get bone cache ptr.
		CBoneCache* cache = &player->m_BoneCache( );

		// copy memory over to this record
		std::memcpy(m_bones, player->bone_cache1().m_pCachedBones, sizeof(matrix3x4_t) * m_bone_count);


		m_origin     = player->m_vecOrigin( );
		m_mins       = player->m_vecMins( );
		m_maxs       = player->m_vecMaxs( );
		m_abs_origin = player->GetAbsOrigin( );
		m_abs_ang    = player->GetAbsAngles( );
	}

	__forceinline void restore( Player* player ) {
		// get bone cache ptr.
		CBoneCache* cache = &player->m_BoneCache( );

		// copy over bone memory.
		std::memcpy(player->bone_cache1().m_pCachedBones, m_bones, sizeof(matrix3x4_t) * m_bone_count);

		player->m_vecOrigin( ) = m_origin;
		player->m_vecMins( )   = m_mins;
		player->m_vecMaxs( )   = m_maxs;
		player->SetAbsAngles( m_abs_ang );
		player->SetAbsOrigin( m_origin );
	}
};

class LagRecord {
public:
	// data.
	Player* m_player;
	float   m_immune;
	int     m_tick;
	int     m_lag;
	float   m_lag_time;
	bool    m_dormant;
	int m_shift;

	// netvars.
	float  m_sim_time;
	float  m_old_sim_time;
	int    m_flags;
	vec3_t m_origin;
	vec3_t m_old_origin;
	vec3_t m_velocity;
	vec3_t m_mins;
	vec3_t m_maxs;
	float m_new_body;
	ang_t  m_eye_angles;
	ang_t  m_abs_ang;
	float  m_body;
	float  m_duck;
	std::string m_resolver_mode;
	std::string m_resolver_mode2;
	Color m_resolver_color;
	bool  resolved;

	// anim stuff.
	C_AnimationLayer m_layers[ 13 ];
	float            m_poses[ 24 ];
	vec3_t           m_anim_velocity;

	// bone stuff.
	bool       m_setup;
	BoneArray* m_bones;

	// lagfix stuff.
	bool   m_broke_lc;
	vec3_t m_pred_origin;
	vec3_t m_pred_velocity;
	float  m_pred_time;
	int    m_pred_flags;
	float m_feet_yaw;
	std::optional < bool > m_unfixed_on_ground{};
	bool   m_ground;
	float m_valid_time;

	// resolver stuff.
	size_t m_mode;
	bool   m_shot;
	float  m_away;
	float  m_anim_time;
	C_AnimationLayer m_server_layers[13];
	float  m_animation_speed, m_max_speed;
	bool   m_fake_walk, m_fake_flick;
	
	float	m_foot_yaw{}, m_move_yaw{};
	float m_move_yaw_cur_to_ideal{};
	float m_move_yaw_ideal{};
	float m_move_weight_smoothed{};

	// other stuff.
	float  m_interp_time;
	bool   m_sideway;
	bool   m_invalid;
	float  m_choke_time;
	Weapon* m_weapon;
	float m_shot_time;
	float m_act_time{ };

public:

	// default ctor.
	__forceinline LagRecord() :
		m_setup{ false },
		m_broke_lc{ false },
		m_fake_walk{ false },
		m_sideway{ false },
		m_shot{ false },
		m_lag{},
		m_lag_time{},
		m_bones{} {}

	// ctor.
	__forceinline LagRecord(Player* player) :
		m_setup{ false },
		m_broke_lc{ false },
		m_fake_walk{ false },
		m_sideway{ false },
		m_shot{ false },
		m_lag{},
		m_lag_time{},
		m_bones{} {

		store(player);
	}

	// dtor.
	__forceinline ~LagRecord() {
		// free heap allocated game mem.
		g_csgo.m_mem_alloc->Free(m_bones);
	}

	__forceinline void invalidate() {
		// free heap allocated game mem.
		g_csgo.m_mem_alloc->Free(m_bones);

		// mark as not setup.
		m_setup = false;

		// allocate new memory.
		m_bones = (BoneArray*)g_csgo.m_mem_alloc->Alloc(sizeof(BoneArray) * 128);
	}

	// function: allocates memory for SetupBones and stores relevant data.
	void store(Player* player) {
		// allocate game heap.
		m_bones = (BoneArray*)g_csgo.m_mem_alloc->Alloc(sizeof(BoneArray) * 128);

		// player data.
		m_player = player;
		m_immune = player->m_fImmuneToGunGameDamageTime();
		m_tick = g_csgo.m_cl->m_server_tick;

		// netvars.
		m_pred_time = m_sim_time = player->m_flSimulationTime();
		m_old_sim_time = player->m_flOldSimulationTime();
		m_pred_flags = m_flags = player->m_fFlags();
		m_pred_origin = m_origin = player->m_vecOrigin();
		m_old_origin = player->m_vecOldOrigin();
		m_eye_angles = player->m_angEyeAngles();
		m_abs_ang = player->GetAbsAngles();
		m_body = player->m_flLowerBodyYawTarget();
		m_mins = player->m_vecMins();
		m_maxs = player->m_vecMaxs();
		m_duck = player->m_flDuckAmount();
		m_pred_velocity = m_velocity = m_anim_velocity = player->m_vecVelocity();
		m_weapon = player->GetActiveWeapon ( );
		m_shot_time = m_weapon ? m_weapon->m_fLastShotTime( ) : 0.f;

		// save networked animlayers.
		player->GetAnimLayers(m_layers);

		// normalize eye angles.
		m_eye_angles.normalize();
		math::clamp(m_eye_angles.x, -90.f, 90.f);

		// get lag.
		m_lag_time = m_sim_time - m_old_sim_time; // lagcomp time, its just so we got smth incase it goes wrong
		m_lag = game::TIME_TO_TICKS(m_sim_time - m_old_sim_time);

		// compute animtime.
		m_anim_time = m_old_sim_time + g_csgo.m_globals->m_interval;

		if( const auto anim_state = player->m_PlayerAnimState( ) )
			m_foot_yaw = anim_state->m_foot_yaw;
	}

	// function: restores 'predicted' variables to their original.
	__forceinline void predict() {
		m_broke_lc = broke_lc();
		m_pred_origin = m_origin;
		m_pred_velocity = m_velocity;
		m_pred_time = m_sim_time;
		m_pred_flags = m_flags;
	}

	// function: writes current record to bone cache.
	__forceinline void cache() {
		// get bone cache ptr.
		CBoneCache* cache = &m_player->m_BoneCache();

		if (m_setup)
			memcpy(cache->m_pCachedBones, m_bones, cache->m_CachedBoneCount * sizeof(matrix3x4_t));

		m_player->m_vecOrigin() = m_pred_origin;
		m_player->m_vecMins() = m_mins;
		m_player->m_vecMaxs() = m_maxs;

		m_player->SetAbsAngles(m_abs_ang);
		m_player->SetAbsOrigin(m_pred_origin);
	}

	__forceinline bool dormant() {
		return m_dormant;
	}

	__forceinline bool immune() {
		return m_immune > 0.f;
	}

	__forceinline bool broke_lc() {
		return (this->m_origin - this->m_old_origin).length_2d_sqr() > 4096.f;
	}

	__forceinline void simulate( LagRecord* previous, Player* entry ) {
		const auto& cur_alive_loop_layer = m_layers[ 11 ];

		/* es0 should touch some grass */
		if( !previous
			|| previous->m_dormant ) {
			if( !previous ) {
				if( ( m_flags & FL_ONGROUND ) ) {
					auto max_speed = m_weapon ? std::max( 0.1f, m_weapon->max_speed( entry->m_bIsScoped( ) ) ) : 260.f;

					if( m_layers[ 6 ].m_weight > 0.f && m_layers[ 6 ].m_playback_rate > 0.f
						&& m_anim_velocity.length( ) > 0.f ) {
						if( ( m_flags & FL_DUCKING ) )
							max_speed *= 0.34f;
						else if( entry->m_bIsWalking( ) )
							max_speed *= 0.52f;

						const auto move_multiplier = m_layers[ 6 ].m_weight * max_speed;
						const auto speed_multiplier = move_multiplier / m_anim_velocity.length( );

						m_anim_velocity.x *= speed_multiplier;
						m_anim_velocity.y *= speed_multiplier;
					}
				}
			}

			return;
		}
 
		/* recalculating simulation time via 11th layer, s/o eso */
		if( m_weapon == previous->m_weapon
			&& m_layers[ 11 ].m_playback_rate > 0.f && previous->m_layers[ 11 ].m_playback_rate > 0.f ) {

			const auto cur_11th_cycle = cur_alive_loop_layer.m_cycle;
			auto prev_11th_cycle = previous->m_layers[ 11 ].m_cycle;

			const auto cycles_delta = cur_11th_cycle - prev_11th_cycle;

			if( cycles_delta != 0.f ) {
				const auto sim_ticks_delta = game::TIME_TO_TICKS( m_sim_time - m_old_sim_time );

				if( sim_ticks_delta != 1 ) {
					auto prev_11th_rate = previous->m_layers[ 11 ].m_playback_rate;
					std::ptrdiff_t resimulated_sim_ticks{};

					if( cycles_delta >= 0.f ) {
						resimulated_sim_ticks = game::TIME_TO_TICKS( cycles_delta / prev_11th_rate );
					}
					else {
						std::ptrdiff_t ticks_iterated{};
						float cur_simulated_cycle{ 0.f };
						while ( true ) {
							++resimulated_sim_ticks;

							if( ( prev_11th_rate * g_csgo.m_globals->m_interval ) + prev_11th_cycle >= 1.f )
								prev_11th_rate = m_layers[ 11 ].m_playback_rate;

							cur_simulated_cycle = ( prev_11th_rate * g_csgo.m_globals->m_interval ) + prev_11th_cycle;
							prev_11th_cycle = cur_simulated_cycle;
							if( cur_simulated_cycle > 0.f )
								break;

							if( ++ticks_iterated >= 16 ) {
								goto leave_cycle;
							}
						}

						const auto first_val = prev_11th_cycle - cur_simulated_cycle;

						auto recalc_everything = cur_11th_cycle - first_val;
						recalc_everything /= m_layers[ 11 ].m_playback_rate;
						recalc_everything /= g_csgo.m_globals->m_interval;
						recalc_everything += 0.5f;

						resimulated_sim_ticks += recalc_everything;
					}

				leave_cycle:
					if( resimulated_sim_ticks < sim_ticks_delta ) {
						if( resimulated_sim_ticks 
								&& g_csgo.m_cl->m_server_tick - game::TIME_TO_TICKS( m_sim_time ) == resimulated_sim_ticks ) {
							entry->m_flSimulationTime( ) = m_sim_time = ( resimulated_sim_ticks * g_csgo.m_globals->m_interval ) + m_old_sim_time;
						}
					}
				}
			}
		}

		if( previous ) {
			const auto& prev_alive_loop_layer = previous->m_layers[ 11 ];

			auto sim_ticks = game::TIME_TO_TICKS( m_sim_time - previous->m_sim_time );

			if( ( sim_ticks - 1 > 31  || previous->m_sim_time == 0.f ) ) {
				sim_ticks = 1;
			}

			auto cur_cycle = m_layers[ 11 ].m_cycle;
			auto prev_rate = previous->m_layers[ 11 ].m_playback_rate;

			if( prev_rate > 0.f &&
				m_layers[ 11 ].m_playback_rate > 0.f &&
				m_weapon == previous->m_weapon ) {
				auto prev_cycle = previous->m_layers[ 11 ].m_cycle;
				sim_ticks = 0;

				if( prev_cycle > cur_cycle )
					cur_cycle += 1.f;

				while ( cur_cycle > prev_cycle ) {
					const auto last_cmds = sim_ticks;

					const auto next_rate = g_csgo.m_globals->m_interval * prev_rate;
					prev_cycle += g_csgo.m_globals->m_interval * prev_rate;

					if( prev_cycle >= 1.f )
						prev_rate = m_layers[ 11 ].m_playback_rate;

					++sim_ticks;

					if( prev_cycle > cur_cycle && ( prev_cycle - cur_cycle ) > ( next_rate * 0.5f ) )
						sim_ticks = last_cmds;
				}
			}

			m_lag = std::clamp( sim_ticks, 0, 17 );

			if( m_lag >= 2 ) {
				auto origin_diff = m_origin - previous->m_origin;

				if( !( previous->m_flags & FL_ONGROUND ) || ( m_flags & FL_ONGROUND ) ) {
					const auto is_ducking = m_flags & FL_DUCKING;

					if( ( previous->m_flags & FL_DUCKING ) != is_ducking ) {
						float duck_mod{};

						if( is_ducking )
							duck_mod = 9.f;
						else
							duck_mod = -9.f;

						origin_diff.z -= duck_mod;
					}
				}

				const auto total_cmds_time = game::TICKS_TO_TIME( m_lag );
				if( total_cmds_time > 0.f
					&& total_cmds_time < 1.f )
					m_anim_velocity = origin_diff * ( 1.f / total_cmds_time );

				m_anim_velocity.validate_vec( );
			}
		}

		if( m_weapon
			&& m_shot_time > ( m_sim_time - game::TICKS_TO_TIME( m_lag ) )
			&& m_sim_time >= m_shot_time )
			m_shot = true;
	
		/* s/o onetap, not to mention that it looks like they pasted it from skeet lol */
		if( m_lag >= 2 ) {		
			if( ( m_flags & FL_ONGROUND )
				&& ( !previous || ( previous->m_flags & FL_ONGROUND ) ) ) {
				if( m_layers[ 6 ].m_playback_rate == 0.f )
					m_anim_velocity = {};
				else {
					if( m_weapon == previous->m_weapon ) {				
	    				if( cur_alive_loop_layer.m_weight > 0.f && cur_alive_loop_layer.m_weight < 1.f ) {
							const auto speed_2d = m_anim_velocity.length( );
							const auto max_speed = m_weapon ? std::max( 0.1f, m_weapon->max_speed( m_player->m_bIsScoped( ) ) ) : 260.f;
							if( speed_2d ) {
								const auto reversed_val = ( 1.f - cur_alive_loop_layer.m_weight ) * 0.35f;

								if( reversed_val > 0.f && reversed_val < 1.f ) {
									const auto speed_as_portion_of_run_top_speed = ( ( reversed_val + 0.55f ) * max_speed ) / speed_2d;
									if( speed_as_portion_of_run_top_speed ) {
										m_anim_velocity.x *= speed_as_portion_of_run_top_speed;
										m_anim_velocity.y *= speed_as_portion_of_run_top_speed;
									}
								}
							}
						}
					}
				}
			}

			/* pasta from onepasta but its pasted from skeet :| */
			if( ( m_flags & FL_ONGROUND )
				&& ( previous->m_flags & FL_ONGROUND ) ) {
				if( m_layers[ 6 ].m_playback_rate == 0.f )
					m_anim_velocity = {};
				else {
					if( m_layers[ 6 ].m_weight >= 0.1f ) {
						if( ( cur_alive_loop_layer.m_weight <= 0.f || cur_alive_loop_layer.m_weight >= 1.f )
							&& m_anim_velocity.length( ) > 0.f ) {
							const bool valid_6th_layer = ( m_layers[ 6 ].m_weight < 1.f )
								&& ( m_layers[ 6 ].m_weight >= previous->m_layers[ 6 ].m_weight );
							const auto max_spd = m_weapon ? std::max( 0.1f, m_weapon->max_speed( entry->m_bIsScoped( ) ) ) : 260.f;

							if( valid_6th_layer ) {
								const auto max_duck_speed = max_spd * 0.34f;
								const auto speed_multiplier = std::max( 0.f, ( max_spd * 0.52f ) - ( max_spd * 0.34f ) );
								const auto duck_modifier = 1.f - m_duck;

								const auto speed_via_6th_layer = ( ( ( duck_modifier * speed_multiplier ) + max_duck_speed ) * m_layers[ 6 ].m_weight ) / m_anim_velocity.length( );

								m_anim_velocity.x *= speed_via_6th_layer;
								m_anim_velocity.y *= speed_via_6th_layer;
							}
						}
					}
					else {
						if( m_layers[ 6 ].m_weight ) {
							auto weight = m_layers[ 6 ].m_weight;
							const auto length_2d = m_anim_velocity.length( );

							if( m_flags & FL_DUCKING )
								weight *= 0.34f;
							else {
								if( entry->m_bIsWalking( ) ) {
									weight *= 0.52f;
								}
							}
							if( length_2d ) {
								m_anim_velocity.x = ( m_anim_velocity.x / length_2d ) * weight;
								m_anim_velocity.y = ( m_anim_velocity.y / length_2d ) * weight;
							}
						}
					}
				}
			}
		}
	}

	// function: checks if LagRecord obj is hittable if we were to fire at it now.
	bool valid( ) {
		if (!this)
			return false;

		INetChannel* info = g_csgo.m_engine->GetNetChannelInfo();
		if (!info || !g_cl.m_local || dormant())
			return false;

		// get correct based on out latency + in latency + lerp time and clamp on sv_maxunlag
		float correct = 0;

		// add out latency
		correct += info->GetLatency(INetChannel::FLOW_OUTGOING);

		// add in latency
		correct += info->GetLatency(INetChannel::FLOW_INCOMING);

		// add interpolation amount
		correct += g_cl.m_lerp;

		// clamp this shit
		correct = std::clamp(correct, 0.f, g_csgo.sv_maxunlag->GetFloat());

		// def cur time
		float curtime = g_cl.m_local->alive() ? game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) : g_csgo.m_globals->m_curtime;

		// simtime -> tickcount -> target time
		float target_time = game::TICKS_TO_TIME(game::TIME_TO_TICKS(this->m_sim_time));

		// get delta time
		// lol this looks cancer but thats the way it is ._.
		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/server/player_lagcompensation.cpp#L287
		float delta_time = correct - (curtime - target_time);

		// if out of range
		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/server/player_lagcompensation.cpp#L292
		if (fabs(delta_time) > 0.19f)
			return false;

		// just ignore dead time or not?
		return true;
	}
};