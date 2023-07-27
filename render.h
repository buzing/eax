#pragma once

struct Point {
	int x;
	int y;
};

struct Rect {
	int x;
	int y;
	int w;
	int h;
};

namespace render {
	struct FontSize_t {
		int m_width;
		int m_height;
	};

	enum StringFlags_t {
		ALIGN_LEFT = 0,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	class Font {
	public:
		HFont      m_handle;
		FontSize_t m_size;

	public:
		__forceinline Font( ) : m_handle{}, m_size{} {};

		// ctor.
		__forceinline Font( const std::string& name, int s, int w, int flags ) {
			m_handle = g_csgo.m_surface->CreateFont( );
			g_csgo.m_surface->SetFontGlyphSet( m_handle, name.data( ), s, w, 0, 0, flags );
			m_size = size( XOR( "A" ) );
		}

		// ctor.
		__forceinline Font( HFont font ) {
			m_handle = font;
			m_size = size( XOR( "A" ) );
		}

		void string( int x, int y, Color color, const std::string& text, StringFlags_t flags = ALIGN_LEFT );
		void string( int x, int y, Color color, const std::stringstream& text, StringFlags_t flags = ALIGN_LEFT );
		void wstring( int x, int y, Color color, const std::wstring& text, StringFlags_t flags = ALIGN_LEFT );
		render::FontSize_t size( const std::string& text );
		FontSize_t wsize( const std::wstring& text );
	};

	extern Font menu;
	extern Font menu_shade;
	extern Font esp;
	extern Font esp2;
	extern Font esp_small;
	extern Font esp_small2;
	extern Font hud;
	extern Font cs;
	extern Font indicator;
	extern Font indicator2;
	extern Font console;

	void init( );
	bool WorldToScreen( const vec3_t& world, vec2_t& screen );
	void RoundedBoxStatic(int x, int y, int w, int h, int radius, Color col);
	void RoundedBoxStaticOutline(int x, int y, int w, int h, int radius, Color col);
	void line( vec2_t v0, vec2_t v1, Color color );
	void line( int x0, int y0, int x1, int y1, Color color );
	void rect( int x, int y, int w, int h, Color color );
	void rect_filled( int x, int y, int w, int h, Color color );
	void RectFilled(vec2_t pos, vec2_t size, Color color);
	void rect_filled_fade( int x, int y, int w, int h, Color color, int a1, int a2 );
	void rect_outlined( int x, int y, int w, int h, Color color, Color color2 );
	void circle( int x, int y, int radius, int segments, Color color );
	void gradient( int x, int y, int w, int h, Color color1, Color color2 );
	void round_rect(int x, int y, int w, int h, int r, Color color);
	void gradient2(int x, int y, int w, int h, Color color1, Color color2);
	void sphere(vec3_t origin, float radius, float angle, float scale, Color color);
	void triangle(vec2_t point_one, vec2_t point_two, vec2_t point_three, Color color);
	bool WorldToScreen2(const vec3_t& world, vec3_t& screen);
	void Draw3DFilledCircle(const vec3_t& origin, float radius, Color color);
	Vertex RotateVertex( const vec2_t& p, const Vertex& v, float angle );
	vec2_t GetScreenSize();
}

// nitro du hurensohn