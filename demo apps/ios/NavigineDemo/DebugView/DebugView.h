//
//  ConsoleView.h
//  Navigine
//
//  Created by Администратор on 18.01.14.
//  Copyright (c) 2014 Navigine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DebugHelper.h"
#import "CustomTabBarViewController.h"

#define CONNECTION_STATUS_DISCONNECTED 0
#define CONNECTION_STATUS_CONNECTING   1
#define CONNECTION_STATUS_CONNECTED    2
#define SERVER_DEFAULT_INPUT_PORT      27016
#define SERVER_DEFAULT_OUTPUT_PORT     27015

@interface DebugView : UIViewController <UITextFieldDelegate>{
}

@property (weak, nonatomic) IBOutlet UITextField *serverTF;
@property (weak, nonatomic) IBOutlet UITextField *frequencyTF;
@property (weak, nonatomic) IBOutlet UILabel *connectionStatusReadSocketLabel;
@property (weak, nonatomic) IBOutlet UILabel *connectionStatusLabel;
@property (weak, nonatomic) IBOutlet UISwitch *swStartStop;
@property (weak, nonatomic) IBOutlet UISwitch *swSaveLog;
@property (weak, nonatomic) IBOutlet UILabel *saveLogToFile;

@property (weak, nonatomic) IBOutlet UILabel *loadLogFile;
@property (weak, nonatomic) IBOutlet UIButton *searchBtn;

- (IBAction)startStopSwitchPressed:(id)sender;
- (IBAction)deleteLogs:(id)sender;
- (IBAction)saveLogSwitchPressed:(id)sender;
- (IBAction)textFieldReturn:(id)sender;
- (IBAction)navigateByLog:(id)sender;

@end