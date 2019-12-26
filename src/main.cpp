#include <bits/stdint-uintn.h>
#include <iterator>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <vector>

const int WIDTH  = 800;
const int HEIGHT = 600;

const std::vector<const char*> g_validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamiliIndices
{
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() { return graphicsFamily.has_value(); }
};

QueueFamiliIndices findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamiliIndices indices;

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (auto it = queueFamilies.cbegin(); it != queueFamilies.cend(); ++it) {
        if (it->queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = std::distance(queueFamilies.cbegin(), it);

        if (indices.isComplete()) break;
    }

    return indices;
}

bool isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamiliIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

class HelloTriangleApplication
{
  public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

  private:
    void initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan()
    {
        createInstance();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void createInstance()
    {
        if (enableValidationLayers && !checkValidationLayerSupport())
            throw std::runtime_error{"validation layers requested but not available!"};

        VkApplicationInfo appInfo  = {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "No Engine";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_0;

        printAvailableExtensions();

        uint32_t glfwExtensionsCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

        VkInstanceCreateInfo createInfo    = {};
        createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo        = &appInfo;
        createInfo.enabledExtensionCount   = glfwExtensionsCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount   = g_validationLayers.size();
            createInfo.ppEnabledLayerNames = g_validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
            throw std::runtime_error{"failed to create instance!"};
    }

    void printAvailableExtensions()
    {
        uint32_t extensionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

        std::cout << "Available extensions:\n";
        for (const auto& e : extensions)
            std::cout << '\t' << e.extensionName << '\n';
    }

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        std::cout << "Available layers:\n";
        for (const auto& lp : availableLayers)
            std::cout << '\t' << lp.layerName << '\n';

        for (const char* layerName : g_validationLayers) {
            bool available =
                std::any_of(availableLayers.cbegin(), availableLayers.cend(),
                            [=](const auto& lp) { return strcmp(layerName, lp.layerName) == 0; });
            if (!available) return false;
        }

        return true;
    }

    void pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

        if (deviceCount == 0) throw std::runtime_error{"failed to find GPUs with Vulkan support!"};

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        auto it = std::find_if(devices.cbegin(), devices.cend(), isDeviceSuitable);

        if (it != devices.cend()) {
            m_physicalDevice = *it;
        } else {
            throw std::runtime_error{"failed to find a suitable GPU!"};
        }
    }

    void createLogicalDevice()
    {
        QueueFamiliIndices indices = findQueueFamilies(m_physicalDevice);

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex        = indices.graphicsFamily.value();
        queueCreateInfo.queueCount              = 1;
        float queuePriority                     = 1.0f;
        queueCreateInfo.pQueuePriorities        = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo    = {};
        createInfo.sType                 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos     = &queueCreateInfo;
        createInfo.queueCreateInfoCount  = 1;
        createInfo.pEnabledFeatures      = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;
        createInfo.enabledLayerCount     = 0;

        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
            throw std::runtime_error{"failed to create logical device!"};

        vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();
        }
    }

    void cleanup()
    {
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    GLFWwindow* m_window = nullptr;
    VkInstance m_instance;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE; // Destroyed with instance
    VkDevice m_device;
    VkQueue m_graphicsQueue;
};

int main()
{
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
