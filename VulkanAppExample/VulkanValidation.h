#pragma once

#include <vector>

// Switch ON/OFF validation layers.
const bool validationEnabled = true;

// List of validation layers to use.
// VK_LUNARG_standard_validation = All standard validation layers.
const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"				// Try to validate all the code. (Debug purposes)
};

// Callback function for validation debugging (will be called when validation information record)
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,						// Type of error
	VkDebugReportObjectTypeEXT objType,					// Type of object causing error
	uint64_t obj,										// ID of object
	size_t location,									// Location of the error. 
	int32_t code,										// See docs.
	const char *layerPrefix,	
	const char *message,								// Validation Information
	void *userData
)
{
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		printf("VALIDATION ERROR: %s\n",message);
		return VK_TRUE;
	}

	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		printf("VALIDATION WARNING: %s\n", message);
		return VK_FALSE;
	}

	return VK_FALSE;
}

// Wrapper

static VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

	if (func != nullptr)
	{
		func(instance, callback, pAllocator);
	}
}