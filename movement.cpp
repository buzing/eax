#include "includes.h"

Movement g_movement{ };;

void Movement::JumpRelated( ) {
	if( g_cl.m_local->m_MoveType( ) == MOVETYPE_NOCLIP )
		return;

	if( ( g_cl.m_cmd->m_buttons & IN_JUMP ) && !( g_cl.m_flags & FL_ONGROUND ) ) {
		// bhop.
		if( g_menu.main.misc.bhop.get( ) )
			g_cl.m_cmd->m_buttons &= ~IN_JUMP;

		// duck jump ( crate jump ).
		if( g_menu.main.misc.airduck.get( ) )
			g_cl.m_cmd->m_buttons |= IN_DUCK;
	}
}

void Movement::Strafe() {
	vec3_t velocity;
	float  delta, abs_delta, velocity_delta, correct;

	// the strafe in air removal (real).
	if (VK_SHIFT && g_cl.m_flags & FL_ONGROUND)
		return;

	// don't strafe while noclipping or on ladders..
	if (g_cl.m_local->m_MoveType() == MOVETYPE_NOCLIP || g_cl.m_local->m_MoveType() == MOVETYPE_LADDER)
		return;

	// get networked velocity ( maybe absvelocity better here? ).
	// meh, should be predicted anyway? ill see.
	velocity = g_cl.m_local->m_vecAbsVelocity();

	// get the velocity len2d ( speed ).
	m_speed = velocity.length_2d();

	// compute the ideal strafe angle for our velocity.
	m_ideal = (m_speed > 0.f) ? math::rad_to_deg(std::asin(15.f / m_speed)) : 90.f;
	m_ideal2 = (m_speed > 0.f) ? math::rad_to_deg(std::asin(30.f / m_speed)) : 90.f;

	// some additional sanity.
	math::clamp(m_ideal, 0.f, 90.f);
	math::clamp(m_ideal2, 0.f, 90.f);

	// save entity bounds ( used much in circle-strafer ).
	m_mins = g_cl.m_local->m_vecMins();
	m_maxs = g_cl.m_local->m_vecMaxs();

	// save our origin
	m_origin = g_cl.m_local->m_vecOrigin();

	// disable strafing while pressing shift.
	if ((g_cl.m_buttons & IN_SPEED) || (g_cl.m_flags & FL_ONGROUND))
		return;

	// for changing direction.
	// we want to change strafe direction every call.
	m_switch_value *= -1.f;

	// for allign strafer.
	++m_strafe_index;

	if (g_cl.m_pressing_move && g_menu.main.misc.autostrafe.get()) {
		// took this idea from stacker, thank u !!!!
		enum EDirections {
			FORWARDS = 0,
			BACKWARDS = 180,
			LEFT = 90,
			RIGHT = -90,
			BACK_LEFT = 135,
			BACK_RIGHT = -135
		};

		float wish_dir{ };

		// get our key presses.
		bool holding_w = g_cl.m_buttons & IN_FORWARD;
		bool holding_a = g_cl.m_buttons & IN_MOVELEFT;
		bool holding_s = g_cl.m_buttons & IN_BACK;
		bool holding_d = g_cl.m_buttons & IN_MOVERIGHT;

		// move in the appropriate direction.
		if (holding_w) {
			//    forward left
			if (holding_a) {
				wish_dir += (EDirections::LEFT / 2);
			}
			//    forward right
			else if (holding_d) {
				wish_dir += (EDirections::RIGHT / 2);
			}
			//    forward
			else {
				wish_dir += EDirections::FORWARDS;
			}
		}
		else if (holding_s) {
			//    back left
			if (holding_a) {
				wish_dir += EDirections::BACK_LEFT;
			}
			//    back right
			else if (holding_d) {
				wish_dir += EDirections::BACK_RIGHT;
			}
			//    back
			else {
				wish_dir += EDirections::BACKWARDS;
			}

			g_cl.m_cmd->m_forward_move = 0;
		}
		else if (holding_a) {
			//    left
			wish_dir += EDirections::LEFT;
		}
		else if (holding_d) {
			//    right
			wish_dir += EDirections::RIGHT;
		}

		g_cl.m_strafe_angles.y += math::NormalizeYaw(wish_dir);
	}

	// cancel out any forwardmove values.
	g_cl.m_cmd->m_forward_move = 0.f;

	if (!g_menu.main.misc.autostrafe.get())
		return;

	// get our viewangle change.
	delta = math::NormalizedAngle(g_cl.m_strafe_angles.y - m_old_yaw);

	// convert to absolute change.
	abs_delta = std::abs(delta);

	// save old yaw for next call.
	m_circle_yaw = m_old_yaw = g_cl.m_strafe_angles.y;

	// set strafe direction based on mouse direction change.
	if (delta > 0.f)
		g_cl.m_cmd->m_side_move = -450.f;

	else if (delta < 0.f)
		g_cl.m_cmd->m_side_move = 450.f;

	// we can accelerate more, because we strafed less then needed
	// or we got of track and need to be retracked.
	if (abs_delta <= m_ideal || abs_delta >= 30.f) {
		// compute angle of the direction we are traveling in.
		ang_t velocity_angle;
		math::VectorAngles(velocity, velocity_angle);

		// get the delta between our direction and where we are looking at.
		velocity_delta = math::NormalizeYaw(g_cl.m_strafe_angles.y - velocity_angle.y);

		// correct our strafe amongst the path of a circle.
		correct = m_ideal;

		if (velocity_delta <= correct || m_speed <= 15.f) {
			// not moving mouse, switch strafe every tick.
			if (-correct <= velocity_delta || m_speed <= 15.f) {
				g_cl.m_strafe_angles.y += (m_ideal * m_switch_value);
				g_cl.m_cmd->m_side_move = 450.f * m_switch_value;
			}

			else {
				g_cl.m_strafe_angles.y = velocity_angle.y - correct;
				g_cl.m_cmd->m_side_move = 450.f;
			}
		}

		else {
			g_cl.m_strafe_angles.y = velocity_angle.y + correct;
			g_cl.m_cmd->m_side_move = -450.f;
		}
	}
}

void Movement::DoPrespeed( ) {
	float   mod, min, max, step, strafe, time, angle;
	vec3_t  plane;

	// min and max values are based on 128 ticks.
	mod = g_csgo.m_globals->m_interval * 128.f;

	// scale min and max based on tickrate.
	min = 2.25f * mod;
	max = 5.f * mod;

	// compute ideal strafe angle for moving in a circle.
	strafe = m_ideal * 2.f;

	// clamp ideal strafe circle value to min and max step.
	math::clamp( strafe, min, max );

	// calculate time.
	time = 320.f / m_speed;

	// clamp time.
	math::clamp( time, 0.35f, 1.f );

	// init step.
	step = strafe;

	while( true ) {
		// if we will not collide with an object or we wont accelerate from such a big step anymore then stop.
		if( !WillCollide( time, step ) || max <= step )
			break;

		// if we will collide with an object with the current strafe step then increment step to prevent a collision.
		step += 0.2f;
	}

	if( step > max ) {
		// reset step.
		step = strafe;

		while( true ) {
			// if we will not collide with an object or we wont accelerate from such a big step anymore then stop.
			if( !WillCollide( time, step ) || step <= -min )
				break;

			// if we will collide with an object with the current strafe step decrement step to prevent a collision.
			step -= 0.2f;
		}

		if( step < -min ) {
			if( GetClosestPlane( plane ) ) {
				// grab the closest object normal
				// compute the angle of the normal
				// and push us away from the object.
				angle = math::rad_to_deg( std::atan2( plane.y, plane.x ) );
				step = -math::NormalizedAngle( m_circle_yaw - angle ) * 0.1f;
			}
		}

		else
			step -= 0.2f;
	}

	else
		step += 0.2f;

	// add the computed step to the steps of the previous circle iterations.
	m_circle_yaw = math::NormalizedAngle( m_circle_yaw + step );

	// apply data to usercmd.
	g_cl.m_cmd->m_view_angles.y = m_circle_yaw;
	g_cl.m_cmd->m_side_move = ( step >= 0.f ) ? -450.f : 450.f;
}

bool Movement::GetClosestPlane( vec3_t &plane ) {
	CGameTrace            trace;
	CTraceFilterWorldOnly filter;
	vec3_t                start{ m_origin };
	float                 smallest{ 1.f };
	const float		      dist{ 75.f };

	// trace around us in a circle
	for( float step{ }; step <= math::pi_2; step += ( math::pi / 10.f ) ) {
		// extend endpoint x units.
		vec3_t end = start;
		end.x += std::cos( step ) * dist;
		end.y += std::sin( step ) * dist;

		g_csgo.m_engine_trace->TraceRay( Ray( start, end, m_mins, m_maxs ), CONTENTS_SOLID, &filter, &trace );

		// we found an object closer, then the previouly found object.
		if( trace.m_fraction < smallest ) {
			// save the normal of the object.
			plane = trace.m_plane.m_normal;
			smallest = trace.m_fraction;
		}
	}

	// did we find any valid object?
	return smallest != 1.f && plane.z < 0.1f;
}

bool Movement::WillCollide( float time, float change ) {
	struct PredictionData_t {
		vec3_t start;
		vec3_t end;
		vec3_t velocity;
		float  direction;
		bool   ground;
		float  predicted;
	};

	PredictionData_t      data;
	CGameTrace            trace;
	CTraceFilterWorldOnly filter;

	// set base data.
	data.ground = g_cl.m_flags & FL_ONGROUND;
	data.start = m_origin;
	data.end = m_origin;
	data.velocity = g_cl.m_local->m_vecVelocity( );
	data.direction = math::rad_to_deg( std::atan2( data.velocity.y, data.velocity.x ) );

	for( data.predicted = 0.f; data.predicted < time; data.predicted += g_csgo.m_globals->m_interval ) {
		// predict movement direction by adding the direction change.
		// make sure to normalize it, in case we go over the -180/180 turning point.
		data.direction = math::NormalizedAngle( data.direction + change );

		// pythagoras.
		float hyp = data.velocity.length_2d( );

		// adjust velocity for new direction.
		data.velocity.x = std::cos( math::deg_to_rad( data.direction ) ) * hyp;
		data.velocity.y = std::sin( math::deg_to_rad( data.direction ) ) * hyp;

		// assume we bhop, set upwards impulse.
		if( data.ground )
			data.velocity.z = g_csgo.sv_jump_impulse->GetFloat( );

		else
			data.velocity.z -= g_csgo.sv_gravity->GetFloat( ) * g_csgo.m_globals->m_interval;

		// we adjusted the velocity for our new direction.
		// see if we can move in this direction, predict our new origin if we were to travel at this velocity.
		data.end += ( data.velocity * g_csgo.m_globals->m_interval );

		// trace
		g_csgo.m_engine_trace->TraceRay( Ray( data.start, data.end, m_mins, m_maxs ), MASK_PLAYERSOLID, &filter, &trace );

		// check if we hit any objects.
		if( trace.m_fraction != 1.f && trace.m_plane.m_normal.z <= 0.9f )
			return true;
		if( trace.m_startsolid || trace.m_allsolid )
			return true;

		// adjust start and end point.
		data.start = data.end = trace.m_endpos;

		// move endpoint 2 units down, and re-trace.
		// do this to check if we are on th floor.
		g_csgo.m_engine_trace->TraceRay( Ray( data.start, data.end - vec3_t{ 0.f, 0.f, 2.f }, m_mins, m_maxs ), MASK_PLAYERSOLID, &filter, &trace );

		// see if we moved the player into the ground for the next iteration.
		data.ground = trace.hit( ) && trace.m_plane.m_normal.z > 0.7f;
	}

	// the entire loop has ran
	// we did not hit shit.
	return false;
}

void Movement::FixMove( CUserCmd *cmd, const ang_t &wish_angles ) {
	vec3_t  move, dir;
	float   delta, len;
	ang_t   move_angle;

	// roll nospread fix.
	if( !( g_cl.m_flags & FL_ONGROUND ) && cmd->m_view_angles.z != 0.f )
		cmd->m_side_move = 0.f;

	// convert movement to vector.
	move = { cmd->m_forward_move, cmd->m_side_move, 0.f };

	// get move length and ensure we're using a unit vector ( vector with length of 1 ).
	len = move.normalize( );
	if( !len )
		return;

	// convert move to an angle.
	math::VectorAngles( move, move_angle );

	// calculate yaw delta.
	delta = ( cmd->m_view_angles.y - wish_angles.y );

	// accumulate yaw delta.
	move_angle.y += delta;

	// calculate our new move direction.
	// dir = move_angle_forward * move_length
	math::AngleVectors( move_angle, &dir );

	// scale to og movement.
	dir *= len;

	// strip old flags.
	g_cl.m_cmd->m_buttons &= ~( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT );

	// fix ladder and noclip.
	if( g_cl.m_local->m_MoveType( ) == MOVETYPE_LADDER ) {
		// invert directon for up and down.
		if( cmd->m_view_angles.x >= 45.f && wish_angles.x < 45.f && std::abs( delta ) <= 65.f )
			dir.x = -dir.x;

		// write to movement.
		cmd->m_forward_move = dir.x;
		cmd->m_side_move = dir.y;

		// set new button flags.
		if( cmd->m_forward_move > 200.f )
			cmd->m_buttons |= IN_FORWARD;

		else if( cmd->m_forward_move < -200.f )
			cmd->m_buttons |= IN_BACK;

		if( cmd->m_side_move > 200.f )
			cmd->m_buttons |= IN_MOVERIGHT;

		else if( cmd->m_side_move < -200.f )
			cmd->m_buttons |= IN_MOVELEFT;
	}

	// we are moving normally.
	else {
		// we must do this for pitch angles that are out of bounds.
		if( cmd->m_view_angles.x < -90.f || cmd->m_view_angles.x > 90.f )
			dir.x = -dir.x;

		// set move.
		cmd->m_forward_move = dir.x;
		cmd->m_side_move = dir.y;

		// set new button flags.
		if( cmd->m_forward_move > 0.f )
			cmd->m_buttons |= IN_FORWARD;

		else if( cmd->m_forward_move < 0.f )
			cmd->m_buttons |= IN_BACK;

		if( cmd->m_side_move > 0.f )
			cmd->m_buttons |= IN_MOVERIGHT;

		else if( cmd->m_side_move < 0.f )
			cmd->m_buttons |= IN_MOVELEFT;
		slidew(cmd);
	}
}

void Movement::AutoPeek(CUserCmd* cmd, float wish_yaw) {
	// set to invert if we press the button.
	if (g_input.GetKeyState(g_menu.main.aimbot.quickpeekassist.get())) {
		if (start_position.IsZero()) {
			start_position = g_cl.m_local->GetAbsOrigin();

			if (!(g_cl.m_flags & FL_ONGROUND)) {
				CTraceFilterWorldOnly filter;
				CGameTrace trace;

				g_csgo.m_engine_trace->TraceRay(Ray(start_position, start_position - vec3_t(0.0f, 0.0f, 1000.0f)), MASK_SOLID, &filter, &trace);

				if (trace.m_fraction < 1.0f)
					start_position = trace.m_endpos + vec3_t(0.0f, 0.0f, 2.0f);
			}
		}
		else {
			bool revolver_shoot = g_cl.m_weapon_id == REVOLVER && !g_cl.m_revolver_fire && (cmd->m_buttons & IN_ATTACK || cmd->m_buttons & IN_ATTACK2);

			if (g_cl.m_old_shot)
				fired_shot = true;

			if (fired_shot) {
				vec3_t current_position = g_cl.m_local->GetAbsOrigin();
				vec3_t difference = current_position - start_position;

				if (difference.length_2d() > 5.0f) {
					vec3_t velocity = vec3_t(difference.x * cos(wish_yaw / 180.0f * math::pi) + difference.y * sin(wish_yaw / 180.0f * math::pi), difference.y * cos(wish_yaw / 180.0f * math::pi) - difference.x * sin(wish_yaw / 180.0f * math::pi), difference.z);

					if (difference.length_2d() < 50.0f) {
						cmd->m_forward_move = -velocity.x * 20.0f;
						cmd->m_side_move = velocity.y * 20.0f;
					}
					else if (difference.length_2d() < 100.0f) {
						cmd->m_forward_move = -velocity.x * 10.0f;
						cmd->m_side_move = velocity.y * 10.0f;
					}
					else if (difference.length_2d() < 150.0f) {
						cmd->m_forward_move = -velocity.x * 5.0f;
						cmd->m_side_move = velocity.y * 5.0f;
					}
					else if (difference.length_2d() < 250.0f) {
						cmd->m_forward_move = -velocity.x * 2.0f;
						cmd->m_side_move = velocity.y * 2.0f;
					}
					else {
						cmd->m_forward_move = -velocity.x * 1.0f;
						cmd->m_side_move = velocity.y * 1.0f;
					}
				}
				else {
					fired_shot = false;
					start_position.clear();
				}
			}
		}
	}
	else {
		fired_shot = false;
		start_position.clear();

		// memory leak
		return;
	}
}


void Movement::accelerate(const CUserCmd& cmd, const vec3_t& wishdir, const float wishspeed, vec3_t& velocity, float acceleration) {
	const auto cur_speed = velocity.dot(wishdir);

	static auto sv_accelerate_use_weapon_speed = g_csgo.m_cvar->FindVar(HASH("sv_accelerate_use_weapon_speed"));

	const auto add_speed = wishspeed - cur_speed;
	if (add_speed <= 0.f)
		return;

	const auto v57 = std::max(cur_speed, 0.f);

	const auto ducking =
		cmd.m_buttons & IN_DUCK;

	auto v20 = true;
	if (ducking
		|| !(cmd.m_buttons & IN_SPEED))
		v20 = false;

	auto finalwishspeed = std::max(wishspeed, 250.f);
	auto abs_finalwishspeed = finalwishspeed;

	const auto weapon = g_cl.m_local->GetActiveWeapon();

	bool slow_down_to_fast_nigga{};

	if (weapon
		&& sv_accelerate_use_weapon_speed->GetInt()) {
		const auto item_index = static_cast<std::uint16_t>(weapon->m_iItemDefinitionIndex());
		if (weapon->m_zoomLevel() > 0
			&& (item_index == 11 || item_index == 38 || item_index == 9 || item_index == 8 || item_index == 39 || item_index == 40))
			slow_down_to_fast_nigga = (g_movement.m_max_weapon_speed * 0.52f) < 110.f;

		const auto modifier = std::min(1.f, g_movement.m_max_weapon_speed / 250.f);

		abs_finalwishspeed *= modifier;

		if ((!ducking && !v20)
			|| slow_down_to_fast_nigga)
			finalwishspeed *= modifier;
	}

	if (ducking) {
		if (!slow_down_to_fast_nigga)
			finalwishspeed *= 0.34f;

		abs_finalwishspeed *= 0.34f;
	}

	if (v20) {
		if (!slow_down_to_fast_nigga)
			finalwishspeed *= 0.52f;

		abs_finalwishspeed *= 0.52f;

		const auto abs_finalwishspeed_minus5 = abs_finalwishspeed - 5.f;
		if (v57 < abs_finalwishspeed_minus5) {
			const auto v30 =
				std::max(v57 - abs_finalwishspeed_minus5, 0.f)
				/ std::max(abs_finalwishspeed - abs_finalwishspeed_minus5, 0.f);

			const auto v27 = 1.f - v30;
			if (v27 >= 0.f)
				acceleration = std::min(v27, 1.f) * acceleration;
			else
				acceleration = 0.f;
		}
	}

	const auto v33 = std::min(
		add_speed,
		((g_csgo.m_globals->m_interval * acceleration) * finalwishspeed)
		* g_cl.m_local->m_surfaceFriction()
	);

	velocity += wishdir * v33;

	const auto len = velocity.length();
	if (len
		&& len > g_movement.m_max_weapon_speed)
		velocity *= g_movement.m_max_weapon_speed / len;

}

void Movement::walk_move(const CUserCmd& cmd, vec3_t& move, vec3_t& fwd, vec3_t& right, vec3_t& velocity) {
	if (fwd.z != 0.f)
		fwd.normalize();

	if (right.z != 0.f)
		right.normalize();

	vec3_t wishvel{
		fwd.x * move.x + right.x * move.y,
		fwd.y * move.x + right.y * move.y,
		0.f
	};
	static auto sv_accelerate = g_csgo.m_cvar->FindVar(HASH("sv_accelerate"));
	auto wishdir = wishvel;

	auto wishspeed = wishdir.normalize();
	if (wishspeed
		&& wishspeed > g_movement.m_max_player_speed) {
		wishvel *= g_movement.m_max_player_speed / wishspeed;

		wishspeed = g_movement.m_max_player_speed;
	}

	velocity.z = 0.f;
	accelerate(cmd, wishdir, wishspeed, velocity, sv_accelerate->GetFloat());
	velocity.z = 0.f;

	const auto speed_sqr = velocity.length_sqr();
	if (speed_sqr > (g_movement.m_max_player_speed * g_movement.m_max_player_speed))
		velocity *= g_movement.m_max_player_speed / std::sqrt(speed_sqr);

	if (velocity.length() < 1.f)
		velocity = {};
}

void Movement::full_walk_move(const CUserCmd cmd, vec3_t move, vec3_t fwd, vec3_t right, vec3_t velocity) {
	static auto sv_maxvelocity = g_csgo.m_cvar->FindVar(HASH("sv_maxvelocity"));
	static auto sv_friction = g_csgo.m_cvar->FindVar(HASH("sv_friction"));

	if (reinterpret_cast <ulong_t>(g_cl.m_local->GetGroundEntity())) {
		velocity.z = 0.f;

		const auto speed = velocity.length();
		if (speed >= 0.1f) {
			const auto friction = sv_friction->GetFloat() * g_cl.m_local->m_surfaceFriction();
			const auto sv_stopspeed = sv_friction->GetFloat();
			const auto control = speed < sv_stopspeed ? sv_stopspeed : speed;

			const auto new_speed = std::max(0.f, speed - ((control * friction) * g_csgo.m_globals->m_interval));
			if (speed != new_speed)
				velocity *= new_speed / speed;
		}

		walk_move(cmd, move, fwd, right, velocity);

		velocity.z = 0.f;
	}

	const auto sv_maxvelocity_ = sv_maxvelocity->GetFloat();
	for (std::size_t i{}; i < 3u; ++i) {
		auto& element = velocity.at(i);

		if (element > sv_maxvelocity_)
			element = sv_maxvelocity_;
		else if (element < -sv_maxvelocity_)
			element = -sv_maxvelocity_;
	}
}

void Movement::modify_move(CUserCmd cmd, vec3_t velocity) {
	vec3_t fwd{}, right{};

	math::AngleVectors(cmd.m_view_angles, &fwd, &right, nullptr);

	const auto speed_sqr = vec3_t(cmd.m_side_move, cmd.m_forward_move, cmd.m_up_move).length_sqr();
	if (speed_sqr > (g_movement.m_max_player_speed * g_movement.m_max_player_speed))
		vec3_t(cmd.m_side_move, cmd.m_forward_move, cmd.m_up_move) *= g_movement.m_max_player_speed / std::sqrt(speed_sqr);

	full_walk_move(cmd, vec3_t(cmd.m_side_move, cmd.m_forward_move, cmd.m_up_move), fwd, right, velocity);
}

void Movement::predict_move(const CUserCmd cmd, vec3_t velocity) {
	vec3_t fwd{}, right{};

	math::AngleVectors(cmd.m_view_angles, &fwd, &right, nullptr);

	auto move = vec3_t(cmd.m_side_move, cmd.m_forward_move, cmd.m_up_move);

	const auto speed_sqr = vec3_t(cmd.m_side_move, cmd.m_forward_move, cmd.m_up_move).length_sqr();
	if (speed_sqr > (g_movement.m_max_player_speed * g_movement.m_max_player_speed))
		move *= g_movement.m_max_player_speed / std::sqrt(speed_sqr);

	full_walk_move(cmd, move, fwd, right, velocity);
}

void Movement::AutoStop() {

	static ang_t wish_ang{};

	if (!g_cl.CanFireWeapon())
		return;

	if (const auto weapon = g_cl.m_local->GetActiveWeapon()) {
		if (weapon->GetWpnData()) {
			if (weapon->GetWpnData()->m_weapon_type == WEAPONTYPE_GRENADE
				|| weapon->GetWpnData()->m_weapon_type == WEAPONTYPE_C4
				|| weapon->GetWpnData()->m_weapon_type == WEAPONTYPE_KNIFE)
				return;
		}
	}

	if (!(g_cl.m_local->m_fFlags() & FL_ONGROUND)
		|| g_cl.m_cmd->m_buttons & IN_JUMP)
		return;

	static auto sv_accelerate_use_weapon_speed = g_csgo.m_cvar->FindVar(HASH("sv_accelerate_use_weapon_speed"));

	static auto sv_accelerate = g_csgo.m_cvar->FindVar(HASH("sv_accelerate"));

	const auto weapon = g_cl.m_local->GetActiveWeapon();
	if (!weapon)
		return;

	const auto wpn_data = weapon->GetWpnData();
	if (!wpn_data)
		return;

	auto max_speed{ 260.f };

	if (g_cl.m_local->GetActiveWeapon()
		&& g_cl.m_local->GetActiveWeapon()->GetWpnData()) {
		max_speed = g_cl.m_local->m_bIsScoped() ?
			g_cl.m_local->GetActiveWeapon()->GetWpnData()->m_max_player_speed_alt :
			g_cl.m_local->GetActiveWeapon()->GetWpnData()->m_max_player_speed;
	}

	vec3_t cur_velocity{ g_cl.m_local->m_vecVelocity() };
	const auto speed_2d = cur_velocity.length();

	g_cl.m_cmd->m_buttons &= ~IN_SPEED;

	auto finalwishspeed = std::min(max_speed, 250.f);

	const auto ducking =
		g_cl.m_cmd->m_buttons & IN_DUCK;

	bool slow_down_to_fast_nigga{};

	if (sv_accelerate_use_weapon_speed->GetInt()) {
		const auto item_index = static_cast<std::uint16_t>(g_cl.m_local->GetActiveWeapon()->m_iItemDefinitionIndex());
		if (g_cl.m_local->GetActiveWeapon()->m_zoomLevel() > 0
			&& (item_index == 11 || item_index == 38 || item_index == 9 || item_index == 8 || item_index == 39 || item_index == 40))
			slow_down_to_fast_nigga = (max_speed * 0.52f) < 110.f;

		if (!ducking
			|| slow_down_to_fast_nigga)
			finalwishspeed *= std::min(1.f, max_speed / 250.f);
	}

	if (ducking
		&& !slow_down_to_fast_nigga)
		finalwishspeed *= 0.34f;

	finalwishspeed =
		((g_csgo.m_globals->m_interval * sv_accelerate->GetFloat()) * finalwishspeed)
		* g_cl.m_local->m_surfaceFriction();

	if (max_speed * 0.34f <= speed_2d) {
		ang_t dir{};
		math::VectorAngles(cur_velocity *= -1.f, dir);

		dir.y = g_cl.m_cmd->m_view_angles.y - dir.y;

		vec3_t dir_ang_handler{};

		math::AngleVectors(dir, &dir_ang_handler, nullptr, nullptr);

		g_cl.m_cmd->m_forward_move = dir_ang_handler.x * finalwishspeed;
		g_cl.m_cmd->m_side_move = dir_ang_handler.y * finalwishspeed;
	}
	else {
		m_max_player_speed = m_max_weapon_speed = 47.5f;

		modify_move(*g_cl.m_cmd, cur_velocity);
	}
}

void Movement::FakeWalk( ) {
	vec3_t velocity{ g_cl.m_local->m_vecVelocity( ) };
	int    ticks{ }, max{ 16 };

	if( !g_input.GetKeyState( g_menu.main.misc.fakewalk.get( ) ) )
		return;

	if( !g_cl.m_local->GetGroundEntity( ) )
		return;

	// user was running previously and abrubtly held the fakewalk key
	// we should quick-stop under this circumstance to hit the 0.22 flick
	// perfectly, and speed up our fakewalk after running even more.
	//if( g_cl.m_initial_flick ) {
	//	Movement::QuickStop( );
	//	return;
	//}
	
	// reference:
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/gamemovement.cpp#L1612

	// calculate friction.
	float friction = g_csgo.sv_friction->GetFloat( ) * g_cl.m_local->m_surfaceFriction( );

	for( ; ticks < g_cl.m_max_lag; ++ticks ) {
		// calculate speed.
		float speed = velocity.length( );

		// if too slow return.
		if( speed <= 0.1f )
			break;

		// bleed off some speed, but if we have less than the bleed, threshold, bleed the threshold amount.
		float control = std::max( speed, g_csgo.sv_stopspeed->GetFloat( ) );

		// calculate the drop amount.
		float drop = control * friction * g_csgo.m_globals->m_interval;

		// scale the velocity.
		float newspeed = std::max( 0.f, speed - drop );

		if( newspeed != speed ) {
			// determine proportion of old speed we are using.
			newspeed /= speed;

			// adjust velocity according to proportion.
			velocity *= newspeed;
		}
	}

	// zero forwardmove and sidemove.
	if( ticks > ( ( max - 1 ) - g_csgo.m_cl->m_choked_commands ) || !g_csgo.m_cl->m_choked_commands ) {
		g_cl.m_cmd->m_forward_move = g_cl.m_cmd->m_side_move = 0.f;
	}
}

void Movement::FastStop() {
	if (!g_cl.m_pressing_move && g_menu.main.misc.fast_stop.get() && g_cl.m_speed > 15.f) {
		if (!g_cl.m_cmd || !g_cl.m_local || !g_cl.m_local->alive())
			return;

		auto weapon = g_cl.m_local->GetActiveWeapon();

		// don't fake movement while noclipping or on ladders..
		if (!weapon || !weapon->GetWpnData() || g_cl.m_local->m_MoveType() == MOVETYPE_NOCLIP || g_cl.m_local->m_MoveType() == MOVETYPE_LADDER)
			return;

		if (!(g_cl.m_local->m_fFlags() & FL_ONGROUND))
			return;

		if (g_cl.m_cmd->m_buttons & IN_JUMP)
			return;

		auto move_speed = sqrtf((g_cl.m_cmd->m_forward_move * g_cl.m_cmd->m_forward_move) + (g_cl.m_cmd->m_side_move * g_cl.m_cmd->m_side_move));
		auto pre_prediction_velocity = g_cl.m_local->m_vecVelocity().length_2d();

		auto v58 = g_csgo.sv_stopspeed->GetFloat();
		v58 = fmaxf(v58, pre_prediction_velocity);
		v58 = g_csgo.sv_friction->GetFloat() * v58;
		auto slow_walked_speed = fmaxf(pre_prediction_velocity - (v58 * g_csgo.m_globals->m_interval), 0.0f);

		if (slow_walked_speed <= 0 || pre_prediction_velocity <= slow_walked_speed) {
			g_cl.m_cmd->m_forward_move = 0;
			g_cl.m_cmd->m_side_move = 0;
			return;
		}

		ang_t angle;
		math::VectorAngles(g_cl.m_local->m_vecVelocity(), angle);

		// get our current speed of travel.
		float speed = g_cl.m_local->m_vecVelocity().length();

		// fix direction by factoring in where we are looking.
		angle.y = g_cl.m_view_angles.y - angle.y;

		// convert corrected angle back to a direction.
		vec3_t direction;
		math::AngleVectors(angle, &direction);

		vec3_t stop = direction * -speed;

		g_cl.m_cmd->m_forward_move = stop.x;
		g_cl.m_cmd->m_side_move = stop.y;
	}
}

void Movement::slidew(CUserCmd* cmd)
{
	if (g_cl.m_local->m_MoveType() == MOVETYPE_NOCLIP || g_cl.m_local->m_MoveType() == MOVETYPE_LADDER || ((g_cl.m_buttons & IN_JUMP) || !(g_cl.m_flags & FL_ONGROUND)))
		return;

	// get current origin.
	vec3_t cur = g_cl.m_local->m_vecOrigin();

	// get prevoius origin.
	vec3_t prev = g_cl.m_net_pos.empty() ? g_cl.m_local->m_vecOrigin() : g_cl.m_net_pos.front().m_pos;

	// delta between the current origin and the last sent origin.
	float delta = (cur - prev).length_sqr();

	if (cmd->m_side_move < 25.f && delta > 8.2 && g_cl.m_speed > 8.2) {
		cmd->m_buttons |= IN_MOVERIGHT;
		cmd->m_buttons &= ~IN_MOVELEFT;
	}

	if (cmd->m_side_move > 25.f && delta > 8.2 && g_cl.m_speed > 8.2) {
		cmd->m_buttons |= IN_MOVELEFT;
		cmd->m_buttons &= ~IN_MOVERIGHT;
	}

	if (cmd->m_forward_move > 25.f && delta > 8.2 && g_cl.m_speed > 8.2) {
		cmd->m_buttons |= IN_BACK;
		cmd->m_buttons &= ~IN_FORWARD;
	}

	if (cmd->m_forward_move < 25.f && delta > 8.2 && g_cl.m_speed > 8.2) {
		cmd->m_buttons |= IN_FORWARD;
		cmd->m_buttons &= ~IN_BACK;
	}
}
