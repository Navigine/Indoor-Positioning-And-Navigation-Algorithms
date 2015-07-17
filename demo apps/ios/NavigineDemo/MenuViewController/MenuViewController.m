//
//  MenuViewController.m
//  SVO
//
//  Created by Valentine on 17.06.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import "MenuViewController.h"
#import "MenuTableViewCell.h"


@interface MenuViewController ()

@end

@implementation MenuViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.view.backgroundColor = kWhiteColor;
  
    menuArray = [[NSMutableArray alloc] initWithObjects:@"LOCATION",@"MAP",@"CONSOLE",@"DEBUG", nil];
    
    [_tv selectRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0] animated:NO scrollPosition:UITableViewScrollPositionNone];
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    
    return menuArray.count;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *CellIdentifier = @"cell";
    
    MenuTableViewCell *cell = (MenuTableViewCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
   
    cell.titleLabel.font  = [UIFont fontWithName:@"Intro-Light" size:18.0f];
    cell.titleLabel.textColor = kBlackColor;
    cell.titleLabel.highlightedTextColor = kGrayColor;
  
    cell.titleLabel.text = menuArray[indexPath.row];
    cell.titleLabel.text = cell.titleLabel.text.uppercaseString;
    
    UIView *myBackView = [[UIView alloc] initWithFrame:cell.frame];
    myBackView.backgroundColor = [UIColor clearColor];
    cell.selectedBackgroundView = myBackView;
    
    return cell;
}



- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (indexPath.row > 3) {
        return;
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"menuItemPressed" object:nil userInfo:@{@"index": indexPath}];
}

@end
