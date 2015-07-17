//
//  TextView.h
//  NavigineDemo
//
//  Created by Администратор on 15/12/14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NavigineManager.h"
#import "CustomTabBarViewController.h"
#import <CoreLocation/CoreLocation.h>
#import "NavigineSDK.h"
#import "LoaderView.h"

@interface TextView: UIViewController <NavigineManagerDelegate>{
}

@property (weak, nonatomic) IBOutlet UILabel *errCode;
@property (weak, nonatomic) IBOutlet UILabel *txtResult;

@property (weak, nonatomic) IBOutlet UILabel *acc;
@property (weak, nonatomic) IBOutlet UILabel *om;
@property (weak, nonatomic) IBOutlet UILabel *attitude;
@property (weak, nonatomic) IBOutlet UILabel *magn;

@property (weak, nonatomic) IBOutlet UILabel *bleCount;
@property (weak, nonatomic) IBOutlet UILabel *bleList;

@property (weak, nonatomic) IBOutlet UILabel *buildVersion;

@property (weak, nonatomic) IBOutlet UILabel *txtGPS;
@end
