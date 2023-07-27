#pragma once

struct GlowObjectDefinition_t {

	bool IsUnused() const { return m_next_free_slot != GlowObjectDefinition_t::ENTRY_IN_USE; }

	Entity* m_entity;
	vec3_t m_color;
	float  m_alpha;
	PAD(0x8);
	float  m_bloom_amount;
	PAD(0x4);
	bool   m_render_occluded;
	bool   m_render_unoccluded;
	bool   m_render_full_bloom;
	PAD(0x1);
	int    m_full_bloom_stencil;
	PAD(0x4);
	int    m_slot;
	int    m_next_free_slot;

	static const int END_OF_FREE_LIST = -1;
	static const int ENTRY_IN_USE = -2;
};

class CGlowObjectManager {
public:
	CUtlVector< GlowObjectDefinition_t >	m_object_definitions;
	int										m_first_free_slot;
};