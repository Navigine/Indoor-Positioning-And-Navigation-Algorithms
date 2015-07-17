//
//  MapViewHelper.m
//  Navigine_Demo
//
//  Created by Администратор on 04/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "MapHelper.h"

@interface MapHelper(){
  NSDictionary *bluetoothOptions;
  NSTimer      *timerNavigation;
  NSInteger    error;
}
@property (nonatomic, strong) LoaderHelper     *loaderHelper;
@property (nonatomic, strong) NavigineManager  *navigineManager;
@property (nonatomic, strong) CBCentralManager *bluetoothManager;

@end

@implementation MapHelper

+(MapHelper *) sharedInstance{
  static MapHelper * _sharedInstance = nil;
  
  static dispatch_once_t oncePredicate;
  dispatch_once(&oncePredicate, ^{
    _sharedInstance = [[MapHelper alloc] init];
  });
  return _sharedInstance;
}


- (instancetype)init{
  self = [super init];
  if(self){
    self.loaderHelper = [LoaderHelper sharedInstance];
    self.navigineManager = [NavigineManager sharedManager];
    self.webViewArray = [NSMutableArray new];
    self.sublocId = [NSMutableDictionary new];
    
    self.contentView= [[UIWebView alloc] init];
    self.contentView.delegate = self;
    [self.contentView.scrollView setScrollEnabled:NO];
    [self.contentView.scrollView setPagingEnabled:NO];
    [self.contentView.scrollView setUserInteractionEnabled:NO];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(setNewLocation:)
                                                 name:@"setLocation"
                                               object:nil];
    
    bluetoothOptions = [[NSDictionary alloc] initWithObjectsAndKeys:[NSNumber numberWithBool:NO], CBCentralManagerOptionShowPowerAlertKey, nil];
    error = 0;
  }
  return self;
}


- (void) getMapFromZip{
  NSData *imageData = nil;
  NSDictionary *sublocs = nil;
  CGFloat scale;
  CGSize s;
  CGRect screenRect = [[UIScreen mainScreen] bounds];
  CGFloat screenWidth = screenRect.size.width;
  CGFloat screenHeight = screenRect.size.height;
  
  error = [self.navigineManager getSublocNum: nil :&sublocs];
  if(error !=0 ){
    [UIAlertView showWithTitle:@"ERROR" message:@"Incorrect Sublocation" cancelButtonTitle:@"OK"];
    return;
  }
  
  for(int i = 0; i < [sublocs count]; i++){
    [self.sublocId setObject:[sublocs objectForKey:[NSNumber numberWithInt:i]] forKey:[NSNumber numberWithInt:i]];
  }
  
  for(int i = 0; i < [sublocs count]; i++){
    NSString *mimeType = nil;
    
    error = [self.navigineManager getSVGImageByIndex:i :&imageData];
    if(error == 0){
      NSError *svgError;
      TBXML *sourceXML = [TBXML newTBXMLWithXMLData:imageData error:&svgError];
      TBXMLElement *rootElement = sourceXML.rootXMLElement;
      NSString *widthAttribute = [TBXML valueOfAttributeNamed:@"width" forElement:rootElement];
      NSString *heightAttribute = [TBXML valueOfAttributeNamed:@"height" forElement:rootElement];
      NSInteger SVGwidth = [widthAttribute integerValue];
      NSInteger SVGheight = [heightAttribute integerValue];
      if(screenHeight/screenWidth > SVGheight/SVGwidth) {
        scale = screenHeight/SVGheight;
      }
      else scale = screenWidth/SVGwidth;
      s = (CGSize)CGSizeMake(SVGwidth*scale, SVGheight*scale);
      mimeType = @"image/svg+xml";
    }
    else{
      error = [self.navigineManager getPNGImageByIndex:i :&imageData];
      if(error == 0){
        UIImage *PNGim = [UIImage imageWithData:imageData];
        if(screenHeight/screenWidth > PNGim.size.height/PNGim.size.width) {
          scale = screenHeight/PNGim.size.height;
        }
        else scale = screenWidth/PNGim.size.width;
        s = (CGSize)CGSizeMake(PNGim.size.width*scale, PNGim.size.height*scale);
        mimeType = @"image/png";
      }
      if(error !=0 ){
        [UIAlertView showWithTitle:@"ERROR" message:@"Incorrect Image inside archive" cancelButtonTitle:@"OK"];
      }
    }
    UIWebView *currentView= [[UIWebView alloc] init];
    currentView.delegate = self;
    
    [currentView.scrollView setScrollEnabled:NO];
    [currentView.scrollView setPagingEnabled:NO];
    [currentView.scrollView setUserInteractionEnabled:NO];
    
    currentView.scalesPageToFit = YES;
    [currentView loadData:imageData MIMEType:mimeType textEncodingName:@"utf-8" baseURL:nil];
    currentView.bounds = CGRectMake(0, 0, s.width, s.height);
    currentView.hidden = YES;
    
    [self.webViewArray addObject:currentView];
  }
}


-(void)setNewLocation:(NSNotification *)notification{
  self.bluetoothManager = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue() options:bluetoothOptions];
  [self.webViewArray removeAllObjects];
  [self getMapFromZip];
}

- (void)isLECapableHardware{
  NSString * state = nil;
  
  switch ([self.bluetoothManager state]){
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
      break;
    case CBCentralManagerStatePoweredOn:
      [self startNavigate];
      break;
    case CBCentralManagerStateResetting:
      state = @"Resetting Bluetooth...";
      [UIAlertView showWithTitle:@"Wait" message:state cancelButtonTitle:@"OK"];
      break;
    case CBCentralManagerStateUnknown:
      [self stopNavigate];
      break;
  }
}

- (void)startNavigate{
  if(self.delegate && [self.delegate respondsToSelector:@selector(startNavigation)]){
    [self.delegate startNavigation];
  }
  if(timerNavigation == nil) {
    timerNavigation = [NSTimer scheduledTimerWithTimeInterval:1.0/10
                                                        target:self
                                                      selector:@selector(naviganeTick:)
                                                      userInfo:nil
                                                       repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:timerNavigation forMode:NSRunLoopCommonModes];
    
  }
}

- (void)stopNavigate {
  if(self.delegate && [self.delegate respondsToSelector:@selector(stopNavigation)]){
    [self.delegate stopNavigation];
  }
  [timerNavigation invalidate];
  timerNavigation = nil;
  
}

- (void)naviganeTick: (NSTimer *)timer{
  if(self.delegate && [self.delegate respondsToSelector:@selector(changeCoordinates)]){
    [self.delegate changeCoordinates];
  }
}

#pragma mark - UIWebViewDelegate methods

- (void)webViewDidFinishLoad:(UIWebView *)webView{
  CGRect frame = self.contentView.frame;
  CGSize fittingSize = [self.contentView sizeThatFits:self.contentView.scrollView.contentSize];
  frame.size = fittingSize;
  self.contentView.frame = frame;
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType{
  return YES;
}

#pragma mark - CBCentralManagerDelegate methods

- (void)centralManagerDidUpdateState:(CBCentralManager *)central {
  [self isLECapableHardware];
}
@end
