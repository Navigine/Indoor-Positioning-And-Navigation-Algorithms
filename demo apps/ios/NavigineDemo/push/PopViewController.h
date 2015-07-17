//
//  PopViewController.h
//  SVO
//
//  Created by Valentine on 02.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PopViewController : UIViewController

@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet UILabel *infoLabel;

@property (weak, nonatomic) IBOutlet UIButton *confirmBtn;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UIImageView *back;
@property (weak, nonatomic) IBOutlet UIImageView *line;
@property (weak, nonatomic) IBOutlet UIImage *image;

@property (nonatomic,strong) NSString *pushImage;
@property (nonatomic,strong) NSString *pushTitle;
@property (nonatomic,strong) NSString *pushContent;


@end
