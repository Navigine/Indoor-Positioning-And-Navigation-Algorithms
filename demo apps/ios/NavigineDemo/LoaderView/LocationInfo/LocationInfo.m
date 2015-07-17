//
//  LoaderInfo.m
//  Navigine_Demo
//
//  Created by Администратор on 21/05/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "LocationInfo.h"

@implementation LocationInfo

-(id) init{
  self = [super init];
  if(self){
    self.isSet = NO;
    self.serverVersion = 0;
    self.loadingProcess = 0;
    self.location = [Location new];
  }
  return self;
}

@end
