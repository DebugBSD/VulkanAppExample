// VulkanAppExample.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#define GLFW_INCLUDE_VULKAN 

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

int main()
{
    
    glfwInit(); // Init glfw.
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Switch OFF OpenGL 
    GLFWwindow* pWindow = glfwCreateWindow(800, 600, "test", nullptr, nullptr);

    uint32_t extensionsCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
    std::cout << "Extension count: " << extensionsCount << std::endl;

    glm::mat4 testMatrix{ 1.0f };
    glm::vec4 testVector{ 1.0f };
    auto testResult =  testMatrix * testVector;


    while (!glfwWindowShouldClose(pWindow))
    {
        glfwPollEvents();

    }

    glfwDestroyWindow(pWindow);
    glfwTerminate();
    

    return EXIT_SUCCESS;
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
