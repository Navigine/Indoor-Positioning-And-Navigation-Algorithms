//
//  TextView.m
//  NavigineDemo
//
//  Created by Администратор on 15/12/14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import "TextView.h"

@interface TextView(){
  NSTimer *refreshTimer;
}

@property (nonatomic,strong) NavigineManager *navigineManager;
@end


@implementation TextView: UIViewController

- (void)viewDidLoad{
  [super viewDidLoad];
  
  self.view.backgroundColor = kWhiteColor;
  self.navigationController.navigationBar.barTintColor = kColorFromHex(0x00CDF9);
  self.navigationController.navigationBar.translucent = NO;
  
  CustomTabBarViewController *slide = (CustomTabBarViewController *)self.tabBarController;
  slide.tabBar.hidden = YES;
  [self addLeftButton];
  self.title = @"CONSOLE";
  
  refreshTimer = nil;
  
  self.navigineManager = [NavigineManager sharedManager];
  self.navigineManager.dataDelegate = self;
  
  [_bleList setNumberOfLines:0];
  NSString * version = [[NSBundle mainBundle] objectForInfoDictionaryKey: @"CFBundleShortVersionString"];
  NSString * build = [[NSBundle mainBundle] objectForInfoDictionaryKey: (NSString *)kCFBundleVersionKey];
  self.buildVersion.text = [NSString stringWithFormat:@"v.%@ b.%@",version,build];
  [self startTimer];
}




- (void)addLeftButton {
  UIImage *buttonImage = [UIImage imageNamed:@"btnMenu"];
  UIButton *leftButton = [UIButton buttonWithType:UIButtonTypeCustom];
  [leftButton setBackgroundImage:buttonImage forState:UIControlStateNormal];
  leftButton.frame = CGRectMake(0.0, 0.0, buttonImage.size.width,   buttonImage.size.height);
  UIBarButtonItem *aBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:leftButton];
  [leftButton addTarget:self action:@selector(menuPressed:)  forControlEvents:UIControlEventTouchUpInside];
  
  UIBarButtonItem *negativeSpacer = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
  [negativeSpacer setWidth:-17];
  
  [self.navigationItem setLeftBarButtonItems:[NSArray arrayWithObjects:negativeSpacer,aBarButtonItem,nil] animated:YES];
  
}

- (IBAction)menuPressed:(id)sender {
  if(self.slidingPanelController.sideDisplayed == MSSPSideDisplayedLeft) {
    [self.slidingPanelController closePanel];
  }
  else {
    [self.slidingPanelController openLeftPanel];
  }
}

- (void) startTimer{
  if (refreshTimer==nil) {
    refreshTimer = [NSTimer scheduledTimerWithTimeInterval:.1 target:self selector:@selector(onTimerTV) userInfo:nil repeats:YES ];
  }
}



-(void)onTimerTV{
  _acc.text  = [self.navigineManager getAccelerometer];
  _om.text   = [self.navigineManager getGyroscope];
  _magn.text = [self.navigineManager getMagnetometer];
  _attitude.text = [self.navigineManager getOrientation];
  _txtResult.text = [NSString stringWithFormat:@"x: %3.3lf y: %3.3lf",[self.navigineManager getNavigationResults].X, [self.navigineManager getNavigationResults].Y];
  _errCode.text = [NSString stringWithFormat:@"%zd Sublocation: %zd",[self.navigineManager getNavigationResults].ErrorCode, [self.navigineManager getNavigationResults].outSubLocation];
}

- (void) didRangeBeacons:(NSArray *)beacons{
  _bleCount.text = [NSString stringWithFormat:@"BLE devices(%zd):",beacons.count];
  _bleList.text = @"";
  int i = 0;
  NSArray *sortedArray = [beacons sortedArrayUsingComparator:^NSComparisonResult(id a, id b) {
    CLBeacon *first = (CLBeacon*)a;
    CLBeacon *second = (CLBeacon*)b;
    
    if (first.rssi < second.rssi) {
      return NSOrderedAscending;
    }
    else if (first.rssi > second.rssi) {
      return NSOrderedDescending;
    }
    // rssi is the same
    return NSOrderedSame;
  }];
  
  if ([sortedArray count] > 0) {
    for (id obj in sortedArray){
      if(i>16){
        [_bleList sizeToFit];
        return;
      }
      CLBeacon *b;
      b = obj;
      
      NSString *beaconProp=[NSString stringWithFormat:@"%05d  %05d  %19@ %zd\n",[[b major] intValue],[[b minor] intValue],[[b proximityUUID] UUIDString],[b rssi]];
      NSString *tmp = [[_bleList.text stringByAppendingFormat:@"%02d) ",i+1] stringByAppendingString:@" "];
      _bleList.text = [tmp stringByAppendingString:beaconProp];
      i++;
      
    }
    [_bleList sizeToFit];
  }
}

-(void)getLattitude:(double)lattitude Longitude:(double)longitude{
  self.txtGPS.text = [NSString stringWithFormat:@"lattitude: %3.3f longitude: %3.3f",lattitude,longitude];
}
@end
