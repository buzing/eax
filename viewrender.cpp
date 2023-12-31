#include "includes.h"

void Hooks::RenderView(const CViewSetup& view, const CViewSetup& hud_view, int clear_flags, int what_to_draw) {
	// ...

	g_hooks.m_view_render.GetOldMethod< RenderView_t >(CViewRender::RENDERVIEW)(this, view, hud_view, clear_flags, what_to_draw);
}

void Hooks::Render2DEffectsPostHUD(const CViewSetup& setup) {
	if (!g_menu.main.visuals.removals.get(3))
		g_hooks.m_view_render.GetOldMethod< Render2DEffectsPostHUD_t >(CViewRender::RENDER2DEFFECTSPOSTHUD)(this, setup);
}

void Hooks::RenderSmokeOverlay(bool unk) {
	// do not render smoke overlay.
	if (!g_menu.main.visuals.removals.get(1))
		g_hooks.m_view_render.GetOldMethod< RenderSmokeOverlay_t >(CViewRender::RENDERSMOKEOVERLAY)(this, unk);
}
