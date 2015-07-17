//
//  DetailLoaderView.h
//  NavigineDemo
//
//  Created by Администратор on 13/01/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//
#import "LoaderView.h"

@protocol DetailLoaderViewDelegate <NSObject>
-(void)setLocation :(NSString *)name;
-(void)deleteLocation :(NSString *)name;
@end

@interface DetailLoaderView :UIViewController{
}
@property (nonatomic, weak) NSObject <DetailLoaderViewDelegate> *delegate;

@property (nonatomic, weak) Location *location;
@property (nonatomic, assign) NSInteger serverVersion;

@property (weak, nonatomic) IBOutlet UILabel *txtLocationName;
@property (weak, nonatomic) IBOutlet UILabel *txtLocationId;

@property (weak, nonatomic) IBOutlet UILabel *txtLocalVersion;

@property (weak, nonatomic) IBOutlet UILabel *txtServerVersion;

@property (weak, nonatomic) IBOutlet UILabel *txtSublocations;

- (IBAction)btnSetLocation:(id)sender;
- (IBAction)btnDeleteLocation:(id)sender;

@end