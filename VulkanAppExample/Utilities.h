#pragma once

#include <fstream>

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices (location) of queue families if the exist at all
struct QueueFamilyIndices {
	int graphicsFamily = -1;			// Location of Graphics Queue Family
	int presentationFamily = -1;		// Location of Presentation Queue Family

	// Check if queue families are valid.
	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}


};

struct SwapChainDetails {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;		// Surface properties e.g. image size/extent
	std::vector<VkSurfaceFormatKHR> formats;			// Surface image formats. (RGB, RGBA, ...)
	std::vector<VkPresentModeKHR> presentationModes;	// How images should be presented to screen.
};

struct SwapchainImage {
	VkImage imagen;
	VkImageView imageView;
};

static std::vector<char> readFile(const std::string& fileName)
{
	// Open the file to the end to get the size.
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		throw std::runtime_error("ERROR: Failed open a file");
	}

	size_t length = (size_t)file.tellg();

	std::vector<char> fileBuffer(length);

	file.seekg(0); // Reset the pointer to the beginning of the file
	file.read(fileBuffer.data(), length); // Read the file into the buffer.

	// Close the stream.
	file.close();

	return fileBuffer;

}