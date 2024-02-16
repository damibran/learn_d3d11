#pragma once

namespace dmbrn
{
	class Game;

	class IGameComponent
	{
	public:
		IGameComponent(Game& game) :game(game) {}

		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Draw() = 0;

		virtual void DestroyResources() = 0;

	protected:
		Game& game;
	};
}