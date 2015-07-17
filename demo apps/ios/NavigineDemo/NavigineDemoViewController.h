//
//  NavigineDemoViewController.h
//  NavigineDemo
//
//  Created by Administrator on 7/14/14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NavigineManager.h"
#import <CoreBluetooth/CoreBluetooth.h>
#import "MRProgressOverlayView.h"
#import <UIKit/UIGestureRecognizer.h>



typedef enum {
    DistanceInMinutes = 0,
    DistanceInMeters
} DistanceType;


typedef enum {
    RouteTypeNone = 0,
    RouteTypeFromIcon ,
    RouteTypeFromClick
} RouteType;


@interface NavigineDemoViewController : UIViewController <CBCentralManagerDelegate>
{
    MRProgressOverlayView *progressView;
    
    int testPoint;
    CGFloat angle;
    NavigineManager *navigineManager;
    NavigationResults res;
  
    NSMutableArray *routeArray;
    CAShapeLayer   *routeLayer;
    UIBezierPath   *uipath;
    
    UIImageView    *current;
    UIImageView    *lastImg;
    UIImageView    *currentArrow;
    CGPoint        oldPoint;
    CGPoint        oldOrigin;
    CGSize         oldSize;
    
    
    BOOL isRoutingNow;
    CGPoint routePoint;
    UIImageView *pin;
    
}


@property (weak, nonatomic) IBOutlet UIView *finishView;
@property (weak, nonatomic) IBOutlet UILabel *finishTitle;
@property (weak, nonatomic) IBOutlet UILabel *finishSubTitle;

@property (weak, nonatomic) IBOutlet UIView *blurView;
@property (weak, nonatomic) IBOutlet UILabel *blurText;
@property (weak, nonatomic) IBOutlet UIButton *blutBtnCancel;
@property (weak, nonatomic) IBOutlet UIButton *blurBtnRoute;

@property (weak, nonatomic) IBOutlet UILabel *distanceLabel;
@property (nonatomic, strong) CBCentralManager *bluetoothManager;


@property (weak, nonatomic) IBOutlet UISegmentedControl *segmentedControl;
@property (weak, nonatomic) IBOutlet UIScrollView *sv;
@property (weak, nonatomic) IBOutlet UILabel *floorNumberLabel;


@property (weak, nonatomic) IBOutlet UIButton *measureButton;

@property (weak, nonatomic) IBOutlet UIButton *zoomInBtn;
@property (weak, nonatomic) IBOutlet UIButton *zoomOutBtn;
@property (weak, nonatomic) IBOutlet UIView *navigationView;
@property (weak, nonatomic) IBOutlet UILabel *navigationTitle;

@property (nonatomic, retain) NSTimer *timerNavigation;

@property (weak, nonatomic) IBOutlet UIImageView *contentView;

@property (nonatomic, strong) id delegate;
@property BOOL firstLoad;
@property BOOL enableMeasure;
@property int error4count;
@property int floor;

@property (nonatomic, assign) DistanceType distanceType;
@property (nonatomic, assign) RouteType routeType;

- (IBAction)zoomInTouch:(id)sender;

- (IBAction)zoomOutTouch:(id)sender;

- (IBAction)measuring:(id)sender;
- (void)starNavigation;

@end
