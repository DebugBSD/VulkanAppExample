#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::Init(GLFWwindow* pWindow)
{
	m_pWindow = pWindow;

	try {
		createInstance();
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
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::createInstance()
{
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

	// Check instance extensions supported
	if (!checkInstanceExtensionsSupport(&instanceExtensions))
	{
		throw std::runtime_error("VkInstance does not support required extensions");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// TODO: Set up validation layers that instance will use
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR: Failed to create a Vulkan instance");
	}
}

void VulkanRenderer::createLogicalDevice()
{
	// Get the queue family indices for the chosen physical device.
	QueueFamiliyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Queue the logical device needs to create and info to do so
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;

	float priority = 1.0;
	queueCreateInfo.pQueuePriorities = &priority;

	// Information to create logical device 
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;
	
	// Physical device features that logical device will be using.
	VkPhysicalDeviceFeatures deviceFeatures = {};

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR: Failed to create a Logical Device!");
	}

	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	
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

	QueueFamiliyIndices indices = getQueueFamilies(device);

	return indices.isValid();
}

QueueFamiliyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamiliyIndices indices;

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

		// Check if queue family indices are in a valid state, stop searching if so
		if (indices.isValid())
		{
			break;
		}

		i++;
	}


	return indices;
}