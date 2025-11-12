# CMakeList.txt: CMake-Projekt für "NorthernKingdom". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)

# Aktivieren Sie Hot Reload für MSVC-Compiler, sofern unterstützt.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (NorthernKingdom 
"src/NorthernKingdom.cpp"
 "src/Shader.h" "src/Shader.cpp" "src/Geometry.h" "src/Geometry.cpp" "src/Material.h" "src/Material.cpp" "src/Texture.cpp" "src/Texture.h" "src/Texture.cpp" "src/Utils/stb_image.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NorthernKingdom PROPERTY CXX_STANDARD 20)
endif()

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.

#Für vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(gli CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(NorthernKingdom PRIVATE glfw)
target_link_libraries(NorthernKingdom PRIVATE GLEW::glew)
target_link_libraries(NorthernKingdom PRIVATE glm::glm)
target_link_libraries(NorthernKingdom PRIVATE gli)
target_link_libraries(NorthernKingdom PRIVATE OpenGL::GL)

target_include_directories(NorthernKingdom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils)