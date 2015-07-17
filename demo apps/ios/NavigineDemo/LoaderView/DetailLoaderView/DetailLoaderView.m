//
//  DetailLoaderView.m
//  NavigineDemo
//
//  Created by Администратор on 13/01/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "DetailLoaderView.h"

@implementation DetailLoaderView

-(void)viewDidLoad{
  [super viewDidLoad];
  self.txtLocationName.text = [NSString stringWithFormat:@"Name: %@", self.location.name];
  self.txtLocationId.text = [NSString stringWithFormat:@"Id: %zd",self.location.id];
  self.txtLocalVersion.text = [NSString stringWithFormat:@"Local version: %zd", self.location.version];
  self.txtServerVersion.text = [NSString stringWithFormat:@"Server version: %zd", self.serverVersion];
  NSMutableString *sublocText = [[NSMutableString alloc] initWithString:@""];
  self.txtSublocations.numberOfLines = self.location.subLocations.count;
  for(int i = 0; i < self.location.subLocations.count; i++){
    if(i > 0) [sublocText appendString:@"\n"];
    Sublocation *subloc  = self.location.subLocations[i];
    NSString *text = [NSString stringWithFormat: @"%02zd) Name: %@ Id: %zd", i+1, subloc.name, subloc.id];
    [sublocText appendString:text];
  }
  self.txtSublocations.text = [NSString stringWithString:sublocText];
  [self.txtSublocations sizeToFit];
}

- (IBAction)btnSetLocation:(id)sender {
//  [loaderView saveLocationForSet:self.location.name];
//  if(self.delegate && [self.delegate respondsToSelector:@selector(setLocation:)]){
//    [self.delegate setLocation:self.location.name];
//  }
  [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)btnDeleteLocation:(id)sender {
//  [loaderView.navigineManager stopNavigine];
//  [loaderView deleteLocation:self.location.name];
  [self.navigationController popViewControllerAnimated:YES];
}

@end