#include "includes.h"

void Button::draw( ) {
	Rect  area{ m_parent->GetElementsRect( ) };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };
	Rect btn = { p.x + BUTTON_X_OFFSET, p.y, m_w - BUTTON_X_OFFSET, BUTTON_BOX_HEIGHT };

	// inner blob
	    render::gradient2(p.x + BUTTON_X_OFFSET + 1, p.y + 1, m_w - BUTTON_X_OFFSET - 2, BUTTON_BOX_HEIGHT - 2, { 40, 40, 40, m_parent->m_alpha }, { 33, 33, 33, m_parent->m_alpha });
		if (g_input.IsCursorInRect(btn)) {
			render::gradient2(p.x + BUTTON_X_OFFSET + 1, p.y + 1, m_w - BUTTON_X_OFFSET - 2, BUTTON_BOX_HEIGHT - 2, { 49, 49, 49, m_parent->m_alpha }, { 41, 41, 41, m_parent->m_alpha });
		}

			if (g_input.GetKeyState(VK_LBUTTON) && g_input.IsCursorInRect(btn)) {
				render::gradient2(p.x + BUTTON_X_OFFSET + 1, p.y + 1, m_w - BUTTON_X_OFFSET - 2, BUTTON_BOX_HEIGHT - 2, { 35, 35, 35, m_parent->m_alpha }, { 27, 27, 27, m_parent->m_alpha });
			}

			render::rect(p.x + BUTTON_X_OFFSET + 1, p.y + 1, m_w - BUTTON_X_OFFSET - 2, BUTTON_BOX_HEIGHT - 2, { 53,53,53, m_parent->m_alpha });
			render::rect(p.x + BUTTON_X_OFFSET, p.y, m_w - BUTTON_X_OFFSET, BUTTON_BOX_HEIGHT, { 15,15,15, m_parent->m_alpha });

			// button name
			render::menu.string(p.x + ((BUTTON_X_OFFSET + m_w) / 2),
				p.y + 6, { 205, 205, 205, m_parent->m_alpha }, m_label, render::ALIGN_CENTER);
}

void Button::think( ) {}

void Button::click( ) {
	Rect  area{ m_parent->GetElementsRect( ) };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	// area where user has to click.
	Rect btn = { p.x + BUTTON_X_OFFSET, p.y, m_w - BUTTON_X_OFFSET, BUTTON_BOX_HEIGHT };

	if( g_input.IsCursorInRect( btn ) && m_callback )
		m_callback( );
}