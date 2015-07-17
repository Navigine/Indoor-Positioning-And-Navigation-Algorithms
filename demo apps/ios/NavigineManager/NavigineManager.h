//
//  NaviganeManager.h
//  Navitech
//
//  Created by Valentine on 17.04.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NavigineSDK.h"
#import "Location.h"
#import "PopViewController.h"


#define DEBUG_MODE NO
#define kColorFromHex(color)[UIColor colorWithRed:((float)((color & 0xFF0000) >> 16))/255.0 green:((float)((color & 0xFF00) >> 8))/255.0 blue:((float)(color & 0xFF))/255.0 alpha:1.0]

@protocol NavigineManagerDelegate;

@interface NavigineManager : NavigineCore <NavigineCoreDelegate>{
    NSInteger locationId;
}

@property (nonatomic, weak) id<NavigineManagerDelegate> dataDelegate;
@property (nonatomic, strong) Location *location;

@property (nonatomic, strong) NSString *userHash;
@property (nonatomic, assign) NSInteger currentVersion;
@property (nonatomic, strong) NSMutableArray *venues;

@property int loaderID;
@property double DEFAULT_WIDTH;
@property double DEFAULT_HEIGHT;
@property PopViewController *pvc;

- (NSString*) getAccelerometer;
- (NSString*) getGyroscope;
- (NSString*) getMagnetometer;
- (NSString*) getOrientation;

+ (id) sharedManager;
- (void) startNavigine;

- (BOOL)loadArchive:(NSString *)location;
- (BOOL)isNavigineFine;
- (int) checkLocationLoader;
- (void) stopLocationLoader;
- (void) startLocationLoader :(NSString *)userID :(NSString *)location;

- (int) getWidthAndHeightbyIndex :(NSInteger)index;
- (int) getWidthAndHeightbyId :(NSInteger)id;
- (int) getCurrentVersion :(NSInteger *)currentVersion;
- (int) getCurrentVersion :(NSInteger *)currentVersion at :(NSString *)zipPath;

- (int) getConnectionStatusWriteSocket;
- (int) getConnectionStatusReadSocket;
- (void) setServer: (const char*) serverIP  andPort: (int) writePort;
- (int) setConnectionStatus: (int) i;
- (void) launchNavigineSocketThreads: (const char*)serverIP : (int)serverWritePort;
- (int) sendPacket;
@end

@protocol NavigineManagerDelegate <NSObject>
- (void) didRangeBeacons: (NSArray *)beacons;
@end