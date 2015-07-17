//
//  MapView
//  NavigineDemo
//
//  Created by Administrator on 7/14/14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "MapViewController.h"
#import "NavigineSDK.h"
#import "NavigineManager.h"

@interface MapViewController(){
}

@property (nonatomic, strong) LoaderHelper *loaderHelper;
@property (nonatomic, strong) MapHelper *mapHelper;
@end

@implementation MapViewController

- (void)viewDidLoad
{
  [super viewDidLoad];
  // Do any additional setup after loading the view, typically from a nib.
  
  self.view.backgroundColor = kWhiteColor;
  self.navigationController.navigationBar.barTintColor = kColorFromHex(0x00CDF9);
  self.navigationController.navigationBar.translucent = NO;
  
  CustomTabBarViewController *slide = (CustomTabBarViewController *)self.tabBarController;
  slide.tabBar.hidden = YES;
  
  self.title = @"MAP";
  
  [self addLeftButton];
  
  self.venueDelegate = nil;
  
  self.navigineManager = [NavigineManager sharedManager];
  
  self.mapHelper = [MapHelper sharedInstance];
  self.mapHelper.delegate = self;
  
  self.indicatorView.hidden = YES;
  self.indicatorView.layer.cornerRadius = 5.0f;
  
  self.rotateButton.hidden = NO;
  
  self.navigationView.layer.cornerRadius = 5.0f;
  self.navigationView.clipsToBounds = YES;
  
  error = 0;
  _enableFollow = NO;
  _floor = 0;
  testPoint = 0;
  angle = 0;
  floorsNum = 0;
  zoomScale = 1.0f;
  pins = [[NSMutableArray alloc] init];
  routeToSublocId = -1;
  
  floorPicker = [[AFPickerView alloc] initWithFrame:CGRectMake(270.0, 10.0, 39.0, 197)];
  floorPicker.dataSource = self;
  floorPicker.delegate = self;
  [self.view addSubview:floorPicker];
  
  [self.sv addSubview:self.mapHelper.contentView];
  current = [[UIImageView alloc] init];
  current.frame = CGRectMake(0, 0, 56, 56);
  current.backgroundColor = kRedColor;
  current.layer.cornerRadius = current.frame.size.height/2.0;
  current.alpha = 0.5;
  current.hidden = YES;
  [self.mapHelper.contentView addSubview:current];
  
  arrow = [[UIView alloc] init];
  arrow.hidden = YES;
  [self.mapHelper.contentView addSubview:arrow];
  
  lastImg = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"elmEmptyPin-copy-2"]];
  [lastImg sizeToFit];
  lastImg.hidden   = YES;
  [self.mapHelper.contentView addSubview:lastImg];
  
  UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPress:)];
  longPress.minimumPressDuration = 1;
  longPress.delaysTouchesBegan   = NO;
  [_sv addGestureRecognizer:longPress];
  
  UIRotationGestureRecognizer *rotate=[[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(rotation:)];
  [self.mapHelper.contentView addGestureRecognizer:rotate];
  
  UITapGestureRecognizer *tapPress = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapPress:)];
  tapPress.delaysTouchesBegan   = NO;
  [_sv addGestureRecognizer:tapPress];
  
  self.automaticallyAdjustsScrollViewInsets = NO;
}

-(void) rotation:(UIRotationGestureRecognizer *) sender{
  if (([sender state] == UIGestureRecognizerStateBegan || [sender state] == UIGestureRecognizerStateChanged) && !_enableFollow) {
    [sender view].transform = CGAffineTransformRotate([[sender view] transform], [(UIRotationGestureRecognizer *)sender rotation]);
//    [(UIRotationGestureRecognizer *)sender setRotation:0];
  }
}

- (void)viewDidAppear:(BOOL)animated {
  [super viewDidAppear:animated];
  
  if([self.venueDelegate respondsToSelector:@selector(routeToPlace)]) {
    
    if([[self.venueDelegate showType] isEqualToString:@"route"]) {
      
      Venue *ve = [self.venueDelegate routeToPlace];
      [self startRouteWithFinishPoint:CGPointMake(ve.realX.doubleValue, ve.realY.doubleValue) andRouteType:RouteTypeFromIcon];
      [self showNavigationViewWithTitle:ve.name];
    }
    else {
      Venue *ve = [self.venueDelegate routeToPlace];
      [self selectPinWithVenue:ve];
    }
    self.venueDelegate = nil;
  }
  [floorPicker reloadData];
}

- (void)selectPinWithVenue:(Venue *)v {
  
  for (MapPin *m in pins) {
    
    if(m.venues == v) {
      
      currentPin = m;
      
      [self zoomToPoint:currentPin.center withScale:1.0 animated:YES];
      [self showAnnotationForMapPin:currentPin];
    }
  }
}

- (void)showNavigationViewWithTitle:(NSString *)title {
  
  //navigineManager = [NavigineManager sharedManager];
  
  if(!self.navigineManager.isNavigineFine) {
    return;
  }
  
  [self.navigationController setNavigationBarHidden:YES animated:YES];
  
  _navigationTitle.text = title;
  _navigationView.hidden = NO;
  
  [UIView animateWithDuration:0.3 animations:^{
    
    _navigationView.alpha = 1.0;
  }completion:^(BOOL finished) {
    //[self reloadFramesForAllPins];
  }];
  
}

- (void)didReceiveMemoryWarning{
  [super didReceiveMemoryWarning];
  // Dispose of any resources that can be recreated.
}

- (void) addPointWithFrame :(CGRect)frame and :(CGFloat)R{
  
  return;
}

- (void)movePositionWithZoom:(BOOL)isZoom {
  
  if(error) return;

  res = [self.navigineManager getNavigationResults];
  

  [arrowLayer removeFromSuperlayer];
  arrowLayer = nil;
  
  [arrowPath removeAllPoints];
  arrowPath = nil;
  
  if((res.X == 0.0 && res.Y == 0.0) || res.ErrorCode != 0)  {
    
    if(res.ErrorCode == 4 && _error4count < 10) {
      _error4count++;
    }
    else {
      lastImg.hidden = YES;
      current.hidden = YES;
      arrow.hidden = YES;
      routeLayer.hidden = YES;
      return;
    }
  }
  if(res.ErrorCode == 0){
    _error4count = 0;
    arrow.hidden = NO;
    current.hidden = NO;
  }
  
  testPoint++;
  CGFloat mapWidthInMeter = [self.navigineManager DEFAULT_WIDTH];
  CGFloat mapOriginalWidth = (CGFloat)self.mapHelper.contentView.bounds.size.width;
  CGFloat poX = (CGFloat)res.X;// + testPoint;
  
  
  CGFloat mapWidthInHeight = [self.navigineManager DEFAULT_HEIGHT];
  CGFloat mapOriginalHeight = (CGFloat)self.mapHelper.contentView.bounds.size.height;
  CGFloat poY = (CGFloat)res.Y;// + testPoint/2.0;
  
  CGFloat xPoint =  (poX * mapOriginalWidth) / mapWidthInMeter;
  CGFloat yPoint =  mapOriginalHeight - poY * mapOriginalHeight / mapWidthInHeight;
  
  CGPoint point = CGPointMake(xPoint, yPoint);
//  CGPoint point = CGPointMake(0.0f, 0.0f);
  
  [UIView animateWithDuration:1.0/10 animations:^{
    current.alpha = 1.0f/res.R;
    CGFloat xPixInMeter = (CGFloat)mapOriginalWidth/mapWidthInMeter;
    CGFloat yPixInMeter = (CGFloat)mapOriginalWidth/mapWidthInMeter;
    
    CGRect pointFrame = CGRectMake(0.0,0.0, 2.0f * xPixInMeter * res.R, 2.0f * yPixInMeter * res.R);
    CGRect arrowFrame = CGRectMake(0.0,0.0, 2.0f * 2 * xPixInMeter, 2.0f * 2 * yPixInMeter);
    current.bounds = pointFrame;
    arrow.bounds = arrowFrame;
    
    current.layer.cornerRadius = (CGFloat)res.R*mapOriginalWidth/mapWidthInMeter;
    
    arrowPath = [UIBezierPath bezierPath];
    [arrowPath moveToPoint:CGPointMake(2.0f * yPixInMeter, 0.0f)];
    [arrowPath addLineToPoint:CGPointMake(2.0f * (1.0f + sqrt(2.0)/2.0) * xPixInMeter, 2.0f * (1.0f + sqrt(2.0)/2.0) * yPixInMeter)];
    [arrowPath addLineToPoint:CGPointMake(2.0f * xPixInMeter, 2.0f * yPixInMeter)];
    [arrowPath addLineToPoint:CGPointMake(2.0f * (1.0f - sqrt(2.0)/2.0) * xPixInMeter, 2.0f * (1.0f + sqrt(2.0)/2.0) * yPixInMeter)];
    [arrowPath closePath];
    
    arrowLayer = [CAShapeLayer layer];
    [arrowLayer setPath: [arrowPath CGPath]];
    [arrowLayer setStrokeColor:[kBlueColor CGColor]];
    [arrowLayer setLineWidth:1.0f];
    [arrowLayer setLineJoin:kCALineJoinRound];
    [arrowLayer setFillColor:[kBlueColor CGColor]];
    [arrow.layer addSublayer:arrowLayer];
    
    current.transform = CGAffineTransformMakeRotation((CGFloat)res.Yaw);
    arrow.transform = CGAffineTransformMakeRotation((CGFloat)res.Yaw);
    current.center = point;
    arrow.center = point;
  }];
  
  
  
  current.hidden = NO;
  arrow.hidden = NO;
  routeFromSublocId = res.outSubLocation;
  
  if (self.imageSublocationId != res.outSubLocation){
    current.hidden = YES;
    arrowLayer.hidden = YES;
    _enableFollow = NO;
  }
  else{
    current.hidden = NO;
    arrowLayer.hidden = NO;
  }
  
  if(_enableFollow ){
    angle += 0.001;
    
    CGSize zoomSize;
    zoomSize.width  = _sv.bounds.size.width;
    zoomSize.height = _sv.bounds.size.height;
    //offset the zoom rect so the actual zoom point is in the middle of the rectangle
    
    CGRect zoomRect;
    zoomRect.origin.x    = (point.x*zoomScale - zoomSize.width / 2.0f);
    zoomRect.origin.y    = (point.y*zoomScale - zoomSize.height / 2.0f);
    zoomRect.size.width  = zoomSize.width;
    zoomRect.size.height = zoomSize.height;
    
    _sv.contentOffset = CGPointMake(zoomRect.origin.x, zoomRect.origin.y);
    
    rotateAngle = -res.Yaw;
    //rotatePoint = CGPointMake(0,0);
    rotatePoint = CGPointMake(point.x - self.mapHelper.contentView.bounds.size.width/2.0, point.y - self.mapHelper.contentView.bounds.size.height/2.0);
    self.mapHelper.contentView.transform = CGAffineTransformMakeRotationAt(rotateAngle, rotatePoint, zoomScale);
  }
  
  
  if(isZoom) {
    [self zoomToPoint:point withScale:1 animated:YES];
  }
  
  if(!CGPointEqualToPoint(routePoint, CGPointZero)) {
    isRoutingNow = YES;
    [self drawRouteWithXml:[self.navigineManager makeRoute :(int)routeFromSublocId :res.X :res.Y :(int)routeToSublocId :routePoint.x :routePoint.y]];
  }
  
}


CGAffineTransform CGAffineTransformMakeRotationAt(CGFloat angle, CGPoint pt, CGFloat scale){
  const CGFloat fx = pt.x;
  const CGFloat fy = pt.y;
  const CGFloat fcos = cos(angle);
  const CGFloat fsin = sin(angle);
  return CGAffineTransformMake(fcos*scale, fsin*scale, -fsin*scale, fcos*scale, (fx - fx * fcos + fy * fsin)*scale, (fy - fx * fsin - fy * fcos)*scale);
}

-(void)drawRouteWithXml:(NSArray *)str {
  Vertex *vertex;// = [[Vertex alloc] init];
  
  routeArray = [[NSMutableArray alloc] init];
  [routeArray removeAllObjects];
  
  for(int i = 0; i< [str count]; i++){
    vertex = [str objectAtIndex:i];
    if(_imageSublocationId == vertex.subLocation){
      CGPoint p = CGPointMake([vertex x], [vertex y]);
      [routeArray addObject:[NSValue valueWithCGPoint:p]];
    }
  }
  [self drawWayWithArray];
  return;
}

-(void)drawWayWithArray {
  
  if(routeArray.count == 0) return;
  
  [routeLayer removeFromSuperlayer];
  routeLayer = nil;
  
  [uipath removeAllPoints];
  uipath = nil;
  
  
  uipath     = [[UIBezierPath alloc] init];
  routeLayer = [CAShapeLayer layer];
  
  
  float distance = 0;
  CGPoint prevPoint = [[routeArray objectAtIndex:0] CGPointValue];
  
  for(int i = 0; i < routeArray.count; i++) {
    
    if(i == routeArray.count-1)//
    {
      
      CGPoint p = [[routeArray objectAtIndex:routeArray.count-1] CGPointValue];

      CGFloat mapWidthInMeter = [self.navigineManager DEFAULT_WIDTH];
      CGFloat mapOriginalWidth = (CGFloat)self.mapHelper.contentView.bounds.size.width;
      CGFloat poX = (CGFloat)p.x;
      
      
      CGFloat mapWidthInHeight = [self.navigineManager DEFAULT_HEIGHT];
      CGFloat mapOriginalHeight = (CGFloat)self.mapHelper.contentView.bounds.size.height;
      CGFloat poY = (CGFloat)p.y;
      
      CGFloat xPoint =  (poX * mapOriginalWidth) / mapWidthInMeter;
      CGFloat yPoint =  mapOriginalHeight - poY * mapOriginalHeight / mapWidthInHeight;
      
      CGPoint point = CGPointMake(xPoint, yPoint);
      
      
      lastImg.bottom = yPoint;
      lastImg.centerX = xPoint;
      lastImg.hidden = YES;
      
      
      if(routeToSublocId == _imageSublocationId){
        lastImg.hidden = NO;
      }
      [self.mapHelper.contentView bringSubviewToFront:lastImg];
      
    }
    
    CGPoint p = [[routeArray objectAtIndex:i] CGPointValue];
    
    CGFloat mapWidthInMeter = [self.navigineManager DEFAULT_WIDTH];
    CGFloat mapOriginalWidth = (CGFloat)self.mapHelper.contentView.bounds.size.width;
    CGFloat poX = (CGFloat)p.x;
    
    
    CGFloat mapWidthInHeight = [self.navigineManager DEFAULT_HEIGHT];
    CGFloat mapOriginalHeight = (CGFloat)self.mapHelper.contentView.bounds.size.height;
    CGFloat poY = (CGFloat)p.y;
    
    CGFloat xPoint =  (poX * mapOriginalWidth) / mapWidthInMeter;
    CGFloat yPoint =  mapOriginalHeight - poY * mapOriginalHeight / mapWidthInHeight;
    
    if(i == 0) {
      [uipath moveToPoint:CGPointMake(xPoint, yPoint)];
    }
    else {
      [uipath addLineToPoint:CGPointMake(xPoint, yPoint)];
    }
    distance += sqrtf((p.x - prevPoint.x) * (p.x - prevPoint.x) + (p.y - prevPoint.y) * (p.y - prevPoint.y));
    prevPoint = p;
  }
  
  NSString *finishDistance;
  NSString *finishDistanceTime;
  
  int timeToFinish = lround(distance) / 100.0f;
  if(timeToFinish == 0) {
    timeToFinish = 1;
    finishDistanceTime   = [NSString stringWithFormat:@"< %d min",timeToFinish];
    
  }
  else {
    finishDistanceTime   = [NSString stringWithFormat:@"%d min",timeToFinish];
  }
  
  if(lround(distance) < 10) {
    finishDistance   = [NSString stringWithFormat:@"< 10m"];
  }
  else {
    finishDistance   = [NSString stringWithFormat:@"%ldm", lround(distance)];
  }
  
  _distanceLabelTime.text = finishDistanceTime;
  _distanceLabel.text = finishDistance;
  
  routeLayer.hidden = NO;
  routeLayer.path            = [uipath CGPath];
  routeLayer.strokeColor     = [[UIColor blueColor] CGColor];
  routeLayer.lineWidth       = 2.0;
  routeLayer.lineJoin        = kCALineJoinRound;
  routeLayer.fillColor       = [[UIColor clearColor] CGColor];
  
  //[_contentView.layer insertSublayer:routeLayer atIndex:0];
  [self.mapHelper.contentView.layer addSublayer:routeLayer];
  [self.mapHelper.contentView bringSubviewToFront:current];
  
  if(distance <= 5 && distance >= 2) {
    //[self showFinishViewWithTitle:@"Ресторан"];
    [self stopRoute];
  }
}

- (void)addPinToMapWithVenue:(Venue *)v andImage:(UIImage *)image
{
  CGFloat mapWidthInMeter = [self.navigineManager DEFAULT_WIDTH];
  CGFloat mapOriginalWidth = (CGFloat)self.mapHelper.contentView.bounds.size.width;
  CGFloat poX = v.realX.doubleValue;
  
  
  CGFloat mapWidthInHeight = [self.navigineManager DEFAULT_HEIGHT];
  CGFloat mapOriginalHeight = (CGFloat)self.mapHelper.contentView.bounds.size.height;
  CGFloat poY = v.realY.doubleValue;
  
  CGFloat xPoint =  (poX * mapOriginalWidth) / mapWidthInMeter;
  CGFloat yPoint =  mapOriginalHeight - poY * mapOriginalHeight / mapWidthInHeight;
  
  
  CGPoint point = CGPointMake(xPoint, yPoint);
  
  MapPin *btnPin = [[MapPin alloc] initWithFrame:CGRectZero];
  [btnPin setImage:image forState:UIControlStateNormal];
  [btnPin setImage:image forState:UIControlStateHighlighted];
  [btnPin addTarget:self action:@selector(btnPinPressed:) forControlEvents:UIControlEventTouchUpInside];
  [btnPin sizeToFit];
  btnPin.bottom  = point.y;
  btnPin.centerX = point.x;
  btnPin.originalPoint  = CGPointMake(v.realX.doubleValue, v.realY.doubleValue);
  btnPin.title  = v.name;
  btnPin.venues = v;
  [self.mapHelper.contentView addSubview:btnPin];
  [pins addObject:btnPin];
  
}

- (IBAction)btnPinPressed:(id)sender {
  currentPin = (MapPin *)sender;
  
  [self zoomToPoint:currentPin.center withScale:1.0 animated:YES];
  [self showAnnotationForMapPin:currentPin];
}

- (void)showAnnotationForMapPin:(MapPin *)mappin {
  
  if(mapView) {
    [mapView removeFromSuperview];
    mapView = nil;
    
  }
  
  mapView = [[UIView alloc] initWithFrame:CGRectMake(0,0, 150, 35)];
  mapView.backgroundColor = [UIColor clearColor];
  mapView.clipsToBounds = NO;
  mapView.alpha = 0;
  
  UIImageView *bg = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 150, 35)];
  
  bg.backgroundColor = kColorFromHex(0x222428);
  bg.alpha = 0.9f;
  bg.layer.cornerRadius = 5.0f;
  [mapView addSubview:bg];
  
  
  UIImageView *pipka = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"elmPipka"]];
  [pipka sizeToFit];
  pipka.top = bg.bottom - 1.0f;
  pipka.centerX = bg.centerX;
  [mapView addSubview:pipka];
  
  
  UIButton *btn = [UIButton buttonWithType:UIButtonTypeCustom];
  [btn setImage:[UIImage imageNamed:@"btnMapInfo"] forState:UIControlStateNormal];
  [btn setImage:[UIImage imageNamed:@"btnMapInfo"] forState:UIControlStateHighlighted];
  [btn addTarget:self action:@selector(btn) forControlEvents:UIControlEventTouchUpInside];
  [btn sizeToFit];
  [mapView addSubview:btn];
  btn.right = bg.right;
  
  
  UILabel *title = [[UILabel alloc] init];
  title.font = [UIFont fontWithName:@"Intro-Book" size:10.0f];
  title.text = currentPin.title.uppercaseString;
  title.textColor = [UIColor whiteColor];
  [title sizeToFit];
  [mapView addSubview:title];
  
  
  title.left   = (((mapView.bounds.size.width - btn.bounds.size.width)/2) - (title.bounds.size.width / 2)) ;
  
  title.centerY = bg.centerY;
  
  mapView.bottom   = mappin.top;
  mapView.centerX  = mappin.centerX;
  
  [self.mapHelper.contentView addSubview:mapView];
  
  [UIView animateWithDuration:0.3 animations:^{
    mapView.alpha = 1;
    mapView.bottom   = mappin.top - 9.0f;
  }];
}

- (void)btn {
  
  self.venueDelegate = [PlaceView sharedManager];
  
  // [MRProgressOverlayView showOverlayAddedTo:self.navigationController.view title:@"Построение маршрута" mode:MRProgressOverlayViewModeIndeterminateSmall animated:YES];
  
  //[self startRouteWithFinishPoint:currentPin.originalPoint andRouteType:RouteTypeFromIcon];
  //[self showNavigationViewWithTitle:currentPin.title];
  
  PlaceView *vc = (PlaceView *)[self.storyboard instantiateViewControllerWithIdentifier:@"place"];
  vc.venues = currentPin.venues;
  vc.fromType = @"map";
  UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:vc];
  nav.navigationBarHidden = YES;
  
  [self presentViewController:nav animated:YES completion:nil];
  //[self deselectPins];
  
}

- (IBAction)backRoutePressed:(id)sender {
  
  [self.navigationController setNavigationBarHidden:NO animated:YES];
  
  _routeType = RouteTypeNone;
  self.sv.origin = CGPointZero;
//
//  if([self.delegate respondsToSelector:@selector(getCoordinates)]) {
//    
//    [self stopRoute];
//    
//    CustomTabBarViewController *slide = (CustomTabBarViewController *)self.tabBarController;
//    self.delegate = nil;
//    [slide setFlights];
//    
//  }
//  else {
    [self stopRoute];
//  }
  
}

- (void)deselectPins {
  
  [UIView animateWithDuration:0.3 animations:^{
    mapView.alpha = 0;
    mapView.bottom  = currentPin.bottom ;
  } completion:^(BOOL finished) {
    currentPin = nil;
    [mapView removeFromSuperview];
    mapView = nil;
  }];
}

- (void)tapPress:(UITapGestureRecognizer *)gesture {
  [self deselectPins];
}

- (void)startRouteWithFinishPoint:(CGPoint)point andRouteType:(RouteType)type {
  //navigineManager = [NavigineManager sharedManager];
  
  //BOOL t = navigineManager.isNavigineFine;
  
  if(![self.navigineManager isNavigineFine]) {
    
    [UIAlertView showWithTitle:@"Невозможно построить маршрут" message:@"Вы находитесь вне зоны действия навигации" cancelButtonTitle:@"OK"];
    //[MRProgressOverlayView dismissOverlayForView:self.navigationController.view animated:YES];
    
    if(pin && type == RouteTypeFromClick) {
      [pin removeFromSuperview];
    }
    return;
  }
  
  _routeType = type;
  
  if(isRoutingNow) {
    [self stopRoute];
  }
  
  routePoint = point;
  routeToSublocId = _imageSublocationId;
  isRoutingNow = YES;
  //[MRProgressOverlayView dismissOverlayForView:self.navigationController.view animated:YES];
  
}

- (void)stopRoute {
  
  if(pin && (_routeType != RouteTypeFromClick || _routeType == RouteTypeNone)) {
    [pin removeFromSuperview];
  }
  
   _navigationTitle.text  = @"";
   _navigationView.alpha  = 0;
   _navigationView.hidden = YES;
  
  isRoutingNow = NO;
  routePoint = CGPointZero;
  
  [routeLayer removeFromSuperlayer];
  routeLayer = nil;
  
  [uipath removeAllPoints];
  uipath = nil;
  
  lastImg.hidden = YES;
  
  
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView {
  
  return self.mapHelper.contentView;
}

- (void)scrollViewDidZoom:(UIScrollView *)scrollView {
  [self centerScrollViewContents];
  if(self.enableFollow == YES) self.mapHelper.contentView.origin = CGPointMake(0.f, 0.f);
}

- (void)scrollViewDidEndZooming:(UIScrollView *)scrollView withView:(UIView *)view atScale:(CGFloat)scale {
  [self movePositionWithZoom:NO];
}

- (IBAction)currentLocationPressed:(id)sender {
  [self movePositionWithZoom:YES];
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

- (IBAction)zoomButton:(id)sender {
  
  UIButton *btn = (UIButton *)sender;
  
  [UIView animateWithDuration:0.3 delay:0 options:UIViewAnimationOptionCurveEaseOut animations:^{
    btn.transform = CGAffineTransformMakeScale(1.2, 1.2);
  } completion:^(BOOL finished) {
    
  }];
}

- (IBAction)zoomButtonOut:(id)sender {
  
  UIButton *btn = (UIButton *)sender;
  
  [UIView animateWithDuration:0.3 delay:0 options:UIViewAnimationOptionCurveEaseOut animations:^{
    btn.transform = CGAffineTransformMakeScale(1.0, 1.0);
  } completion:^(BOOL finished) {
    
  }];
  
  if(sender == _zoomInBtn) {
    [_sv setZoomScale:_sv.zoomScale + 0.2f animated:YES];
  }
  else {
    [_sv setZoomScale:_sv.zoomScale - 0.2f animated:YES];
  }
}

- (void)zoomToPoint:(CGPoint)zoomPoint withScale:(CGFloat)scale animated: (BOOL)animated
{
  //Normalize current content size back to content scale of 1.0f
  CGSize contentSize;
  contentSize.width  = (_sv.contentSize.width / _sv.zoomScale);
  contentSize.height = (_sv.contentSize.height / _sv.zoomScale);
  
  //translate the zoom point to relative to the content rect
  //zoomPoint.x = (zoomPoint.x / _sv.bounds.size.width) * contentSize.width;
  //zoomPoint.y = (zoomPoint.y / _sv.bounds.size.height) * contentSize.height;
  
  //derive the size of the region to zoom to
  CGSize zoomSize;
  zoomSize.width  = _sv.bounds.size.width / scale;
  zoomSize.height = _sv.bounds.size.height / scale;
  
  //offset the zoom rect so the actual zoom point is in the middle of the rectangle
  CGRect zoomRect;
  zoomRect.origin.x    = zoomPoint.x - zoomSize.width / 2.0f;
  zoomRect.origin.y    = zoomPoint.y - zoomSize.height / 2.0f;
  zoomRect.size.width  = zoomSize.width;
  zoomRect.size.height = zoomSize.height;
  
  //apply the resize
  
  [_sv zoomToRect:zoomRect animated: YES];
  
}


- (void)centerScrollViewContents {
  
  CGSize boundsSize = _sv.bounds.size;
  CGRect contentsFrame = self.mapHelper.contentView.frame;
  
  if (contentsFrame.size.width < boundsSize.width) {
    contentsFrame.origin.x = (boundsSize.width - contentsFrame.size.width) / 2.0f;
  } else {
    contentsFrame.origin.x = 0.0f;
  }
  
  if (contentsFrame.size.height < boundsSize.height) {
    contentsFrame.origin.y = (boundsSize.height - contentsFrame.size.height) / 2.0f;
  } else {
    contentsFrame.origin.y = 0.0f;
  }
  self.mapHelper.contentView.frame = contentsFrame;
}

- (void)longPress:(UIGestureRecognizer *)gesture {
  
  if (gesture.state == UIGestureRecognizerStateBegan) {
    CGPoint translatedPoint = [(UIGestureRecognizer*)gesture locationInView:self.mapHelper.contentView];
    
    if(pin) {
      [pin removeFromSuperview];
    }
    
    
    pin = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"elmEmptyPin-copy-3"]];
    [pin sizeToFit];
    pin.center = CGPointMake(translatedPoint.x, 0);
    [self.mapHelper.contentView addSubview:pin];
    
    
    // Animate drop
    [UIView animateWithDuration:0.2 delay:0 options: UIViewAnimationOptionCurveLinear animations:^{
      
      pin.bottom = translatedPoint.y;
      pin.centerX = translatedPoint.x;
      
      // Animate squash
    } completion:^(BOOL finished){
      if (finished) {
        [UIView animateWithDuration:0.05 animations:^{
          pin.transform = CGAffineTransformMakeScale(0.8, 0.8);
          
        }completion:^(BOOL finished){
          if (finished) {
            [UIView animateWithDuration:0.1 animations:^{
              pin.transform = CGAffineTransformIdentity;
            }];
            
          }
        }];
      }
    }];
    
    CGFloat mapWidthInMeter  = [self.navigineManager DEFAULT_WIDTH];
    CGFloat mapOriginalWidth = (CGFloat)self.mapHelper.contentView.bounds.size.width;

    CGFloat mapWidthInHeight  = [self.navigineManager DEFAULT_HEIGHT];
    CGFloat mapOriginalHeight = (CGFloat)self.mapHelper.contentView.bounds.size.height;
    
    
    CGFloat xPoint = (translatedPoint.x / mapOriginalWidth) * mapWidthInMeter;
    CGFloat yPoint = (mapOriginalHeight - translatedPoint.y) /  mapOriginalHeight * mapWidthInHeight;
    CGPoint point = CGPointMake(xPoint, yPoint);
    
    //[MRProgressOverlayView showOverlayAddedTo:self.navigationController.view title:@"Построение маршрута" mode:MRProgressOverlayViewModeIndeterminateSmall animated:YES];
    
    [self startRouteWithFinishPoint:point andRouteType:RouteTypeFromClick];
    [self showNavigationViewWithTitle:@"До точки"];
    
  }
  
}

- (void)viewWillAppear:(BOOL)animated {
  
  [super viewWillAppear:animated];
  
  _sv.minimumZoomScale = minimumZoomScale;
  _sv.maximumZoomScale = maximumZoomScale;
  _sv.zoomScale = 1.0f;
  zoomScale = 1.0f;
  
}

- (IBAction)menuPressed:(id)sender {
  if(self.slidingPanelController.sideDisplayed == MSSPSideDisplayedLeft) {
    [self.slidingPanelController closePanel];
  }
  else {
    [self.slidingPanelController openLeftPanel];
  }
}

- (IBAction)zoomInTouch:(id)sender {
  [_sv setZoomScale:zoomScale + 0.2f animated:NO];
  zoomScale = _sv.zoomScale;
  [self movePositionWithZoom:NO];
}

- (IBAction)zoomOutTouch:(id)sender {
  [_sv setZoomScale:zoomScale - 0.2f animated:NO];
  zoomScale = _sv.zoomScale;
  [self movePositionWithZoom:NO];
}

- (IBAction)folowing:(id)sender {
  if (_enableFollow){
    [_rotateButton setImage:[UIImage imageNamed:@"rotate.png"] forState:UIControlStateNormal];
    _enableFollow = NO;
    _sv.scrollEnabled = YES;
    self.mapHelper.contentView.transform = CGAffineTransformMakeRotationAt(0.0, rotatePoint,zoomScale);
    zoomScale = _sv.zoomScale;
    self.sv.pinchGestureRecognizer.enabled = YES;
    [self.sv setZoomScale:zoomScale - 0.0001f];
  }
  else{
    [_rotateButton setImage:[UIImage imageNamed:@"rotatePressed.png"] forState:UIControlStateNormal];
    _enableFollow = YES;
    _sv.scrollEnabled = NO;
    zoomScale = _sv.zoomScale;
    self.sv.pinchGestureRecognizer.enabled = NO;
    [self.sv setZoomScale:zoomScale + 0.0001f];
  }
}



#pragma mark - AFPickerViewDataSource

- (NSInteger)numberOfRowsInPickerView:(AFPickerView *)pickerView{
  if (self.mapHelper.webViewArray.count == 0){
    [UIAlertView showWithTitle:@"ATTENTION!" message:@"Location not set" cancelButtonTitle:@"OK"];
    [self.mapHelper.contentView removeFromSuperview];
  }
  return self.mapHelper.webViewArray.count;
}

- (NSString *)pickerView:(AFPickerView *)pickerView titleForRow:(NSInteger)row{
  return [NSString stringWithFormat:@"%zd", row + 1];
}




#pragma mark - AFPickerViewDelegate methods

- (void)pickerView:(AFPickerView *)pickerView didSelectRow:(NSInteger)row{
  self.floor = row;
  self.imageSublocationId = [[self.mapHelper.sublocId objectForKey:[NSNumber numberWithInteger:row]] intValue];
  
  error = [self.navigineManager getWidthAndHeightbyIndex:self.floor];
  if(error != 0){
    [UIAlertView showWithTitle:@"ERROR" message:@"Incorrect width and height" cancelButtonTitle:@"OK"];
  }
  
  [self.mapHelper.contentView removeFromSuperview];
  self.mapHelper.contentView = self.mapHelper.webViewArray[self.floor];
  
  for(UIImageView *p in pins) [p removeFromSuperview];
  
  [self.mapHelper.contentView addSubview:current];
  [self.mapHelper.contentView addSubview:arrow];
  [self.mapHelper.contentView addSubview:lastImg];
  
  [_sv setZoomScale:zoomScale animated:YES];
  for (Venue *v in [self.navigineManager venues]) {
    if(v.sublocationId == _imageSublocationId){
      [self addPinToMapWithVenue:v  andImage:[UIImage imageNamed:[NSString stringWithFormat:@"cat%d", 1]]];//v.category.intValue]]];
    }
  }
  
  [self.sv addSubview:self.mapHelper.contentView];
  self.mapHelper.contentView.hidden = NO;
  if (_enableFollow){
    [_rotateButton setImage:[UIImage imageNamed:@"rotate.png"] forState:UIControlStateNormal];
    _enableFollow = NO;
    _sv.scrollEnabled = YES;
    self.mapHelper.contentView.transform = CGAffineTransformMakeRotationAt(0.0, rotatePoint,zoomScale);
  }
  [self centerScrollViewContents];
}

#pragma mark - MapHelperDelegate

- (void) startNavigation{
}

- (void) stopNavigation{
  current.hidden = YES;
  arrowLayer.hidden = YES;
  _enableFollow = NO;
}

- (void) changeCoordinates{
  [self movePositionWithZoom:NO];
}

@end

