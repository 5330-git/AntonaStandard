/**
 * @file Macros.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 本文件定义一系列宏用于区分当前所处平台，从而实现跨平台支持
 * @version 1.0.0 
 * @date 2024-03-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CPS_MACROS_H
#define CPS_MACROS_H
/**
 * @brief 如果该宏被定义说明此时的平台是 Windows 
 */
#define AntonaStandard_PLATFORM_WINDOWS 0
/**
 * @brief 如果该宏被定义说明此时的平台是 Andorid 
 */
#define AntonaStandard_PLATFORM_ANDROID 0
/**
 * @brief 如果该宏被定义说明此时的平台是 Linux 
 */

#define AntonaStandard_PLATFORM_LINUX 0
/**
 * @brief 如果该宏被定义说明此时的平台是 IOS  
 */

#define AntonaStandard_PLATFORM_IOS 0
/**
 * @brief 如果该宏被定义此时的平台未知
 */
#define AntonaStandard_PLATFORM_UNKNOWN 0

#undef AntonaStandard_PLATFORM_WINDOWS
#undef AntonaStandard_PLATFORM_ANDROID
#undef AntonaStandard_PLATFORM_LINUX
#undef AntonaStandard_PLATFORM_IOS
#undef AntonaStandard_PLATFORM_UNKNOWN

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
    #define AntonaStandard_PLATFORM_WINDOWS 1 
#elif defined(ANDROID) || defined(_ANDROID_)
    #define AntonaStandard_PLATFORM_ANDROID 1 
#elif defined(__linux__)
    #define AntonaStandard_PLATFORM_LINUX  1 
#elif defined(__APPLE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_MAC)
    #define AntonaStandard_PLATFORM_IOS	 1 
#else
    /** @cond SKIP_COND */
    #define AntonaStandard_PLATFORM_UNKNOWN 1
    /** @endcond*/
#endif

#endif