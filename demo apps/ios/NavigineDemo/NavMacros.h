//
//  Header.h
//  NavigineSDK
//
//  Created by Администратор on 12/01/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#ifndef NavigineSDK_Header_h
#define NavigineSDK_Header_h

#define IS_OS_8_OR_LATER ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0)
#define kLogFileError 30
#define kGraphError   31


#ifdef NAV_DEBUG_MODE
#   define DLog(fmt, ...) NSLog((@"%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#   define DLog(...)
#endif


#define kBlackColor     [UIColor     blackColor]
#define kWhiteColor     [UIColor     whiteColor]
#define kBlueColor      [UIColor      blueColor]
#define kRedColor       [UIColor       redColor]
#define kGreenColor     [UIColor     greenColor]
#define kGrayColor      [UIColor      grayColor]
#define kDarkGrayColor  [UIColor  darkGrayColor]
#define kLightGrayColor [UIColor lightGrayColor]
#define kClearColor     [UIColor     clearColor]

#define kColorFromHex(color)[UIColor colorWithRed:((float)((color & 0xFF0000) >> 16))/255.0 green:((float)((color & 0xFF00) >> 8))/255.0 blue:((float)(color & 0xFF))/255.0 alpha:1.0]

#define RGBCOLOR(r,g,b) [UIColor colorWithRed:(r)/255.0 green:(g)/255.0 blue:(b)/255.0 alpha:1]
#define RGBACOLOR(r,g,b,a) [UIColor colorWithRed:(r)/255.0 green:(g)/255.0 blue:(b)/255.0 alpha:(a)]

#endif
