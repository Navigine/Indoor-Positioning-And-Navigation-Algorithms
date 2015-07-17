//
//  MenuViewController.h
//  SVO
//
//  Created by Valentine on 17.06.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MenuViewController : UIViewController {
    NSMutableArray *menuArray;
}

@property (weak, nonatomic) IBOutlet UITableView *tv;

@end
