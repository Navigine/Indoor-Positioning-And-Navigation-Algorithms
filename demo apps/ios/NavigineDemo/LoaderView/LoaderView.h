//
//  LoaderView.h
//  NavigineDemo
//
//  Created by Администратор on 13/01/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Reachability.h"
#import "NavigineManager.h"
#import "LocationTableViewCell.h"
#import "CustomTabBarViewController.h"
#import "MenuTableViewCell.h"
#import "Location.h"
#import "DetailLoaderView.h"
#import "LocationInfo.h"
#import "LoaderHelper.h"

@interface LoaderView : UITableViewController <UITextFieldDelegate, UIAlertViewDelegate, LoaderHelperDelegate>{
  NSString *locationForDownload;
  NSArray *tableData;
  UIView *downloadingView;
  UILabel *downloadingLabel;
  UIActivityIndicatorView *indicator;
  __block BOOL isDownloadingLocation;
}

- (IBAction)btnDownload:(id)sender;
- (void)setLocation :(NSString *)locationForSet;

@end