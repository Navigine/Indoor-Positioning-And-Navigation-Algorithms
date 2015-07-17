//
//  PopTextViewController.h
//  SVO
//
//  Created by Valentine on 03.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AFNetworking.h"
#import "UIKit+AFNetworking.h"

@interface PopTextViewController : UIViewController


@property (nonatomic,strong) NSString *pushImage;
@property (nonatomic,strong) NSString *pushTitle;
@property (nonatomic,strong) NSString  *pushContent;


@property (weak, nonatomic) IBOutlet UIImageView *image;
@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet UITextView *textLabel;
@property (weak, nonatomic) IBOutlet UIButton *btnConfirm;

@end
