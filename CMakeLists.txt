cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(AudioSpectrumViewer VERSION 1.0.0)

# Set optimization flags for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Raspberry Pi 3B+ specific optimizations with NEON support
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a53 -march=armv8-a+simd -mtune=cortex-a53 -funsafe-math-optimizations")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a53 -march=armv8-a+simd -mtune=cortex-a53 -funsafe-math-optimizations")
        message(STATUS "Building for Raspberry Pi 3B+ (64-bit ARM Linux) with NEON optimizations")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -funsafe-math-optimizations")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -funsafe-math-optimizations")
        message(STATUS "Building for Raspberry Pi 3B+ (32-bit ARM Linux) with NEON optimizations")
    endif()
    
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
        find_package(ALSA REQUIRED)
    endif()
endif()

set(SOURCE 
    source/${PROJECT_NAME}.cpp
    source/FFTProcessor.cpp
    source/SpectrumAnalyzerView.cpp
    source/Main.cpp
)

set(HEADERS 
    source/${PROJECT_NAME}.h
    source/FFTProcessor.h
    source/SpectrumAnalyzerView.h
    source/AmpButton.h
    source/AmpColours.h
)

add_subdirectory(externals/JUCE)
add_subdirectory(externals/CMP)

juce_add_gui_app(${PROJECT_NAME}
    PRODUCT_NAME "Audio Spectrum Viewer"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "This example app needs permission do use your microphone.")

target_sources(${PROJECT_NAME} PRIVATE
    ${SOURCE})
    
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${HEADERS}
    ${ALSA_INCLUDE_DIRS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>")

target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_gui_extra
    juce::juce_dsp
    juce::juce_audio_utils
    cmp_plot
    ${ALSA_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/img $<TARGET_FILE_DIR:${PROJECT_NAME}>/img)
