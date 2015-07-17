//
//  PopTextViewController.m
//  SVO
//
//  Created by Valentine on 03.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import "PopTextViewController.h"

@interface PopTextViewController ()

@end

@implementation PopTextViewController

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
    
    _titleLabel.text = _pushTitle;
    _textLabel.text  = _pushContent;
    _image.alpha = 0;
    
    [_image setImageWithURLRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:_pushImage]]
                  placeholderImage:nil
                  success:^(NSURLRequest *request, NSHTTPURLResponse *response, UIImage *image) {
                          _image.image = image;
        
                          [UIView animateWithDuration:1.0 animations:^{
                              _image.alpha = 1;
                          }];
        
                  } failure:^(NSURLRequest *request, NSHTTPURLResponse *response, NSError *error) {
        
    }];
    
    _titleLabel.font = [UIFont fontWithName:@"Intro-Regular" size:18.0f];
    _titleLabel.text = _titleLabel.text.uppercaseString;
    
    _textLabel.font = [UIFont fontWithName:@"Intro-Regular" size:13.0f];
    
    [_textLabel sizeToFit];
    _textLabel.textAlignment = NSTextAlignmentCenter;
    
    _textLabel.centerX = self.view.centerX;
    _btnConfirm.top = _textLabel.bottom;
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)backPressed:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)scrollViewDidScroll:(UIScrollView *)scroll {
    
    CGFloat y = scroll.contentOffset.y;
    
    if(y<0) {
        y=y*(-1);
        _image.frame = CGRectMake(0, 0-y, _image.width, 205+y);
    }
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
