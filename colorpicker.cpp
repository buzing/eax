#include "includes.h"
#include "helpers.h"

#define dt std::cout
#define nl std::endl

// static variables.
int Colorpicker::texture = 0;;
std::unique_ptr< Color[] > Colorpicker::gradient = nullptr;;

void Colorpicker::init() {
	const int w{ color_size };
	const int h{ color_size };

	// should never happen.
	if (gradient)
		return;

	// allocate.
	gradient = std::make_unique< Color[] >(w * h);

	// init.
	float hue{}, sat{ 0.99f }, lum{ 1.f };

	// iterate width.
	for (int i{}; i < w; ++i) {

		// iterate height.
		for (int j{}; j < h; ++j) {

			// write back to array.
			*(Color*)(gradient.get() + i + j * w) = Color::hsl_to_rgb(hue, sat, lum);

			hue += (1.f / w);
		}

		lum -= (1.f / h);
		hue = 0.f;
	}

	// allocate new texture in engine.
	texture = g_csgo.m_surface->CreateNewTextureID(true);

	// assign allocated memory containing the picker to said texture.
	g_csgo.m_surface->DrawSetTextureRGBA(texture, gradient.get(), w, h);
}

void rgb_to_hsv(float r, float g, float b, float& out_h, float& out_s, float& out_v)
{
	float K = 0.f;
	if (g < b)
	{
		const auto temp = b;
		b = g;
		g = temp;
		K = -1.f;
	}
	if (r < g)
	{
		const auto temp = r;
		r = g;
		g = temp;
		K = -2.f / 6.f - K;
	}

	const float chroma = r - (g < b ? g : b);
	out_h = fabsf(K + (g - b) / (6.f * chroma + 1e-20f));
	out_s = chroma / (r + 1e-20f);
	out_v = r;
}
void hsv_to_rgb(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{
	if (s == 0.0f)
	{
		// gray
		out_r = out_g = out_b = v;
		return;
	}

	h = fmodf(h, 1.0f) / (60.0f / 360.0f);
	int   i = (int)h;
	float f = h - (float)i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	switch (i)
	{
	case 0: out_r = v; out_g = t; out_b = p; break;
	case 1: out_r = q; out_g = v; out_b = p; break;
	case 2: out_r = p; out_g = v; out_b = t; break;
	case 3: out_r = p; out_g = q; out_b = v; break;
	case 4: out_r = t; out_g = p; out_b = v; break;
	case 5: default: out_r = v; out_g = p; out_b = q; break;
	}
}

void Colorpicker::draw() {
	Rect  area{ m_parent->GetElementsRect() };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	// label.
	render::menu_shade.string(p.x + LABEL_OFFSET, p.y - 2, { 205, 205, 205, m_parent->m_alpha }, m_label);

	// outline.
	render::rect(p.x + m_w - COLORPICKER_WIDTH, p.y, COLORPICKER_WIDTH, COlORPICKER_HEIGHT, { 0, 0, 0, m_parent->m_alpha });

	// make a copy of the color.
	Color preview = m_color;

	// modify the alpha channel of this color.
	preview.a() = m_parent->m_alpha;

	// draw preview window.
	render::rect_filled(p.x + m_w - COLORPICKER_WIDTH + 1, p.y + 1, COLORPICKER_WIDTH - 2, COlORPICKER_HEIGHT - 2, preview);

	// fade / shine.
	render::rect_filled_fade(p.x + m_w - COLORPICKER_WIDTH + 1, p.y + 1, COLORPICKER_WIDTH - 2, COlORPICKER_HEIGHT - 2, { 50, 50, 35, m_parent->m_alpha }, 0, 150);

	/* lets do the open boss */ {
		/* animate when openning */
		c_help::animate_lower(m_open, 0.2, 0.1, c_help::m_cpicker_anim);

		if (m_open) {
			m_color = Color().hsb_to_rgb(m_hue, m_saturation, m_value).alpha(m_color.a());
			const int t_height = m_w + 10;

			/* main pos */
			int x = p.x;
			int y = p.y;
			int w = m_w;
			int h = m_h;
			int cp_y = y + 12 + m_h;
			int width = w;

			/* drawing */
			render::rect_filled(x - 1, cp_y, width + 2, t_height + 2, Color(60, 60, 60, m_parent->m_alpha));
			render::rect_filled(x, cp_y + 1, width, t_height, Color(40, 40, 40, m_parent->m_alpha));

			// now we render the rounded color preview
			const float colpalwidth = m_w - 8;
			const float colpalwidth2 = m_w - 22;
			const float colpalheight = m_h + 2;

			render::rect_filled(x + width - 14, cp_y + 5, colpalheight, colpalheight, m_color);
			render::rect(x + width - 14, cp_y + 5, colpalheight, colpalheight, Color(26, 26, 26, m_parent->m_alpha));;

			// colorpall
			for (int i = 0; i < colpalwidth2; ++i) {
				render::rect_filled(p.x + i + 4, cp_y + 5, 1, colpalheight, Color().hsb_to_rgb(i / colpalwidth2, 1.f, 1.f));
			}
			render::rect(x + 4, cp_y + 5, colpalwidth2, colpalheight, Color(60, 60, 60, m_parent->m_alpha));

			const auto shade_height = (t_height - colpalheight - 18);
			const int box_size = 3;
			for (int c = 0; c < colpalwidth; c += box_size) {
				for (int r = 0; r < shade_height; r += box_size) {
					const auto cpos = vec2_t(c, r);
					const auto saturation_percent = math::clamp2(cpos.x / colpalwidth, 0.f, 1.f);
					const auto brightness_percent = math::clamp2(1.f - (cpos.y / shade_height), 0.f, 1.f);
					auto size_x = box_size;
					auto size_y = box_size;

					{
						if (p.x + c + 4 + size_x > p.x + m_w - 3)
							size_x = (p.x + m_w - 4) - (p.x + c + 4);
						if (p.y + 14 + m_h + colpalheight + 4 + r + size_y > p.y + 14 + m_h + colpalheight + 4 + shade_height)
							size_y = (p.y + 14 + m_h + colpalheight + 4 + shade_height) - (p.y + 14 + m_h + colpalheight + 4 + r);
					}


					float hue, saturation, value;
					rgb_to_hsv(float(m_color.r()) / 255.f, float(m_color.g()) / 255.f, float(m_color.b()) / 255.f, hue, saturation, value);
					const auto pixel_color = Color().hsb_to_rgb(hue, saturation_percent, brightness_percent);
					render::rect_filled(p.x + c + 4, p.y + 16 + m_h + colpalheight + 4 + r, size_x, size_y, pixel_color);
				}
			}

			render::rect(p.x + 4, p.y + 28 + m_h, colpalwidth + 2, shade_height + 1, Color(60, 60, 60, m_parent->m_alpha));

			// alpha bar
			int alpha_x = x + 4;
			int alpha_y = y + 27 + m_h + shade_height + 3;

			render::rect_filled(x + 4, y + 27 + m_h + shade_height + 3, colpalwidth, colpalheight - 2, Color(m_color.r(), m_color.g(), m_color.b(), m_parent->m_alpha));
			render::rect(alpha_x, alpha_y, colpalwidth, colpalheight - 2, Color(60, 60, 60, m_parent->m_alpha));

			// drag hue system
			if (DoDragHue) {
				if (GetAsyncKeyState(VK_LBUTTON)) {
					if (g_input.Mousein(vec3_t(p.x + 4, cp_y + 5, 0), vec3_t(colpalwidth2, colpalheight, 0))) {
						m_hue_pos.x = g_input.m_mouse.x - (p.x + 4);
						m_hue_pos.y = g_input.m_mouse.y - (cp_y + 5);
						m_hue = (g_input.m_mouse.x - p.x + 4) / (colpalwidth2);
					}
				}
				else
				{
					DoDragHue = false;
				}
			}

			if (do_drag_alpha) {
				if (GetAsyncKeyState(VK_LBUTTON)) {
					if (g_input.Mousein(vec3_t(alpha_x, alpha_y, 0), vec3_t(colpalwidth, colpalheight - 2, 0))) {
						m_alpha_pos.x = math::clamp2(int(g_input.m_mouse.x - (alpha_x)), 0, 255);
						m_alpha_pos.y = g_input.m_mouse.x - (alpha_y); // mouse.x was
						m_color.a() = int(255 * float(m_alpha_pos.x / colpalwidth));
					}
				}
				else if (!GetAsyncKeyState(VK_LBUTTON)) {
					do_drag_alpha = false;
				}
			}

			// drag color
			if (DoDragCol) {
				if (GetAsyncKeyState(VK_LBUTTON)) {
					if (g_input.Mousein(vec3_t(p.x + 4, p.y + 16 + m_h + colpalheight + 4, 0), vec3_t(colpalwidth, shade_height, 0))) {

						m_saturation = math::clamp2((g_input.m_mouse.x - (p.x + 4)) / colpalwidth, 0.f, 1.f);
						m_value = math::clamp2(1.f - (g_input.m_mouse.y - (p.y + 16 + m_h + colpalheight + 4)) / shade_height, 0.f, 1.f);
						ColorPos.y = g_input.m_mouse.y - p.y;
						ColorPos.x = g_input.m_mouse.x - p.x;
					}
				}
				else {
					DoDragCol = false;
				}
			}

			// override
			m_color = Color().hsb_to_rgb(m_hue, m_saturation, m_value).alpha(m_color.a());

			if (ColorPos.x && ColorPos.y)
			{
				ColorPos.x = math::clamp2(ColorPos.x, 0.f, colpalwidth - 5);
				ColorPos.y = math::clamp2(ColorPos.y, 0.f, shade_height + 35);
				render::rect(ColorPos.x + p.x, ColorPos.y + p.y, 5, 5, Color(43, 44, 46, 255));
			}

			if (m_hue_pos.x && m_hue_pos.y)
			{
				m_hue_pos.x = math::clamp2(m_hue_pos.x, 2.f, colpalwidth2 - 2);
				render::rect(m_hue_pos.x - 2 + (p.x + 4), cp_y + 5, 4, colpalheight, Color(43, 44, 46, 255));
			}

			if (m_alpha_pos.x && m_alpha_pos.y) {
				m_alpha_pos.x = math::clamp2(m_alpha_pos.x, 2.f, colpalwidth - 2);
				render::rect(m_alpha_pos.x - 2 + (alpha_x), alpha_y, 4, colpalheight - 2, Color(43, 44, 46, 255));
			}

			// override alpha
			m_color.a() = math::clamp2(m_color.a(), 0, 255);
		}

		/* to do:
			fix copy paste stff
		*/

		if (copy_paste) {
			m_open = false;

			auto height = render::menu.size("copy").m_width + 2 + render::menu.size("paste").m_width;
			render::rect_filled(p.x + m_w - COLORPICKER_WIDTH + 1, p.y + 10, height + 18, 20, Color(60, 60, 60, m_parent->m_alpha));
			render::rect_filled(p.x + m_w - COLORPICKER_WIDTH + 2, p.y + 11, height + 16, 18, Color(40, 40, 40, m_parent->m_alpha));

			render::menu.string(p.x + 161, p.y + 13, Color(205, 205, 205, m_parent->m_alpha), XOR("copy /"));
			render::menu.string(p.x + 161 + render::menu.size("copy").m_width + 5, p.y + 13, Color(205, 205, 205, m_parent->m_alpha), XOR("  paste"));
		}

		/* reset animation */
		if (!m_open) {
			c_help::m_cpicker_anim = 0.f;

			/* run once */
			return;
		}
	}
}

void Colorpicker::think() {
	Rect  area{ m_parent->GetElementsRect() };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	int x = p.x;
	int y = p.y;
	int w = m_w;
	int h = m_h;
	int cp_y = y + 12 + m_h;
	int width = w;

	const float colpalwidth = m_w - 8;
	const float colpalwidth2 = m_w - 22;
	const float colpalheight = m_h + 2;
	const int t_height = m_w + 20;
	const auto shade_height = (t_height - colpalheight - 7);
	int alpha_x = x + 4;
	int alpha_y = y + 27 + m_h + shade_height + 3;

	Rect picker_bos{ p.x + m_w - COLORPICKER_WIDTH + 1, p.y + 1, 12, 12 };
	if (g_input.GetKeyPress(VK_RBUTTON) && g_input.IsCursorInRect(picker_bos))
		copy_paste = true;

	if (m_open) {
		Rect picker{ p.x + m_w - COLORPICKER_WIDTH + 1, p.y + 1, colpalwidth, colpalheight };

		if (GetKeyState(VK_LBUTTON) && g_input.IsCursorInRect(picker)) {
			m_open = true;
		}

		if (!DoDragCol || !DoDragHue || !do_drag_alpha) {
			if (g_input.Mousein(vec3_t(p.x + 2, cp_y + 5, 0), vec3_t(colpalwidth2, colpalheight, 0)))
			{
				DoDragCol = false;
				DoDragHue = true;
				do_drag_alpha = false;

				m_parent->m_active_element = this;
			}

			if (g_input.Mousein(vec3_t(p.x + 4, p.y + 16 + m_h + colpalheight + 4, 0), vec3_t(colpalwidth, shade_height, 0)))
			{
				DoDragCol = true;
				DoDragHue = false;
				do_drag_alpha = false;

				m_parent->m_active_element = this;
			}


			if (g_input.Mousein(vec3_t(alpha_x, alpha_y - 20, 0), vec3_t(colpalwidth, colpalheight - 2, 0))) {
				DoDragCol = false;
				DoDragHue = false;
				do_drag_alpha = true;

				m_parent->m_active_element = this;
			}

		}
		if (!g_input.Mousein(vec3_t(p.x - 20, p.y, 0), vec3_t(m_w + 40, m_h + m_h + shade_height + 40, 0))) {
			DoDragHue = false;
			DoDragCol = false;
			do_drag_alpha = false;
			m_open = false;

			if (m_callback)
				m_callback();
		}

		if (m_parent->m_active_element != this) {
			m_open = false;
		}
	}

	if (copy_paste) {
		for (int i = 0; i < 2; i++) {
			Rect textarea{ p.x + m_w - COLORPICKER_WIDTH + 1 * (i + 1) + (render::menu.size("copy").m_width * i), p.y + 2, render::menu.size(XOR("paste")).m_width + 5, render::menu.size("copy").m_width };
			if (g_input.IsCursorInRect(textarea) && g_input.GetKeyPress(VK_LBUTTON)) {
				// copy
				if (i == 0) {
					g_gui.copied_color = m_color;
				}
				// paste
				if (i == 1) {
					m_color = g_gui.copied_color;
				}
				copy_paste = false;
			}
		}

		m_parent->m_active_element = this;
	}

	if (m_ptr)
		*m_ptr = m_color;
}

void Colorpicker::click() {
	m_open = !m_open;
}