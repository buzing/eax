#pragma once


struct RestoreData {
	void Reset() {
		m_aimPunchAngle = ang_t();
		m_aimPunchAngleVel = ang_t();
		m_viewPunchAngle = ang_t();

		m_vecViewOffset = vec3_t();
		m_vecBaseVelocity = vec3_t();
		m_vecVelocity = vec3_t();
		m_vecOrigin = vec3_t();

		m_flFallVelocity = 0.0f;
		m_flVelocityModifier = 0.0f;
		m_flDuckAmount = 0.0f;
		m_flDuckSpeed = 0.0f;
		m_surfaceFriction = 0.0f;

		m_fAccuracyPenalty = 0.0f;
		m_flRecoilIndex = 0.f;

		m_hGroundEntity = 0;
		m_nMoveType = 0;
		m_nFlags = 0;
		m_nTickBase = 0;
	}

	void Setup(Player* player);

	void Apply(Player* player);

	CMoveData m_MoveData;

	ang_t m_aimPunchAngle = { };
	ang_t m_aimPunchAngleVel = { };
	ang_t m_viewPunchAngle = { };

	vec3_t m_vecViewOffset = { };
	vec3_t m_vecBaseVelocity = { };
	vec3_t m_vecVelocity = { };
	vec3_t m_vecOrigin = { };

	float m_flFallVelocity = 0.0f;
	float m_flVelocityModifier = 0.0f;
	float m_flDuckAmount = 0.0f;
	float m_flDuckSpeed = 0.0f;
	float m_surfaceFriction = 0.0f;

	float m_fAccuracyPenalty = 0.0f;
	float m_flRecoilIndex = 0;

	int m_hGroundEntity = 0;
	int m_nMoveType = 0;
	int m_nFlags = 0;
	int m_nTickBase = 0;

	bool is_filled = false;
};

struct PlayerData {
	int m_nTickbase = 0;
	int m_nCommandNumber = 0;
	ang_t m_aimPunchAngle = { };
	ang_t m_aimPunchAngleVel = { };
	vec3_t m_vecViewOffset = { };
	float m_flVelocityModifier = 0.f;
};

struct LastPredData {
	ang_t m_aimPunchAngle = { };
	ang_t m_aimPunchAngleVel = { };
	vec3_t m_vecBaseVelocity = { };
	vec3_t m_vecViewOffset = { };
	vec3_t m_vecOrigin = { };
	float m_flFallVelocity = 0.0f;
	float m_flDuckAmount = 0.0f;
	float m_flDuckSpeed = 0.0f;
	float m_flVelocityModifier = 0.0f;
	int m_nTickBase = 0;
};

struct PredictionData {
	CUserCmd* m_pCmd = nullptr;
	bool* m_pSendPacket = nullptr;

	Player* m_pPlayer = nullptr;
	Weapon* m_pWeapon = nullptr;
	WeaponInfo* m_pWeaponInfo = nullptr;

	int m_nTickBase = 0;
	int m_fFlags = 0;
	vec3_t m_vecVelocity{ };

	bool m_bInPrediction = false;
	bool m_bFirstTimePrediction = false;

	float m_flCurrentTime = 0.0f;
	float m_flFrameTime = 0.0f;

	CMoveData m_MoveData = { };
	RestoreData m_RestoreData;

	PlayerData m_Data[150] = { };

	std::vector<int> m_ChokedNr;

	bool m_bInitDatamap = false;
	int m_Offset_nImpulse;
	int m_Offset_nButtons;
	int m_Offset_afButtonLast;
	int m_Offset_afButtonPressed;
	int m_Offset_afButtonReleased;
	int m_Offset_afButtonForced;

	void Setup() {
		m_pCmd = nullptr;
		m_pSendPacket = nullptr;

		m_pPlayer = nullptr;
		m_pWeapon = nullptr;
		m_pWeaponInfo = nullptr;

		m_nTickBase = 0;
		m_fFlags = 0;
		m_vecVelocity = vec3_t();

		m_bInPrediction = false;
		m_bFirstTimePrediction = false;

		m_flCurrentTime = 0.0f;
		m_flFrameTime = 0.0f;

		m_MoveData = { };
		m_RestoreData;

		m_Data[150] = { };

		m_bInitDatamap = false;
		m_Offset_nImpulse = 0;
		m_Offset_nButtons = 0;
		m_Offset_afButtonLast = 0;
		m_Offset_afButtonPressed = 0;
		m_Offset_afButtonReleased = 0;
		m_Offset_afButtonForced = 0;
	}
};

class InputPrediction {
public:
	float m_curtime;
	float m_frametime;

	LastPredData m_LastPredictedData;
	CUserCmd* m_pLastCmd;


	CUserCmd* m_pCmd = nullptr;
	bool* m_pSendPacket = nullptr;

	Player* m_pPlayer = nullptr;
	Weapon* m_pWeapon = nullptr;
	WeaponInfo* m_pWeaponInfo = nullptr;

	int m_nTickBase = 0;
	int m_fFlags = 0;
	vec3_t m_vecVelocity{ };

	bool m_bInPrediction = false;
	bool m_bFirstTimePrediction = false;

	float m_flCurrentTime = 0.0f;
	float m_flFrameTime = 0.0f;

	CMoveData m_MoveData = { };
	RestoreData m_RestoreData;

	PlayerData m_Data[150] = { };

	std::vector<int> m_ChokedNr;

	bool m_bInitDatamap = false;
	int m_Offset_nImpulse;
	int m_Offset_nButtons;
	int m_Offset_afButtonLast;
	int m_Offset_afButtonPressed;
	int m_Offset_afButtonReleased;
	int m_Offset_afButtonForced;

	int m_iSeqDiff = 0;

	float m_flSpread = 0.f;
	float m_flInaccuracy = 0.f;
	float m_flWeaponRange = 0.f;

	bool m_bFixSendDataGram = false;
	bool m_bNeedStoreNetvarsForFixingNetvarCompresion = false;
	bool m_bGetNetvarCompressionData = false;
public:
	void update();
	void run();
	void restore();
	void Begin(CUserCmd* cmd, bool* send_packet, int command_number);
	void Repredict();
	void End();
	void Invalidate();

	int GetFlags();
	vec3_t GetVelocity();

	CUserCmd* GetCmd();

	float GetFrametime();
	float GetCurtime();
	float GetSpread();
	float GetInaccuracy();
	float GetWeaponRange();

	void OnFrameStageNotify(Stage_t stage);
	void OnRunCommand(Player* player, CUserCmd* cmd);

	void PostEntityThink(Player* player);

	bool InPrediction() {
		return m_bInPrediction;
	}
};


extern InputPrediction g_inputpred;