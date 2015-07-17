//
//  Actions.h
//  SVO
//
//  Created by Valentine on 24.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface Actions : NSManagedObject

@property (nonatomic, retain) NSNumber * action_id;
@property (nonatomic, retain) NSString * content;
@property (nonatomic, retain) NSString * image;
@property (nonatomic, retain) NSNumber * next_available;
@property (nonatomic, retain) NSString * title;

@end
