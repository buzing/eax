#pragma once

class AimbotTab : public Tab {
public:
	// col1.
	Checkbox	  enable;
	Checkbox	  silent;
	MultiDropdown hitbox;
	MultiDropdown multipoint;
	Checkbox	center;
	Slider		  scale;
	Slider		  body_scale;
	Slider		  minimal_damage;
	Keybind quickpeekassist;
	Colorpicker	autopeek_active;
	// col2.
	Checkbox      zoom;
	Checkbox      nospread;
	Checkbox      norecoil;
	Checkbox      hitchance;
	Slider	      hitchance_amount;
	Checkbox hitchance_in_air;
	Slider   in_air_hitchance;
	Slider zeusbot_hc;
	Checkbox ignore_limbs;
	Checkbox	  correct;
	Checkbox	  correct_network;
	Checkbox	correct_network1;
	Checkbox	auto_weapons;
	Checkbox autostop;
	Checkbox whitelist;
	Checkbox      delay_shot;
	Checkbox      prefer_baim;
	MultiDropdown prefer_baim_disablers;
	Slider        baim_hp;
	Keybind       baim_key;
	Keybind		  override;
	Keybind       override_dmg_key;
	Slider        override_dmg_value;

public:
	void init( ) {
		// title.
		SetTitle( XOR( "aimbot" ) );

		enable.setup( XOR( "enabled" ), XOR( "enable" ) );
		RegisterElement( &enable );

		hitbox.setup(XOR("target hitbox"), XOR("hitbox"), { XOR("head"), XOR("neck"), XOR("chest"), XOR("stomach"), XOR("arms"), XOR("legs"), XOR("feet")});
		RegisterElement(&hitbox);

		multipoint.setup(XOR("multi-point"), XOR("multipoint"), { XOR("head"), XOR("chest"), XOR("stomach"), XOR("legs") });
		RegisterElement(&multipoint);

		scale.setup("", XOR("hitbox_scale"), 1.f, 100.f, false, 0, 90.f, 1.f, XOR(L"%"));
		scale.AddShowCallback(callbacks::IsMultipointOn);
		RegisterElement(&scale);

		body_scale.setup(XOR("body hitbox scale"), XOR("body_hitbox_scale"), 1.f, 100.f, true, 0, 50.f, 1.f, XOR(L"%"));
		body_scale.AddShowCallback(callbacks::IsMultipointBodyOn);
		RegisterElement(&body_scale);

		silent.setup( XOR( "silent aim" ), XOR( "silent" ) );
		RegisterElement( &silent );

		hitchance.setup(XOR("minimum hitchance"), XOR("hitchance"));
		RegisterElement(&hitchance);

		hitchance_amount.setup("", XOR("hitchance_amount"), 1.f, 100.f, false, 0, 50.f, 1.f, XOR(L"%"));
		hitchance_amount.AddShowCallback(callbacks::IsHitchanceOn);
		RegisterElement(&hitchance_amount);

		hitchance_in_air.setup(XOR("in air hitchance"), XOR("custom_in_air_hitchance"));
		RegisterElement(&hitchance_in_air);

		in_air_hitchance.setup("in air hitchance", XOR("in_air_hitchance"), 1.f, 100.f, false, 0, 50.f, 1.f, XOR(L"%"));
		in_air_hitchance.AddShowCallback(callbacks::IsHitchancAireOn);
		RegisterElement(&in_air_hitchance);

		zeusbot_hc.setup("zeus hitchance", XOR("zeusbot_hc"), 1.f, 100.f, true, 0, 50.f, 1.f, XOR(L"%"));
		RegisterElement(&zeusbot_hc);

		minimal_damage.setup( XOR( "minimum damage" ), XOR( "minimal_damage" ), 1.f, 120.f, true, 0, 40.f, 1.f );
		RegisterElement( &minimal_damage );

		override_dmg_key.setup(XOR("minimum damage override"), XOR("override_dmg"));
		override_dmg_key.SetToggleCallback(callbacks::ToggleDMG);
		RegisterElement(&override_dmg_key);

		override_dmg_value.setup(" ", XOR("dmg_override"), 1.f, 100.f, false, 0, 1.f, 1.f, XOR(L"hp"));
		override_dmg_value.AddShowCallback(callbacks::IsOverrideDamage);
		RegisterElement(&override_dmg_value);

		zoom.setup(XOR("automatic scope"), XOR("zoom"));
		RegisterElement(&zoom);

		center.setup(XOR("prefer center multipoint"), XOR("center"));
		RegisterElement(&center);

		quickpeekassist.setup(XOR("quick peek assist"), XOR("quickpeekassist"));
		RegisterElement(&quickpeekassist);


		autopeek_active.setup(XOR("autopeek color"), XOR("autoactive"), colors::red);
		RegisterElement(&autopeek_active);


		// col2.

		//nospread.setup( XOR( "remove spread" ), XOR( "nospread" ) );
		//RegisterElement( &nospread, 1 );

		norecoil.setup( XOR( "remove recoil" ), XOR( "norecoil" ) );
		RegisterElement( &norecoil, 1 );

		correct.setup(XOR("anti-aim correction"), XOR("correct"));
		RegisterElement(&correct, 1);

		correct_network.setup(XOR("display resolver mode"), XOR("correct_network"));
		RegisterElement(&correct_network, 1);

		correct_network1.setup(XOR("correct network angles"), XOR("correct_network1"));
		RegisterElement(&correct_network1, 1);

		auto_weapons.setup(XOR("automatic weapons"), XOR("auto_weapons"));
		RegisterElement(&auto_weapons, 1);

		autostop.setup(XOR("quick stop"), XOR("autostop"));
		RegisterElement(&autostop, 1);

		delay_shot.setup(XOR("delay breaking lc enemy"), XOR("delay_shot"));
		RegisterElement(&delay_shot, 1);


		whitelist.setup(XOR("ignore other cheat users"), XOR("whitelist"));
		RegisterElement(&whitelist, 1);

		ignore_limbs.setup(XOR("ignore limbs on moving"), XOR("ignore_limbs"));
		RegisterElement(&ignore_limbs, 1);

		prefer_baim.setup(XOR("prefer body aim"), XOR("prefer_baim"));
		RegisterElement(&prefer_baim, 1);

		prefer_baim_disablers.setup( XOR( "prefer body aim disablers" ), XOR( "prefer_baim_disablers" ), { XOR( "body update" ), XOR( "moving" ), XOR("networked"), XOR("low body yaw"), XOR("enable lethal") });
		RegisterElement( &prefer_baim_disablers, 1 );

		baim_key.setup( XOR( "body aim on key" ), XOR( "body aim on key" ) );
		RegisterElement( &baim_key, 1 );

		override.setup(XOR("override on key"), XOR("override on key"));
		RegisterElement(&override, 1);
	}
};

class AntiAimTab : public Tab {
public:
	// col 1.
	Checkbox enable;
	Checkbox edge;
	Dropdown mode;

	Dropdown pitch_stand;
	Dropdown yaw_stand;
	Slider   jitter_range_stand;
	Slider   rot_range_stand;
	Slider   rot_speed_stand;
	Slider   rand_update_stand;
	Dropdown dir_stand;
	Slider   dir_time_stand;
	Slider   dir_custom_stand;
	Checkbox dir_lock;
	Dropdown base_angle_stand;
	Dropdown body_fake_stand;

	Dropdown pitch_walk;
	Dropdown yaw_walk;
	Slider   jitter_range_walk;
	Slider   rot_range_walk;
	Slider   rot_speed_walk;
	Slider   rand_update_walk;
	Dropdown dir_walk;
	Slider	 dir_time_walk;
	Slider   dir_custom_walk;
	Dropdown base_angle_walk;

	Dropdown pitch_air;
	Dropdown yaw_air;
	Slider   jitter_range_air;
	Slider   rot_range_air;
	Slider   rot_speed_air;
	Slider   rand_update_air;
	Dropdown dir_air;
	Slider   dir_time_air;
	Slider   dir_custom_air;
	Dropdown base_angle_air;
	Dropdown body_fake_air;

	// col 2.
	Dropdown fake_yaw;
	Slider	 fake_relative;
	Slider	 fake_jitter_range;

	Checkbox      lag_enable;
	MultiDropdown lag_active;
	Dropdown      lag_mode;
	Slider        lag_limit;
	Checkbox      lag_land;

	Colorpicker   color_manul_antiaim;
	Checkbox	  manul_antiaim;
	Keybind		  left;
	Keybind		  right;
	Keybind		  backward;
	Keybind forward;

public:
	void init( ) {
		SetTitle( XOR( "anti-aim" ) );

		enable.setup( XOR( "enable" ), XOR( "enable" ) );
		RegisterElement( &enable );

		edge.setup( XOR( "edge" ), XOR( "edge" ) );
		RegisterElement( &edge );

		mode.setup( "", XOR( "mode" ), { XOR( "stand" ), XOR( "walk" ), XOR( "air" ) }, false );
		RegisterElement( &mode );

		// stand.
		pitch_stand.setup( XOR( "pitch" ), XOR( "pitch_stnd" ), { XOR( "off" ), XOR( "down" ), XOR( "up" ), XOR( "random" ), XOR( "ideal" ) } );
		pitch_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		RegisterElement( &pitch_stand );

		yaw_stand.setup( XOR( "yaw" ), XOR( "yaw_stnd" ), { XOR( "off" ), XOR( "direction" ), XOR( "jitter" ), XOR( "rotate" ), XOR( "random" ) } );
		yaw_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		RegisterElement( &yaw_stand );

		jitter_range_stand.setup( "", XOR( "jitter_range_stnd" ), 1.f, 180.f, false, 0, 45.f, 5.f, XOR( L"°" ) );
		jitter_range_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		jitter_range_stand.AddShowCallback( callbacks::IsStandYawJitter );
		RegisterElement( &jitter_range_stand );

		rot_range_stand.setup( "", XOR( "rot_range_stnd" ), 0.f, 360.f, false, 0, 360.f, 5.f, XOR( L"°" ) );
		rot_range_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		rot_range_stand.AddShowCallback( callbacks::IsStandYawRotate );
		RegisterElement( &rot_range_stand );

		rot_speed_stand.setup( "", XOR( "rot_speed_stnd" ), 1.f, 100.f, false, 0, 10.f, 1.f, XOR( L"%" ) );
		rot_speed_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		rot_speed_stand.AddShowCallback( callbacks::IsStandYawRotate );
		RegisterElement( &rot_speed_stand );

		rand_update_stand.setup( "", XOR( "rand_update_stnd" ), 0.f, 1.f, false, 1, 0.f, 0.1f );
		rand_update_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		rand_update_stand.AddShowCallback( callbacks::IsStandYawRnadom );
		RegisterElement( &rand_update_stand );

		dir_stand.setup( XOR( "direction" ), XOR( "dir_stnd" ), { XOR( "auto" ), XOR( "backwards" ), XOR( "left" ), XOR( "right" ), XOR( "custom" ) } );
		dir_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		dir_stand.AddShowCallback( callbacks::HasStandYaw );
		RegisterElement( &dir_stand );

		dir_time_stand.setup( "", XOR( "dir_time_stnd" ), 0.f, 10.f, false, 0, 0.f, 1.f, XOR( L"s" ) );
		dir_time_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		dir_time_stand.AddShowCallback( callbacks::HasStandYaw );
		dir_time_stand.AddShowCallback( callbacks::IsStandDirAuto );
		RegisterElement( &dir_time_stand );

		dir_custom_stand.setup( "", XOR( "dir_custom_stnd" ), -180.f, 180.f, false, 0, 0.f, 5.f, XOR( L"°" ) );
		dir_custom_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		dir_custom_stand.AddShowCallback( callbacks::HasStandYaw );
		dir_custom_stand.AddShowCallback( callbacks::IsStandDirCustom );
		RegisterElement( &dir_custom_stand );

		base_angle_stand.setup( XOR( "base angle" ), XOR( "base_angle_stand" ), { XOR( "off" ), XOR( "static" ), XOR( "away crosshair" ), XOR( "away distance" ) } );
		base_angle_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		base_angle_stand.AddShowCallback( callbacks::HasStandYaw );
		RegisterElement( &base_angle_stand );

		dir_lock.setup( XOR( "lock direction" ), XOR( "dir_lock" ) );
		dir_lock.AddShowCallback( callbacks::IsAntiAimModeStand );
		dir_lock.AddShowCallback( callbacks::HasStandYaw );
		RegisterElement( &dir_lock );

		body_fake_stand.setup( XOR( "fake body" ), XOR( "body_fake_stnd" ), { XOR( "off" ), XOR( "left" ), XOR( "right" ), XOR( "opposite" ), XOR( "z" ) } );
		body_fake_stand.AddShowCallback( callbacks::IsAntiAimModeStand );
		body_fake_stand.AddShowCallback( callbacks::HasStandYaw );
		RegisterElement( &body_fake_stand );

		// walk.
		pitch_walk.setup( XOR( "pitch" ), XOR( "pitch_walk" ), { XOR( "off" ), XOR( "down" ), XOR( "up" ), XOR( "random" ), XOR( "ideal" ) } );
		pitch_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		RegisterElement( &pitch_walk );

		yaw_walk.setup( XOR( "yaw" ), XOR( "yaw_walk" ), { XOR( "off" ), XOR( "direction" ), XOR( "jitter" ), XOR( "rotate" ), XOR( "random" ) } );
		yaw_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		RegisterElement( &yaw_walk );

		jitter_range_walk.setup( "", XOR( "jitter_range_walk" ), 1.f, 180.f, false, 0, 45.f, 5.f, XOR( L"°" ) );
		jitter_range_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		jitter_range_walk.AddShowCallback( callbacks::IsWalkYawJitter );
		RegisterElement( &jitter_range_walk );

		rot_range_walk.setup( "", XOR( "rot_range_walk" ), 0.f, 360.f, false, 0, 360.f, 5.f, XOR( L"°" ) );
		rot_range_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		rot_range_walk.AddShowCallback( callbacks::IsWalkYawRotate );
		RegisterElement( &rot_range_walk );

		rot_speed_walk.setup( "", XOR( "rot_speed_walk" ), 1.f, 100.f, false, 0, 10.f, 1.f, XOR( L"%" ) );
		rot_speed_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		rot_speed_walk.AddShowCallback( callbacks::IsWalkYawRotate );
		RegisterElement( &rot_speed_walk );

		rand_update_walk.setup( "", XOR( "rand_update_walk" ), 0.f, 1.f, false, 1, 0.f, 0.1f );
		rand_update_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		rand_update_walk.AddShowCallback( callbacks::IsWalkYawRandom );
		RegisterElement( &rand_update_walk );

		dir_walk.setup( XOR( "direction" ), XOR( "dir_walk" ), { XOR( "auto" ), XOR( "backwards" ), XOR( "left" ), XOR( "right" ), XOR( "custom" ) } );
		dir_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		dir_walk.AddShowCallback( callbacks::WalkHasYaw );
		RegisterElement( &dir_walk );

		dir_time_walk.setup( "", XOR( "dir_time_walk" ), 0.f, 10.f, false, 0, 0.f, 1.f, XOR( L"s" ) );
		dir_time_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		dir_time_walk.AddShowCallback( callbacks::WalkHasYaw );
		dir_time_walk.AddShowCallback( callbacks::IsWalkDirAuto );
		RegisterElement( &dir_time_walk );

		dir_custom_walk.setup( "", XOR( "dir_custom_walk" ), -180.f, 180.f, false, 0, 0.f, 5.f, XOR( L"°" ) );
		dir_custom_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		dir_custom_walk.AddShowCallback( callbacks::WalkHasYaw );
		dir_custom_walk.AddShowCallback( callbacks::IsWalkDirCustom );
		RegisterElement( &dir_custom_walk );

		base_angle_walk.setup( XOR( "base angle" ), XOR( "base_angle_walk" ), { XOR( "off" ), XOR( "static" ), XOR( "away crosshair" ), XOR( "away distance" ) } );
		base_angle_walk.AddShowCallback( callbacks::IsAntiAimModeWalk );
		base_angle_walk.AddShowCallback( callbacks::WalkHasYaw );
		RegisterElement( &base_angle_walk );

		// air.
		pitch_air.setup( XOR( "pitch" ), XOR( "pitch_air" ), { XOR( "off" ), XOR( "down" ), XOR( "up" ), XOR( "random" ), XOR( "ideal" ) } );
		pitch_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		RegisterElement( &pitch_air );

		yaw_air.setup( XOR( "yaw" ), XOR( "yaw_air" ), { XOR( "off" ), XOR( "direction" ), XOR( "jitter" ), XOR( "rotate" ), XOR( "random" ) } );
		yaw_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		RegisterElement( &yaw_air );

		jitter_range_air.setup( "", XOR( "jitter_range_air" ), 1.f, 180.f, false, 0, 45.f, 5.f, XOR( L"°" ) );
		jitter_range_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		jitter_range_air.AddShowCallback( callbacks::IsAirYawJitter );
		RegisterElement( &jitter_range_air );

		rot_range_air.setup( "", XOR( "rot_range_air" ), 0.f, 360.f, false, 0, 360.f, 5.f, XOR( L"°" ) );
		rot_range_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		rot_range_air.AddShowCallback( callbacks::IsAirYawRotate );
		RegisterElement( &rot_range_air );

		rot_speed_air.setup( "", XOR( "rot_speed_air" ), 1.f, 100.f, false, 0, 10.f, 1.f, XOR( L"%" ) );
		rot_speed_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		rot_speed_air.AddShowCallback( callbacks::IsAirYawRotate );
		RegisterElement( &rot_speed_air );

		rand_update_air.setup( "", XOR( "rand_update_air" ), 0.f, 1.f, false, 1, 0.f, 0.1f );
		rand_update_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		rand_update_air.AddShowCallback( callbacks::IsAirYawRandom );
		RegisterElement( &rand_update_air );

		dir_air.setup( XOR( "direction" ), XOR( "dir_air" ), { XOR( "auto" ), XOR( "backwards" ), XOR( "left" ), XOR( "right" ), XOR( "custom" ) } );
		dir_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		dir_air.AddShowCallback( callbacks::AirHasYaw );
		RegisterElement( &dir_air );

		dir_time_air.setup( "", XOR( "dir_time_air" ), 0.f, 10.f, false, 0, 0.f, 1.f, XOR( L"s" ) );
		dir_time_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		dir_time_air.AddShowCallback( callbacks::AirHasYaw );
		dir_time_air.AddShowCallback( callbacks::IsAirDirAuto );
		RegisterElement( &dir_time_air );

		dir_custom_air.setup( "", XOR( "dir_custom_air" ), -180.f, 180.f, false, 0, 0.f, 5.f, XOR( L"°" ) );
		dir_custom_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		dir_custom_air.AddShowCallback( callbacks::AirHasYaw );
		dir_custom_air.AddShowCallback( callbacks::IsAirDirCustom );
		RegisterElement( &dir_custom_air );

		base_angle_air.setup( XOR( "base angle" ), XOR( "base_angle_air" ), { XOR( "off" ), XOR( "static" ), XOR( "away crosshair" ), XOR( "away distance" ) } );
		base_angle_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		base_angle_air.AddShowCallback( callbacks::AirHasYaw );
		RegisterElement( &base_angle_air );

		body_fake_air.setup( XOR( "fake body" ), XOR( "body_fake_air" ), { XOR( "off" ), XOR( "left" ), XOR( "right" ), XOR( "opposite" ) } );
		body_fake_air.AddShowCallback( callbacks::IsAntiAimModeAir );
		body_fake_air.AddShowCallback( callbacks::AirHasYaw );
		RegisterElement( &body_fake_air );

		// col2.
		fake_yaw.setup( XOR( "fake yaw" ), XOR( "fake_yaw" ), { XOR( "off" ), XOR( "default" ), XOR( "relative" ), XOR( "jitter" ), XOR( "rotate" ), XOR( "random" ), XOR( "local view" ) } );
		RegisterElement( &fake_yaw, 1 );

		fake_relative.setup( "", XOR( "fake_relative" ), -90.f, 90.f, false, 0, 0.f, 5.f, XOR( L"°" ) );
		fake_relative.AddShowCallback( callbacks::IsFakeAntiAimRelative );
		RegisterElement( &fake_relative, 1 );

		fake_jitter_range.setup( "", XOR( "fake_jitter_range" ), 1.f, 90.f, false, 0, 0.f, 5.f, XOR( L"°" ) );
		fake_jitter_range.AddShowCallback( callbacks::IsFakeAntiAimJitter );
		RegisterElement( &fake_jitter_range, 1 );

		// col 2.
		lag_enable.setup( XOR( "fake-lag" ), XOR( "lag_enable" ) );
		RegisterElement( &lag_enable, 1 );

		lag_active.setup( "", XOR( "lag_active" ), { XOR("stand"), XOR("move"), XOR("air"), XOR("crouch")}, false);
		RegisterElement( &lag_active, 1 );

		lag_mode.setup( "", XOR( "lag_mode" ), { XOR( "max" ), XOR( "break" ), XOR( "random" ), XOR( "break step" ) }, false );
		RegisterElement( &lag_mode, 1 );

		lag_limit.setup( XOR( "limit" ), XOR( "lag_limit" ), 2, 14, true, 0, 2, 1.f );
		RegisterElement( &lag_limit, 1 );

		lag_land.setup( XOR( "on land" ), XOR( "lag_land" ) );
		RegisterElement( &lag_land, 1 );

		manul_antiaim.setup(XOR("manual anti-aim indicators"), XOR("manul_antiaim"));
		RegisterElement(&manul_antiaim, 1);

		color_manul_antiaim.setup(XOR("manual anti-aim color"), XOR("color_manul_antiaim"), colors::burgundy);
		RegisterElement(&color_manul_antiaim, 1);

		left.setup(XOR("left"), XOR("left"));
		left.SetToggleCallback(callbacks::ToggleLeft);
		RegisterElement(&left);

		right.setup(XOR("right"), XOR("right"));
		right.SetToggleCallback(callbacks::ToggleRight);
		RegisterElement(&right);

		backward.setup(XOR("backward"), XOR("backward"));
		backward.SetToggleCallback(callbacks::ToggleBack);
		RegisterElement(&backward);

		forward.setup(XOR("forward"), XOR("forward"));
		forward.SetToggleCallback(callbacks::ToggleForward);
		RegisterElement(&forward);
	}
};

class PlayersTab : public Tab {
public:
	Checkbox teammates;
	Checkbox box;
	Colorpicker   box_enemy;
	Slider name_esp_alpha;
	Slider box_color_alpha;
	Checkbox      dormant;
	Checkbox      offscreen;
	Colorpicker   offscreen_color;
	Checkbox      footstep;
	Colorpicker   footstepcol;
	Checkbox name;
	Colorpicker   name_color;
	Slider name_color_alpha;
	Checkbox health;
	Slider ammo_alpha;
	MultiDropdown flags_enemy;
	Checkbox weaponicon;
	Checkbox weapontext;
	Colorpicker weaponcolor;
	Slider weaponcoloralpha;
	Checkbox      ammo;
	Checkbox distance;
	Colorpicker   ammo_color;
	Checkbox      lby_update;
	Colorpicker   lby_update_color;
	Checkbox     hitmarker;
	Checkbox     hitmarker_sound;
	MultiDropdown flags_friendly;
	Slider box_esp_alpha;

	// col2.
	Checkbox skeleton;
	Colorpicker   skeleton_enemy;
	MultiDropdown rainbow_visuals;
	Checkbox glow;
	Colorpicker   glow_enemy;
	Slider        glow_blend;
	Slider lby_timer_alpha;
	Slider skeleton_alpha;
	Dropdown	  chams_entity_selection;
	Checkbox      chams_local;
	Checkbox      fake_indicator;
	Dropdown	  chams_local_mat;
	Colorpicker   chams_local_col;
	Colorpicker   chams_local2_col;
	Slider        chams_local_blend;
	Checkbox      chams_local_scope;
	Slider		  chams_local_scope_blend;

	MultiDropdown chams_enemy;
	Dropdown	  chams_enemy_mat;
	Colorpicker   chams_enemy_vis;
	Colorpicker   chams_enemy2_vis;
	Colorpicker   chams_enemy_invis;
	Colorpicker   chams_enemy2_invis;
	Colorpicker   chams_enemy_reflectivity;
	Slider        chams_reflectivity;
	Slider        chams_enemy_shine;
	Slider        chams_enemy_rim;
	Slider        chams_enemy_blend;
	Checkbox      chams_enemy_history;
	Slider        chams_enemy_blend2;
	Dropdown	  chams_enemy_history_mat;
	Colorpicker   chams_enemy_history_col;
	Slider        chams_enemy_history_blend;
	Checkbox      history_skeleton;
	Colorpicker   history_skeleton_col;

	MultiDropdown chams_friendly;
	Dropdown	  chams_friendly_mat;
	Colorpicker   chams_friendly_vis;
	Colorpicker   chams_friendly_invis;
	Slider        chams_friendly_blend;

	Checkbox      chams_fake;
	Dropdown	  chams_fake_mat;
	Colorpicker   chams_fake_col;
	Colorpicker   chams_fake_glow_col;
	Slider        chams_fake_blend;

	Checkbox      chams_shot;
	Dropdown      chams_shot_mat;
	Colorpicker   chams_shot_col;
	Slider        chams_shot_blend;
	Slider        chams_shot_fadetime;
	Slider rainbow_speed;
	Checkbox      weapon_chams;
	Dropdown      weapon_chams_mat;
	Colorpicker   weapon_chams_col;
	Colorpicker   weapon_chams_col2;
	Slider        weapon_chams_blend;
	Slider        weapon_chams_blend2;



public:
	void init() {
		SetTitle(XOR("players"));

		teammates.setup(XOR("teammates"), XOR("teammates"));
		RegisterElement(&teammates);

		dormant.setup(XOR("dormant"), XOR("dormant"));
		RegisterElement(&dormant);

		box.setup(XOR("bounding box"), XOR("box"));
		RegisterElement(&box);

		box_enemy.setup(XOR("bounding box color"), XOR("box_enemy"), { 255, 255, 255, 180 });
		box_enemy.AddShowCallback(callbacks::boxon);
		RegisterElement(&box_enemy);

		box_esp_alpha.setup("box_esp_alpha", XOR("box_esp_alpha"), 0.f, 255.f, false, 0, 170.f, 1.f, XOR(L"%"));
		box_esp_alpha.AddShowCallback(callbacks::boxon);
		RegisterElement(&box_esp_alpha);

		health.setup(XOR("health bar"), XOR("health"));
		RegisterElement(&health);

		name.setup(XOR("name esp"), XOR("name"));
		RegisterElement(&name);

		name_color.setup(XOR("name esp color"), XOR("name_color"), { 255, 255, 255 });
		name_color.AddShowCallback(callbacks::nameon);
		RegisterElement(&name_color);

		name_esp_alpha.setup("name_esp_alpha", XOR("name_esp_alpha"), 0.f, 255.f, false, 0, 150.f, 1.f, XOR(L"%"));
		name_esp_alpha.AddShowCallback(callbacks::nameon);
		RegisterElement(&name_esp_alpha);

		flags_enemy.setup(XOR("flags enemy"), XOR("flags_enemy"), { XOR("money"), XOR("reload"), XOR("armor"), XOR("flashed"), XOR("zoom"), XOR("bomb"), XOR("fake"), XOR("lethal")});
		RegisterElement(&flags_enemy);

		weapontext.setup(XOR("weapon text"), XOR("weapontext"));
		RegisterElement(&weapontext);

		weaponicon.setup(XOR("weapon icon"), XOR("weaponicon"));
		RegisterElement(&weaponicon);

		weaponcolor.setup(XOR("weapon icon color"), XOR("weaponcolor"), { 255, 255, 255 });
		weaponcolor.AddShowCallback(callbacks::weaponiconon);
		RegisterElement(&weaponcolor);

		ammo.setup(XOR("ammo"), XOR("ammo"));
		RegisterElement(&ammo);

		ammo_color.setup(XOR("ammo color"), XOR("ammo_color"), { 80, 140, 200, 235 });
		ammo_color.AddShowCallback(callbacks::ammpespon);
		RegisterElement(&ammo_color);

		distance.setup(XOR("distance"), XOR("distance"));
		RegisterElement(&distance);

		lby_update.setup(XOR("lby timer"), XOR("lby_update"));
		RegisterElement(&lby_update);

		lby_update_color.setup(XOR("lby timer color"), XOR("lby_update_color"), { 255, 0, 255 });
		lby_update_color.AddShowCallback(callbacks::lbytimeron);
		RegisterElement(&lby_update_color);

		glow.setup(XOR("glow"), XOR("glow"));
		RegisterElement(&glow);

		glow_enemy.setup(XOR("glow color"), XOR("glow_enemy"), { 180, 57, 119 });
		glow_enemy.AddShowCallback(callbacks::glowon);
		RegisterElement(&glow_enemy);

		glow_blend.setup("", XOR("glow_blend"), 0.f, 100.f, false, 0, 30.f, 1.f, XOR(L"%"));
		glow_blend.AddShowCallback(callbacks::glowon);
		RegisterElement(&glow_blend);

		hitmarker.setup(XOR("hitmarker"), XOR("hitmarker"));
		RegisterElement(&hitmarker);

		hitmarker_sound.setup(XOR("hitmarker sound"), XOR("hitmarker_sound"));
		RegisterElement(&hitmarker_sound);

		skeleton.setup(XOR("skeleton"), XOR("skeleton"));
		RegisterElement(&skeleton);

		skeleton_enemy.setup(XOR("skeleton color"), XOR("skeleton_enemy"), { 255, 255, 170 });
		skeleton_enemy.AddShowCallback(callbacks::skeleton);
		RegisterElement(&skeleton_enemy);

		skeleton_alpha.setup("", XOR("skeleton_alpha"), 0.f, 255, false, 0, 255, 1.f, XOR(L"%"));
		skeleton_alpha.AddShowCallback(callbacks::skeleton);
		RegisterElement(&skeleton_alpha);

		offscreen.setup(XOR("out of pov arrow"), XOR("offscreen"));
		RegisterElement(&offscreen);

		offscreen_color.setup(XOR("offscreen esp color"), XOR("offscreen_color"), colors::white);
		offscreen_color.AddShowCallback(callbacks::outofpov);
		RegisterElement(&offscreen_color);

		footstep.setup(XOR("visualize sounds"), XOR("footstep"));
		RegisterElement(&footstep);

		footstepcol.setup(XOR("visualize sounds color"), XOR("footstepcol"), { 250, 60, 60 });
		footstepcol.AddShowCallback(callbacks::soundesp);
		RegisterElement(&footstepcol);

		// col2

		rainbow_visuals.setup(XOR("rainbow visual elements"), XOR("rainbow_visuals"), {XOR("glow"), XOR("visible chams"), XOR("invisible chams"), XOR("history chams"), XOR("shot chams"), XOR("weapon chams 1"), XOR("weapon chams 2") });
		RegisterElement(&rainbow_visuals, 1);

		rainbow_speed.setup("rainbow speed", XOR("rainbow_speed"), 1.f, 50.f, true, 0, 1, 1, XOR(L""));
		RegisterElement(&rainbow_speed, 1);

		chams_entity_selection.setup("chams selection", XOR("chams_entity_selection"), { XOR("local"), XOR("enemy"), XOR("friendly"), XOR("history"), XOR("fake"), XOR("shot"), XOR("weapon") });
		RegisterElement(&chams_entity_selection, 1);

		chams_local.setup(XOR("chams local"), XOR("chams_local"));
		chams_local.AddShowCallback(callbacks::IsChamsSelection0);
		RegisterElement(&chams_local, 1);

		chams_local_mat.setup(XOR("chams local material"), XOR("chams_local_mat"), { XOR("material"), XOR("flat"), XOR("metallic"), XOR("gloss"), XOR("glow") });
		chams_local_mat.AddShowCallback(callbacks::IsChamsSelection0);
		RegisterElement(&chams_local_mat, 1);

		chams_local_col.setup(XOR("color"), XOR("chams_local_col"), { 50, 50, 50 });
		chams_local_col.AddShowCallback(callbacks::IsChamsSelection0);
		RegisterElement(&chams_local_col, 1);

		chams_local2_col.setup(XOR("glow color"), XOR("chams_local2_col"), { 9, 45, 59 });
		chams_local2_col.AddShowCallback(callbacks::LocalGlowChamsSelection);
		RegisterElement(&chams_local2_col, 1);

		chams_local_blend.setup("", XOR("chams_local_blend"), 0.f, 100.f, false, 0, 100.f, 1.f, XOR(L"%"));
		chams_local_blend.AddShowCallback(callbacks::IsChamsSelection0);
		RegisterElement(&chams_local_blend, 1);

		chams_local_scope.setup(XOR("blend when scoped"), XOR("chams_local_scope"));
		chams_local_scope.AddShowCallback(callbacks::IsChamsSelection0);
		RegisterElement(&chams_local_scope, 1);

		chams_local_scope_blend.setup("", XOR("chams_local_scope_blend"), 0.f, 100.f, false, 0, 100.f, 1.f, XOR(L"%"));
		chams_local_scope_blend.AddShowCallback(callbacks::IsChamsSelection0);
		RegisterElement(&chams_local_scope_blend, 1);

		chams_enemy.setup(XOR("chams enemy"), XOR("chams_enemy"), { XOR("visible"), XOR("invisible") });
		chams_enemy.AddShowCallback(callbacks::IsChamsSelection1);
		RegisterElement(&chams_enemy, 1);

		chams_enemy_mat.setup(XOR("chams enemy material"), XOR("chams_enemy_mat"), { XOR("material"), XOR("flat"), XOR("metallic"), XOR("gloss"), XOR("glow"), XOR("outline glow") });
		chams_enemy_mat.AddShowCallback(callbacks::IsChamsSelection1);
		RegisterElement(&chams_enemy_mat, 1);

		chams_enemy_vis.setup(XOR("color visible"), XOR("chams_enemy_vis"), { 150, 200, 60 });
		chams_enemy_vis.AddShowCallback(callbacks::IsChamsSelection1);
		RegisterElement(&chams_enemy_vis, 1);

		chams_enemy2_vis.setup(XOR("glow color"), XOR("chams_enemy2_vis"), { 255, 255, 200 });
		chams_enemy2_vis.AddShowCallback(callbacks::EnemyGlowChamsSelection);
		RegisterElement(&chams_enemy2_vis, 1);

		chams_enemy_invis.setup(XOR("color invisible"), XOR("chams_enemy_invis"), { 60, 120, 180 });
		chams_enemy_invis.AddShowCallback(callbacks::IsChamsSelection1);
		RegisterElement(&chams_enemy_invis, 1);

		chams_enemy2_invis.setup(XOR("glow color"), XOR("chams_enemy2_invis"), { 255, 255, 200, 180 });
		chams_enemy2_invis.AddShowCallback(callbacks::EnemyGlowChamsSelection);
		RegisterElement(&chams_enemy2_invis, 1);

		chams_enemy_blend.setup("visible", XOR("chams_enemy_blend"), 0.f, 100.f, true, 0, 100.f, 1.f, XOR(L"%"));
		chams_enemy_blend.AddShowCallback(callbacks::IsChamsSelection1);
		RegisterElement(&chams_enemy_blend, 1);

		chams_enemy_blend2.setup("invis", XOR("chams_enemy_blend2"), 0.f, 100.f, true, 0, 50.f, 1.f, XOR(L"%"));
		chams_enemy_blend2.AddShowCallback(callbacks::IsChamsSelection1);
		RegisterElement(&chams_enemy_blend2, 1);

		chams_enemy_reflectivity.setup(XOR("reflectivity color"), XOR("chams_enemy_reflectivity"), { 255,178,00 });
		chams_enemy_reflectivity.AddShowCallback(callbacks::EnemyMetallicChamsSelection);
		RegisterElement(&chams_enemy_reflectivity, 1);

		chams_reflectivity.setup("chams reflectivity", XOR("chams_reflectivity"), 0.f, 100.f, true, 0, 67.f, 1.f, XOR(L"%"));
		chams_reflectivity.AddShowCallback(callbacks::EnemyMetallicChamsSelection);
		RegisterElement(&chams_reflectivity, 1);

		chams_enemy_shine.setup("chams enemy shine", XOR("chams_enemy_shine"), 0.f, 100.f, true, 0, 10.f, 1.f, XOR(L"%"));
		chams_enemy_shine.AddShowCallback(callbacks::EnemyMetallicChamsSelection);
		RegisterElement(&chams_enemy_shine, 1);

		chams_enemy_rim.setup("chams enemy rim", XOR("chams_enemy_rim"), 0.f, 100.f, true, 0, 10.f, 1.f, XOR(L"%"));
		chams_enemy_rim.AddShowCallback(callbacks::EnemyMetallicChamsSelection);
		RegisterElement(&chams_enemy_rim, 1);

		chams_friendly.setup(XOR("chams friendly"), XOR("chams_friendly"), { XOR("visible"), XOR("invisible") });
		chams_friendly.AddShowCallback(callbacks::IsChamsSelection2);
		RegisterElement(&chams_friendly, 1);

		chams_friendly_mat.setup(XOR("chams friendly material"), XOR("chams_friendly_mat"), { XOR("material"), XOR("flat"), XOR("metallic"), XOR("gloss"), XOR("glow"), XOR("outline glow") });
		chams_friendly_mat.AddShowCallback(callbacks::IsChamsSelection2);
		RegisterElement(&chams_friendly_mat, 1);

		chams_friendly_vis.setup(XOR("color visible"), XOR("chams_friendly_vis"), { 255, 200, 0 });
		chams_friendly_vis.AddShowCallback(callbacks::IsChamsSelection2);
		RegisterElement(&chams_friendly_vis, 1);

		chams_friendly_invis.setup(XOR("color invisible"), XOR("chams_friendly_invis"), { 255, 50, 0 });
		chams_friendly_invis.AddShowCallback(callbacks::IsChamsSelection2);
		RegisterElement(&chams_friendly_invis, 1);

		chams_friendly_blend.setup("", XOR("chams_friendly_blend"), 0.f, 100.f, false, 0, 100.f, 1.f, XOR(L"%"));
		chams_friendly_blend.AddShowCallback(callbacks::IsChamsSelection2);
		RegisterElement(&chams_friendly_blend, 1);

		chams_enemy_history.setup(XOR("chams history"), XOR("chams_history"));
		chams_enemy_history.AddShowCallback(callbacks::IsChamsSelection3);
		RegisterElement(&chams_enemy_history, 1);

		chams_enemy_history_mat.setup(XOR("chams history material"), XOR("chams_enemy_history_mat"), { XOR("material"), XOR("flat"), XOR("metallic"), XOR("gloss"), XOR("glow"), XOR("outline glow") });
		chams_enemy_history_mat.AddShowCallback(callbacks::IsChamsSelection3);
		RegisterElement(&chams_enemy_history_mat, 1);

		chams_enemy_history_col.setup(XOR("color"), XOR("chams_history_col"), { 0, 0, 0 });
		chams_enemy_history_col.AddShowCallback(callbacks::IsChamsSelection3);
		RegisterElement(&chams_enemy_history_col, 1);

		chams_enemy_history_blend.setup("", XOR("chams_history_blend"), 0.f, 100.f, false, 0, 100.f, 1.f, XOR(L"%"));
		chams_enemy_history_blend.AddShowCallback(callbacks::IsChamsSelection3);
		RegisterElement(&chams_enemy_history_blend, 1);

		history_skeleton.setup(XOR("history skeleton"), XOR("history_skeleton"));
		history_skeleton.AddShowCallback(callbacks::IsChamsSelection3);
		RegisterElement(&history_skeleton, 1);

		history_skeleton_col.setup(XOR("color"), XOR("history_skeleton_col"), { 255, 255, 255 });
		history_skeleton_col.AddShowCallback(callbacks::IsChamsSelection3);
		RegisterElement(&history_skeleton_col, 1);

		chams_fake.setup(XOR("fake chams"), XOR("chams_fake"));
		chams_fake.AddShowCallback(callbacks::IsChamsSelection4);
		RegisterElement(&chams_fake, 1);

		chams_fake_mat.setup(XOR("chams material"), XOR("chams_fake_mat"), { XOR("material"), XOR("flat"), XOR("metallic"), XOR("gloss"), XOR("glow"), XOR("outline glow") });
		chams_fake_mat.AddShowCallback(callbacks::IsChamsSelection4);
		RegisterElement(&chams_fake_mat, 1);

		chams_fake_col.setup(XOR("color"), XOR("chams_fake_col"), { 255, 200, 0 });
		chams_fake_col.AddShowCallback(callbacks::IsChamsSelection4);
		RegisterElement(&chams_fake_col, 1);

		chams_fake_glow_col.setup(XOR("glow color"), XOR("chams_fake_glow_col"), { 255, 200, 0 });
		chams_fake_glow_col.AddShowCallback(callbacks::IsChamsSelection4);
		chams_fake_glow_col.AddShowCallback(callbacks::FakeGlowChamsSelection);
		RegisterElement(&chams_fake_glow_col, 1);

		chams_fake_blend.setup("", XOR("chams_fake_blend"), 0.f, 100.f, false, 0, 100.f, 1.f, XOR(L"%"));
		chams_fake_blend.AddShowCallback(callbacks::IsChamsSelection4);
		RegisterElement(&chams_fake_blend, 1);


		chams_shot.setup(XOR("shot chams"), XOR("chams_shot"));
		chams_shot.AddShowCallback(callbacks::IsChamsSelection5);
		RegisterElement(&chams_shot, 1);

		chams_shot_mat.setup(XOR("chams material"), XOR("chams_shot_mat"), { XOR("material"), XOR("flat"), XOR("metallic"), XOR("gloss"), XOR("glow"), XOR("outline glow") });
		chams_shot_mat.AddShowCallback(callbacks::IsChamsSelection5);
		RegisterElement(&chams_shot_mat, 1);

		chams_shot_col.setup(XOR("color"), XOR("chams_shot_col"), { 255, 255, 255 });
		chams_shot_col.AddShowCallback(callbacks::IsChamsSelection5);
		RegisterElement(&chams_shot_col, 1);

		chams_shot_blend.setup("", XOR("chams_shot_blend"), 1.f, 255.f, false, 0, 255.f, 1.f, XOR(L"%"));
		chams_shot_blend.AddShowCallback(callbacks::IsChamsSelection5);
		RegisterElement(&chams_shot_blend, 1);

		chams_shot_fadetime.setup("fade time", XOR("chams_shot_fadetime"), 0.0f, 10, true, 1, 0.3f, 0.1f, XOR(L"s"));
		chams_shot_fadetime.AddShowCallback(callbacks::IsChamsSelection5);
		RegisterElement(&chams_shot_fadetime, 1);

		weapon_chams.setup(XOR("weapon chams"), XOR("weapon_chams"));
		weapon_chams.AddShowCallback(callbacks::IsChamsSelection6);
		RegisterElement(&weapon_chams, 1);

		weapon_chams_mat.setup(XOR("weapon material"), XOR("weapon_chams_mat"), { XOR("material"), XOR("flat"), XOR("metallic"), XOR("gloss"), XOR("glow"), XOR("outline glow"), XOR("styles")});
		weapon_chams_mat.AddShowCallback(callbacks::IsChamsSelection6);
		RegisterElement(&weapon_chams_mat, 1);

		weapon_chams_col.setup(XOR("weapon color"), XOR("weapon_chams_col"), { 0,0,0 });
		weapon_chams_col.AddShowCallback(callbacks::IsChamsSelection6);
		RegisterElement(&weapon_chams_col, 1);

		weapon_chams_blend.setup("", XOR("weapon_chams_blend"), 1.f, 255.f, false, 0, 255.f, 1.f, XOR(L"%"));
		weapon_chams_blend.AddShowCallback(callbacks::IsChamsSelection6);
		RegisterElement(&weapon_chams_blend, 1);

		weapon_chams_col2.setup(XOR("weapon color 2"), XOR("weapon_chams_col2"), { 255, 255, 255 });
		weapon_chams_col2.AddShowCallback(callbacks::IsChamsSelection6);
		RegisterElement(&weapon_chams_col2, 1);

		weapon_chams_blend2.setup("", XOR("weapon_chams_blend2"), 1.f, 255.f, false, 0, 255.f, 1.f, XOR(L"%"));
		weapon_chams_blend2.AddShowCallback(callbacks::IsChamsSelection6);
		RegisterElement(&weapon_chams_blend2, 1);

	}
};

class VisualsTab : public Tab {
public:
	Checkbox      items;
	Checkbox      itemsglow;
	Checkbox items_distance;
	Checkbox      ammo;
	Colorpicker   item_color;
	Slider        item_color_alpha;
	Slider        glow_color_alpha;;
	Colorpicker   ammo_color;
	Slider        ammo_color_alpha;
	Checkbox      proj;
	Colorpicker   proj_color;
	Checkbox proj_range;
	Colorpicker proj_range_color;
	Colorpicker   proj_col;
	Slider        proj_col_slider;
	Checkbox planted_c4;
	Colorpicker         bomb_col;
	Slider        bomb_col_slider;
	Slider        bomb_col_glow_slider;
	Slider        tracers_color_alpha;
	Colorpicker   tracers_color;
	Checkbox      disableteam;
	MultiDropdown	  world;
	Colorpicker        nightcolor;
	Checkbox      transparent_props;
	Slider		  transparent_props_amount;
	Colorpicker   propscolor;
	Colorpicker   ambient_color;
	Slider ambient_alpha;
	Slider        walls_amount;
	Checkbox      enemy_radar;
	Colorpicker penetrable_color;
	Colorpicker unpenetrable_color;
	// col2.
	MultiDropdown removals;
	Checkbox      fov_scoped;
	Checkbox      spectators;
	Checkbox      force_xhair;
	Checkbox postprocess;
	Checkbox      pen_crosshair;
	MultiDropdown indicators;
	Checkbox      tracers;
	Checkbox      impact_beams;
	Dropdown      beam_type;
	Colorpicker   impact_beams_color;
	Colorpicker   impact_beams_hurt_color;
	Slider        impact_beams_time;
	Keybind       thirdperson;
	Slider speed_beam;
	Slider alpha2;
	Checkbox bullet_impacts;

public:
	void init( ) {
		SetTitle( XOR( "visuals" ) );

		enemy_radar.setup(XOR("radar"), XOR("enemy_radar"));
		RegisterElement(&enemy_radar);

		//items.setup(XOR("dropped weapons text"), XOR("items"));
		//RegisterElement(&items);

		//itemsglow.setup(XOR("dropped weapons glow"), XOR("itemsglow"));
		//RegisterElement(&itemsglow);

		//glow_color_alpha.setup("", XOR("glow_color_alpha"), 0.f, 100.f, false, 0, 85.f, 1.f, XOR(L"%"));
		//glow_color_alpha.AddShowCallback(callbacks::droppedglowewpaon);
		//RegisterElement(&glow_color_alpha);

		//items_distance.setup(XOR("dropped weapons distance"), XOR("items_distance"));
		//RegisterElement(&items_distance);

		//item_color.setup(XOR("dropped weapons color"), XOR("item_color"), colors::white);
		//item_color.AddShowCallback(callbacks::droppedwpnson);
		//RegisterElement(&item_color);

		//item_color_alpha.setup("", XOR("item_color_alpha"), 0.f, 255, false, 0, 180, 1.f, XOR(L"%"));
		//item_color_alpha.AddShowCallback(callbacks::droppedwpnson);
		//RegisterElement(&item_color_alpha);

		//ammo.setup(XOR("dropped weapons ammo"), XOR("ammo"));
		//RegisterElement(&ammo);

		//ammo_color.setup(XOR("weapon ammo color"), XOR("ammo_color"), colors::white);
		//ammo_color.AddShowCallback(callbacks::ammoon);
		//RegisterElement(&ammo_color);

		//ammo_color_alpha.setup("", XOR("ammo_color_alpha"), 0.f, 255, false, 0, 180, 1.f, XOR(L"%"));
		//ammo_color_alpha.AddShowCallback(callbacks::ammoon);
		//RegisterElement(&ammo_color_alpha);

		proj.setup(XOR("projectiles"), XOR("proj"));
		RegisterElement(&proj);

		proj_color.setup(XOR("color"), XOR("proj_color"), colors::white);
		proj_color.AddShowCallback(callbacks::IsProjectiles);
		RegisterElement(&proj_color);

		proj_range.setup(XOR("projectile timer"), XOR("proj_range"));
		proj_range.AddShowCallback(callbacks::IsProjectiles);
		RegisterElement(&proj_range);

		proj_range_color.setup(XOR("timer color"), XOR("proj_range_color"), colors::burgundy);
		proj_range_color.AddShowCallback(callbacks::IsProjectiles);
		RegisterElement(&proj_range_color);

		proj_col.setup(XOR("grenades color"), XOR("proj_col"), { 255, 72, 0 });
		proj_col.AddShowCallback(callbacks::grenadeson);
		RegisterElement(&proj_col);

		//proj_col_slider.setup("", XOR("proj_col_slider"), 0.f, 100.f, false, 0, 100, 1.f, XOR(L"%"));
		//proj_col_slider.AddShowCallback(callbacks::grenadeson);
		//RegisterElement(&proj_col_slider);

		force_xhair.setup(XOR("crosshair"), XOR("force_xhair"));
		RegisterElement(&force_xhair);

		planted_c4.setup(XOR("bomb"), XOR("planted_c4"));
		RegisterElement(&planted_c4);

		bomb_col.setup(XOR("bomb color"), XOR("bomb_col"), { 151, 200, 60 });
		bomb_col.AddShowCallback(callbacks::bombon);
		RegisterElement(&bomb_col);

		//bomb_col_glow_slider.setup("bomb glow color", XOR("bomb_col_glow_slider"), 0.f, 100.f, true, 0, 80, 1.f, XOR(L"%"));
		//bomb_col_glow_slider.AddShowCallback(callbacks::bombon);
		//RegisterElement(&bomb_col_glow_slider);

		//bomb_col_slider.setup("bomb text color", XOR("bomb_col_slider"), 0.f, 255.f, true, 0, 180, 1.f, XOR(L"%"));
		//bomb_col_slider.AddShowCallback(callbacks::bombon);
		//RegisterElement(&bomb_col_slider);

		tracers.setup(XOR("grenade trajectory"), XOR("tracers"));
		RegisterElement(&tracers);

		tracers_color.setup(XOR("grenade trajectory color"), XOR("tracers_color"), { 50, 149, 255 });
		tracers_color.AddShowCallback(callbacks::trajectoryon);
		RegisterElement(&tracers_color);

		spectators.setup(XOR("spectators"), XOR("spectators"));
		RegisterElement(&spectators);

		//_crosshair.setup(XOR("penetration reticle"), XOR("pen_xhair"));
		//RegisterElement(&pen_crosshair);

		//penetrable_color.setup(XOR("penetrable color"), XOR("penetrable_color"), { 0, 255, 0, 210 });
		//penetrable_color.AddShowCallback(callbacks::peneton);
		//RegisterElement(&penetrable_color);

		//unpenetrable_color.setup(XOR("unpenetrable color"), XOR("ammo_color"), { 255, 0, 0, 210 });
		//unpenetrable_color.AddShowCallback(callbacks::peneton);
		//RegisterElement(&unpenetrable_color);

		// col2.

		removals.setup(XOR("removals"), XOR("removals"), { XOR("visual recoil"), XOR("smoke"), XOR("fog"), XOR("flashbang"), XOR("scope") });
		RegisterElement(&removals, 1);

		walls_amount.setup("transparent walls", XOR("walls_amount"), 0.f, 100, true, 0, 100, 1.f, XOR(L"%"));
		walls_amount.SetCallback(Visuals::ModulateWorld);
		RegisterElement(&walls_amount, 1);

		transparent_props_amount.setup("transparent props", XOR("transparent_props_amount"), 0.f, 100.f, true, 0, 75.f, 1.f, XOR(L"%"));
		transparent_props_amount.SetCallback(Visuals::ModulateWorld);
		RegisterElement(&transparent_props_amount, 1);

		world.setup(XOR("brightness adjustment"), XOR("world"), { XOR("night"), XOR("fullbright"), XOR("ambient")});
		world.SetCallback(Visuals::ModulateWorld);
		RegisterElement(&world, 1);

		//nightcolor.setup("world color", XOR("nightcolor"), { 43, 41, 46 });
		nightcolor.SetCallback(Visuals::ModulateWorld);
		nightcolor.AddShowCallback(callbacks::IsNightMode);
		RegisterElement(&nightcolor, 1);

		propscolor.setup("props color", XOR("propscolor"), { 255, 255, 255 });
		propscolor.SetCallback(Visuals::ModulateWorld);
		RegisterElement(&propscolor, 1);

		ambient_color.setup("ambient color", XOR("ambient_color"), { 25, 25, 25 });
		ambient_color.AddShowCallback(callbacks::isambient);
		RegisterElement(&ambient_color, 1);

		ambient_alpha.setup("", XOR("ambient_alpha"), 0.f, 255.f, false, 0, 255.f, 1.f, XOR(L"%"));
		ambient_alpha.AddShowCallback(callbacks::isambient);
		RegisterElement(&ambient_alpha, 1);

		fov_scoped.setup(XOR("instant scope"), XOR("fov_scoped"));
		RegisterElement(&fov_scoped, 1);

		postprocess.setup(XOR("disable post processing"), XOR("postprocess"));
		RegisterElement(&postprocess, 1);

		thirdperson.setup(XOR("force thirdperson"), XOR("thirdperson"));
		thirdperson.SetToggleCallback(callbacks::ToggleThirdPerson);
		RegisterElement(&thirdperson, 1);

		//disableteam.setup(XOR("disable rendering of teammates"), XOR("disableteam"));
		//RegisterElement(&disableteam, 1);

		impact_beams.setup(XOR("bullet tracers"), XOR("impact_beams"));
		RegisterElement(&impact_beams, 1);

		beam_type.setup(XOR("beam style"), XOR("beam_type"), { XOR("kaaba"), XOR("laser") });
		beam_type.AddShowCallback(callbacks::isbullettracer);
		RegisterElement(&beam_type, 1);

		speed_beam.setup("speed", XOR("speed_beam"), 0.0f, 1.5, true, 1, 0.9f, 0.1f, XOR(L"s"));
		speed_beam.AddShowCallback(callbacks::islaseron);
		RegisterElement(&speed_beam, 1);

		impact_beams_color.setup(XOR("bullet tracer colors"), XOR("impact_beams_color"), { 151, 130, 255 });
		impact_beams_color.AddShowCallback(callbacks::isbullettracer);
		RegisterElement(&impact_beams_color, 1);

		alpha2.setup("", XOR("alpha2"), 0.f, 255.f, false, 0, 255.f, 1.f, XOR(L"%"));
		alpha2.AddShowCallback(callbacks::isbullettracer);
		RegisterElement(&alpha2, 1);

		bullet_impacts.setup(XOR("bullet impacts"), XOR("bullet_impacts"));
		RegisterElement(&bullet_impacts, 1);
	}
};

class SkinsTab : public Tab {
public:
	Checkbox enable;

	Edit     id_deagle;
	Checkbox stattrak_deagle;
	Slider   quality_deagle;
	Slider	 seed_deagle;

	Edit     id_elite;
	Checkbox stattrak_elite;
	Slider   quality_elite;
	Slider	 seed_elite;

	Edit     id_fiveseven;
	Checkbox stattrak_fiveseven;
	Slider   quality_fiveseven;
	Slider	 seed_fiveseven;

	Edit     id_glock;
	Checkbox stattrak_glock;
	Slider   quality_glock;
	Slider	 seed_glock;

	Edit     id_ak47;
	Checkbox stattrak_ak47;
	Slider   quality_ak47;
	Slider	 seed_ak47;

	Edit     id_aug;
	Checkbox stattrak_aug;
	Slider   quality_aug;
	Slider	 seed_aug;

	Edit     id_awp;
	Checkbox stattrak_awp;
	Slider   quality_awp;
	Slider	 seed_awp;

	Edit     id_famas;
	Checkbox stattrak_famas;
	Slider   quality_famas;
	Slider	 seed_famas;

	Edit     id_g3sg1;
	Checkbox stattrak_g3sg1;
	Slider   quality_g3sg1;
	Slider	 seed_g3sg1;

	Edit     id_galil;
	Checkbox stattrak_galil;
	Slider   quality_galil;
	Slider	 seed_galil;

	Edit     id_m249;
	Checkbox stattrak_m249;
	Slider   quality_m249;
	Slider	 seed_m249;

	Edit     id_m4a4;
	Checkbox stattrak_m4a4;
	Slider   quality_m4a4;
	Slider	 seed_m4a4;

	Edit     id_mac10;
	Checkbox stattrak_mac10;
	Slider   quality_mac10;
	Slider	 seed_mac10;

	Edit     id_p90;
	Checkbox stattrak_p90;
	Slider   quality_p90;
	Slider	 seed_p90;

	Edit     id_ump45;
	Checkbox stattrak_ump45;
	Slider   quality_ump45;
	Slider	 seed_ump45;

	Edit     id_xm1014;
	Checkbox stattrak_xm1014;
	Slider   quality_xm1014;
	Slider	 seed_xm1014;

	Edit     id_bizon;
	Checkbox stattrak_bizon;
	Slider   quality_bizon;
	Slider	 seed_bizon;

	Edit     id_mag7;
	Checkbox stattrak_mag7;
	Slider   quality_mag7;
	Slider	 seed_mag7;

	Edit     id_negev;
	Checkbox stattrak_negev;
	Slider   quality_negev;
	Slider	 seed_negev;

	Edit     id_sawedoff;
	Checkbox stattrak_sawedoff;
	Slider   quality_sawedoff;
	Slider	 seed_sawedoff;

	Edit     id_tec9;
	Checkbox stattrak_tec9;
	Slider   quality_tec9;
	Slider	 seed_tec9;

	Edit     id_p2000;
	Checkbox stattrak_p2000;
	Slider   quality_p2000;
	Slider	 seed_p2000;

	Edit     id_mp7;
	Checkbox stattrak_mp7;
	Slider   quality_mp7;
	Slider	 seed_mp7;

	Edit     id_mp9;
	Checkbox stattrak_mp9;
	Slider   quality_mp9;
	Slider	 seed_mp9;

	Edit     id_nova;
	Checkbox stattrak_nova;
	Slider   quality_nova;
	Slider	 seed_nova;

	Edit     id_p250;
	Checkbox stattrak_p250;
	Slider   quality_p250;
	Slider	 seed_p250;

	Edit     id_scar20;
	Checkbox stattrak_scar20;
	Slider   quality_scar20;
	Slider	 seed_scar20;

	Edit     id_sg553;
	Checkbox stattrak_sg553;
	Slider   quality_sg553;
	Slider	 seed_sg553;

	Edit     id_ssg08;
	Checkbox stattrak_ssg08;
	Slider   quality_ssg08;
	Slider	 seed_ssg08;

	Edit     id_m4a1s;
	Checkbox stattrak_m4a1s;
	Slider   quality_m4a1s;
	Slider	 seed_m4a1s;

	Edit     id_usps;
	Checkbox stattrak_usps;
	Slider   quality_usps;
	Slider	 seed_usps;

	Edit     id_cz75a;
	Checkbox stattrak_cz75a;
	Slider   quality_cz75a;
	Slider	 seed_cz75a;

	Edit     id_revolver;
	Checkbox stattrak_revolver;
	Slider   quality_revolver;
	Slider	 seed_revolver;

	Edit     id_bayonet;
	Checkbox stattrak_bayonet;
	Slider   quality_bayonet;
	Slider	 seed_bayonet;

	Edit     id_flip;
	Checkbox stattrak_flip;
	Slider   quality_flip;
	Slider	 seed_flip;

	Edit     id_gut;
	Checkbox stattrak_gut;
	Slider   quality_gut;
	Slider	 seed_gut;

	Edit     id_karambit;
	Checkbox stattrak_karambit;
	Slider   quality_karambit;
	Slider	 seed_karambit;

	Edit     id_m9;
	Checkbox stattrak_m9;
	Slider   quality_m9;
	Slider	 seed_m9;

	Edit     id_huntsman;
	Checkbox stattrak_huntsman;
	Slider   quality_huntsman;
	Slider	 seed_huntsman;

	Edit     id_falchion;
	Checkbox stattrak_falchion;
	Slider   quality_falchion;
	Slider	 seed_falchion;

	Edit     id_bowie;
	Checkbox stattrak_bowie;
	Slider   quality_bowie;
	Slider	 seed_bowie;

	Edit     id_butterfly;
	Checkbox stattrak_butterfly;
	Slider   quality_butterfly;
	Slider	 seed_butterfly;

	Edit     id_daggers;
	Checkbox stattrak_daggers;
	Slider   quality_daggers;
	Slider	 seed_daggers;

	// col 2.
	Dropdown knife;
	Dropdown glove;
	Edit	 glove_id;

public:
	void init( ) {
		SetTitle( XOR( "skins" ) );

		enable.setup( XOR( "enable" ), XOR( "skins_enable" ) );
		enable.SetCallback( callbacks::ForceFullUpdate );
		RegisterElement( &enable );

		// weapons...
		id_deagle.setup( XOR( "paintkit id" ), XOR( "id_deagle" ), 3 );
		id_deagle.SetCallback( callbacks::SkinUpdate );
		id_deagle.AddShowCallback( callbacks::DEAGLE );
		RegisterElement( &id_deagle );

		stattrak_deagle.setup( XOR( "stattrak" ), XOR( "stattrak_deagle" ) );
		stattrak_deagle.SetCallback( callbacks::SkinUpdate );
		stattrak_deagle.AddShowCallback( callbacks::DEAGLE );
		RegisterElement( &stattrak_deagle );

		quality_deagle.setup( XOR( "quality" ), XOR( "quality_deagle" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_deagle.SetCallback( callbacks::SkinUpdate );
		quality_deagle.AddShowCallback( callbacks::DEAGLE );
		RegisterElement( &quality_deagle );

		seed_deagle.setup( XOR( "seed" ), XOR( "seed_deagle" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_deagle.SetCallback( callbacks::SkinUpdate );
		seed_deagle.AddShowCallback( callbacks::DEAGLE );
		RegisterElement( &seed_deagle );

		id_elite.setup( XOR( "paintkit id" ), XOR( "id_elite" ), 3 );
		id_elite.SetCallback( callbacks::SkinUpdate );
		id_elite.AddShowCallback( callbacks::ELITE );
		RegisterElement( &id_elite );

		stattrak_elite.setup( XOR( "stattrak" ), XOR( "stattrak_elite" ) );
		stattrak_elite.SetCallback( callbacks::SkinUpdate );
		stattrak_elite.AddShowCallback( callbacks::ELITE );
		RegisterElement( &stattrak_elite );

		quality_elite.setup( XOR( "quality" ), XOR( "quality_elite" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_elite.SetCallback( callbacks::SkinUpdate );
		quality_elite.AddShowCallback( callbacks::ELITE );
		RegisterElement( &quality_elite );

		seed_elite.setup( XOR( "seed" ), XOR( "seed_elite" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_elite.SetCallback( callbacks::SkinUpdate );
		seed_elite.AddShowCallback( callbacks::ELITE );
		RegisterElement( &seed_elite );

		id_fiveseven.setup( XOR( "paintkit id" ), XOR( "id_fiveseven" ), 3 );
		id_fiveseven.SetCallback( callbacks::SkinUpdate );
		id_fiveseven.AddShowCallback( callbacks::FIVESEVEN );
		RegisterElement( &id_fiveseven );

		stattrak_fiveseven.setup( XOR( "stattrak" ), XOR( "stattrak_fiveseven" ) );
		stattrak_fiveseven.SetCallback( callbacks::SkinUpdate );
		stattrak_fiveseven.AddShowCallback( callbacks::FIVESEVEN );
		RegisterElement( &stattrak_fiveseven );

		quality_fiveseven.setup( XOR( "quality" ), XOR( "quality_fiveseven" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_fiveseven.SetCallback( callbacks::SkinUpdate );
		quality_fiveseven.AddShowCallback( callbacks::FIVESEVEN );
		RegisterElement( &quality_fiveseven );

		seed_fiveseven.setup( XOR( "seed" ), XOR( "seed_fiveseven" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_fiveseven.SetCallback( callbacks::SkinUpdate );
		seed_fiveseven.AddShowCallback( callbacks::FIVESEVEN );
		RegisterElement( &seed_fiveseven );

		id_glock.setup( XOR( "paintkit id" ), XOR( "id_glock" ), 3 );
		id_glock.SetCallback( callbacks::SkinUpdate );
		id_glock.AddShowCallback( callbacks::GLOCK );
		RegisterElement( &id_glock );

		stattrak_glock.setup( XOR( "stattrak" ), XOR( "stattrak_glock" ) );
		stattrak_glock.SetCallback( callbacks::SkinUpdate );
		stattrak_glock.AddShowCallback( callbacks::GLOCK );
		RegisterElement( &stattrak_glock );

		quality_glock.setup( XOR( "quality" ), XOR( "quality_glock" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_glock.SetCallback( callbacks::SkinUpdate );
		quality_glock.AddShowCallback( callbacks::GLOCK );
		RegisterElement( &quality_glock );

		seed_glock.setup( XOR( "seed" ), XOR( "seed_glock" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_glock.SetCallback( callbacks::SkinUpdate );
		seed_glock.AddShowCallback( callbacks::GLOCK );
		RegisterElement( &seed_glock );

		id_ak47.setup( XOR( "paintkit id" ), XOR( "id_ak47" ), 3 );
		id_ak47.SetCallback( callbacks::SkinUpdate );
		id_ak47.AddShowCallback( callbacks::AK47 );
		RegisterElement( &id_ak47 );

		stattrak_ak47.setup( XOR( "stattrak" ), XOR( "stattrak_ak47" ) );
		stattrak_ak47.SetCallback( callbacks::SkinUpdate );
		stattrak_ak47.AddShowCallback( callbacks::AK47 );
		RegisterElement( &stattrak_ak47 );

		quality_ak47.setup( XOR( "quality" ), XOR( "quality_ak47" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_ak47.SetCallback( callbacks::SkinUpdate );
		quality_ak47.AddShowCallback( callbacks::AK47 );
		RegisterElement( &quality_ak47 );

		seed_ak47.setup( XOR( "seed" ), XOR( "seed_ak47" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_ak47.SetCallback( callbacks::SkinUpdate );
		seed_ak47.AddShowCallback( callbacks::AK47 );
		RegisterElement( &seed_ak47 );

		id_aug.setup( XOR( "paintkit id" ), XOR( "id_aug" ), 3 );
		id_aug.SetCallback( callbacks::SkinUpdate );
		id_aug.AddShowCallback( callbacks::AUG );
		RegisterElement( &id_aug );

		stattrak_aug.setup( XOR( "stattrak" ), XOR( "stattrak_aug" ) );
		stattrak_aug.SetCallback( callbacks::SkinUpdate );
		stattrak_aug.AddShowCallback( callbacks::AUG );
		RegisterElement( &stattrak_aug );

		quality_aug.setup( XOR( "quality" ), XOR( "quality_aug" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_aug.SetCallback( callbacks::SkinUpdate );
		quality_aug.AddShowCallback( callbacks::AUG );
		RegisterElement( &quality_aug );

		seed_aug.setup( XOR( "seed" ), XOR( "seed_aug" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_aug.SetCallback( callbacks::SkinUpdate );
		seed_aug.AddShowCallback( callbacks::AUG );
		RegisterElement( &seed_aug );

		id_awp.setup( XOR( "paintkit id" ), XOR( "id_awp" ), 3 );
		id_awp.SetCallback( callbacks::SkinUpdate );
		id_awp.AddShowCallback( callbacks::AWP );
		RegisterElement( &id_awp );

		stattrak_awp.setup( XOR( "stattrak" ), XOR( "stattrak_awp" ) );
		stattrak_awp.SetCallback( callbacks::SkinUpdate );
		stattrak_awp.AddShowCallback( callbacks::AWP );
		RegisterElement( &stattrak_awp );

		quality_awp.setup( XOR( "quality" ), XOR( "quality_awp" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_awp.SetCallback( callbacks::SkinUpdate );
		quality_awp.AddShowCallback( callbacks::AWP );
		RegisterElement( &quality_awp );

		seed_awp.setup( XOR( "seed" ), XOR( "seed_awp" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_awp.SetCallback( callbacks::SkinUpdate );
		seed_awp.AddShowCallback( callbacks::AWP );
		RegisterElement( &seed_awp );

		id_famas.setup( XOR( "paintkit id" ), XOR( "id_famas" ), 3 );
		id_famas.SetCallback( callbacks::SkinUpdate );
		id_famas.AddShowCallback( callbacks::FAMAS );
		RegisterElement( &id_famas );

		stattrak_famas.setup( XOR( "stattrak" ), XOR( "stattrak_famas" ) );
		stattrak_famas.SetCallback( callbacks::SkinUpdate );
		stattrak_famas.AddShowCallback( callbacks::FAMAS );
		RegisterElement( &stattrak_famas );

		quality_famas.setup( XOR( "quality" ), XOR( "quality_famas" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_famas.SetCallback( callbacks::SkinUpdate );
		quality_famas.AddShowCallback( callbacks::FAMAS );
		RegisterElement( &quality_famas );

		seed_famas.setup( XOR( "seed" ), XOR( "seed_famas" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_famas.SetCallback( callbacks::SkinUpdate );
		seed_famas.AddShowCallback( callbacks::FAMAS );
		RegisterElement( &seed_famas );

		id_g3sg1.setup( XOR( "paintkit id" ), XOR( "id_g3sg1" ), 3 );
		id_g3sg1.SetCallback( callbacks::SkinUpdate );
		id_g3sg1.AddShowCallback( callbacks::G3SG1 );
		RegisterElement( &id_g3sg1 );

		stattrak_g3sg1.setup( XOR( "stattrak" ), XOR( "stattrak_g3sg1" ) );
		stattrak_g3sg1.SetCallback( callbacks::SkinUpdate );
		stattrak_g3sg1.AddShowCallback( callbacks::G3SG1 );
		RegisterElement( &stattrak_g3sg1 );

		quality_g3sg1.setup( XOR( "quality" ), XOR( "quality_g3sg1" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_g3sg1.SetCallback( callbacks::SkinUpdate );
		quality_g3sg1.AddShowCallback( callbacks::G3SG1 );
		RegisterElement( &quality_g3sg1 );

		seed_g3sg1.setup( XOR( "seed" ), XOR( "seed_g3sg1" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_g3sg1.SetCallback( callbacks::SkinUpdate );
		seed_g3sg1.AddShowCallback( callbacks::G3SG1 );
		RegisterElement( &seed_g3sg1 );

		id_galil.setup( XOR( "paintkit id" ), XOR( "id_galil" ), 3 );
		id_galil.SetCallback( callbacks::SkinUpdate );
		id_galil.AddShowCallback( callbacks::GALIL );
		RegisterElement( &id_galil );

		stattrak_galil.setup( XOR( "stattrak" ), XOR( "stattrak_galil" ) );
		stattrak_galil.SetCallback( callbacks::SkinUpdate );
		stattrak_galil.AddShowCallback( callbacks::GALIL );
		RegisterElement( &stattrak_galil );

		quality_galil.setup( XOR( "quality" ), XOR( "quality_galil" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_galil.SetCallback( callbacks::SkinUpdate );
		quality_galil.AddShowCallback( callbacks::GALIL );
		RegisterElement( &quality_galil );

		seed_galil.setup( XOR( "seed" ), XOR( "seed_galil" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_galil.SetCallback( callbacks::SkinUpdate );
		seed_galil.AddShowCallback( callbacks::GALIL );
		RegisterElement( &seed_galil );

		id_m249.setup( XOR( "paintkit id" ), XOR( "id_m249" ), 3 );
		id_m249.SetCallback( callbacks::SkinUpdate );
		id_m249.AddShowCallback( callbacks::M249 );
		RegisterElement( &id_m249 );

		stattrak_m249.setup( XOR( "stattrak" ), XOR( "stattrak_m249" ) );
		stattrak_m249.SetCallback( callbacks::SkinUpdate );
		stattrak_m249.AddShowCallback( callbacks::M249 );
		RegisterElement( &stattrak_m249 );

		quality_m249.setup( XOR( "quality" ), XOR( "quality_m249" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_m249.SetCallback( callbacks::SkinUpdate );
		quality_m249.AddShowCallback( callbacks::M249 );
		RegisterElement( &quality_m249 );

		seed_m249.setup( XOR( "seed" ), XOR( "seed_m249" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_m249.SetCallback( callbacks::SkinUpdate );
		seed_m249.AddShowCallback( callbacks::M249 );
		RegisterElement( &seed_m249 );

		id_m4a4.setup( XOR( "paintkit id" ), XOR( "id_m4a4" ), 3 );
		id_m4a4.SetCallback( callbacks::SkinUpdate );
		id_m4a4.AddShowCallback( callbacks::M4A4 );
		RegisterElement( &id_m4a4 );

		stattrak_m4a4.setup( XOR( "stattrak" ), XOR( "stattrak_m4a4" ) );
		stattrak_m4a4.SetCallback( callbacks::SkinUpdate );
		stattrak_m4a4.AddShowCallback( callbacks::M4A4 );
		RegisterElement( &stattrak_m4a4 );

		quality_m4a4.setup( XOR( "quality" ), XOR( "quality_m4a4" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_m4a4.SetCallback( callbacks::SkinUpdate );
		quality_m4a4.AddShowCallback( callbacks::M4A4 );
		RegisterElement( &quality_m4a4 );

		seed_m4a4.setup( XOR( "seed" ), XOR( "seed_m4a4" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_m4a4.SetCallback( callbacks::SkinUpdate );
		seed_m4a4.AddShowCallback( callbacks::M4A4 );
		RegisterElement( &seed_m4a4 );

		id_mac10.setup( XOR( "paintkit id" ), XOR( "id_mac10" ), 3 );
		id_mac10.SetCallback( callbacks::SkinUpdate );
		id_mac10.AddShowCallback( callbacks::MAC10 );
		RegisterElement( &id_mac10 );

		stattrak_mac10.setup( XOR( "stattrak" ), XOR( "stattrak_mac10" ) );
		stattrak_mac10.SetCallback( callbacks::SkinUpdate );
		stattrak_mac10.AddShowCallback( callbacks::MAC10 );
		RegisterElement( &stattrak_mac10 );

		quality_mac10.setup( XOR( "quality" ), XOR( "quality_mac10" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_mac10.SetCallback( callbacks::SkinUpdate );
		quality_mac10.AddShowCallback( callbacks::MAC10 );
		RegisterElement( &quality_mac10 );

		seed_mac10.setup( XOR( "seed" ), XOR( "seed_mac10" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_mac10.SetCallback( callbacks::SkinUpdate );
		seed_mac10.AddShowCallback( callbacks::MAC10 );
		RegisterElement( &seed_mac10 );

		id_p90.setup( XOR( "paintkit id" ), XOR( "id_p90" ), 3 );
		id_p90.SetCallback( callbacks::SkinUpdate );
		id_p90.AddShowCallback( callbacks::P90 );
		RegisterElement( &id_p90 );

		stattrak_p90.setup( XOR( "stattrak" ), XOR( "stattrak_p90" ) );
		stattrak_p90.SetCallback( callbacks::SkinUpdate );
		stattrak_p90.AddShowCallback( callbacks::P90 );
		RegisterElement( &stattrak_p90 );

		quality_p90.setup( XOR( "quality" ), XOR( "quality_p90" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_p90.SetCallback( callbacks::SkinUpdate );
		quality_p90.AddShowCallback( callbacks::P90 );
		RegisterElement( &quality_p90 );

		seed_p90.setup( XOR( "seed" ), XOR( "seed_p90" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_p90.SetCallback( callbacks::SkinUpdate );
		seed_p90.AddShowCallback( callbacks::P90 );
		RegisterElement( &seed_p90 );

		id_ump45.setup( XOR( "paintkit id" ), XOR( "id_ump45" ), 3 );
		id_ump45.SetCallback( callbacks::SkinUpdate );
		id_ump45.AddShowCallback( callbacks::UMP45 );
		RegisterElement( &id_ump45 );

		stattrak_ump45.setup( XOR( "stattrak" ), XOR( "stattrak_ump45" ) );
		stattrak_ump45.SetCallback( callbacks::SkinUpdate );
		stattrak_ump45.AddShowCallback( callbacks::UMP45 );
		RegisterElement( &stattrak_ump45 );

		quality_ump45.setup( XOR( "quality" ), XOR( "quality_ump45" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_ump45.SetCallback( callbacks::SkinUpdate );
		quality_ump45.AddShowCallback( callbacks::UMP45 );
		RegisterElement( &quality_ump45 );

		seed_ump45.setup( XOR( "seed" ), XOR( "seed_ump45" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_ump45.SetCallback( callbacks::SkinUpdate );
		seed_ump45.AddShowCallback( callbacks::UMP45 );
		RegisterElement( &seed_ump45 );

		id_xm1014.setup( XOR( "paintkit id" ), XOR( "id_xm1014" ), 3 );
		id_xm1014.SetCallback( callbacks::SkinUpdate );
		id_xm1014.AddShowCallback( callbacks::XM1014 );
		RegisterElement( &id_xm1014 );

		stattrak_xm1014.setup( XOR( "stattrak" ), XOR( "stattrak_xm1014" ) );
		stattrak_xm1014.SetCallback( callbacks::SkinUpdate );
		stattrak_xm1014.AddShowCallback( callbacks::XM1014 );
		RegisterElement( &stattrak_xm1014 );

		quality_xm1014.setup( XOR( "quality" ), XOR( "quality_xm1014" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_xm1014.SetCallback( callbacks::SkinUpdate );
		quality_xm1014.AddShowCallback( callbacks::XM1014 );
		RegisterElement( &quality_xm1014 );

		seed_xm1014.setup( XOR( "seed" ), XOR( "seed_xm1014" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_xm1014.SetCallback( callbacks::SkinUpdate );
		seed_xm1014.AddShowCallback( callbacks::XM1014 );
		RegisterElement( &seed_xm1014 );

		id_bizon.setup( XOR( "paintkit id" ), XOR( "id_bizon" ), 3 );
		id_bizon.SetCallback( callbacks::SkinUpdate );
		id_bizon.AddShowCallback( callbacks::BIZON );
		RegisterElement( &id_bizon );

		stattrak_bizon.setup( XOR( "stattrak" ), XOR( "stattrak_bizon" ) );
		stattrak_bizon.SetCallback( callbacks::SkinUpdate );
		stattrak_bizon.AddShowCallback( callbacks::BIZON );
		RegisterElement( &stattrak_bizon );

		quality_bizon.setup( XOR( "quality" ), XOR( "quality_bizon" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_bizon.SetCallback( callbacks::SkinUpdate );
		quality_bizon.AddShowCallback( callbacks::BIZON );
		RegisterElement( &quality_bizon );

		seed_bizon.setup( XOR( "seed" ), XOR( "seed_bizon" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_bizon.SetCallback( callbacks::SkinUpdate );
		seed_bizon.AddShowCallback( callbacks::BIZON );
		RegisterElement( &seed_bizon );

		id_mag7.setup( XOR( "paintkit id" ), XOR( "id_mag7" ), 3 );
		id_mag7.SetCallback( callbacks::SkinUpdate );
		id_mag7.AddShowCallback( callbacks::MAG7 );
		RegisterElement( &id_mag7 );

		stattrak_mag7.setup( XOR( "stattrak" ), XOR( "stattrak_mag7" ) );
		stattrak_mag7.SetCallback( callbacks::SkinUpdate );
		stattrak_mag7.AddShowCallback( callbacks::MAG7 );
		RegisterElement( &stattrak_mag7 );

		quality_mag7.setup( XOR( "quality" ), XOR( "quality_mag7" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_mag7.SetCallback( callbacks::SkinUpdate );
		quality_mag7.AddShowCallback( callbacks::MAG7 );
		RegisterElement( &quality_mag7 );

		seed_mag7.setup( XOR( "seed" ), XOR( "seed_mag7" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_mag7.SetCallback( callbacks::SkinUpdate );
		seed_mag7.AddShowCallback( callbacks::MAG7 );
		RegisterElement( &seed_mag7 );

		id_negev.setup( XOR( "paintkit id" ), XOR( "id_negev" ), 3 );
		id_negev.SetCallback( callbacks::SkinUpdate );
		id_negev.AddShowCallback( callbacks::NEGEV );
		RegisterElement( &id_negev );

		stattrak_negev.setup( XOR( "stattrak" ), XOR( "stattrak_negev" ) );
		stattrak_negev.SetCallback( callbacks::SkinUpdate );
		stattrak_negev.AddShowCallback( callbacks::NEGEV );
		RegisterElement( &stattrak_negev );

		quality_negev.setup( XOR( "quality" ), XOR( "quality_negev" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_negev.SetCallback( callbacks::SkinUpdate );
		quality_negev.AddShowCallback( callbacks::NEGEV );
		RegisterElement( &quality_negev );

		seed_negev.setup( XOR( "seed" ), XOR( "seed_negev" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_negev.SetCallback( callbacks::SkinUpdate );
		seed_negev.AddShowCallback( callbacks::NEGEV );
		RegisterElement( &seed_negev );

		id_sawedoff.setup( XOR( "paintkit id" ), XOR( "id_sawedoff" ), 3 );
		id_sawedoff.SetCallback( callbacks::SkinUpdate );
		id_sawedoff.AddShowCallback( callbacks::SAWEDOFF );
		RegisterElement( &id_sawedoff );

		stattrak_sawedoff.setup( XOR( "stattrak" ), XOR( "stattrak_sawedoff" ) );
		stattrak_sawedoff.SetCallback( callbacks::SkinUpdate );
		stattrak_sawedoff.AddShowCallback( callbacks::SAWEDOFF );
		RegisterElement( &stattrak_sawedoff );

		quality_sawedoff.setup( XOR( "quality" ), XOR( "quality_sawedoff" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_sawedoff.SetCallback( callbacks::SkinUpdate );
		quality_sawedoff.AddShowCallback( callbacks::SAWEDOFF );
		RegisterElement( &quality_sawedoff );

		seed_sawedoff.setup( XOR( "seed" ), XOR( "seed_sawedoff" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_sawedoff.SetCallback( callbacks::SkinUpdate );
		seed_sawedoff.AddShowCallback( callbacks::SAWEDOFF );
		RegisterElement( &seed_sawedoff );

		id_tec9.setup( XOR( "paintkit id" ), XOR( "id_tec9" ), 3 );
		id_tec9.SetCallback( callbacks::SkinUpdate );
		id_tec9.AddShowCallback( callbacks::TEC9 );
		RegisterElement( &id_tec9 );

		stattrak_tec9.setup( XOR( "stattrak" ), XOR( "stattrak_tec9" ) );
		stattrak_tec9.SetCallback( callbacks::SkinUpdate );
		stattrak_tec9.AddShowCallback( callbacks::TEC9 );
		RegisterElement( &stattrak_tec9 );

		quality_tec9.setup( XOR( "quality" ), XOR( "quality_tec9" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_tec9.SetCallback( callbacks::SkinUpdate );
		quality_tec9.AddShowCallback( callbacks::TEC9 );
		RegisterElement( &quality_tec9 );

		seed_tec9.setup( XOR( "seed" ), XOR( "seed_tec9" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_tec9.SetCallback( callbacks::SkinUpdate );
		seed_tec9.AddShowCallback( callbacks::TEC9 );
		RegisterElement( &seed_tec9 );

		id_p2000.setup( XOR( "paintkit id" ), XOR( "id_p2000" ), 3 );
		id_p2000.SetCallback( callbacks::SkinUpdate );
		id_p2000.AddShowCallback( callbacks::P2000 );
		RegisterElement( &id_p2000 );

		stattrak_p2000.setup( XOR( "stattrak" ), XOR( "stattrak_p2000" ) );
		stattrak_p2000.SetCallback( callbacks::SkinUpdate );
		stattrak_p2000.AddShowCallback( callbacks::P2000 );
		RegisterElement( &stattrak_p2000 );

		quality_p2000.setup( XOR( "quality" ), XOR( "quality_p2000" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_p2000.SetCallback( callbacks::SkinUpdate );
		quality_p2000.AddShowCallback( callbacks::P2000 );
		RegisterElement( &quality_p2000 );

		seed_p2000.setup( XOR( "seed" ), XOR( "seed_p2000" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_p2000.SetCallback( callbacks::SkinUpdate );
		seed_p2000.AddShowCallback( callbacks::P2000 );
		RegisterElement( &seed_p2000 );

		id_mp7.setup( XOR( "paintkit id" ), XOR( "id_mp7" ), 3 );
		id_mp7.SetCallback( callbacks::SkinUpdate );
		id_mp7.AddShowCallback( callbacks::MP7 );
		RegisterElement( &id_mp7 );

		stattrak_mp7.setup( XOR( "stattrak" ), XOR( "stattrak_mp7" ) );
		stattrak_mp7.SetCallback( callbacks::SkinUpdate );
		stattrak_mp7.AddShowCallback( callbacks::MP7 );
		RegisterElement( &stattrak_mp7 );

		quality_mp7.setup( XOR( "quality" ), XOR( "quality_mp7" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_mp7.SetCallback( callbacks::SkinUpdate );
		quality_mp7.AddShowCallback( callbacks::MP7 );
		RegisterElement( &quality_mp7 );

		seed_mp7.setup( XOR( "seed" ), XOR( "seed_mp7" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_mp7.SetCallback( callbacks::SkinUpdate );
		seed_mp7.AddShowCallback( callbacks::MP7 );
		RegisterElement( &seed_mp7 );

		id_mp9.setup( XOR( "paintkit id" ), XOR( "id_mp9" ), 3 );
		id_mp9.SetCallback( callbacks::SkinUpdate );
		id_mp9.AddShowCallback( callbacks::MP9 );
		RegisterElement( &id_mp9 );

		stattrak_mp9.setup( XOR( "stattrak" ), XOR( "stattrak_mp9" ) );
		stattrak_mp9.SetCallback( callbacks::SkinUpdate );
		stattrak_mp9.AddShowCallback( callbacks::MP9 );
		RegisterElement( &stattrak_mp9 );

		quality_mp9.setup( XOR( "quality" ), XOR( "quality_mp9" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_mp9.SetCallback( callbacks::SkinUpdate );
		quality_mp9.AddShowCallback( callbacks::MP9 );
		RegisterElement( &quality_mp9 );

		seed_mp9.setup( XOR( "seed" ), XOR( "seed_mp9" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_mp9.SetCallback( callbacks::SkinUpdate );
		seed_mp9.AddShowCallback( callbacks::MP9 );
		RegisterElement( &seed_mp9 );

		id_nova.setup( XOR( "paintkit id" ), XOR( "id_nova" ), 3 );
		id_nova.SetCallback( callbacks::SkinUpdate );
		id_nova.AddShowCallback( callbacks::NOVA );
		RegisterElement( &id_nova );

		stattrak_nova.setup( XOR( "stattrak" ), XOR( "stattrak_nova" ) );
		stattrak_nova.SetCallback( callbacks::SkinUpdate );
		stattrak_nova.AddShowCallback( callbacks::NOVA );
		RegisterElement( &stattrak_nova );

		quality_nova.setup( XOR( "quality" ), XOR( "quality_nova" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_nova.SetCallback( callbacks::SkinUpdate );
		quality_nova.AddShowCallback( callbacks::NOVA );
		RegisterElement( &quality_nova );

		seed_nova.setup( XOR( "seed" ), XOR( "seed_nova" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_nova.SetCallback( callbacks::SkinUpdate );
		seed_nova.AddShowCallback( callbacks::NOVA );
		RegisterElement( &seed_nova );

		id_p250.setup( XOR( "paintkit id" ), XOR( "id_p250" ), 3 );
		id_p250.SetCallback( callbacks::SkinUpdate );
		id_p250.AddShowCallback( callbacks::P250 );
		RegisterElement( &id_p250 );

		stattrak_p250.setup( XOR( "stattrak" ), XOR( "stattrak_p250" ) );
		stattrak_p250.SetCallback( callbacks::SkinUpdate );
		stattrak_p250.AddShowCallback( callbacks::P250 );
		RegisterElement( &stattrak_p250 );

		quality_p250.setup( XOR( "quality" ), XOR( "quality_p250" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_p250.SetCallback( callbacks::SkinUpdate );
		quality_p250.AddShowCallback( callbacks::P250 );
		RegisterElement( &quality_p250 );

		seed_p250.setup( XOR( "seed" ), XOR( "seed_p250" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_p250.SetCallback( callbacks::SkinUpdate );
		seed_p250.AddShowCallback( callbacks::P250 );
		RegisterElement( &seed_p250 );

		id_scar20.setup( XOR( "paintkit id" ), XOR( "id_scar20" ), 3 );
		id_scar20.SetCallback( callbacks::SkinUpdate );
		id_scar20.AddShowCallback( callbacks::SCAR20 );
		RegisterElement( &id_scar20 );

		stattrak_scar20.setup( XOR( "stattrak" ), XOR( "stattrak_scar20" ) );
		stattrak_scar20.SetCallback( callbacks::SkinUpdate );
		stattrak_scar20.AddShowCallback( callbacks::SCAR20 );
		RegisterElement( &stattrak_scar20 );

		quality_scar20.setup( XOR( "quality" ), XOR( "quality_scar20" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_scar20.SetCallback( callbacks::SkinUpdate );
		quality_scar20.AddShowCallback( callbacks::SCAR20 );
		RegisterElement( &quality_scar20 );

		seed_scar20.setup( XOR( "seed" ), XOR( "seed_scar20" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_scar20.SetCallback( callbacks::SkinUpdate );
		seed_scar20.AddShowCallback( callbacks::SCAR20 );
		RegisterElement( &seed_scar20 );

		id_sg553.setup( XOR( "paintkit id" ), XOR( "id_sg553" ), 3 );
		id_sg553.SetCallback( callbacks::SkinUpdate );
		id_sg553.AddShowCallback( callbacks::SG553 );
		RegisterElement( &id_sg553 );

		stattrak_sg553.setup( XOR( "stattrak" ), XOR( "stattrak_sg553" ) );
		stattrak_sg553.SetCallback( callbacks::SkinUpdate );
		stattrak_sg553.AddShowCallback( callbacks::SG553 );
		RegisterElement( &stattrak_sg553 );

		quality_sg553.setup( XOR( "quality" ), XOR( "quality_sg553" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_sg553.SetCallback( callbacks::SkinUpdate );
		quality_sg553.AddShowCallback( callbacks::SG553 );
		RegisterElement( &quality_sg553 );

		seed_sg553.setup( XOR( "seed" ), XOR( "seed_sg553" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_sg553.SetCallback( callbacks::SkinUpdate );
		seed_sg553.AddShowCallback( callbacks::SG553 );
		RegisterElement( &seed_sg553 );

		id_ssg08.setup( XOR( "paintkit id" ), XOR( "id_ssg08" ), 3 );
		id_ssg08.SetCallback( callbacks::SkinUpdate );
		id_ssg08.AddShowCallback( callbacks::SSG08 );
		RegisterElement( &id_ssg08 );

		stattrak_ssg08.setup( XOR( "stattrak" ), XOR( "stattrak_ssg08" ) );
		stattrak_ssg08.SetCallback( callbacks::SkinUpdate );
		stattrak_ssg08.AddShowCallback( callbacks::SSG08 );
		RegisterElement( &stattrak_ssg08 );

		quality_ssg08.setup( XOR( "quality" ), XOR( "quality_ssg08" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_ssg08.SetCallback( callbacks::SkinUpdate );
		quality_ssg08.AddShowCallback( callbacks::SSG08 );
		RegisterElement( &quality_ssg08 );

		seed_ssg08.setup( XOR( "seed" ), XOR( "seed_ssg08" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_ssg08.SetCallback( callbacks::SkinUpdate );
		seed_ssg08.AddShowCallback( callbacks::SSG08 );
		RegisterElement( &seed_ssg08 );

		id_m4a1s.setup( XOR( "paintkit id" ), XOR( "id_m4a1s" ), 3 );
		id_m4a1s.SetCallback( callbacks::SkinUpdate );
		id_m4a1s.AddShowCallback( callbacks::M4A1S );
		RegisterElement( &id_m4a1s );

		stattrak_m4a1s.setup( XOR( "stattrak" ), XOR( "stattrak_m4a1s" ) );
		stattrak_m4a1s.SetCallback( callbacks::SkinUpdate );
		stattrak_m4a1s.AddShowCallback( callbacks::M4A1S );
		RegisterElement( &stattrak_m4a1s );

		quality_m4a1s.setup( XOR( "quality" ), XOR( "quality_m4a1s" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_m4a1s.SetCallback( callbacks::SkinUpdate );
		quality_m4a1s.AddShowCallback( callbacks::M4A1S );
		RegisterElement( &quality_m4a1s );

		seed_m4a1s.setup( XOR( "seed" ), XOR( "seed_m4a1s" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_m4a1s.SetCallback( callbacks::SkinUpdate );
		seed_m4a1s.AddShowCallback( callbacks::M4A1S );
		RegisterElement( &seed_m4a1s );

		id_usps.setup( XOR( "paintkit id" ), XOR( "id_usps" ), 3 );
		id_usps.SetCallback( callbacks::SkinUpdate );
		id_usps.AddShowCallback( callbacks::USPS );
		RegisterElement( &id_usps );

		stattrak_usps.setup( XOR( "stattrak" ), XOR( "stattrak_usps" ) );
		stattrak_usps.SetCallback( callbacks::SkinUpdate );
		stattrak_usps.AddShowCallback( callbacks::USPS );
		RegisterElement( &stattrak_usps );

		quality_usps.setup( XOR( "quality" ), XOR( "quality_usps" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_usps.SetCallback( callbacks::SkinUpdate );
		quality_usps.AddShowCallback( callbacks::USPS );
		RegisterElement( &quality_usps );

		seed_usps.setup( XOR( "seed" ), XOR( "seed_usps" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_usps.SetCallback( callbacks::SkinUpdate );
		seed_usps.AddShowCallback( callbacks::USPS );
		RegisterElement( &seed_usps );

		id_cz75a.setup( XOR( "paintkit id" ), XOR( "id_cz75a" ), 3 );
		id_cz75a.SetCallback( callbacks::SkinUpdate );
		id_cz75a.AddShowCallback( callbacks::CZ75A );
		RegisterElement( &id_cz75a );

		stattrak_cz75a.setup( XOR( "stattrak" ), XOR( "stattrak_cz75a" ) );
		stattrak_cz75a.SetCallback( callbacks::SkinUpdate );
		stattrak_cz75a.AddShowCallback( callbacks::CZ75A );
		RegisterElement( &stattrak_cz75a );

		quality_cz75a.setup( XOR( "quality" ), XOR( "quality_cz75a" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_cz75a.SetCallback( callbacks::SkinUpdate );
		quality_cz75a.AddShowCallback( callbacks::CZ75A );
		RegisterElement( &quality_cz75a );

		seed_cz75a.setup( XOR( "seed" ), XOR( "seed_cz75a" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_cz75a.SetCallback( callbacks::SkinUpdate );
		seed_cz75a.AddShowCallback( callbacks::CZ75A );
		RegisterElement( &seed_cz75a );

		id_revolver.setup( XOR( "paintkit id" ), XOR( "id_revolver" ), 3 );
		id_revolver.SetCallback( callbacks::SkinUpdate );
		id_revolver.AddShowCallback( callbacks::REVOLVER );
		RegisterElement( &id_revolver );

		stattrak_revolver.setup( XOR( "stattrak" ), XOR( "stattrak_revolver" ) );
		stattrak_revolver.SetCallback( callbacks::SkinUpdate );
		stattrak_revolver.AddShowCallback( callbacks::REVOLVER );
		RegisterElement( &stattrak_revolver );

		quality_revolver.setup( XOR( "quality" ), XOR( "quality_revolver" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_revolver.SetCallback( callbacks::SkinUpdate );
		quality_revolver.AddShowCallback( callbacks::REVOLVER );
		RegisterElement( &quality_revolver );

		seed_revolver.setup( XOR( "seed" ), XOR( "seed_revolver" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_revolver.SetCallback( callbacks::SkinUpdate );
		seed_revolver.AddShowCallback( callbacks::REVOLVER );
		RegisterElement( &seed_revolver );

		id_bayonet.setup( XOR( "paintkit id" ), XOR( "id_bayonet" ), 3 );
		id_bayonet.SetCallback( callbacks::SkinUpdate );
		id_bayonet.AddShowCallback( callbacks::KNIFE_BAYONET );
		RegisterElement( &id_bayonet );

		stattrak_bayonet.setup( XOR( "stattrak" ), XOR( "stattrak_bayonet" ) );
		stattrak_bayonet.SetCallback( callbacks::SkinUpdate );
		stattrak_bayonet.AddShowCallback( callbacks::KNIFE_BAYONET );
		RegisterElement( &stattrak_bayonet );

		quality_bayonet.setup( XOR( "quality" ), XOR( "quality_bayonet" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_bayonet.SetCallback( callbacks::SkinUpdate );
		quality_bayonet.AddShowCallback( callbacks::KNIFE_BAYONET );
		RegisterElement( &quality_bayonet );

		seed_bayonet.setup( XOR( "seed" ), XOR( "seed_bayonet" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_bayonet.SetCallback( callbacks::SkinUpdate );
		seed_bayonet.AddShowCallback( callbacks::KNIFE_BAYONET );
		RegisterElement( &seed_bayonet );

		id_flip.setup( XOR( "paintkit id" ), XOR( "id_flip" ), 3 );
		id_flip.SetCallback( callbacks::SkinUpdate );
		id_flip.AddShowCallback( callbacks::KNIFE_FLIP );
		RegisterElement( &id_flip );

		stattrak_flip.setup( XOR( "stattrak" ), XOR( "stattrak_flip" ) );
		stattrak_flip.SetCallback( callbacks::SkinUpdate );
		stattrak_flip.AddShowCallback( callbacks::KNIFE_FLIP );
		RegisterElement( &stattrak_flip );

		quality_flip.setup( XOR( "quality" ), XOR( "quality_flip" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_flip.SetCallback( callbacks::SkinUpdate );
		quality_flip.AddShowCallback( callbacks::KNIFE_FLIP );
		RegisterElement( &quality_flip );

		seed_flip.setup( XOR( "seed" ), XOR( "seed_flip" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_flip.SetCallback( callbacks::SkinUpdate );
		seed_flip.AddShowCallback( callbacks::KNIFE_FLIP );
		RegisterElement( &seed_flip );

		id_gut.setup( XOR( "paintkit id" ), XOR( "id_gut" ), 3 );
		id_gut.SetCallback( callbacks::SkinUpdate );
		id_gut.AddShowCallback( callbacks::KNIFE_GUT );
		RegisterElement( &id_gut );

		stattrak_gut.setup( XOR( "stattrak" ), XOR( "stattrak_gut" ) );
		stattrak_gut.SetCallback( callbacks::SkinUpdate );
		stattrak_gut.AddShowCallback( callbacks::KNIFE_GUT );
		RegisterElement( &stattrak_gut );

		quality_gut.setup( XOR( "quality" ), XOR( "quality_gut" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_gut.SetCallback( callbacks::SkinUpdate );
		quality_gut.AddShowCallback( callbacks::KNIFE_GUT );
		RegisterElement( &quality_gut );

		seed_gut.setup( XOR( "seed" ), XOR( "seed_gut" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_gut.SetCallback( callbacks::SkinUpdate );
		seed_gut.AddShowCallback( callbacks::KNIFE_GUT );
		RegisterElement( &seed_gut );

		id_karambit.setup( XOR( "paintkit id" ), XOR( "id_karambit" ), 3 );
		id_karambit.SetCallback( callbacks::SkinUpdate );
		id_karambit.AddShowCallback( callbacks::KNIFE_KARAMBIT );
		RegisterElement( &id_karambit );

		stattrak_karambit.setup( XOR( "stattrak" ), XOR( "stattrak_karambit" ) );
		stattrak_karambit.SetCallback( callbacks::SkinUpdate );
		stattrak_karambit.AddShowCallback( callbacks::KNIFE_KARAMBIT );
		RegisterElement( &stattrak_karambit );

		quality_karambit.setup( XOR( "quality" ), XOR( "quality_karambit" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_karambit.SetCallback( callbacks::SkinUpdate );
		quality_karambit.AddShowCallback( callbacks::KNIFE_KARAMBIT );
		RegisterElement( &quality_karambit );

		seed_karambit.setup( XOR( "seed" ), XOR( "seed_karambit" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_karambit.SetCallback( callbacks::SkinUpdate );
		seed_karambit.AddShowCallback( callbacks::KNIFE_KARAMBIT );
		RegisterElement( &seed_karambit );

		id_m9.setup( XOR( "paintkit id" ), XOR( "id_m9" ), 3 );
		id_m9.SetCallback( callbacks::SkinUpdate );
		id_m9.AddShowCallback( callbacks::KNIFE_M9_BAYONET );
		RegisterElement( &id_m9 );

		stattrak_m9.setup( XOR( "stattrak" ), XOR( "stattrak_m9" ) );
		stattrak_m9.SetCallback( callbacks::SkinUpdate );
		stattrak_m9.AddShowCallback( callbacks::KNIFE_M9_BAYONET );
		RegisterElement( &stattrak_m9 );

		quality_m9.setup( XOR( "quality" ), XOR( "quality_m9" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_m9.SetCallback( callbacks::SkinUpdate );
		quality_m9.AddShowCallback( callbacks::KNIFE_M9_BAYONET );
		RegisterElement( &quality_m9 );

		seed_m9.setup( XOR( "seed" ), XOR( "seed_m9" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_m9.SetCallback( callbacks::SkinUpdate );
		seed_m9.AddShowCallback( callbacks::KNIFE_M9_BAYONET );
		RegisterElement( &seed_m9 );

		id_huntsman.setup( XOR( "paintkit id" ), XOR( "id_huntsman" ), 3 );
		id_huntsman.SetCallback( callbacks::SkinUpdate );
		id_huntsman.AddShowCallback( callbacks::KNIFE_HUNTSMAN );
		RegisterElement( &id_huntsman );

		stattrak_huntsman.setup( XOR( "stattrak" ), XOR( "stattrak_huntsman" ) );
		stattrak_huntsman.SetCallback( callbacks::SkinUpdate );
		stattrak_huntsman.AddShowCallback( callbacks::KNIFE_HUNTSMAN );
		RegisterElement( &stattrak_huntsman );

		quality_huntsman.setup( XOR( "quality" ), XOR( "quality_huntsman" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_huntsman.SetCallback( callbacks::SkinUpdate );
		quality_huntsman.AddShowCallback( callbacks::KNIFE_HUNTSMAN );
		RegisterElement( &quality_huntsman );

		seed_huntsman.setup( XOR( "seed" ), XOR( "seed_huntsman" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_huntsman.SetCallback( callbacks::SkinUpdate );
		seed_huntsman.AddShowCallback( callbacks::KNIFE_HUNTSMAN );
		RegisterElement( &seed_huntsman );

		id_falchion.setup( XOR( "paintkit id" ), XOR( "id_falchion" ), 3 );
		id_falchion.SetCallback( callbacks::SkinUpdate );
		id_falchion.AddShowCallback( callbacks::KNIFE_FALCHION );
		RegisterElement( &id_falchion );

		stattrak_falchion.setup( XOR( "stattrak" ), XOR( "stattrak_falchion" ) );
		stattrak_falchion.SetCallback( callbacks::SkinUpdate );
		stattrak_falchion.AddShowCallback( callbacks::KNIFE_FALCHION );
		RegisterElement( &stattrak_falchion );

		quality_falchion.setup( XOR( "quality" ), XOR( "quality_falchion" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_falchion.SetCallback( callbacks::SkinUpdate );
		quality_falchion.AddShowCallback( callbacks::KNIFE_FALCHION );
		RegisterElement( &quality_falchion );

		seed_falchion.setup( XOR( "seed" ), XOR( "seed_falchion" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_falchion.SetCallback( callbacks::SkinUpdate );
		seed_falchion.AddShowCallback( callbacks::KNIFE_FALCHION );
		RegisterElement( &seed_falchion );

		id_bowie.setup( XOR( "paintkit id" ), XOR( "id_bowie" ), 3 );
		id_bowie.SetCallback( callbacks::SkinUpdate );
		id_bowie.AddShowCallback( callbacks::KNIFE_BOWIE );
		RegisterElement( &id_bowie );

		stattrak_bowie.setup( XOR( "stattrak" ), XOR( "stattrak_bowie" ) );
		stattrak_bowie.SetCallback( callbacks::SkinUpdate );
		stattrak_bowie.AddShowCallback( callbacks::KNIFE_BOWIE );
		RegisterElement( &stattrak_bowie );

		quality_bowie.setup( XOR( "quality" ), XOR( "quality_bowie" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_bowie.SetCallback( callbacks::SkinUpdate );
		quality_bowie.AddShowCallback( callbacks::KNIFE_BOWIE );
		RegisterElement( &quality_bowie );

		seed_bowie.setup( XOR( "seed" ), XOR( "seed_bowie" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_bowie.SetCallback( callbacks::SkinUpdate );
		seed_bowie.AddShowCallback( callbacks::KNIFE_BOWIE );
		RegisterElement( &seed_bowie );

		id_butterfly.setup( XOR( "paintkit id" ), XOR( "id_butterfly" ), 3 );
		id_butterfly.SetCallback( callbacks::SkinUpdate );
		id_butterfly.AddShowCallback( callbacks::KNIFE_BUTTERFLY );
		RegisterElement( &id_butterfly );

		stattrak_butterfly.setup( XOR( "stattrak" ), XOR( "stattrak_butterfly" ) );
		stattrak_butterfly.SetCallback( callbacks::SkinUpdate );
		stattrak_butterfly.AddShowCallback( callbacks::KNIFE_BUTTERFLY );
		RegisterElement( &stattrak_butterfly );

		quality_butterfly.setup( XOR( "quality" ), XOR( "quality_butterfly" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_butterfly.SetCallback( callbacks::SkinUpdate );
		quality_butterfly.AddShowCallback( callbacks::KNIFE_BUTTERFLY );
		RegisterElement( &quality_butterfly );

		seed_butterfly.setup( XOR( "seed" ), XOR( "seed_butterfly" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_butterfly.SetCallback( callbacks::SkinUpdate );
		seed_butterfly.AddShowCallback( callbacks::KNIFE_BUTTERFLY );
		RegisterElement( &seed_butterfly );

		id_daggers.setup( XOR( "paintkit id" ), XOR( "id_daggers" ), 3 );
		id_daggers.SetCallback( callbacks::SkinUpdate );
		id_daggers.AddShowCallback( callbacks::KNIFE_SHADOW_DAGGERS );
		RegisterElement( &id_daggers );

		stattrak_daggers.setup( XOR( "stattrak" ), XOR( "stattrak_daggers" ) );
		stattrak_daggers.SetCallback( callbacks::SkinUpdate );
		stattrak_daggers.AddShowCallback( callbacks::KNIFE_SHADOW_DAGGERS );
		RegisterElement( &stattrak_daggers );

		quality_daggers.setup( XOR( "quality" ), XOR( "quality_daggers" ), 1.f, 100.f, true, 0, 100.f, 1.f, XOR( L"%" ) );
		quality_daggers.SetCallback( callbacks::SkinUpdate );
		quality_daggers.AddShowCallback( callbacks::KNIFE_SHADOW_DAGGERS );
		RegisterElement( &quality_daggers );

		seed_daggers.setup( XOR( "seed" ), XOR( "seed_daggers" ), 0.f, 255.f, true, 0, 0.f, 1.f );
		seed_daggers.SetCallback( callbacks::SkinUpdate );
		seed_daggers.AddShowCallback( callbacks::KNIFE_SHADOW_DAGGERS );
		RegisterElement( &seed_daggers );

		// col 2.
		knife.setup( XOR( "knife model" ), XOR( "skins_knife_model" ), { XOR( "off" ), XOR( "bayonet" ), XOR( "bowie" ), XOR( "butterfly" ), XOR( "falchion" ), XOR( "flip" ), XOR( "gut" ), XOR( "huntsman" ), XOR( "karambit" ), XOR( "m9 bayonet" ), XOR( "daggers" ) } );
		knife.SetCallback( callbacks::SkinUpdate );
		RegisterElement( &knife, 1 );

		glove.setup( XOR( "glove model" ), XOR( "skins_glove_model" ), { XOR( "off" ), XOR( "bloodhound" ), XOR( "sport" ), XOR( "driver" ), XOR( "handwraps" ), XOR( "moto" ), XOR( "specialist" ) } );
		glove.SetCallback( callbacks::ForceFullUpdate );
		RegisterElement( &glove, 1 );

		glove_id.setup( XOR( "glove paintkit id" ), XOR( "skins_glove_id" ), 2 );
		glove_id.SetCallback( callbacks::ForceFullUpdate );
		RegisterElement( &glove_id, 1 );
	}
};

class MiscTab : public Tab {
public:
	// col1.
	Slider    fov_amt;
	Checkbox fast_stop;
	Checkbox bhop;
	Checkbox airduck;
	Checkbox autostrafe;
	Checkbox enableautobuy;
	Checkbox zeusbot;
	Checkbox knifebot;
	Checkbox enablefakewalk;
	Keybind fakewalk;
	Dropdown buy1;
	Dropdown buy2;
	MultiDropdown buy3;
	Checkbox      weaponpurchases;
	Checkbox      logdamagedealt;
	Keybind       fake_latency;
	Slider		  fake_latency_amt;

	// col2.
	Colorpicker menu_color;
	Slider bg_alpha;
	Slider text_alpha;
	Slider testlo25;
	Checkbox autoaccept;
	Checkbox unlock;
	Checkbox killfeed;
	Checkbox ranks;
	Dropdown config;
	Keybind  key1;
	Keybind  key2;
	Keybind  key3;
	Keybind  key4;
	Keybind  key5;
	Keybind  key6;
	Button   save;
	Button   load;
	Checkbox interpolation;
	Checkbox bodeeeelean;
	Checkbox dumper;
	Checkbox senddata;
	Dropdown esp_style;

public:
	void init( ) {
		SetTitle( XOR( "misc" ) );

		fov_amt.setup("override fov", XOR("fov_amt"), 60.f, 120.f, true, 0, 90.f, 1.f, XOR(L"°"));
		RegisterElement(&fov_amt);

		fast_stop.setup(XOR("standalone quick stop"), XOR("fast_stop"));
		RegisterElement(&fast_stop);

		bhop.setup(XOR("bunny hop"), XOR("bhop"));
		RegisterElement(&bhop);

		autostrafe.setup(XOR("air strafe"), XOR("autostrafe"));
		RegisterElement(&autostrafe);

		//airduck.setup(XOR("air duck"), XOR("airduck"));
		//RegisterElement(&airduck);

		knifebot.setup(XOR("knifebot"), XOR("knifebot"));
		RegisterElement(&knifebot);

		zeusbot.setup(XOR("zeusbot"), XOR("zeusbot"));
		RegisterElement(&zeusbot);

		enablefakewalk.setup(XOR("slow motion"), XOR("enablefakewalk"));
		RegisterElement(&enablefakewalk);

		fakewalk.setup(XOR("slow motion bind"), XOR("fakewalk"));
		fakewalk.AddShowCallback(callbacks::ToggleFakeWalkOn);
		RegisterElement(&fakewalk);

		fake_latency.setup(XOR("fake latency"), XOR("fake_latency"));
		fake_latency.SetToggleCallback(callbacks::ToggleFakeLatency);
		RegisterElement(&fake_latency);

		fake_latency_amt.setup("", XOR("fake_latency_amt"), 10.f, 1000.f, false, 0, 1000.f, 10.f, XOR(L"ms"));
		fake_latency_amt.AddShowCallback(callbacks::ToggleFakeLatencyOn);
		RegisterElement(&fake_latency_amt);

		//weaponpurchases.setup(XOR("log weapon purchases"), XOR("weaponpurchases"));
		//RegisterElement(&weaponpurchases);

		logdamagedealt.setup(XOR("log damage dealt"), XOR("logdamagedealt"));
		RegisterElement(&logdamagedealt);

		killfeed.setup(XOR("persistent killfeed"), XOR("killfeed"));
		killfeed.SetCallback(callbacks::ToggleKillfeed);
		RegisterElement(&killfeed);

		enableautobuy.setup(XOR("auto buy"), XOR("enableautobuy"));
		RegisterElement(&enableautobuy);

		buy1.setup(XOR("auto buy"), XOR("auto_buy1"),
			{
				XOR("none"),
				XOR("awp"),
				XOR("ssg08"),
				XOR("g3sg1/scar-20"),
			}, false);
		buy1.AddShowCallback(callbacks::autobuyon);
		RegisterElement(&buy1);

		buy2.setup("", XOR("auto_buy2"),
			{
				XOR("none"),
				XOR("elite"),
				XOR("p250"),
				XOR("tec9/five-seven"),
				XOR("deagle/r8"),
			}, false);
		buy2.AddShowCallback(callbacks::autobuyon);
		RegisterElement(&buy2);

		buy3.setup("", XOR("auto_buy3"),
			{
				XOR("vest"),
				XOR("vesthelm"),
				XOR("taser"),
				XOR("defuser"),
				XOR("molotov"),
				XOR("hegrenade"),
				XOR("smokegrenade"),
			}, false);
		buy3.AddShowCallback(callbacks::autobuyon);
		RegisterElement(&buy3);

		esp_style.setup(XOR("esp style"), XOR("esp_style"), { XOR("kaaba style"), XOR("unique ig style") });
		RegisterElement(&esp_style, 1);

		bg_alpha.setup("indicator background alpha", XOR("indicbgalpha"), 0.f, 255.f, true, 0, 255.f, 1.f, XOR(L"%"));
		bg_alpha.AddShowCallback(callbacks::backgroundindicalphawhatever);
		RegisterElement(&bg_alpha, 1);

		text_alpha.setup("indicator text alpha", XOR("text_alpha"), 0.f, 255.f, true, 0, 255.f, 1.f, XOR(L"%"));
		text_alpha.AddShowCallback(callbacks::backgroundindicalphawhatever);
		RegisterElement(&text_alpha, 1);

		interpolation.setup(XOR("interpolation"), XOR("interpolation"));
		RegisterElement(&interpolation, 1);

		//bodeeeelean.setup(XOR("bodee lean"), XOR("bodeeeelean"));
		//RegisterElement(&bodeeeelean, 1);

		menu_color.setup(XOR("menu color"), XOR("menu_color"), { colors::light_blue }, &g_gui.m_color);
		RegisterElement(&menu_color, 1);

		dumper.setup(XOR("listen to voice data"), XOR("vcdatadbg"));
		RegisterElement(&dumper, 1);

		senddata.setup(XOR("send networked data"), XOR("senddata"));
		RegisterElement(&senddata, 1);

		config.setup(XOR("configuration"), XOR("cfg"), { XOR("1"), XOR("2"), XOR("3"), XOR("4"), XOR("5"), XOR("6") });
		config.RemoveFlags(ElementFlags::SAVE);
		RegisterElement(&config, 1);

		key1.setup(XOR("configuration key 1"), XOR("cfg_key1"));
		key1.RemoveFlags(ElementFlags::SAVE);
		key1.SetCallback(callbacks::SaveHotkeys);
		key1.AddShowCallback(callbacks::IsConfig1);
		key1.SetToggleCallback(callbacks::ConfigLoad1);
		RegisterElement(&key1, 1);

		key2.setup(XOR("configuration key 2"), XOR("cfg_key2"));
		key2.RemoveFlags(ElementFlags::SAVE);
		key2.SetCallback(callbacks::SaveHotkeys);
		key2.AddShowCallback(callbacks::IsConfig2);
		key2.SetToggleCallback(callbacks::ConfigLoad2);
		RegisterElement(&key2, 1);

		key3.setup(XOR("configuration key 3"), XOR("cfg_key3"));
		key3.RemoveFlags(ElementFlags::SAVE);
		key3.SetCallback(callbacks::SaveHotkeys);
		key3.AddShowCallback(callbacks::IsConfig3);
		key3.SetToggleCallback(callbacks::ConfigLoad3);
		RegisterElement(&key3, 1);

		key4.setup(XOR("configuration key 4"), XOR("cfg_key4"));
		key4.RemoveFlags(ElementFlags::SAVE);
		key4.SetCallback(callbacks::SaveHotkeys);
		key4.AddShowCallback(callbacks::IsConfig4);
		key4.SetToggleCallback(callbacks::ConfigLoad4);
		RegisterElement(&key4, 1);

		key5.setup(XOR("configuration key 5"), XOR("cfg_key5"));
		key5.RemoveFlags(ElementFlags::SAVE);
		key5.SetCallback(callbacks::SaveHotkeys);
		key5.AddShowCallback(callbacks::IsConfig5);
		key5.SetToggleCallback(callbacks::ConfigLoad5);
		RegisterElement(&key5, 1);

		key6.setup(XOR("configuration key 6"), XOR("cfg_key6"));
		key6.RemoveFlags(ElementFlags::SAVE);
		key6.SetCallback(callbacks::SaveHotkeys);
		key6.AddShowCallback(callbacks::IsConfig6);
		key6.SetToggleCallback(callbacks::ConfigLoad6);
		RegisterElement(&key6, 1);

		load.setup(XOR("load"));
		load.SetCallback(callbacks::ConfigLoad);
		RegisterElement(&load, 1);

		save.setup(XOR("save"));
		save.SetCallback(callbacks::ConfigSave);
		RegisterElement(&save, 1);
	}
};

class MainForm : public Form {
public:
	// aimbot.
	AimbotTab    aimbot;
	AntiAimTab   antiaim;

	// visuals.
	PlayersTab	 players;
	VisualsTab	 visuals;

	// misc.
	SkinsTab     skins;
	MiscTab	     misc;
public:
	void init( ) {
		SetPosition( 50, 50 );
		SetSize( 630, 500 );

		// aim.
		RegisterTab( &aimbot );
		aimbot.init( );

		RegisterTab( &antiaim );
		antiaim.init( );

		// visuals.
		RegisterTab( &players );
		players.init( );

		RegisterTab( &visuals );
		visuals.init( );

		// misc.

		RegisterTab( &skins );
		skins.init( );

		RegisterTab( &misc );
		misc.init( );
	}
};

class Menu {
public:
	MainForm main;

public:
	void init( ) {
		Colorpicker::init( );

		main.init( );
		g_gui.RegisterForm( &main, VK_INSERT );
	}
};

extern Menu g_menu;