//
//  MapPin.h
//  SVO
//
//  Created by Valentine on 30.06.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NavigineSDK.h"

@interface MapPin : UIButton

@property (nonatomic, assign) CGPoint originalPoint;
@property (nonatomic, strong) NSString *title;
@property (nonatomic, strong) Venue *venues;

@end
