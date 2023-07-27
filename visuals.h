#pragma once

struct OffScreenDamageData_t {
    float m_time, m_color_step;
    Color m_color;

    __forceinline OffScreenDamageData_t( ) : m_time{ 0.f }, m_color{ colors::white } {}
    __forceinline OffScreenDamageData_t( float time, float m_color_step, Color color ) : m_time{ time }, m_color{ color } {}
};

struct matrices_t {
	int                         ent_index;
	ModelRenderInfo_t           info;
	DrawModelState_t            state;
	matrix3x4_t                 pBoneToWorld[128] = { };
	float                       time;
	matrix3x4_t                 model_to_world;
};

struct impact_info {
	float x, y, z;
	float time;
};

struct client_hit_verify_t
{
	vec3_t pos;
	float time;
	float expires;
};


class Visuals {
public:
	std::array< bool, 64 >                  m_draw;
	std::array< float, 2048 >               m_opacities;
    std::array< OffScreenDamageData_t, 64 > m_offscreen_damage;
	std::array<std::pair<short, WeaponInfo*>, 65> m_arrWeaponInfos;
	vec2_t                                  m_crosshair;
	std::vector<impact_info>                m_impacts;
	bool                                    m_thirdperson;
	float					                m_hit_start, m_hit_end, m_hit_duration;

    // info about planted c4.
    bool        m_c4_planted;
    Entity* m_planted_c4;
    float       m_planted_c4_explode_time;
    vec3_t      m_planted_c4_explosion_origin;
    float       m_planted_c4_damage;
    float       m_planted_c4_radius;
    float       m_planted_c4_radius_scaled;
    float       m_planted_c4_defuse;
    bool        m_planted_c4_indefuse;
    bool        m_planted_c4_planting;
    int         m_final_damage;
    float       m_plant_start;
    float       m_plant_end;
    float       m_plant_duration;
    float       m_defuse_start;
    float       m_defuse_end;
    float       m_defuse_duration;
    float       m_planted_c4_start;
    float       m_planted_c4_end;
	float m_anim_start_time;
    std::string m_last_bombsite;

	std::vector< matrices_t >               m_hit_matrix;

	IMaterial* smoke1;
	IMaterial* smoke2;
	IMaterial* smoke3;
	IMaterial* smoke4;

	std::unordered_map< int, char > m_weapon_icons = {
        { WEAPON_DEAGLE, 'A' },
		{ WEAPON_ELITE, 'B' },
		{ WEAPON_FIVESEVEN, 'C' },
		{ WEAPON_GLOCK, 'D' },
		{ WEAPON_P2000, 'E' },
		{ WEAPON_P250, 'F' },
		{ WEAPON_USPS, 'G' },
		{ WEAPON_TEC9, 'H' },
		{ WEAPON_CZ75, 'I' },
		{ WEAPON_REVOLVER, 'J' },
		{ WEAPON_MAC10, 'K' },
		{ WEAPON_UMP45, 'L' },
		{ WEAPON_BIZON, 'M' },
		{ WEAPON_MP7, 'N' },
		{ WEAPON_MP5SD, 'L' },
		{ WEAPON_MP9, 'O' },
		{ WEAPON_P90, 'P' },
		{ WEAPON_GALIL, 'Q' },
		{ WEAPON_FAMAS, 'R' },
		{ WEAPON_M4A4, 'S' },
		{ WEAPON_M4A1S, 'T' },
		{ WEAPON_AUG, 'U' },
		{ WEAPON_SG553, 'V' },
		{ WEAPON_AK47, 'W' },
		{ WEAPON_G3SG1, 'X' },
		{ WEAPON_SCAR20, 'Y' },
		{ WEAPON_AWP, 'Z' },
		{ WEAPON_SSG08, 'a' },
		{ WEAPON_XM1014, 'b' },
		{ WEAPON_SAWEDOFF, 'c' },
		{ WEAPON_MAG7, 'd' },
		{ WEAPON_NOVA, 'e' },
		{ WEAPON_NEGEV, 'f' },
		{ WEAPON_M249, 'g' },
		{ WEAPON_ZEUS, 'h' },
		{ WEAPON_KNIFE_T, 'i' },
		{ WEAPON_KNIFE, 'j' },
		{ WEAPON_KNIFE_FALCHION, '0' },
		{ WEAPON_KNIFE_BAYONET, '1' },
		{ WEAPON_KNIFE_FLIP, '2' },
		{ WEAPON_KNIFE_GUT, '3' },
		{ WEAPON_KNIFE_KARAMBIT, '4' },
		{ WEAPON_KNIFE_M9_BAYONET, '5' },
		{ WEAPON_KNIFE_TACTICAL, '6' },
		{ WEAPON_KNIFE_SURVIVAL_BOWIE, '7' },
		{ WEAPON_KNIFE_BUTTERFLY, '8' },
		{ WEAPON_KNIFE_URSUS, 'j' },
		{ WEAPON_KNIFE_GYPSY_JACKKNIFE, 'j' },
		{ WEAPON_KNIFE_STILETTO, 'j' },
		{ WEAPON_KNIFE_WIDOWMAKER, 'j' },
		{ WEAPON_FLASHBANG, 'k' },
		{ WEAPON_HEGRENADE, 'l' },
		{ WEAPON_SMOKEGRENADE, 'm' },
		{ WEAPON_MOLOTOV, 'n' },
		{ WEAPON_DECOY, 'o' },
		{ WEAPON_INC, 'p' },
		{ WEAPON_C4, 'q' },
	};

public:
	struct BulletImpactInfo
	{
		float m_flExpTime;
		vec3_t m_vecStartPos;
		vec3_t m_vecHitPos;
		Color m_cColor;
		int m_nIndex;
		int m_nTickBase;
		bool ignore;
		bool m_bRing;
	};

	std::vector<BulletImpactInfo> bulletImpactInfo;

	static void ModulateWorld( );
	void ThirdpersonThink( );
	void Hitmarker( );
	void NoSmoke( );
	void think( );
	void Spectators( );
	void StatusIndicators( );
	void ImpactData();
	void ManualAntiAim();
    void PenetrationCrosshair( );
    void DrawPlantedC4();
	void draw( Entity* ent );
	void DrawProjectile( Weapon* ent );
	void AutopeekIndicator();
	void DrawItem( Weapon* item );
	void OffScreen( Player* player, int alpha );
    std::string GetWeaponIcon(const int id);
	void DrawPlayer( Player* player );
	bool GetPlayerBoxRect( Player* player, Rect& box );
	void AddMatrix(Player* player, matrix3x4_t* bones);
	void override_material(bool ignoreZ, bool use_env, Color& color, IMaterial* material);
	void on_post_screen_effects();
	void DrawSkeleton( Player* player, int opacity );
	void RenderGlow( );
//	void DrawBeam();
	void Add(BulletImpactInfo beamEffect);
	void DrawHistorySkeleton(Player* player, int opacity);
};

extern Visuals g_visuals;