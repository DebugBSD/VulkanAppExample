#pragma once

// Indices (location) of queue families if the exist at all
struct QueueFamiliyIndices {
	int graphicsFamily = -1;			// Location of Graphics Queue Family


	// Check if queue families are valid.
	bool isValid()
	{
		return graphicsFamily >= 0;
	}


};