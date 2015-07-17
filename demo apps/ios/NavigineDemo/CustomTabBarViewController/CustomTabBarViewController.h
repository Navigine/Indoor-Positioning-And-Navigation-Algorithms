//
//  CustomTabBarViewController.h
//  SVO
//
//  Created by Valentine on 03.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NavigineManager.h"

@interface CustomTabBarViewController : UITabBarController

- (void)setMapLoaderHelper:(id)delegate;
- (void)setLocation;

@end
