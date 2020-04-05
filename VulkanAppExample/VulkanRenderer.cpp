#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::Init(GLFWwindow* pWindow)
{
	m_pWindow = pWindow;

	try {
		createInstance();
		createDebugCallback();
		createSurface();
		getPhysicalDevice();
		createLogicalDevice();
	}
	catch (const std::runtime_error& e)
	{
		std::cout << "ERROR: " << e.what();
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::cleanup()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	if(validationEnabled)
		DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::createInstance()
{
	if (validationEnabled && !checkValidationLayerSupport())
	{
		throw std::runtime_error("Required validation layers not supported");
	}
		
	// Information about the application itself.
	// Most data here doesn't affect the program and is for developer convenience
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Application Example"; // Custom name of the application
	appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);	 // Custom application version
	appInfo.pEngineName = "Custom";							 // Custom engine name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);        // Custom engine version
	appInfo.apiVersion = VK_API_VERSION_1_0;				 // Version of Vulkan to USE.

	// Creation Information for Vulkan Instance
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	// Set up extensions to use
	uint32_t glfwExtensionsCount = 0; // GLFW may require multiple extensions.
	const char** glfwExtensions; // Extensions passed as array of cstring, so need pointer...

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	// Add GLDFW extensions to list of extensions
	for (size_t i = 0; i < glfwExtensionsCount; i++)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	// if validation enabled, add extension to report validation debug info
	if (validationEnabled)
	{
		instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	// Check instance extensions supported
	if (!checkInstanceExtensionsSupport(&instanceExtensions))
	{
		throw std::runtime_error("VkInstance does not support required extensions");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// TODO: Set up validation layers that instance will use
	if (validationEnabled)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR: Failed to create a Vulkan instance");
	}
}

void VulkanRenderer::createDebugCallback()
{
	if (validationEnabled)
	{
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = debugCallback;

		VkResult result = CreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Debug Callback!");
		}
	}
}

void VulkanRenderer::createLogicalDevice()
{
	// Get the queue family indices for the chosen physical device.
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Vector for queue creation information and set for family indices
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily };

	for (int queueFamilyIndex: queueFamilyIndices)
	{

		// Queue the logical device needs to create and info to do so
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		float priority = 1.0;
		queueCreateInfo.pQueuePriorities = &priority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Information to create logical device 
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	// Physical device features that logical device will be using.
	VkPhysicalDeviceFeatures deviceFeatures = {};

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR: Failed to create a Logical Device!");
	}

	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);
	
}

void VulkanRenderer::createSurface()
{
	VkResult result = glfwCreateWindowSurface(instance, m_pWindow, nullptr, &surface);

	if(result != VK_SUCCESS)
	{	
		throw std::runtime_error("ERROR: Failed to create a surface!");
	}
}

void VulkanRenderer::getPhysicalDevice()
{
	// Enumerate physical devices the VkInstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	// Check if there are devices available to support Vulkan
	if (deviceCount == 0)
	{
		throw std::runtime_error("ERROR:  Can't find GPU's that support Vulkan Instance!");
	}

	// Get list of physical devices
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (checkDeviceSuitable(device))
		{
			mainDevice.physicalDevice = device;
			break;
		}
	}
}

bool VulkanRenderer::checkInstanceExtensionsSupport(std::vector<const char*>* checkExtensions)
{
	// Need to get the number of extensions to create array of correct size to hold extensions.
	uint32_t extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

	// Creeate a list of VkExtensionProperties using count.
	std::vector<VkExtensionProperties> extensions( extensionsCount );
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

	// Check if given extensions are in list of available extensions
	for (const auto& checkExtension : *checkExtensions)
	{
		bool hasExtension = false;
		for (const auto& extension : extensions)
		{
			if (!strcmp(checkExtension, extension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if(!hasExtension)
			return false;
	}

	return true;
}

bool VulkanRenderer::checkValidationLayerSupport()
{
	// Get number of validation layers to create vector of apropiate size
	uint32_t validationLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

	// Check if no validation layers found and we want at least 1
	if (validationLayerCount == 0 && validationLayers.size() > 0)
	{
		return false;
	}

	// Creeate a list of VkExtensionProperties using count.
	std::vector<VkLayerProperties> availableLayers(validationLayerCount);
	vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

	// Check if given extensions are in list of available extensions
	for (const auto& validationLayer : validationLayers)
	{
		bool hasLayer = false;
		for (const auto& availableLayer : availableLayers)
		{
			if (!strcmp(validationLayer, availableLayer.layerName))
			{
				hasLayer = true;
				break;
			}
		}

		if (!hasLayer)
			return false;
	}
	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	// Information about the device itself (Id, name, type, vendor, ...)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	
	// Information about what the device can do (geo shader, tess shader, wide lines, ...)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/

	QueueFamilyIndices indices = getQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainValid = false;
	if (extensionsSupported)
	{
		SwapChainDetails swapChainDetails = getSwapChainDetails(device);
		swapChainValid = !swapChainDetails.presentationModes.empty() && !swapChainDetails.formats.empty();
	}

	return indices.isValid() && extensionsSupported && swapChainValid;
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	if (extensionCount == 0) return false;

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

	for (const auto& deviceExtension : deviceExtensions)
	{
		bool hasExtension = false;
		for (const auto& extension : extensions)
		{
			if (!strcmp(deviceExtension, extension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if(!hasExtension) return false;
	}

	return true;
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get all Queue Family Property info for the given device
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	// Check if given extensions are in list of available extensions
	int i = 0;
	for (const auto& queueFamily : queueFamilyList)
	{
		// First check if queue family has at least 1 queue in that Family ...
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i; // if queue family is valid, thet, get the index.
		}

		// Check if queue family support presentation
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,&presentationSupport );

		// Check if queue is presentation type can be both grapphics and presentation
		if (queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}

		// Check if queue family indices are in a valid state, stop searching if so
		if (indices.isValid())
		{
			break;
		}

		i++;
	}


	return indices;
}

SwapChainDetails VulkanRenderer::getSwapChainDetails(VkPhysicalDevice device)
{
	SwapChainDetails swapChainDetails;

	// -- CAPABILITIES --
	// Get the surface capabilities for the given ...
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.surfaceCapabilities);

	// -- FORMATS --
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount > 0)
	{
		swapChainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
	}

	// -- PRESENTATION MODES --
	uint32_t presentationCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);
	if (presentationCount > 0)
	{
		swapChainDetails.presentationModes.resize(presentationCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, swapChainDetails.presentationModes.data());
	}

	return swapChainDetails;
}
