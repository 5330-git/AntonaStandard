#ifndef MULTIPLATFORMMACRO_H
#define MULTIPLATFORMMACRO_H

#define AntonaStandard_MultiPlatformSupport_MultiPlatformMacro_VERSION "1.0.0"
#define AntonaStandard_MultiPlatformSupport_MultiPlatformMacro_EDIT_TIME "2023/8/17"
#define AntonaStandard_MultiPlatformSupport_MultiPlatformMacro_AUTHOR "Anton"

/*
*   Decoded by UTF-8
*   2023/8/17 v-1.0.0 定义了多个平台的宏（Windows,Linux,IOS,Android）
*/
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
    #define AntonaStandard_PLATFORM_WINDOWS 1 //Windows平台
#elif defined(ANDROID) || defined(_ANDROID_)
    #define AntonaStandard_PLATFORM_ANDROID 1 //Android平台
#elif defined(__linux__)
    #define AntonaStandard_PLATFORM_LINUX	 1 //Linux平台
#elif defined(__APPLE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_MAC)
    #define AntonaStandard_PLATFORM_IOS	 1 //iOS、Mac平台
#else
    #define AntonaStandard_PLATFORM_UNKNOWN 1
#endif

#endif