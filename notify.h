#pragma once

// modelled after the original valve 'developer 1' debug console
// https://github.com/LestaD/SourceEngine2007/blob/master/se2007/engine/console.cpp

class NotifyText {
public:
	std::string m_text;
	Color		m_color;
	float		m_time;
	float		m_width;
	float		m_height_offset;

public:
	__forceinline NotifyText( const std::string& text, Color color, float time ) : m_text{ text }, m_color{ color }, m_time{ time } {}
};

class Notify {
private:
	std::deque< std::shared_ptr< NotifyText > > m_notify_text;

public:
	__forceinline Notify( ) : m_notify_text{} {}

	__forceinline void add( const std::string& text, Color color = colors::white, float time = 5.f, bool console = true ) {
		// modelled after 'CConPanel::AddToNotify'
		m_notify_text.push_back( std::make_shared< NotifyText >( text, color, time ) );

		if( console )
		    g_cl.print( text );
	}

	// modelled after 'CConPanel::DrawNotify' and 'CConPanel::ShouldDraw'
	void think( ) {
		int		x{ 8 }, y{ 5 }, size{ render::console.m_size.m_height + 1 };
		Color	color;
		float	left;

		// update lifetimes.
		for (size_t i{ }; i < m_notify_text.size(); ++i) {
			auto notify = m_notify_text[i];

			notify->m_time -= g_csgo.m_globals->m_frametime;

			if (notify->m_time <= 0.f) {
				m_notify_text.erase(m_notify_text.begin() + i);
				continue;
			}
		}

		// we have nothing to draw.
		if (m_notify_text.empty())
			return;

		// clear more than 6 entries.
		if (m_notify_text.size() > 6)
			m_notify_text.pop_front();

		// iterate entries.
		for (size_t i{ }; i < m_notify_text.size(); ++i) {
			auto notify = m_notify_text[i];

			if (notify->m_text.find(XOR("%%%%")) != std::string::npos)
				notify->m_text.erase(notify->m_text.find(XOR("%")), 3);

			left = notify->m_time;
			color = notify->m_color;

			if (left < .5f) {
				float f = left;
				f = math::Clamp(f, 0.f, .5f);

				f /= .5f;

				notify->m_width = (int)(f * 255.f);

				if (i == 0 && f < 0.2f)
					y -= size * (1.f - f / 0.2f);
			}

			else
				notify->m_width = 255;

			render::console.string(x, y, Color(notify->m_color.r(), notify->m_color.g(), notify->m_color.b(), notify->m_width), notify->m_text);
			y += size;
		}
	}
};

extern Notify g_notify;