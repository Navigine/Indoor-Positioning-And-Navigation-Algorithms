//
//  NaviganeManager.m
//  Navitech
//
//  Created by Valentine on 17.04.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import "NavigineManager.h"
#import "NavigineSDK.h"

#define kSTRICT_MODE false

UILocalNotification* localNotification;

@interface NavigineManager(Protected)
@property (nonatomic, strong) Location *_location;

- (NSString*) _getAccelerometer;
- (NSString*) _getGyroscope;
- (NSString*) _getMagnetometer;
- (NSString*) _getOrientation;

- (int) _getConnectionStatusWriteSocket;
- (int) _getConnectionStatusReadSocket;
- (void) _setServer: (const char*) serverIP  andPort: (int) writePort;
- (int) _setConnectionStatus: (int) i;
- (void) _launchNavigineSocketThreads: (const char*)serverIP : (int)serverWritePort;
- (int) _sendPacket;

- (int) _getCurrentVersion :(NSInteger *)currentVersion at :(NSString *)zipPath;
- (void) get_location:(Location *)Location;
- (void) _saveUserHash :(NSString *)userHash;
- (void) _setUserHash:(NSString *)userHash;
@end

@implementation NavigineManager

static NSString *_userHash = nil;

+ (instancetype)sharedManager {
  static NavigineManager *_navigineManager = nil;
  
  if (nil != _navigineManager) {
    return _navigineManager;
  }
  
  static dispatch_once_t pred;        // Lock
  dispatch_once(&pred, ^{
      _navigineManager = [[NavigineManager alloc] init];// This code is called at most once per app
  });
  return _navigineManager;
}

// We can still have a regular init method, that will get called the first time the Singleton is used.
- (id)init{
  if (self = [super init]) {
    super.delegate = self;
    locationId = 0;
    
    localNotification = [[UILocalNotification alloc] init];
    localNotification.userInfo = nil;
    localNotification.soundName = UILocalNotificationDefaultSoundName;
    
    [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert|UIUserNotificationTypeBadge|UIUserNotificationTypeSound categories:nil]];
  }
  return self;
}//init

- (BOOL)isNavigineFine {
  if([self getNavigationResults].ErrorCode != 0 && !DEBUG_MODE) {
    return NO;
  }
  return YES;
}

- (void)sendPushWithText:(NSString *)string andUserInfo:(NSDictionary *)userInfo {
  UIApplicationState state = [[UIApplication sharedApplication] applicationState];
  if (state == UIApplicationStateBackground || state == UIApplicationStateInactive){
    UILocalNotification* localNotification = [[UILocalNotification alloc] init];
    localNotification.fireDate = [NSDate dateWithTimeIntervalSinceNow:1];
    localNotification.alertBody = string;
    localNotification.timeZone = [NSTimeZone defaultTimeZone];
    localNotification.userInfo = userInfo;
    localNotification.soundName = UILocalNotificationDefaultSoundName;
    [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
    
    [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert|UIUserNotificationTypeBadge|UIUserNotificationTypeSound categories:nil]];
  }
}

- (int) checkLocationLoader {
  return [super checkLocationLoader: self.loaderID];
}

- (void) stopLocationLoader {
  [super stopLocationLoader:self.loaderID];
}

- (void) startNavigine{
  @try {
    [super startNavigine];
  }
  @catch (NSException *exception) {
    DLog(@"Exception caught: reason: %@",exception.description);
  }
}

- (void) startLocationLoader :(NSString *)userID :(NSString *)location{
  _loaderID = [super startLocationLoader :userID :location :YES];
}

- (int)getCurrentVersion :(NSInteger *)currentVersion{
  int error = [super getCurrentVersion: currentVersion];
  if (error) return error;
  self.currentVersion = *currentVersion;
  return 0;
}

- (int) getWidthAndHeightbyIndex:(NSInteger)index{
  return [super getWidthAndHeight:&(_DEFAULT_WIDTH) :&(_DEFAULT_HEIGHT) byIndex:index];
}

- (int) getWidthAndHeightbyId:(NSInteger)id{
  return [super getWidthAndHeight:&(_DEFAULT_WIDTH) :&(_DEFAULT_HEIGHT) byIndex:id];
}

// Equally, we don't want to generate multiple copies of the singleton.
- (id)copyWithZone:(NSZone *)zone {
  return self;
}

-(void) setUserHash: (NSString *)userHash{
  if(_userHash != userHash){
    _userHash = userHash;
    [self _setUserHash:userHash];
    self.userHash = userHash;
  }
}

- (BOOL) loadArchive:(NSString *)location{
  BOOL success =  [super loadArchive:location];
  self.location = [[Location alloc] initWithLocation: self._location];
  return success;
}

#pragma mark NavigineCoreDelegate methods


- (void)didRangePushWithTitle:(NSString *)title Content:(NSString *)content Image:(NSString *)image{
  UIStoryboard *st = [UIStoryboard storyboardWithName:[[NSBundle mainBundle].infoDictionary objectForKey:@"UIMainStoryboardFile"] bundle:[NSBundle mainBundle]];
  PopViewController *pop =  [st instantiateViewControllerWithIdentifier:@"popview"];
  pop.pushTitle = title;
  pop.pushContent = content;
  pop.pushImage = image;
  UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:pop];
  nav.navigationBarHidden = YES;
  UIViewController *vc = [[UIApplication sharedApplication] keyWindow].rootViewController;
  [vc presentViewController:nav animated:YES completion:nil];
  [self sendPushWithText:title andUserInfo:nil];
}

- (void) didRangeVenues:(NSArray *)venues :(NSArray *)categories{
  self.venues = [NSMutableArray arrayWithArray:venues];
}

- (void) didRangeBeacons:(NSArray *)beacons{
  if(self.dataDelegate && [self.dataDelegate respondsToSelector:@selector(didRangeBeacons:)])
    [self.dataDelegate didRangeBeacons:beacons];
}

- (void) navigationResultsInBackground :(NavigationResults) navigationResults{
  NavigationResults backGroundNavResults = navigationResults;
  localNotification.alertBody = [NSString stringWithFormat:@"x=%lf y=%lf error = %zd",backGroundNavResults.X,backGroundNavResults.Y, backGroundNavResults.ErrorCode];
  //[[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
}

#pragma mark - hidden methods of NavigineCore

- (NSString*) getAccelerometer{
  return [self _getAccelerometer];
}

- (NSString*) getGyroscope{
  return [self _getGyroscope];
}

- (NSString*) getMagnetometer{
  return [self _getMagnetometer];
}

- (NSString*) getOrientation{
  return [self _getOrientation];
}

- (int) getConnectionStatusWriteSocket{
  return [self _getConnectionStatusWriteSocket];
}

- (int) getConnectionStatusReadSocket{
  return [self _getConnectionStatusReadSocket];
}

- (void) setServer :(const char*)serverIP andPort :(int)writePort{
  return [self _setServer:serverIP andPort:writePort];
}

- (int) setConnectionStatus :(int)i{
  return [self _setConnectionStatus:i];
}

- (void) launchNavigineSocketThreads :(const char*) serverIP :(int)serverWritePort{
  return [self _launchNavigineSocketThreads:serverIP :serverWritePort];
}

- (int) sendPacket{
  return [self _sendPacket];
}

- (int) getCurrentVersion :(NSInteger *)currentVersion at :(NSString *)zipPath{
  return [self _getCurrentVersion:currentVersion at:zipPath];
}

@end
