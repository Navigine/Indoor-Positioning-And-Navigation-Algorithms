//
//  ConsoleViewHelper.m
//  Navigine_Demo
//
//  Created by Администратор on 04/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "DebugHelper.h"

@interface DebugHelper(){

}

@end

@implementation DebugHelper

+(DebugHelper *) sharedInstance{
  static DebugHelper * _sharedInstance = nil;
  
  static dispatch_once_t oncePredicate;
  dispatch_once(&oncePredicate, ^{
    _sharedInstance = [[DebugHelper alloc] init];
  });
  return _sharedInstance;
}

- (instancetype) init{
  self = [super init];
  if(self){
    self.loaderHelper = [LoaderHelper sharedInstance];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(setNewLocation:)
                                                 name:@"setLocation"
                                               object:nil];
    self.saveLogfile = [NSString string];
    self.navigateLogfile = [NSString string];
  }
  return self;
}

-(void)setNewLocation:(NSNotification *)notification{
  NSDictionary *index = notification.userInfo;
  self.saveLogfile = [NSString stringWithFormat:@"%@.log", index[@"locationName"]];
}

@end
