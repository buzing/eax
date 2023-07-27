#include "includes.h"

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

struct lame_string_t

{
	char data[16]{};
	uint32_t current_len = 0;
	uint32_t max_len = 15;
};


struct CIncomingSequence {
	int InSequence;
	int ReliableState;
};

std::vector<CIncomingSequence> IncomingSequences;

int lastsent = 0;
bool __fastcall nem_hooks::SendNetMsg(INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice) {
	if (pNetChan != g_csgo.m_engine->GetNetChannelInfo())
		return oSendNetMsg(pNetChan, msg, bForceReliable, bVoice);

	if (msg.GetType() == 14) // Return and don't send messsage if its FileCRCCheck
		return false;

	if (msg.GetGroup() == 11) {

		bool first_sent_whitelist{ true };

		constexpr int EXPIRE_DURATION = 2500; // miliseconds-ish?
		bool should_send = GetTickCount() - lastsent > EXPIRE_DURATION;
		if (should_send && g_menu.main.misc.senddata.get()) {
			Voice_Vader packet;
			strcpy(packet.cheat_name, first_sent_whitelist ? XOR("fat_Fingers") : XOR("fatniggers8793"));
			packet.make_sure = 1;
			VoiceDataCustom data;
			memcpy(data.get_raw_data(), &packet, sizeof(packet));
			first_sent_whitelist = false;

			CCLCMsg_VoiceData_Legacy msg;
			memset(&msg, 0, sizeof(msg));

			//static DWORD m_construct_voice_message = ( DWORD )Memory::Scan( ( "engine.dll" ), ( "56 57 8B F9 8D 4F 08 C7 07 ? ? ? ? E8 ? ? ? ? C7" ) );

			auto func = (uint32_t(__fastcall*)(void*, void*))g_csgo.m_construct_voice_message;
			func((void*)&msg, nullptr);

			// set our data
			msg.set_data(&data);

			// mad!
			lame_string_t lame_string;

			// set rest
			msg.data = &lame_string;
			msg.format = 0; // VoiceFormat_Steam
			msg.flags = 63; // all flags!

			// send it
			oSendNetMsg(pNetChan, (INetMessage&)msg, false, true);
			lastsent = GetTickCount();
		}
	}
	else if (msg.GetGroup() == 9) { // group 9 is VoiceData
		// Fixing fakelag with voice
		bVoice = true;
		g_cl.m_enable_voice = true;
	}
	else
		g_cl.m_enable_voice = false;

	return oSendNetMsg(pNetChan, msg, bForceReliable, bVoice);
}