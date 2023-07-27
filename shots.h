#pragma once

class ShotRecord {
public:
	__forceinline ShotRecord() : m_target{}, m_record{}, m_time{}, m_lat{}, m_aim_point{}, m_had_pred_error{}, m_record_valid{},  m_damage {}, m_pos{}, m_matched{}, m_impacted{}, m_hitgroup{} {}

public:
	Player* m_target;
	LagRecord* m_record;
	float      m_time, m_lat, m_damage, m_range;
	vec3_t     m_pos, m_impact_pos;
	bool       m_matched, m_impacted, m_hurt, m_had_pred_error, m_record_valid;
	ang_t      m_aim_point;
	int        m_hitgroup;
};

class VisualImpactData_t {
public:
	Player* m_target;
	vec3_t m_impact_pos, m_shoot_pos;
	int    m_tickbase;
	bool   m_ignore, m_hit_player;

public:
	__forceinline VisualImpactData_t(Player* ply, const vec3_t& impact_pos, const vec3_t& shoot_pos, int tickbase) :
		m_target{ ply }, m_impact_pos{ impact_pos }, m_shoot_pos{ shoot_pos }, m_tickbase{ tickbase }, m_ignore{ false }, m_hit_player{ false } {}
};

class ImpactRecord {
public:
	__forceinline ImpactRecord() : m_shot{}, m_pos{}, m_tick{} {}

public:
	ShotRecord* m_shot;
	int         m_tick;
	vec3_t      m_pos;
};

class HitRecord {
public:
	__forceinline HitRecord() : m_impact{}, m_group{ -1 }, m_damage{} {}

public:
	ImpactRecord* m_impact;
	int           m_group;
	float         m_damage;
};

class Shots {

public:
	void OnShotFire(Player* target, float damage, int bullets, LagRecord* record, int hitbox, int hitgroup);
	void OnImpact(IGameEvent* evt);
	void OnHurt(IGameEvent* evt);
	void OnWeaponFire(IGameEvent* evt);
	void OnShotMiss(ShotRecord& shot);
	void Think();

public:
 std::array< std::string, 8 > m_groups = {
        XOR( "body" ),
        XOR( "head" ),
        XOR( "chest" ),
        XOR( "stomach" ),
        XOR( "left arm" ),
        XOR( "right arm" ),
        XOR( "left leg" ),
        XOR( "right leg" )
    };

	std::deque< ShotRecord >          m_shots;
	std::vector< VisualImpactData_t > m_vis_impacts;
	std::deque< ImpactRecord >        m_impacts;
	std::deque< HitRecord >           m_hits;

	float iHitDmg = NULL;
	bool iHit = false;
	bool iHeadshot = false;

	vec3_t iPlayerOrigin, iPlayermins, iPlayermaxs;
	vec2_t iPlayerbottom, iPlayertop;
};

extern Shots g_shots;