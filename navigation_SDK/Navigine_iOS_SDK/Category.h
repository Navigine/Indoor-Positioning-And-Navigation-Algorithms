//
//  Category.h
//  NavigineSDK
//
//  Created by Администратор on 17/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 *  Structure with category content
 */

@interface Categories : NSObject <NSCoding>

@property (nonatomic, assign) NSInteger id;
@property (nonatomic, strong) NSString *name;
@property (nonatomic, strong) NSString *image;
@property (nonatomic, strong) NSMutableSet *venues;

@end
