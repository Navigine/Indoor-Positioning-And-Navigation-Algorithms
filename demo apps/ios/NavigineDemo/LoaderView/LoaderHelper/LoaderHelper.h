//
//  LoaderHelper.h
//  Navigine_Demo
//
//  Created by Администратор on 21/05/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NavigineManager.h"
#import "LocationInfo.h"
#import "Reachability.h"
#import "TBXML.h"

@protocol LoaderHelperDelegate;

@interface LoaderHelper : NSObject
@property (nonatomic, weak) id <LoaderHelperDelegate> loaderDelegate;
@property (nonatomic, strong) NSString *userHash;
@property (nonatomic ,strong) NSMutableArray *loadedLocations;

+ (LoaderHelper *) sharedInstance;

- (id) init;
- (id) initWithBaseUserHash: (NSString *)userHash;
- (void) startDownloadProcess: (NSString *)location :(BOOL)forced;
- (void) parseLocationList;
- (void) saveUserHashToFile :(NSString *)userHash;
- (BOOL) loadArchive:(NSString *)location;
- (void) startNavigine;
- (void) startRangePushes;
- (void) startRangeVenues;
- (NSInteger) setLocation :(NSString *)locationForSet;
- (void) deleteLocation :(NSString *)locationForDelete;
- (void) deleteAllLocations;
@end

@protocol LoaderHelperDelegate <NSObject>
@optional
- (void) changeDownloadingValue :(NSInteger)value;
- (void) errorWhileDownloading :(NSInteger)error :(LocationInfo *)location;
- (void) successfullDownloading :(LocationInfo *)location;
@end