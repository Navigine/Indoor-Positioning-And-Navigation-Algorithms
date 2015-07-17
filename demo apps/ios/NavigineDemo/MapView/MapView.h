//
//  MapView.h
//  NavigineDemo
//
//  Created by Administrator on 7/14/14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NavigineManager.h"
#import <CoreBluetooth/CoreBluetooth.h>
#import <UIKit/UIGestureRecognizer.h>
//#import "MRProgress.h"
#import "PlaceView.h"
#import "ConsoleView.h"
#import "LoaderView.h"
#import "AFPickerView.h"

#import "MapPin.h"



typedef enum {
  DistanceInMinutes = 0,
  DistanceInMeters
} DistanceType;


typedef enum {
  RouteTypeNone = 0,
  RouteTypeFromIcon ,
  RouteTypeFromClick
} RouteType;



@interface MapView : UIViewController <CBCentralManagerDelegate,UIWebViewDelegate, AFPickerViewDataSource, AFPickerViewDelegate>
{
  //MRProgressOverlayView *progressView;
  int testPoint;
  CGRect originalContent;
  CGFloat angle;
  __block int error;
  
  NavigationResults res;
  
  NSMutableArray *pins;
  
  MapPin *currentPin;
  UIView *mapView;
  
  NSMutableArray *routeArray;
  CAShapeLayer   *routeLayer;
  UIBezierPath   *uipath;
  
  UIBezierPath *arrowPath;
  CAShapeLayer *arrowLayer;
  UIView       *arrow;
  UIImageView    *current;
  UIImageView    *lastImg;
  CGPoint        oldOffset;
  CGPoint        oldOrigin;
  __block NSInteger      floorsNum;
  double rotateAngle;
  CGPoint rotatePoint;
  BOOL isRoutingNow;
  CGFloat zoomScale;
  CGPoint routePoint;
  NSInteger routeToSublocId;
  NSInteger routeFromSublocId;
  UIImageView *pin;
  UISegmentedControl *_segmentedControl;
  __block NSMutableDictionary *sublocId;
  
  NSMutableArray *webViewArray;
  
  NSString *pushTitle;
  NSString *pushContent;
  NSString *pushImage;
  UIView  *curIm;
  
  AFPickerView *floorPicker;
}

@property (nonatomic, weak) NSObject <MapViewDelegate> *venueDelegate;
@property (nonatomic, weak) NSObject <ConsoleViewDelegate> *consoleDelegate;
@property (nonatomic, weak) NSObject <LoaderViewDelegate> *loaderDelegate;
@property (nonatomic, strong) NavigineManager *navigineManager;


@property (weak, nonatomic) IBOutlet UIView *finishView;
@property (weak, nonatomic) IBOutlet UILabel *finishTitle;
@property (weak, nonatomic) IBOutlet UILabel *finishSubTitle;

@property (weak, nonatomic) IBOutlet UIView *blurView;
@property (weak, nonatomic) IBOutlet UILabel *blurText;
@property (weak, nonatomic) IBOutlet UIButton *blutBtnCancel;
@property (weak, nonatomic) IBOutlet UIButton *blurBtnRoute;

@property (weak, nonatomic) IBOutlet UILabel *distanceLabel;
@property (nonatomic, strong) CBCentralManager *bluetoothManager;

@property (weak, nonatomic) IBOutlet UIScrollView *sv;
@property UIImageView *contentPNGView;
@property (nonatomic,strong) UIWebView *contentView;

//@property (nonatomic, copy) SVGKImage *currentImage;

@property (weak, nonatomic) IBOutlet UIButton *rotateButton;

@property (weak, nonatomic) IBOutlet UIButton *zoomInBtn;
@property (weak, nonatomic) IBOutlet UIButton *zoomOutBtn;

@property (weak, nonatomic) IBOutlet UIView *navigationView;
@property (weak, nonatomic) IBOutlet UILabel *navigationTitle;
@property (weak, nonatomic) IBOutlet UIView *indicatorView;

@property (nonatomic, retain) NSTimer *timerNavigation;

//@property (nonatomic, strong) id delegate;
@property (nonatomic,assign) BOOL firstLoad;
@property (nonatomic,assign) BOOL enableFollow;
@property (nonatomic,assign) int error4count;
@property (nonatomic,assign) NSInteger floor;
@property (nonatomic,assign) int imageSublocationId;

@property (nonatomic, assign) DistanceType distanceType;
@property (nonatomic, assign) RouteType routeType;

+ (MapView *)sharedManager;
- (IBAction)zoomInTouch:(id)sender;

- (IBAction)zoomOutTouch:(id)sender;

//- (IBAction)measuring:(id)sender;
- (void)starNavigation;

@end


