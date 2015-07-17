//
//  NavigineDemoViewController.m
//  NavigineDemo
//
//  Created by Administrator on 7/14/14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "NavigineDemoViewController.h"
#import "NavigineSDK.h"
#import "NavigineManager.h"

#import "TBXML.h"

//#define DEFAULT_WIDTH  73.0f
//#define DEFAULT_HEIGHT 62.7f
double oldTime;

@interface NavigineDemoViewController ()

@end

@implementation NavigineDemoViewController



- (void)viewDidLoad
{
    [super viewDidLoad];
    
	// Do any additional setup after loading the view, typically from a nib.
    oldTime = 0.0;
    _firstLoad = YES;
    _enableMeasure = NO;
    _floor = 0;
    navigineManager = [[NavigineManager alloc] init];
    
    int loadProcess = 0;
    loadProcess = [navigineManager CheckContentLoader:[navigineManager LoaderID]];
    while (loadProcess < 100) loadProcess = [navigineManager CheckContentLoader:[navigineManager LoaderID]];
    
    
    
    [self getMapFrom:@"mech-math_floor0.png" inDir:@"mech_math13"];
/*
    CGPoint scrollOrigin = CGPointMake(-_contentView.image.size.width/2.0, -_contentView.image.size.height/2.0);
    CGSize scrollSize = CGSizeMake(_contentView.image.size.width* 2.0, _contentView.image.size.height*2.0);
    _sv.frame = (CGRect){scrollOrigin, scrollSize};
    //NSLog(@"%lf %lf",_sv.height, _sv.width);
    //_sv.height = _contentView.image.size.height*2;
    //_sv.width = _contentView.image.size.width*2;
*/
    NSArray *itemArray = [NSArray arrayWithObjects: @"1", @"2", @"3", nil];
    _segmentedControl = [[UISegmentedControl alloc] initWithItems:itemArray];
    //segmentedControl.frame = CGRectMake(10,self.view.size.height - 48, self.view.size.width-10, 44);
    _segmentedControl.frame = CGRectMake(230,100, 120, 40);
    //segmentedControl.segmentedControlStyle = UISegmentedControlStylePlain;
    _segmentedControl.transform = CGAffineTransformMakeRotation(M_PI / 2.0);
    NSArray *arr = [_segmentedControl subviews];
    for (int i = 0; i < [arr count]; i++) {
        UIView *v = (UIView*) [arr objectAtIndex:i];
        NSArray *subarr = [v subviews];
        for (int j = 0; j < [subarr count]; j++) {
            if ([[subarr objectAtIndex:j] isKindOfClass:[UILabel class]]) {
                UILabel *l = (UILabel*) [subarr objectAtIndex:j];
                l.transform = CGAffineTransformMakeRotation(- M_PI / 2.0); //do the reverse of what Ben did
            }
        }
    }
    
    
    _segmentedControl.selectedSegmentIndex = 0;
 
    [_segmentedControl addTarget:self
                         action:@selector(pickOne:)
               forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:_segmentedControl];
    //[_sv addSubview:segmentedControl];

    
    
    //[_chooseFloor initWithItems:@[@"1",@"2",@"3",nil]];/*
   // [_chooseFloor addTarget:self
    //                     action:@selector(getMapFrom: inDir:)
     //          forControlEvents:UIControlEventValueChanged];
    
    current = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"elmUserDirection"]];
    [current sizeToFit];
    current.alpha = 0.5;
    current.hidden = YES;
    [_contentView addSubview:current];
    
    currentArrow = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"arrow"]];
    currentArrow.size = current.size;
    currentArrow.hidden= YES;
    [_contentView addSubview:currentArrow];
    
    lastImg = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"elmEmptyPin-copy-2"]];
    [lastImg sizeToFit];
    lastImg.hidden   = YES;
    [_contentView addSubview:lastImg];
    
    UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPress:)];
    longPress.minimumPressDuration = 1;
    longPress.delaysTouchesBegan   = NO;
    [_sv addGestureRecognizer:longPress];
    
    UIRotationGestureRecognizer *rotate=[[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(rotation:)];
    [_sv addGestureRecognizer:rotate];
    //[self centerScrollViewContents];
    
    [self detectBluetooth];
    
    return;
}

-(void) pickOne:(id)sender
{
    UISegmentedControl *segmentedControl = (UISegmentedControl *)sender;
    _floor = [segmentedControl selectedSegmentIndex];
    NSString *floorNumber = [[NSString alloc]initWithFormat:@"mech-math_floor%ld.png",(long)[segmentedControl selectedSegmentIndex]];
    [self getMapFrom:floorNumber inDir:@"mech_math13"];
    
    return;
}

-(void) rotation:(UIRotationGestureRecognizer *) sender
{
    if (([sender state] == UIGestureRecognizerStateBegan || [sender state] == UIGestureRecognizerStateChanged) && !_enableMeasure) {
        [sender view].transform = CGAffineTransformRotate([[sender view] transform], [(UIRotationGestureRecognizer *)sender rotation]);
        [(UIRotationGestureRecognizer *)sender setRotation:0];}
}


-(void)getMapFrom: (NSString *)fileName inDir: (NSString *)dirName
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *path = [[[paths objectAtIndex:0] stringByAppendingPathComponent: dirName]stringByAppendingPathComponent: fileName];

    UIImage *im = [[UIImage alloc] initWithContentsOfFile:path];
    //[[UIImage alloc] initWithData:<#(NSData *)#>
    
    [_contentView setImage:im];
    NSArray *_subviews = _sv.subviews;
    

    if (_firstLoad == NO)
    {
        UIImageView *b;
        b = (id)_subviews[0];
        
        _contentView.frame = b.frame;
        [_sv insertSubview:_contentView atIndex:0];
    }
    else  // first load
    {
        CGSize _s= _contentView.image.size;
        CGPoint contentOrigin = _contentView.origin;
        _contentView.frame = (CGRect){.origin=contentOrigin, _s};
        [_sv addSubview:_contentView];
        _firstLoad = NO;
    }
    path = [[[paths objectAtIndex:0] stringByAppendingPathComponent: dirName] stringByAppendingPathComponent:@"map.zip"];
    
    double DEFAULT_WIDTH, DEFAULT_HEIGHT;
    
    [navigineManager getWidthAndHeight:&DEFAULT_WIDTH :&DEFAULT_HEIGHT :_segmentedControl.selectedSegmentIndex :path];
    navigineManager.DEFAULT_WIDTH = DEFAULT_WIDTH;
    navigineManager.DEFAULT_HEIGHT = DEFAULT_HEIGHT;
    return;
}

- (void)centralManagerDidUpdateState:(CBCentralManager *)central {
    [self isLECapableHardware];
}

- (void)didReceiveMemoryWarning

{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void)starNavigation
{
    
    //navigineManager = [NavigineManager sharedManager];
    
    /*if(![navigineManager isNavigineFine]) {
        return;
    }*/
    
    if(_timerNavigation == nil) {
        _timerNavigation = [NSTimer scheduledTimerWithTimeInterval:1.0/10
                                                          target:self
                                                        selector:@selector(naviganeTick:)
                                                        userInfo:nil
                                                         repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:_timerNavigation forMode:NSRunLoopCommonModes];
        
    }
}


- (void)stopNavigate {
    
    navigineManager = nil;
    [_timerNavigation invalidate];
    _timerNavigation = nil;
    current.hidden = YES;
    
}

- (void)detectBluetooth
{
    if(!_bluetoothManager) {
        // Put on main queue so we can call UIAlertView from delegate callbacks.
        _bluetoothManager = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
    }
    
}

/*
 Uses CBCentralManager to check whether the current platform/hardware supports Bluetooth LE. An alert is raised if Bluetooth LE is not enabled or is not supported.
 */
- (BOOL)isLECapableHardware
{
    NSString * state = nil;
    
    switch ([_bluetoothManager state])
    {
        case CBCentralManagerStateUnsupported:
            state = @"The platform/hardware doesn't support Bluetooth Low Energy.";
            [UIAlertView showWithTitle:@"Error" message:state cancelButtonTitle:@"OK"];
            break;
        case CBCentralManagerStateUnauthorized:
            state = @"The app is not authorized to use Bluetooth Low Energy.";
            [UIAlertView showWithTitle:@"Error" message:state cancelButtonTitle:@"OK"];
            break;
        case CBCentralManagerStatePoweredOff:
            state = @"Для работы навигации необходимо включить в настройках Bluetooth";
            [UIAlertView showWithTitle:@"Внимание" message:state cancelButtonTitle:@"OK"];
            [self stopNavigate];
            return FALSE;
            break;
        case CBCentralManagerStatePoweredOn:
            [self starNavigation];
            return TRUE;
        case CBCentralManagerStateUnknown:
        default:
            return FALSE;
            
    }
    
    return FALSE;
}


- (void)naviganeTick:(NSTimer *)timer {
    [self movePositionWithZoom:NO];
    //[self getMapFrom:@"mech-math.png" inDir:@"mech_math13"];
}

- (void)movePositionWithZoom:(BOOL)isZoom {
    
    res = [navigineManager GetNavigationResults];
    if (_floor != res.Z){
        current.hidden = YES;
        currentArrow.hidden = YES;
        return;
    }
    
    if(DEBUG_MODE) {
        testPoint += 5;
        res.X = 1150 - testPoint;
        res.Y = 58;
        res.Z = 0;
        res.ErrorCode = 0;
        
    }
    
    if((res.X == 0.0 && res.Y == 0.0) || res.ErrorCode != 0)  {
        
        if(res.ErrorCode == 4 && _error4count < 10) {
            _error4count++;
        }
        else {
            current.hidden = YES;
            currentArrow.hidden = YES;
            _error4count = 0;
        }
        //return;
    }
    

    CGFloat mapWidthInMeter = [navigineManager DEFAULT_WIDTH];
    CGFloat mapOriginalWidth = (CGFloat)self.contentView.size.width / _sv.zoomScale;
    CGFloat poX = (CGFloat)res.X;
    
    CGFloat mapWidthInHeight = [navigineManager DEFAULT_HEIGHT];
    CGFloat mapOriginalHeight = (CGFloat)self.contentView.size.height / _sv.zoomScale;
    CGFloat poY = (CGFloat)res.Y;
    
    CGFloat xPoint =  (poX * mapOriginalWidth) / mapWidthInMeter;
    CGFloat yPoint =  mapOriginalHeight - poY * mapOriginalHeight / mapWidthInHeight;
    
    CGPoint point = CGPointMake(xPoint, yPoint);
    
    current.center = point;
    current.hidden = NO;
    

    CGFloat dx, dy;
    currentArrow.center = point;
    currentArrow.hidden = NO;

    
    
    if(_enableMeasure ){
        
        
        CGPoint originInView = CGPointMake(current.center.x * _sv.zoomScale + _contentView.origin.x - _sv.contentOffset.x, current.center.y * _sv.zoomScale + _contentView.origin.y - _sv.contentOffset.y);
        CGPoint newOrigin = CGPointMake(self.view.centerX-originInView.x, self.view.centerY - originInView.y);
        
        dx = - point.x + oldPoint.x;
        dy = - point.y + oldPoint.y;
        
        oldSize = _contentView.frame.size;
        newOrigin.x = _sv.contentOffset.x - newOrigin.x - dx;
        newOrigin.y = _sv.contentOffset.y - newOrigin.y - dy;
        _sv.contentOffset = newOrigin;

        oldPoint = current.center;
        angle = 1.0;
        _sv.transform = CGAffineTransformMakeRotation(-(CGFloat)res.Yaw);
    }
    else currentArrow.transform = CGAffineTransformMakeRotation((CGFloat)res.Yaw);
    

    if(isZoom) {
        [self zoomToPoint:point withScale:1 animated:YES];
    }
    
    if(!CGPointEqualToPoint(routePoint, CGPointZero)&&_floor == res.Z) {
        isRoutingNow = YES;
        [self drawRouteWithXml:[navigineManager MakeRoute:res.X:res.Y:res.Z: routePoint.x:routePoint.y:0]];
    }
    
}


CGAffineTransform CGAffineTransformMakeRotationAt(CGFloat angle, CGPoint pt){
    const CGFloat fx = pt.x;
    const CGFloat fy = pt.y;
    const CGFloat fcos = cos(angle);
    const CGFloat fsin = sin(angle);
    return CGAffineTransformMake(fcos, fsin, -fsin, fcos, fx - fx * fcos + fy * fsin, fy - fx * fsin - fy * fcos);
}

-(void)drawRouteWithXml:(NSString *)str {
    
    NSData *data = [str dataUsingEncoding:NSUTF8StringEncoding];
    TBXML *sourceXML = [[TBXML alloc] initWithXMLData:data error:nil];
    TBXMLElement *rootElement = sourceXML.rootXMLElement;
    
    [self traverseXMLElement:rootElement];
}

- (void)traverseXMLElement:(TBXMLElement *)element {
    routeArray = [[NSMutableArray alloc] init];
    [routeArray removeAllObjects];
    
    do {
        if (element->firstChild)
            [self traverseXMLElement:element->firstChild];
        
        
        NSString *xStr  = [TBXML valueOfAttributeNamed:@"x" forElement:element];
        NSString *yStr  = [TBXML valueOfAttributeNamed:@"y" forElement:element];
        //   NSString *zStr  = [TBXML valueOfAttributeNamed:@"z" forElement:element];
        
        
        CGPoint p = CGPointMake([xStr floatValue], [yStr floatValue]);
        [routeArray addObject:[NSValue valueWithCGPoint:p]];
        
    } while ((element = element->nextSibling));
    
    [self drawWayWithArray];
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
    
    for(int i = 0; i < routeArray.count-1; i++) {
        
        if(i == routeArray.count-2)// 
        {
            
            CGPoint p = [[routeArray objectAtIndex:routeArray.count-2] CGPointValue];
            
            CGFloat mapWidthInMeter =  [navigineManager DEFAULT_WIDTH];
            CGFloat mapOriginalWidth = (CGFloat)self.contentView.size.width / _sv.zoomScale;
            CGFloat poX = (CGFloat)p.x;
            
            CGFloat mapWidthInHeight = [navigineManager DEFAULT_HEIGHT];
            CGFloat mapOriginalHeight = (CGFloat)self.contentView.size.height  / _sv.zoomScale;
            CGFloat poY= (CGFloat)p.y;
            
            CGFloat  xPoint =  (poX * mapOriginalWidth) / mapWidthInMeter;
            CGFloat  yPoint =  (mapOriginalHeight - poY * mapOriginalHeight / mapWidthInHeight );
            
            
            lastImg.bottom = yPoint;
            lastImg.centerX = xPoint;
            
            lastImg.hidden = NO;
            [_contentView bringSubviewToFront:lastImg];
            
        }
        
        CGPoint p = [[routeArray objectAtIndex:i] CGPointValue];
        
        CGFloat mapWidthInMeter = [navigineManager DEFAULT_WIDTH];
        CGFloat mapOriginalWidth = (CGFloat)self.contentView.size.width  / _sv.zoomScale;
        CGFloat poX = (CGFloat)p.x;
        
        CGFloat mapWidthInHeight = [navigineManager DEFAULT_HEIGHT];
        CGFloat mapOriginalHeight = (CGFloat)self.contentView.size.height  / _sv.zoomScale;
        CGFloat poY= (CGFloat)p.y;
        
        CGFloat xPoint = (poX * mapOriginalWidth) / mapWidthInMeter;
        CGFloat yPoint = mapOriginalHeight - poY * mapOriginalHeight / mapWidthInHeight;
        
        if(i == 0) {
            [uipath moveToPoint:CGPointMake(xPoint, yPoint)];
        }
        else {
            [uipath addLineToPoint:CGPointMake(xPoint, yPoint)];
        }
        
        
        distance += sqrtf((p.x - prevPoint.x) * (p.x - prevPoint.x) + (p.y - prevPoint.y) * (p.y - prevPoint.y));
        
        prevPoint = p;
        
    }
    
    //    for(int i = 0; i < routeArray.count; i++) {
    //
    //        CGPoint p = [[routeArray objectAtIndex:i] CGPointValue];
    //
    //
    //        distance += sqrtf((p.x - prevPoint.x) * (p.x - prevPoint.x) + (p.y - prevPoint.y) * (p.y - prevPoint.y));
    //
    //        NSLog(@"qwe %f %f %f", p.x ,p.y,distance);
    //
    //        prevPoint = p;
    //    }
    
    
    NSString *finishDistance;
    
    
    if(_distanceType == DistanceInMinutes) {
        int timeToFinish = lround(distance) / 83.0f;
        if(timeToFinish == 0) {
            timeToFinish = 1;
        }
        finishDistance   = [NSString stringWithFormat:@"%d мин.",timeToFinish];
        
    }
    else {
        finishDistance   = [NSString stringWithFormat:@"%ld м.", lround(distance)];
    }
    
   // _distanceLabel.text = finishDistance;
    
    routeLayer.path            = [uipath CGPath];
    routeLayer.strokeColor     = [[UIColor whiteColor] CGColor];
    routeLayer.lineWidth       = 2.0;
    routeLayer.lineJoin        = kCALineJoinRound;
    routeLayer.fillColor       = [[UIColor clearColor] CGColor];
    
    [_contentView.layer insertSublayer:routeLayer atIndex:0];
    [_contentView bringSubviewToFront:current];
    [_contentView bringSubviewToFront:currentArrow];
    
    if(distance <= 15 && distance >= 2) {
        
     //   [self showFinishViewWithTitle:@"Ресторан"];
        [self stopRoute];
    }
}

- (void)startRouteWithFinishPoint:(CGPoint)point andRouteType:(RouteType)type {
    
    NSLog(@"qwe %f %f",point.y,point.x);
    
    //navigineManager = [NavigineManager sharedManager];
    
    //BOOL t = navigineManager.isNavigineFine;
    
    if(!navigineManager.isNavigineFine) {
   
        [UIAlertView showWithTitle:@"Невозможно построить маршрут" message:@"Вы находитесь вне зоны действия навигации" cancelButtonTitle:@"OK"];
        [MRProgressOverlayView dismissOverlayForView:self.navigationController.view animated:YES];
        
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
    isRoutingNow = YES;
    [MRProgressOverlayView dismissOverlayForView:self.navigationController.view animated:YES];
    
}

- (void)stopRoute {
    
    if(pin && (_routeType != RouteTypeFromClick || _routeType == RouteTypeNone)) {
        [pin removeFromSuperview];
    }
    
   // _navigationTitle.text  = @"";
   // _navigationView.alpha  = 0;
   // _navigationView.hidden = YES;
    
    isRoutingNow = NO;
    routePoint = CGPointZero;
    
    [routeLayer removeFromSuperlayer];
    routeLayer = nil;
    
    [uipath removeAllPoints];
    uipath = nil;
    
    lastImg.hidden = YES;
    
    
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView {
    
    return _contentView;
    
}

- (void)scrollViewDidZoom:(UIScrollView *)scrollView {
    
    [self centerScrollViewContents];
    
}

- (void)scrollViewDidEndZooming:(UIScrollView *)scrollView withView:(UIView *)view atScale:(CGFloat)scale {
    
    [self movePositionWithZoom:NO];
    
}

- (IBAction)currentLocationPressed:(id)sender {
    
    [self movePositionWithZoom:YES];
    
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
    _sv.transform = CGAffineTransformMakeRotation(0.0);
    [_sv zoomToRect:zoomRect animated: YES];
    CGPoint originInView = CGPointMake(current.center.x * _sv.zoomScale + _contentView.origin.x - _sv.contentOffset.x, current.center.y * _sv.zoomScale + _contentView.origin.y - _sv.contentOffset.y);
    CGPoint newOrigin = CGPointMake(self.view.centerX-originInView.x, self.view.centerY - originInView.y);
    
    
    oldSize = _contentView.frame.size;
    newOrigin.x = _sv.contentOffset.x - newOrigin.x;
    newOrigin.y = _sv.contentOffset.y - newOrigin.y;
    _sv.contentOffset = newOrigin;
    
    oldPoint = current.center;
    _sv.transform = CGAffineTransformMakeRotation(1.0);
        
    //}
    //_contentView.origin = beforeZoomOrigin;
}


- (void)centerScrollViewContents {
    
    CGSize boundsSize = _sv.bounds.size;
    CGRect contentsFrame = _contentView.frame;
    
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
    _contentView.frame = contentsFrame;
    //_contentView.center = (CGPoint){.x=500, .y=100};
}

- (void)longPress:(UIGestureRecognizer *)gesture {
    
    if (gesture.state == UIGestureRecognizerStateBegan) {
        CGPoint translatedPoint = [(UIGestureRecognizer*)gesture locationInView:_contentView];
        
        if(pin) {
            [pin removeFromSuperview];
        }
        
        
        pin = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"elmEmptyPin-copy-3"]];
        [pin sizeToFit];
        pin.center = CGPointMake(translatedPoint.x, 0);
        [_contentView addSubview:pin];
        
        
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
        
        CGFloat mapWidthInMeter  = [navigineManager DEFAULT_WIDTH];
        CGFloat mapOriginalWidth = (CGFloat)self.contentView.size.width / _sv.zoomScale;
        
        CGFloat mapWidthInHeight  = [navigineManager DEFAULT_HEIGHT];
        CGFloat mapOriginalHeight = (CGFloat)self.contentView.size.height / _sv.zoomScale;
        
        
        CGFloat xPoint = (translatedPoint.x / mapOriginalWidth) * mapWidthInMeter;
        CGFloat yPoint = (mapOriginalHeight - translatedPoint.y) /  mapOriginalHeight * mapWidthInHeight;
        CGPoint point = CGPointMake(xPoint, yPoint);
        
        [MRProgressOverlayView showOverlayAddedTo:self.navigationController.view title:@"Построение маршрута" mode:MRProgressOverlayViewModeIndeterminateSmall animated:YES];
        
        [self startRouteWithFinishPoint:point andRouteType:RouteTypeFromClick];
        //[self showNavigationViewWithTitle:@"До точки"];
        
    }
    
}

- (void)viewWillAppear:(BOOL)animated {
    
    [super viewWillAppear:animated];
    
    _sv.minimumZoomScale = 0.2f;
    _sv.maximumZoomScale = 2.0f;
    _sv.zoomScale = 0.5f;

}



- (IBAction)zoomInTouch:(id)sender {
    if(!_enableMeasure) [_sv setZoomScale:_sv.zoomScale + 0.2f animated:YES];
    else{
        
        CGPoint pointToZoom;
        pointToZoom.x = (current.centerX *_sv.zoomScale + _contentView.origin.x);
        pointToZoom.y = (current.centerY *_sv.zoomScale + _contentView.origin.y);
        [self zoomToPoint: pointToZoom withScale: _sv.zoomScale + 0.2f animated: YES];
    }
}

- (IBAction)zoomOutTouch:(id)sender {
    if(!_enableMeasure) [_sv setZoomScale:_sv.zoomScale - 0.2f animated:YES];
    else{
        CGPoint pointToZoom;
        pointToZoom.x = current.centerX + _contentView.origin.x;
        pointToZoom.y = current.centerY + _contentView.origin.y;
        [self zoomToPoint: _sv.center withScale: _sv.zoomScale - 0.2f animated: YES];
    }
}
- (IBAction)measuring:(id)sender {
    if (_enableMeasure){
        [_measureButton setImage:[UIImage imageNamed:@"rotate.png"] forState:UIControlStateNormal];
        _enableMeasure = NO;
        _sv.scrollEnabled = YES;
        _sv.transform = CGAffineTransformMakeRotation(0.0);
        _contentView.origin = oldOrigin;
    }
    else{
        [_measureButton setImage:[UIImage imageNamed:@"rotatePressed.png"] forState:UIControlStateNormal];
        _enableMeasure = YES;
        
        CGPoint originInView = CGPointMake(current.center.x * _sv.zoomScale + _contentView.origin.x - _sv.contentOffset.x, current.center.y * _sv.zoomScale + _contentView.origin.y - _sv.contentOffset.y);
        
        CGPoint newSchift = CGPointMake(self.view.centerX-originInView.x, self.view.centerY - originInView.y);

        
        oldPoint = current.center;
        oldOrigin = _contentView.origin;
        oldSize = _contentView.frame.size;
        newSchift.x = _sv.contentOffset.x - newSchift.x;
        newSchift.y = _sv.contentOffset.y - newSchift.y;
 
        _sv.contentOffset = newSchift;
        
        _enableMeasure = YES;
        _sv.scrollEnabled = NO;
        
    }
}

@end

