#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept> // For checking errors issued by Vulkan.
#include <vector>

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

	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	struct
	{
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	}mainDevice;

	VkQueue graphicsQueue;

	// Vulkan functions
	// - Create Functions
	void createInstance();
	void createDebugCallback();
	void createLogicalDevice();

	// - Get Functions
	void getPhysicalDevice();

	// - Support Functions
	// -- Checker functions
	bool checkInstanceExtensionsSupport(std::vector<const char *> *checkExtensions);
	bool checkValidationLayerSupport();
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// -- Getter Functions
	QueueFamiliyIndices getQueueFamilies(VkPhysicalDevice device);
};

