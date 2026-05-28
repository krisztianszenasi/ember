#ifndef EMBER_VERSION_H
#define EMBER_VERSION_H

/**
 * @brief Ember major version number.
 */
#define EMBER_VERSION_MAJOR 0

/**
 * @brief Ember minor version number.
 */
#define EMBER_VERSION_MINOR 1

/**
 * @brief Ember patch version number.
 */
#define EMBER_VERSION_PATCH 1

/**
 * @brief Ember build flavor.
 *
 * This should be provided by the build system, for example:
 *   EMBER_BUILD_FLAVOR="Debug"
 *   EMBER_BUILD_FLAVOR="Release"
 */
#ifndef EMBER_BUILD_FLAVOR
#define EMBER_BUILD_FLAVOR "unknown"
#endif

/**
 * @brief Ember target operating system.
 *
 * This can be provided by the build system, but it is auto-detected from the
 * compiler target when not specified.
 */
#ifndef EMBER_TARGET_OS
#if defined(_WIN32)
#define EMBER_TARGET_OS "windows"
#elif defined(__APPLE__) && defined(__MACH__)
#define EMBER_TARGET_OS "macos"
#elif defined(__linux__)
#define EMBER_TARGET_OS "linux"
#else
#define EMBER_TARGET_OS "unknown"
#endif
#endif

/**
 * @brief Ember target architecture.
 *
 * This can be provided by the build system, but it is auto-detected from the
 * compiler target when not specified.
 */
#ifndef EMBER_TARGET_ARCH
#if defined(__aarch64__) || defined(_M_ARM64)
#define EMBER_TARGET_ARCH "arm64"
#elif defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define EMBER_TARGET_ARCH "x64"
#elif defined(__i386__) || defined(_M_IX86)
#define EMBER_TARGET_ARCH "x86"
#elif defined(__arm__) || defined(_M_ARM)
#define EMBER_TARGET_ARCH "arm"
#else
#define EMBER_TARGET_ARCH "unknown"
#endif
#endif

#define EMBER_STRINGIFY_IMPL(value) #value
#define EMBER_STRINGIFY(value) EMBER_STRINGIFY_IMPL(value)

/**
 * @brief Ember semantic version string.
 */
#define EMBER_VERSION_STRING \
    EMBER_STRINGIFY(EMBER_VERSION_MAJOR) "." \
    EMBER_STRINGIFY(EMBER_VERSION_MINOR) "." \
    EMBER_STRINGIFY(EMBER_VERSION_PATCH)

/**
 * @brief Ember project name.
 */
#define EMBER_NAME "Ember"

/**
 * @brief Ember display version string.
 */
#define EMBER_VERSION_DISPLAY \
    EMBER_NAME " " EMBER_VERSION_STRING " (" EMBER_TARGET_OS "-" EMBER_TARGET_ARCH ", " EMBER_BUILD_FLAVOR ")"

/**
 * @brief Version of the Ember executable file format.
 *
 * This value is written into generated executable files and checked when an
 * executable is loaded.
 */
#define EMBER_EXECUTABLE_FORMAT_VERSION 1

#endif
