//
//  Beacons.h
//  SVO
//
//  Created by Valentine on 24.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface Beacons : NSManagedObject

@property (nonatomic, retain) NSNumber * major;
@property (nonatomic, retain) NSNumber * minor;
@property (nonatomic, retain) NSString * uuid;
@property (nonatomic, retain) id actions;

@end
