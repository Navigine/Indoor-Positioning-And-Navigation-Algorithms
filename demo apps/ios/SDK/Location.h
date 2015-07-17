//
//  Location.h
//  NavigineSDK
//
//  Created by Администратор on 11/03/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "Sublocation.h"

@interface Location :NSObject
@property (nonatomic)        NSInteger      id;
@property (nonatomic,strong) NSString       *name;
@property (nonatomic,strong) NSString       *archiveFile;
@property (nonatomic)        NSInteger      version;
@property (nonatomic,strong) NSMutableArray *subLocations;

-(id) initWithLocation :(Location *)location;
- (Sublocation *)getSubLocationById: (NSInteger) id;
- (Sublocation *)getSubLocationByIndex: (NSInteger) index;


@end