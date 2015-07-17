//
//  LoaderHelper.m
//  Navigine_Demo
//
//  Created by Администратор on 21/05/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "LoaderHelper.h"

@interface LoaderHelper(){

}

@property (nonatomic, strong) NavigineManager *navigineManager;

@end

@implementation LoaderHelper

+(LoaderHelper *) sharedInstance{
  static LoaderHelper * _sharedInstance = nil;
  
  static dispatch_once_t oncePredicate;
  dispatch_once(&oncePredicate, ^{
    _sharedInstance = [[LoaderHelper alloc] init];
  });
  return _sharedInstance;
}

-(id)init{
  self = [super init];
  if(self){
    self.loadedLocations = [NSMutableArray new];
    self.navigineManager = [NavigineManager sharedManager];
    [self getUserHashFromFile];
    [self saveUserHashToFile:self.userHash];
    [self.navigineManager setUserHash:self.userHash];
  }
  return self;
}

-(id)initWithBaseUserHash:(NSString *)userHash{
  self = [super init];
  if(self){
    self.loadedLocations = [NSMutableArray new];
    self.navigineManager = [NavigineManager sharedManager];
    self.userHash = userHash;
    [self saveUserHashToFile:userHash];
    [self.navigineManager setUserHash:self.userHash];
  }
  return self;
}

-(void)parseLocationList{
  NSError *error = nil;
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *path = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"locations"];
  NSString *fileContent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error :&error];
  if(error){
    NSLog(@"error in fileContent");
  }
  else{
    [self.loadedLocations removeAllObjects];
    TBXML *sourceXML = [[TBXML alloc] initWithXMLString:fileContent error:&error];
    if(error || !sourceXML){
      NSLog(@"error in sourceXML %@",[error localizedDescription]);
      return;
    }
    TBXMLElement *dataElement = sourceXML.rootXMLElement;
    TBXMLElement *mapElement = [TBXML childElementNamed:@"maps" parentElement:dataElement error:&error];
    if(error || !mapElement){
      NSLog(@"error in maps element %@",[error localizedDescription]);
      return;
    }
    TBXMLElement *itemElement = [TBXML childElementNamed:@"item" parentElement:mapElement error:&error];
    if(error || !itemElement){
      NSLog(@"error in item element %@",[error localizedDescription]);
      return;
    }
    while (itemElement) {
      LocationInfo *newLocation = [LocationInfo new];
      TBXMLElement *titleElement = [TBXML childElementNamed:@"title" parentElement:itemElement error:&error];
      newLocation.location.name = [TBXML textForElement:titleElement];
      if(error){
        NSLog(@"error in title element %@",[error localizedDescription]);
      }
      TBXMLElement *versionElement = [TBXML childElementNamed:@"version" parentElement:itemElement error:&error];
      if(error){
        NSLog(@"error in title element %@",[error localizedDescription]);
      }
      newLocation.serverVersion = [[TBXML textForElement:versionElement] integerValue];
      NSInteger version;
      NSString *pathToDir = [paths[0] stringByAppendingPathComponent:newLocation.location.name];
      NSString *pathToZip = [[pathToDir stringByAppendingPathComponent:newLocation.location.name] stringByAppendingString:@".zip"];
      if ([[NSFileManager defaultManager] fileExistsAtPath: pathToZip ]){
        int err = [self.navigineManager getCurrentVersion:&version at:pathToZip];
        if(err == 0)
          newLocation.location.version = version;
      }
      [self.loadedLocations addObject:newLocation];
      itemElement = itemElement->nextSibling;
    }
  }
}

-(void)startDownloadProcess:(NSString *)location :(BOOL)forced{
  LocationInfo *tmpLocation = [LocationInfo new];
  for(LocationInfo *tmp in self.loadedLocations){
    if([tmp.location.name isEqual:location]){
      tmpLocation = tmp;
      break;
    }
  }
  if(![self checkInternetConnection]){
    if(self.loaderDelegate && [self.loaderDelegate respondsToSelector:@selector(errorWhileDownloading::)]){
      [self.loaderDelegate errorWhileDownloading:-1 :tmpLocation];
    }
  }
  else{
    __block int loadProcess = 0;
    [self.navigineManager setUserHash:self.userHash];
    [self.navigineManager startLocationLoader:self.userHash :location];

    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
      loadProcess = 0;
      while (loadProcess < 100) {
        loadProcess = [self.navigineManager checkLocationLoader];
        if(loadProcess == 255){
          [self.navigineManager stopLocationLoader];
          [self performSelectorOnMainThread:@selector(errorWhileDownloading :) withObject:tmpLocation waitUntilDone:YES];
          break;
        }
        
        tmpLocation.loadingProcess = [NSString stringWithFormat:@"[%zd%%]",loadProcess];
        [self performSelectorOnMainThread:@selector(changeDownloadingValue :) withObject:[NSNumber numberWithInt: loadProcess] waitUntilDone:YES];
        
        if(loadProcess == 100){
          [self.navigineManager stopLocationLoader];
//          [self performSelectorOnMainThread:@selector(setLocation:) withObject:location waitUntilDone:NO];
          [self performSelectorOnMainThread:@selector(successfullDownloading :) withObject:tmpLocation waitUntilDone:YES];
          break;
        }
      }
    });
  }
}

- (void) errorWhileDownloading: (LocationInfo *)tmpLocation{
  if(self.loaderDelegate && [self.loaderDelegate respondsToSelector:@selector(errorWhileDownloading::)]){
    [self.loaderDelegate errorWhileDownloading:-2 :tmpLocation];
  }
}

- (void) successfullDownloading :(LocationInfo *)tmpLocation{
  if(self.loaderDelegate && [self.loaderDelegate respondsToSelector:@selector(successfullDownloading:)]){
    [self.loaderDelegate successfullDownloading :tmpLocation];
  }
}

- (void) changeDownloadingValue:(NSNumber*)loadProcess{
  if(self.loaderDelegate && [self.loaderDelegate respondsToSelector:@selector(changeDownloadingValue:)]){
    [self.loaderDelegate changeDownloadingValue:loadProcess.intValue];
  }
}

- (void) startNavigine{
  [self.navigineManager startNavigine];
}

- (void) startRangePushes{
  [self.navigineManager startRangePushes];
}

- (void) startRangeVenues{
  [self.navigineManager startRangeVenues];
}

- (BOOL) checkInternetConnection{
  Reachability* internetReachability = [Reachability reachabilityForInternetConnection];
  NetworkStatus status = internetReachability.currentReachabilityStatus;
  switch (status) {
    case NotReachable:
      return NO;
      break;
    default:
      return YES;
      break;
  }
}

-(void) getUserHashFromFile{
  NSUserDefaults *preferences = [NSUserDefaults standardUserDefaults];
  NSString *currentLevelKey = @"userHash";
  if ([preferences objectForKey:currentLevelKey]){
    //  Get current level
    self.userHash = [preferences objectForKey:currentLevelKey];
  }
}

- (void)saveUserHashToFile :(NSString *)userHash{
  NSUserDefaults *preferences = [NSUserDefaults standardUserDefaults];
  
  NSString *currentLevelKey = @"userHash";
  [preferences setValue:userHash forKey:currentLevelKey];
  
  //  Save to disk
  const BOOL didSave = [preferences synchronize];
  if (!didSave){
    DLog(@"ERROR with saving User Hash");
  }
}

- (NSInteger)setLocation :(NSString *)locationForSet{
  if([locationForSet isEqualToString:@""]){
    NSUserDefaults *preferences = [NSUserDefaults standardUserDefaults];
    
    NSString *currentLevelKey = @"locationForSet";
    
    if ([preferences objectForKey:currentLevelKey]){
      locationForSet = [NSString stringWithFormat:@"%@",[preferences objectForKey:currentLevelKey] ];
    }
    else return 2;
  }
  if([self.navigineManager.location.name isEqualToString:locationForSet])
    return 3;
  
  BOOL success = [self loadArchive:locationForSet];
  if(success == NO) return 1;
  for(LocationInfo *current in self.loadedLocations){
    current.isSet = NO;
    if([current.location.name isEqual:locationForSet]){
      current.isSet = YES;
    }
  }
  [self.navigineManager.venues removeAllObjects];
  [self saveLocationForSet:locationForSet];
  [self startNavigine];
  [self startRangePushes];
  [self startRangeVenues];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"setLocation" object:nil userInfo:@{@"locationName":locationForSet}];
  return 0;
}
-(void) deleteLocation :(NSString *)locationForDelete{
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSArray *directoryList = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:paths[0] error:nil];
  
  for(NSString *directory in directoryList){
    if([directory isEqualToString:locationForDelete]){
      [[NSFileManager defaultManager] removeItemAtPath:[[paths objectAtIndex:0] stringByAppendingPathComponent:directory] error:nil];

      break;
    }
  }
}

-(BOOL) loadArchive:(NSString *)location{
  self.navigineManager.userHash = self.userHash;
  BOOL success = [self.navigineManager loadArchive:location];
  if(success == YES){
    for (LocationInfo *tmp in self.loadedLocations) {
      if([tmp.location.name isEqualToString:location]){
        tmp.location.version = self.navigineManager.location.version;
      }
    }
  }
  return success;
}

-(void) saveLocationForSet :(NSString *)locationForSet{
  NSUserDefaults *preferences = [NSUserDefaults standardUserDefaults];
  
  NSString *currentLevelKey = @"locationForSet";
  [preferences setValue:locationForSet forKey:currentLevelKey];
  
  //  Save to disk
  BOOL didSave = [preferences synchronize];
  
  if (!didSave){
    DLog(@"ERROR with saving User Hash");
  }
}

-(void) deleteAllLocations{
  [self.loadedLocations removeAllObjects];
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSArray *directoryList = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:paths[0] error:nil];
  for(NSString *directory in directoryList){
    if ([[NSFileManager defaultManager] fileExistsAtPath: [paths objectAtIndex:0] ]){
      [[NSFileManager defaultManager] removeItemAtPath:[[paths objectAtIndex:0] stringByAppendingPathComponent:directory] error:nil];
    }
  }
}


@end
