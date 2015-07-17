//
//  ConsoleView.m
//  Navigine
//
//  Created by Администратор on 18.01.14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import "DebugView.h"

DebugView *sharedConsole = nil;

@interface DebugView (){
  NSTimer *refreshTimer;
  NSString *path;
}
@property (nonatomic, strong) DebugHelper *consoleHelper;
@property (nonatomic, strong) NavigineManager *navigineManager;
@end

@implementation DebugView

- (void)viewDidLoad{
  [super viewDidLoad];
  self.view.backgroundColor = kWhiteColor;
  self.navigationController.navigationBar.barTintColor = kColorFromHex(0x00CDF9);
  self.navigationController.navigationBar.translucent = NO;
  
  CustomTabBarViewController *slide = (CustomTabBarViewController *)self.tabBarController;
  slide.tabBar.hidden = YES;
  
  self.title = @"DEBUG";
  
  [self addLeftButton];
  
  self.consoleHelper = [DebugHelper sharedInstance];
  self.navigineManager = [NavigineManager sharedManager];
  
  [self.searchBtn setImage:[UIImage imageNamed: @"Search.png"] forState:UIControlStateNormal];
  
  UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]
                                 initWithTarget:self
                                 action:@selector(dismissKeyboard)];
  [self.view addGestureRecognizer:tap];
}

- (IBAction)FrequencyChanged:(id)sender{
}

-(void)dismissKeyboard {
  [self.serverTF resignFirstResponder];
  [self.frequencyTF resignFirstResponder];
}

- (void)viewDidUnload{
  [super viewDidUnload];
  
}

- (void)viewWillAppear:(BOOL)animated{
  self.loadLogFile.text = self.consoleHelper.navigateLogfile;
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  path = [[paths[0] stringByAppendingPathComponent:self.navigineManager.location.name] stringByAppendingPathComponent:self.loadLogFile.text];
  [super viewWillAppear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation{
  return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

- (void)viewDidAppear:(BOOL)animated {
  self.saveLogToFile.text = [NSString stringWithFormat:@": %@",self.consoleHelper.saveLogfile];
  [super viewDidAppear:animated];
}

- (void)onTimerCV: (NSTimer *)timer{
  int iConnectionStatusWriteSocket = [self.navigineManager getConnectionStatusWriteSocket];
  if (iConnectionStatusWriteSocket == CONNECTION_STATUS_DISCONNECTED)
    self.connectionStatusLabel.text = @"Disconnected";
  if (iConnectionStatusWriteSocket == CONNECTION_STATUS_CONNECTING)
    self.connectionStatusLabel.text = @"Connecting..";
  if (iConnectionStatusWriteSocket == CONNECTION_STATUS_CONNECTED)
    self.connectionStatusLabel.text = @"Connected";
  
  int iConnectionStatusReadSocket = [self.navigineManager getConnectionStatusReadSocket];
  
  if (iConnectionStatusReadSocket == CONNECTION_STATUS_DISCONNECTED)
    self.connectionStatusReadSocketLabel.text = @"Disconnected";
  if (iConnectionStatusReadSocket == CONNECTION_STATUS_CONNECTING)
    self.connectionStatusReadSocketLabel.text = @"Connecting..";
  if (iConnectionStatusReadSocket == CONNECTION_STATUS_CONNECTED)
    self.connectionStatusReadSocketLabel.text = @"Connected";
  
  [self.navigineManager sendPacket];
  
}

- (IBAction)navigateByLog:(id)sender {
  if([self.loadLogFile.text isEqualToString:@""]){
    [UIAlertView showWithTitle:@"Empty Logfile name" message:nil cancelButtonTitle:@"OK"];
  }
  else{
    if( [self.navigineManager setNavigateByLog :path] != 0){
      [UIAlertView showWithTitle:[NSString stringWithFormat:@"Log File %@ not exists",self.loadLogFile.text] message:nil cancelButtonTitle:@"OK"];
    }
  }
}


-(BOOL) textFieldShouldReturn:(UITextField *)textField{
  [textField resignFirstResponder];
  return YES;
}


- (IBAction)startStopSwitchPressed:(id)sender{
  if (self.swStartStop.on){
    [self startTimer];
    [self startDataSending];
  }
  else
    [self stopDataSending];
}

- (IBAction)deleteLogs:(id)sender {
  [UIAlertView showWithTitle:@"ATTENTION!"
                     message:@"All logs will be deleted!"
           cancelButtonTitle:@"Cancel"
           otherButtonTitles:[NSArray arrayWithObject:@"Delete"]
           andTextFieldStyle:UIAlertViewStyleDefault
            andTextFieldText:nil
                    tapBlock:^(UIAlertView *alertView, NSInteger buttonIndex) {
                      switch (buttonIndex) {
                        case 0:
                          break;
                        case 1:
                          self.consoleHelper.navigateLogfile = @"";
                          self.loadLogFile.text = self.consoleHelper.navigateLogfile;
                          self.consoleHelper.saveLogfile = self.navigineManager.location.name;
                          self.saveLogToFile.text = [NSString stringWithFormat:@": %@.log",self.consoleHelper.saveLogfile];
                          [self.navigineManager deleteLogs];
                          break;
                      }
                    }];
}

- (IBAction)saveLogSwitchPressed:(id)sender {
  if([self.consoleHelper.saveLogfile isEqualToString:@""]){
    [UIAlertView showWithTitle:@"Set location" message:@"" cancelButtonTitle:@"OK"];
  }
  else{
    if(self.swSaveLog.on){
      self.consoleHelper.saveLogfile = [self.navigineManager startSaveLogToFile];
      self.saveLogToFile.text = [NSString stringWithFormat:@": %@",self.consoleHelper.saveLogfile];
    }
    else{
      [self.navigineManager stopSaveLogToFile];
    }
  }
}


- (void)stopDataSending{
  [self.navigineManager setConnectionStatus:CONNECTION_STATUS_DISCONNECTED];
}

- (void)startDataSending{
  [self.navigineManager setServer:[self.serverTF.text UTF8String] andPort:SERVER_DEFAULT_OUTPUT_PORT];
  [self.navigineManager setConnectionStatus:CONNECTION_STATUS_CONNECTED];
  [self.navigineManager launchNavigineSocketThreads :[self.serverTF.text UTF8String]: SERVER_DEFAULT_OUTPUT_PORT];
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



-(IBAction)textFieldReturn:(id)sender{
  [sender resignFirstResponder];
}


- (void) startTimer{
  if (refreshTimer==nil) {
    refreshTimer = [NSTimer scheduledTimerWithTimeInterval:0.1
                                                    target:self
                                                  selector:@selector(onTimerCV:)
                                                  userInfo:nil
                                                   repeats:YES];
  }
  
}



@end
