#pragma once

namespace dmbrn
{
	// actually this could be class extendor of game to component bridge
	struct GameToBallBridge
	{
		int &lRacketScore, &rRacketScore;
	};
}