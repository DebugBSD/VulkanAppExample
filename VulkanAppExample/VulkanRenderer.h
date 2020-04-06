#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept> // For checking errors issued by Vulkan.
#include <vector>
#include <set>
#include <algorithm>
#include <array>

#include "Utilities.h"

#include "VulkanValidation.h"

class VulkanRenderer
{
public:

	VulkanRenderer();

	int Init(GLFWwindow *pWindow);
	void cleanup();

	~VulkanRenderer();


private:
	GLFWwindow* m_pWindow;

	// Vulkan Components
	// - Main
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	struct
	{
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	}mainDevice;

	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;

	std::vector<SwapchainImage> swapChainImages;

	// - Pipeline
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;

	// - Utility
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// Vulkan functions
	// - Create Functions
	void createInstance();
	void createDebugCallback();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
	void createRenderPass();
	void createGraphicsPipeline();

	// - Get Functions
	void getPhysicalDevice();

	// - Support Functions
	// -- Checker functions
	bool checkInstanceExtensionsSupport(std::vector<const char *> *checkExtensions);
	bool checkValidationLayerSupport();
	bool checkDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	// -- Getter Functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	SwapChainDetails getSwapChainDetails(VkPhysicalDevice device);

	// -- Choose function
	VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
	VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
	VkExtent2D chooseBestExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);

	// -- Create functions
	VkImageView createImageView(VkImage image, VkFormat format, VkImageCreateFlags aspectFlags);
	VkShaderModule createShaderModule(const std::vector<char> &code);
};

