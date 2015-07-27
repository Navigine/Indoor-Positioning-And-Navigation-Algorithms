//
//  Venue.h
//  NavigineSDK
//
//  Created by Администратор on 17/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 *  Structure with venues content
 */
@interface Venue : NSObject <NSCoding>

@property(nonatomic, assign) NSInteger id;
@property(nonatomic, assign) NSInteger locationId;
@property(nonatomic, assign) NSInteger sublocationId;  // sublocation id of venue
@property(nonatomic, strong) NSString *name;      // name of venue
@property(nonatomic, assign) NSInteger coordX;
@property(nonatomic, assign) NSInteger coordY;
@property(nonatomic, strong) NSNumber *realX;     // X coordinate of venue (m)
@property(nonatomic, strong) NSNumber *realY;     // Y coordinate of venue (m)
@property(nonatomic, strong) NSNumber *kx;
@property(nonatomic, strong) NSNumber *ky;
@property(nonatomic, strong) NSString *image;     // url path to image of venue content
@property(nonatomic, strong) NSString *phone;     // phone number of venue
@property(nonatomic, strong) NSString *descript;  // other info about venue
@property(nonatomic, assign) NSInteger category;

@end
