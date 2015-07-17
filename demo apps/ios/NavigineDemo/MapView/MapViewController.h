//
//  MapView.h
//  NavigineDemo
//
//  Created by Administrator on 7/14/14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NavigineManager.h"
#import <UIKit/UIGestureRecognizer.h>
#import "PlaceView.h"
#import "CustomTabBarViewController.h"
#import "MapHelper.h"
#import "AFPickerView.h"

#import "MapPin.h"

float const minimumZoomScale = 0.2f;
float const maximumZoomScale = 5.0f;

typedef enum {
  DistanceInMinutes = 0,
  DistanceInMeters
} DistanceType;


typedef enum {
  RouteTypeNone = 0,
  RouteTypeFromIcon ,
  RouteTypeFromClick
} RouteType;



@interface MapViewController : UIViewController <UIScrollViewDelegate, AFPickerViewDataSource, AFPickerViewDelegate, MapHelperDelegate>{
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

  UIView  *curIm;
  
  AFPickerView *floorPicker;
  
  CGAffineTransform currentTransform;
}

@property (nonatomic, weak) NSObject <MapViewDelegate> *venueDelegate;
@property (nonatomic, weak) NSObject <LoaderHelperDelegate> *loaderHelperDelegate;
@property (nonatomic, strong) NavigineManager *navigineManager;


@property (weak, nonatomic) IBOutlet UIView *finishView;
@property (weak, nonatomic) IBOutlet UILabel *finishTitle;
@property (weak, nonatomic) IBOutlet UILabel *finishSubTitle;

@property (weak, nonatomic) IBOutlet UIView *blurView;
@property (weak, nonatomic) IBOutlet UILabel *blurText;
@property (weak, nonatomic) IBOutlet UIButton *blutBtnCancel;
@property (weak, nonatomic) IBOutlet UIButton *blurBtnRoute;

@property (weak, nonatomic) IBOutlet UIScrollView *sv;

@property (weak, nonatomic) IBOutlet UIButton *rotateButton;

@property (weak, nonatomic) IBOutlet UIButton *zoomInBtn;
@property (weak, nonatomic) IBOutlet UIButton *zoomOutBtn;

@property (weak, nonatomic) IBOutlet UIView *navigationView;
@property (weak, nonatomic) IBOutlet UILabel *navigationTitle;
@property (weak, nonatomic) IBOutlet UILabel *distanceLabel;
@property (weak, nonatomic) IBOutlet UILabel *distanceLabelTime;

@property (weak, nonatomic) IBOutlet UIView *indicatorView;

@property (nonatomic) BOOL firstLoad;
@property (nonatomic) BOOL enableFollow;
@property (nonatomic) int error4count;
@property (nonatomic) NSInteger floor;
@property (nonatomic) int imageSublocationId;

@property (nonatomic) DistanceType distanceType;
@property (nonatomic) RouteType routeType;

- (IBAction)zoomInTouch:(id)sender;
- (IBAction)zoomOutTouch:(id)sender;

@end
