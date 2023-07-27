#pragma once

class ShotRecord;

class Resolver {
public:
	enum Modes : size_t {
		RESOLVE_NONE = 0,
		RESOLVE_WALK, // 1
		RESOLVE_LBY, // 2
		RESOLVE_EDGE, // 3
		RESOLVE_AIR_LAST, // 4
		RESOLVE_REVERSEFS, // 5
		RESOLVE_BACK, // 6
		RESOLVE_STAND, // 7
		RESOLVE_STAND1, // 8
		RESOLVE_STAND2, // 9
		RESOLVE_STAND3, // 10
		RESOLVE_AIR, // 11
		RESOLVE_BODY, // 12
		RESOLVE_LASTMOVE, //13
		RESOLVE_AIR_BACK, // 14
		RESOLVE_AIR_LBY, // 15
		RESOLVE_OVERRIDE, //16
		RESOLVE_LOW_LBY, // 17
		RESOLVE_DISTORTION, // 18
		RESOLVE_TEST_FS, // 19
		RESOLVE_LOGIC, // 20
		RESOLVE_UPD_LBY, // 21
		RESOLVE_LAST_UPD_LBY, // 22
		RESOLVE_NETWORK, // 23
		RESOLVE_FAKEWALK, // 24,
		RESOLVE_BRUTEFORCE, // 25
		RESOLVE_EXBACK, // 26
		RESOLVE_SEXBACK, // 27
		RESOLVE_SBACK, // 28
		RESOLVE_SFS, // 29
		RESOLVE_UNSAFELM, // 30
		RESOLVE_UNSAFELBY, // 31
	};

public:
	LagRecord* FindIdealRecord( AimPlayer* data );
	LagRecord* FindLastRecord( AimPlayer* data );

	void FindBestAngle( LagRecord* record, AimPlayer* data );

	void OnBodyUpdate( Player* player, float value );
	float GetAwayAngle( LagRecord* record );

	void MatchShot( AimPlayer* data, LagRecord* record );
	void SetMode( LagRecord* record );

	void ResolveAngles( Player* player, LagRecord* record );
	void ResolveWalk(AimPlayer* data, LagRecord* record, Player* player);
	bool IsYawSideways(Player* entity, float yaw, bool smth);
	bool IsYawDistorting(AimPlayer* data, LagRecord* record, LagRecord* previous_record);
	void ResolveStand( AimPlayer* data, LagRecord* record, Player* player );
	void StandNS( AimPlayer* data, LagRecord* record );
	void ResolveOverride(AimPlayer* data, LagRecord* record, Player* player);
	void ResolveAir(AimPlayer* data, LagRecord* record, Player* player);

	void AirNS( AimPlayer* data, LagRecord* record );
	void ResolvePoses( Player* player, LagRecord* record );

public:
	std::array< vec3_t, 64 > m_impacts;
	int	   iPlayers[64];
};

extern Resolver g_resolver;