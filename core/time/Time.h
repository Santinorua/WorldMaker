#pragma once

namespace WorldMaker
{
	class Time
	{
	private:
		static float s_currentFrame;
		static float s_lastFrame;
	public:
		static void Update();
		static float DeltaTime() { return s_currentFrame - s_lastFrame; }
	};
}
