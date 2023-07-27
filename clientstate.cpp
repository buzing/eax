#include "includes.h"

bool Hooks::TempEntities( void *msg ) {
	if( !g_cl.m_processing ) {
		return g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );
	}

	const bool ret = g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );

	CEventInfo *ei = g_csgo.m_cl->m_events; 
	CEventInfo *next = nullptr;

	if( !ei ) {
		return ret;
	}

	do {
		next = *reinterpret_cast< CEventInfo ** >( reinterpret_cast< uintptr_t >( ei ) + 0x38 );

		uint16_t classID = ei->m_class_id - 1;

		auto m_pCreateEventFn = ei->m_client_class->m_pCreateEvent;
		if( !m_pCreateEventFn ) {
			continue;
		}

		void *pCE = m_pCreateEventFn( );
		if( !pCE ) {
			continue;
		}

		if( classID == 170 ){
			ei->m_fire_delay = 0.0f;
		}
		ei = next;
	} while( next != nullptr );

	return ret;
}



struct VoiceDataCustom
{
	uint32_t xuid_low{};
	uint32_t xuid_high{};
	int32_t sequence_bytes{};
	uint32_t section_number{};
	uint32_t uncompressed_sample_offset{};

	__forceinline uint8_t* get_raw_data()
	{
		return (uint8_t*)this;
	}
};


struct CSVCMsg_VoiceData_Legacy
{
	char pad_0000[8]; //0x0000
	int32_t client; //0x0008
	int32_t audible_mask; //0x000C
	uint32_t xuid_low{};
	uint32_t xuid_high{};
	void* voide_data_; //0x0018
	int32_t proximity; //0x001C
	//int32_t caster; //0x0020
	int32_t format; //0x0020
	int32_t sequence_bytes; //0x0024
	uint32_t section_number; //0x0028
	uint32_t uncompressed_sample_offset; //0x002C

	__forceinline VoiceDataCustom get_data()
	{
		VoiceDataCustom cdata;
		cdata.xuid_low = xuid_low;
		cdata.xuid_high = xuid_high;
		cdata.sequence_bytes = sequence_bytes;
		cdata.section_number = section_number;
		cdata.uncompressed_sample_offset = uncompressed_sample_offset;
		return cdata;
	}
};

struct CCLCMsg_VoiceData_Legacy
{
	uint32_t INetMessage_Vtable; //0x0000
	char pad_0004[4]; //0x0004
	uint32_t CCLCMsg_VoiceData_Vtable; //0x0008
	char pad_000C[8]; //0x000C
	void* data; //0x0014
	uint32_t xuid_low{};
	uint32_t xuid_high{};
	int32_t format; //0x0020
	int32_t sequence_bytes; //0x0024
	uint32_t section_number; //0x0028
	uint32_t uncompressed_sample_offset; //0x002C
	int32_t cached_size; //0x0030

	uint32_t flags; //0x0034

	uint8_t no_stack_overflow[0xFF];

	__forceinline void set_data(VoiceDataCustom* cdata)
	{
		xuid_low = cdata->xuid_low;
		xuid_high = cdata->xuid_high;
		sequence_bytes = cdata->sequence_bytes;
		section_number = cdata->section_number;
		uncompressed_sample_offset = cdata->uncompressed_sample_offset;
	}
};

#define KAABA_KEY 0.0007479516421f
#define CHEESE_KEY 69.f

void __fastcall Hooks::hkVoiceData(void* msg) {

	static auto m_oVoiceData = g_hooks.m_client_state.GetOldMethod< FnVoiceData >(CClientState::VOICEDATA);

	if (!msg) 
		return m_oVoiceData(this, msg);

	auto local = g_cl.m_local;


	CSVCMsg_VoiceData_Legacy* msg_ = (CSVCMsg_VoiceData_Legacy*)msg;
	int idx = msg_->client + 1;
	AimPlayer* data = &g_aimbot.m_players[idx - 1];
	VoiceDataCustom msg_data = msg_->get_data();

	if (!local || !local->alive() || !g_csgo.m_engine->IsConnected() || !g_csgo.m_engine->IsInGame() || !g_cl.m_processing) 
		return m_oVoiceData(this, msg);

	if (local->index() == idx)
		return m_oVoiceData(this, msg);

	if (msg_->format != 0)
		return m_oVoiceData(this, msg);

	// check if its empty
	if (msg_data.section_number == 0 && msg_data.sequence_bytes == 0 && msg_data.uncompressed_sample_offset == 0)
		return m_oVoiceData(this, msg);

	Voice_Vader* packet = (Voice_Vader*)msg_data.get_raw_data();
	player_info_t info{ };

	if (g_menu.main.misc.dumper.get()) {
		g_cl.print("receiving | xuid high: %i , xuid low: %i , seq bytes: %i , section nmbr: %i , uncomp sample offset: %i \n", msg_->xuid_high, msg_->xuid_low, msg_->sequence_bytes, msg_->section_number, msg_->uncompressed_sample_offset);
	}

	if( g_csgo.m_engine->GetPlayerInfo( idx, &info ) ) {
		if (!strcmp(packet->cheat_name, XOR("fat_Fingers"))) {
			g_cl.print( "%s has enabled whitelist \n", info.m_name );
		}
	}

	// eax
	if (!strcmp(packet->cheat_name, XOR("fatniggers8793")))
		data->m_is_eax = true;

	// cheese crack
	else if (msg_->sequence_bytes == 321420420) {
		data->m_is_cheese_crack = true;
		data->m_networked_angle = (*(float*)&msg_->section_number) / CHEESE_KEY;
	}
	// kaaba crack
	else if (msg_->xuid_low == 43955
		|| msg_->xuid_low == 43969
		|| msg_->xuid_low == 43803
		|| msg_->sequence_bytes == -858993664) {
		data->m_is_kaaba = true;
		data->m_networked_angle = (*(float*)&msg_->section_number) * KAABA_KEY;
	}
	// dopium
	else if (msg_->xuid_low == 1338)
		data->m_is_dopium = true;
	// robertpaste
	else if (msg_->xuid_low == 4539731)
		data->m_is_robertpaste = true;
	// godhook
	else if (!strcmp(packet->cheat_name, XOR("g0db00k.indeed")))
		data->m_is_godhook = true;
	// fade
	else if (msg_->xuid_low == 1701077350 || msg_->sequence_bytes == 24930)
		data->m_is_fade = true;

	m_oVoiceData(this, msg);
}
