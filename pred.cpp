#include "includes.h"

InputPrediction g_inputpred{ };;

void InputPrediction::Begin(CUserCmd* _cmd, bool* send_packet, int command_number) {

	m_pCmd = _cmd;
	m_RestoreData.is_filled = false;
	m_pSendPacket = send_packet;

	if (!m_pCmd)
		return;

	m_pPlayer = g_cl.m_local;

	if (!m_pPlayer || !m_pPlayer->alive())
		return;

	m_pWeapon = m_pPlayer->GetActiveWeapon();

	if (!m_pWeapon)
		return;

	m_pWeaponInfo = m_pWeapon->GetWpnData();

	if (!m_pWeaponInfo)
		return;

	m_bInPrediction = true;

	bool valid{ g_csgo.m_cl->m_delta_tick > 0 };
	int start = g_csgo.m_cl->m_last_command_ack;
	int stop = g_csgo.m_cl->m_last_outgoing_command + g_csgo.m_cl->m_choked_commands;

	// call CPrediction::Update.
	g_csgo.m_prediction->Update(g_csgo.m_cl->m_delta_tick, valid, start, stop);

	// correct tickbase.
	if (!m_pLastCmd || m_pLastCmd->m_predicted)
		m_iSeqDiff = command_number - m_pPlayer->m_nTickBase();

	const int nTickBaseBackup = m_pPlayer->m_nTickBase();

	m_nTickBase = command_number - m_iSeqDiff;

	m_pLastCmd = m_pCmd;

	m_fFlags = m_pPlayer->m_fFlags();
	m_vecVelocity = m_pPlayer->m_vecVelocity();

	m_flCurrentTime = g_csgo.m_globals->m_curtime;
	m_flFrameTime = g_csgo.m_globals->m_frametime;

	m_pCmd->m_random_seed = g_csgo.MD5_PseudoRandom(m_pCmd->m_command_number) & 0x7fffffff;

	//bool bEnginePaused = *( bool* )( uintptr_t( g_csgo.m_prediction ) + 10 );

	// StartCommand rebuild
	*(CUserCmd**)((uintptr_t)m_pPlayer + 0x3338) = m_pCmd;
	*g_csgo.m_nPredictionRandomSeed = m_pCmd->m_random_seed;
	g_csgo.m_pPredictionPlayer = m_pPlayer;

	m_pPlayer->m_pCurrentCommand() = m_pCmd; // m_pCurrentCommand
	*reinterpret_cast<CUserCmd**>(uint32_t(m_pPlayer) + 0x326C) = m_pCmd; // unk01

	g_csgo.m_globals->m_curtime = game::TICKS_TO_TIME(m_nTickBase);
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;

	g_csgo.m_game_movement->StartTrackPredictionErrors(m_pPlayer);

	// Setup input.
	g_csgo.m_prediction->SetupMove(m_pPlayer, m_pCmd, g_csgo.m_move_helper, &m_MoveData);

	m_RestoreData.m_MoveData = m_MoveData;
	m_RestoreData.is_filled = true;
	m_RestoreData.Setup(m_pPlayer);

	m_bFirstTimePrediction = g_csgo.m_prediction->m_first_time_predicted;
	m_bInPrediction = g_csgo.m_prediction->m_in_prediction;

	g_csgo.m_prediction->m_first_time_predicted = true;
	g_csgo.m_prediction->m_in_prediction = false;

	g_csgo.m_move_helper->SetHost(m_pPlayer);

	g_csgo.m_game_movement->ProcessMovement(m_pPlayer, &m_MoveData);

	g_csgo.m_prediction->FinishMove(m_pPlayer, m_pCmd, &m_MoveData);

	m_nTickBase = nTickBaseBackup;

	g_csgo.m_prediction->m_first_time_predicted = m_bFirstTimePrediction;
	g_csgo.m_prediction->m_in_prediction = m_bInPrediction;

	m_pWeapon->UpdateAccuracyPenalty();

	m_flSpread = m_pWeapon->GetSpread();
	m_flInaccuracy = m_pWeapon->GetInaccuracy();
	m_flWeaponRange = m_pWeaponInfo->m_range;
}

void InputPrediction::Repredict() {

	bool valid{ g_csgo.m_cl->m_delta_tick > 0 };
	int start = g_csgo.m_cl->m_last_command_ack;
	int stop = g_csgo.m_cl->m_last_outgoing_command + g_csgo.m_cl->m_choked_commands;

	// call CPrediction::Update.
	g_csgo.m_prediction->Update(g_csgo.m_cl->m_delta_tick, valid, start, stop);

	m_RestoreData.Apply(m_pPlayer);

	// set move data members that getting copied from CUserCmd
	// rebuilded from CPrediction::SetupMove
	m_MoveData = m_RestoreData.m_MoveData;
	m_MoveData.m_flForwardMove = m_pCmd->m_forward_move;
	m_MoveData.m_flSideMove = m_pCmd->m_side_move;
	m_MoveData.m_flUpMove = m_pCmd->m_up_move;
	m_MoveData.m_nButtons = m_pCmd->m_buttons;
	m_MoveData.m_vecOldAngles.y = m_pCmd->m_view_angles.x;
	m_MoveData.m_vecOldAngles.z = m_pCmd->m_view_angles.y;
	m_MoveData.m_outStepHeight = m_pCmd->m_view_angles.z;
	m_MoveData.m_vecViewAngles.x = m_pCmd->m_view_angles.x;
	m_MoveData.m_vecViewAngles.y = m_pCmd->m_view_angles.y;
	m_MoveData.m_vecViewAngles.z = m_pCmd->m_view_angles.z;
	m_MoveData.m_nImpulseCommand = m_pCmd->m_impulse;

	g_csgo.m_prediction->m_in_prediction = true;
	g_csgo.m_prediction->m_first_time_predicted = false;

	g_csgo.m_move_helper->SetHost(m_pPlayer);
	g_csgo.m_game_movement->ProcessMovement(m_pPlayer, &m_MoveData);
	g_csgo.m_prediction->FinishMove(m_pPlayer, m_pCmd, &m_MoveData);
	g_csgo.m_move_helper->ProcessImpacts();

	g_csgo.m_prediction->m_first_time_predicted = m_bFirstTimePrediction;
	g_csgo.m_prediction->m_in_prediction = m_bInPrediction;

	m_pWeapon->UpdateAccuracyPenalty();

	m_flSpread = m_pWeapon->GetSpread();
	m_flInaccuracy = m_pWeapon->GetInaccuracy();
	m_flWeaponRange = m_pWeaponInfo->m_range;
}

void InputPrediction::PostEntityThink(Player* player) {

	using PostThinkVPhysicsFn = bool(__thiscall*)(Entity*);
	static auto PostThinkVPhysics = pattern::find(g_csgo.m_client_dll, "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB ? ? ? ? ? 75 50 8B 0D").as<PostThinkVPhysicsFn>();

	using SimulatePlayerSimulatedEntitiesFn = void(__thiscall*)(Entity*);
	static auto SimulatePlayerSimulatedEntities = pattern::find(g_csgo.m_client_dll, "56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 72 90 8B 86").as<SimulatePlayerSimulatedEntitiesFn>();

	if (player && player->alive()) {
		player->UpdateCollisionBounds();

		if (player->m_fFlags() & FL_ONGROUND)
			*reinterpret_cast<uintptr_t*>(uintptr_t(player) + 0x3004) = 0;

		if (*reinterpret_cast<int*>(uintptr_t(player) + 0x28AC) == -1) {
			using SetSequenceFn = void(__thiscall*)(void*, int);
			util::get_method<SetSequenceFn>(player, 213)(player, 0);
		}

		using StudioFrameAdvanceFn = void(__thiscall*)(void*);
		util::get_method<StudioFrameAdvanceFn>(player, 214)(player);

		PostThinkVPhysics(player);
	}

	SimulatePlayerSimulatedEntities(player);
}

void InputPrediction::End() {
	if (!m_pCmd || !m_pPlayer || !m_pWeapon)
		return;

	//CPrediction::RunPostThink rebuild
	PostEntityThink(m_pPlayer);
	g_csgo.m_game_movement->FinishTrackPredictionErrors(m_pPlayer);
	g_csgo.m_move_helper->SetHost(nullptr);

	// Finish command
	*(CUserCmd**)((uintptr_t)m_pPlayer + 0x3338) = nullptr;
	*g_csgo.m_nPredictionRandomSeed = 0;
	g_csgo.m_pPredictionPlayer = nullptr;

	g_csgo.m_game_movement->Reset();

	g_csgo.m_globals->m_curtime = m_flCurrentTime;
	g_csgo.m_globals->m_frametime = m_flFrameTime;

	m_bInPrediction = false;
}

void InputPrediction::Invalidate() {
	m_pCmd = nullptr;
	m_pPlayer = nullptr;
	m_pWeapon = nullptr;
	m_pSendPacket = nullptr;

	m_RestoreData.Reset();
	m_RestoreData.is_filled = false;
}

void InputPrediction::update() {

	if (!g_cl.m_local || !g_cl.m_processing)
		return;

	const int m_tick = g_csgo.m_cl->m_delta_tick;
	if (m_tick > 0) {
		g_csgo.m_prediction->Update(m_tick, true, g_csgo.m_cl->m_last_command_ack,
			g_csgo.m_cl->m_last_outgoing_command + g_csgo.m_cl->m_choked_commands);
	}

	static bool unlocked_fakelag = false;
	if (!unlocked_fakelag) {
		auto cl_move_clamp = pattern::find(g_csgo.m_engine_dll, XOR("B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC")) + 1;
		unsigned long protect = 0;

		VirtualProtect((void*)cl_move_clamp, 4, PAGE_EXECUTE_READWRITE, &protect);
		*(std::uint32_t*)cl_move_clamp = 62;
		VirtualProtect((void*)cl_move_clamp, 4, protect, &protect);
		unlocked_fakelag = true;
	}
}

int InputPrediction::GetFlags() {
	return m_fFlags;
}

vec3_t InputPrediction::GetVelocity() {
	return m_vecVelocity;
}

CUserCmd* InputPrediction::GetCmd() {
	return m_pCmd;
}

float InputPrediction::GetFrametime() {
	return m_flFrameTime;
}

float InputPrediction::GetCurtime() {
	return m_flCurrentTime;
}

float InputPrediction::GetSpread() {
	return m_flSpread;
}

float InputPrediction::GetInaccuracy() {
	return m_flInaccuracy;
}

float InputPrediction::GetWeaponRange() {
	return m_flWeaponRange;
}

void RestoreData::Setup(Player* player) {

	this->m_aimPunchAngle = player->m_aimPunchAngle();
	this->m_aimPunchAngleVel = player->m_aimPunchAngleVel();
	this->m_viewPunchAngle = player->m_viewPunchAngle();
	this->m_vecViewOffset = player->m_vecViewOffset();
	this->m_vecVelocity = player->m_vecVelocity();
	this->m_vecOrigin = player->m_vecOrigin();
	this->m_flDuckAmount = player->m_flDuckAmount();
	this->m_surfaceFriction = player->m_surfaceFriction();
	this->m_nMoveType = player->m_MoveType();
	this->m_nFlags = player->m_fFlags();

	auto weapon = player->GetActiveWeapon();
	if (weapon) {

		this->m_fAccuracyPenalty = player->m_fAccuracyPenalty();
		this->m_flRecoilIndex = weapon->m_flRecoilIndex();
	}

	this->is_filled = true;
}

void RestoreData::Apply(Player* player) {
	if (!this->is_filled)
		return;

	player->m_aimPunchAngle() = this->m_aimPunchAngle;
	player->m_aimPunchAngleVel() = this->m_aimPunchAngleVel;
	player->m_viewPunchAngle() = this->m_viewPunchAngle;
	player->m_vecViewOffset() = this->m_vecViewOffset;
	player->m_vecVelocity() = this->m_vecVelocity;
	player->m_vecOrigin() = this->m_vecOrigin;
	player->m_flDuckAmount() = this->m_flDuckAmount;
	player->m_surfaceFriction() = this->m_surfaceFriction;
	player->m_MoveType() = this->m_nMoveType;
	player->m_fFlags() = this->m_nFlags;

	auto weapon = player->GetActiveWeapon();
	if (weapon) {
		player->m_fAccuracyPenalty() = this->m_fAccuracyPenalty;
		weapon->m_flRecoilIndex() = this->m_flRecoilIndex;
	}
}


CGlobalVarsBase* CPrediction::GetUnpredictedGlobals() {

	if (this->m_in_prediction)
		return reinterpret_cast<CGlobalVarsBase*>(uint32_t(this) + 0x4c);

	return g_csgo.m_globals;
}
