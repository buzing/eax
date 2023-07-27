#pragma once
#include <algorithm>

namespace c_help {
	inline Color notify;
	inline float m_anim{ 0.f };
	inline float m_gradient_line_anim{ 0.f };
	inline float m_drop_anim{ 0.f };
	inline float m_cpicker_anim{ 0.f };

	inline auto m_accent{ Color(247, 54, 54) };
	inline float alphauc(float _alpha = 255.f) { return _alpha * m_anim; }
	inline void animate(bool condition, float speed, float& anim) {
		if (condition) { anim += speed * 0.2; }
		else { anim -= speed * 0.2; }
		anim = std::clamp(anim, 0.f, 1.f);
	}
	inline void animate_lower(bool condition, float speed, float how_much_anim, float& anim) {
		if (condition) { anim += speed * how_much_anim; }
		else { anim -= speed * how_much_anim; }
		anim = std::clamp(anim, 0.f, 1.f);
	}
	inline std::string comp_name() {
		char buff[MAX_PATH];
		GetEnvironmentVariableA("USERNAME", buff, MAX_PATH);

		return std::string(buff);
	}
}

namespace console {
	inline void logger(std::string output) {
		std::cout << output << std::endl;
	}
	inline void logger_xor(std::string output) {
		std::cout << output << std::endl;
	}
	inline void logger_sameline(std::string output) {
		std::cout << output;
	}
}

namespace client_optimization {
	inline int get_fps() {
		return (int)std::round(1.f / g_csgo.m_globals->m_frametime);
	}
}