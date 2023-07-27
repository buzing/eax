#include "includes.h"

inline bool isWeapon(std::string& modelName) {
	if ((modelName.find("v_") != std::string::npos || modelName.find("uid") != std::string::npos || modelName.find("stattrack") != std::string::npos) && modelName.find("arms") == std::string::npos) {
		return true;
	}

	return false;
}

void Hooks::DrawModelExecute(uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone) {

	if (!info.m_model)
		return g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, ctx, state, info, bone);

	if (isWeapon(std::string(info.m_model->m_name)) && g_menu.main.players.weapon_chams.get()) {
		auto alpha2 = (float)g_menu.main.players.weapon_chams_blend2.get() / 255.f;
		auto alpha1 = (float)g_menu.main.players.weapon_chams_blend.get() / 255.f;

		const auto freq = g_menu.main.players.rainbow_speed.get();
		const auto real_time = g_csgo.m_globals->m_realtime * freq;

		const auto r = floor(sin(real_time + 0.f) * 27 + 198);
		const auto g = floor(sin(real_time + 2.f) * 27 + 198);
		const auto b = floor(sin(real_time + 4.f) * 27 + 198);

		g_chams.SetAlpha(alpha2);
		if (g_menu.main.players.rainbow_visuals.get(6)) {
			g_chams.SetupMaterial(g_chams.m_materials[1], Color(r, g, b), false);
		}
		else {
			g_chams.SetupMaterial(g_chams.m_materials[1], g_menu.main.players.weapon_chams_col2.get(), false);
		}
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, ctx, state, info, bone);

		g_chams.SetAlpha(alpha1);
		if (g_menu.main.players.rainbow_visuals.get(5)) {
			g_chams.SetupMaterial(g_chams.m_materials[g_menu.main.players.weapon_chams_mat.get()], Color(r, g, b), false);
		}
		else {
			g_chams.SetupMaterial(g_chams.m_materials[g_menu.main.players.weapon_chams_mat.get()], g_menu.main.players.weapon_chams_col.get(), false);
		}
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, ctx, state, info, bone);
		g_csgo.m_studio_render->ForcedMaterialOverride(nullptr);
		g_csgo.m_render_view->SetColorModulation(colors::white);
		return;
	}

	if (g_csgo.m_engine->IsInGame()) {
		if (strstr(info.m_model->m_name, XOR("player/contactshadow")) != nullptr) {
			return;
		}
	}

	// do chams.
	if (g_chams.DrawModel(ctx, state, info, bone)) {
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, ctx, state, info, bone);
	}

	// disable material force for next call.
	//g_csgo.m_studio_render->ForcedMaterialOverride( nullptr );
}