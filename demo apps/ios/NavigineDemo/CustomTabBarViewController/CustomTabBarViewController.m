//
//  CustomTabBarViewController.m
//  SVO
//
//  Created by Valentine on 03.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import "CustomTabBarViewController.h"
#import "NavigineManager.h"
#import "MapHelper.h"
#import "LoaderHelper.h"
#import "DebugHelper.h"


@interface CustomTabBarViewController (){
  MapHelper       *mapHelper;
  NavigineManager *navigineManager;
  LoaderHelper    *loaderHelper;
  DebugHelper     *debugHelper;
}

@end

@implementation CustomTabBarViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil{
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
  if (self) {
    // Custom initialization
  }
  return self;
}

- (void)viewDidLoad{
  [super viewDidLoad];
  // Do any additional setup after loading the view.
  
  
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(menuItemPressed:)
                                               name:@"menuItemPressed"
                                             object:nil];
  
  self.hidesBottomBarWhenPushed  = YES;
  self.tabBar.hidden             = YES;
  
  navigineManager = [NavigineManager sharedManager];
  mapHelper       = [MapHelper      sharedInstance];
  loaderHelper    = [LoaderHelper   sharedInstance];
  debugHelper     = [DebugHelper    sharedInstance];
}

- (void)didReceiveMemoryWarning{
  [super didReceiveMemoryWarning];
  // Dispose of any resources that can be recreated.
}


- (void)menuItemPressed:(NSNotification *)notification {
  
  NSDictionary *index = notification.userInfo;
  NSIndexPath *path = index[@"index"];
  
  [self setSelectedIndex:path.row];
  
  [self.slidingPanelController closePanel];
}

- (void)setMapLoaderHelper:(id)delegate {
  
  //    UINavigationController *nav = (UINavigationController *)self.viewControllers[1];
  //    MapViewController      *map = (MapViewController *)nav.viewControllers[0];
  
  
  //
//  [map setLoaderHelperDelegate:delegate];
  //
  //    [self setSelectedViewController:nav];
  
}

- (void)setLocation {
  UINavigationController *nav = (UINavigationController *)self.viewControllers[0];
  [self setSelectedViewController:nav];
}



/*
 #pragma mark - Navigation
 
 // In a storyboard-based application, you will often want to do a little preparation before navigation
 - (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
 {
 // Get the new view controller using [segue destinationViewController].
 // Pass the selected object to the new view controller.
 }
 */

@end
