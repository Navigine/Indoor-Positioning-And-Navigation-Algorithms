//
//  PopViewController.m
//  SVO
//
//  Created by Valentine on 02.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import "PopViewController.h"
#import "PopTextViewController.h"
#define kColorWithAlpha(color, alpha) [color colorWithAlphaComponent:(alpha<0.0f?0.0f:(alpha>1.0f?1.0f:alpha))]
@interface PopViewController ()

@end

@implementation PopViewController

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
    

    
    
    _titleLabel.font =  [UIFont fontWithName:@"Intro-Light" size:20.0f];
    _infoLabel.font  =  [UIFont fontWithName:@"Intro-Light" size:16.0f];
    
    
    _titleLabel.top   = self.view.bottom;
    _titleLabel.alpha = 0;
    
    _line.top         = self.view.bottom;
    _line.alpha       = 0;
    
    _infoLabel.top  = self.view.bottom;
    _infoLabel.alpha = 0;
    
    _confirmBtn.top = self.view.bottom;
    _confirmBtn.alpha =0;
    
    _cancelBtn.top  = self.view.bottom;
    _cancelBtn.alpha = 0;
    
}

- (void)viewDidAppear:(BOOL)animated {
    
    UIImage* imageOfUnderlyingView = [self convertViewToImage];
    imageOfUnderlyingView = [imageOfUnderlyingView applyBlurWithRadius:20
                                                             tintColor:kColorWithAlpha(kBlackColor, 0.5)
                                                 saturationDeltaFactor:0.3
                                                             maskImage:nil];
    
    _back.image = imageOfUnderlyingView;
    [UIView animateWithDuration:0.3 animations:^{
        _back.alpha = 1;
    }];
    
    [UIView animateWithDuration:0.3 delay:0.3 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _titleLabel.top   = 195.0;
        _titleLabel.alpha = 1;
       
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0.4 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _line.top   = 256.0;
        _line.alpha = 1.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0.4 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _infoLabel.top   = 271.0;
        _infoLabel.alpha = 1.0;
    } completion:^(BOOL finished) {
        
    }];
    
    
    [UIView animateWithDuration:0.2 delay:0.5 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _confirmBtn.top   = 310.0;
        _confirmBtn.alpha = 1.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0.6 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _cancelBtn.top   = 310.0;
        _cancelBtn.alpha = 1.0;
    } completion:^(BOOL finished) {
        
    }];
    
 
    [super viewDidAppear:YES];
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)confirmPressed:(id)sender {
   
}

- (IBAction)cancelPressed:(id)sender {
    
    
    
    
    [UIView animateWithDuration:0.3 delay:0.3 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _titleLabel.top   = self.view.bottom;
        _titleLabel.alpha = 0;
        
    } completion:^(BOOL finished) {
        [UIView animateWithDuration:0.3 animations:^{
            _back.alpha = 0;
        } completion:^(BOOL finished) {
            [self dismissViewControllerAnimated:NO completion:nil];
        }];
    }];
    
    [UIView animateWithDuration:0.2 delay:0.3 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _line.top   = self.view.bottom;
        _line.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0.3 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _infoLabel.top   = self.view.bottom;
        _infoLabel.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    
    [UIView animateWithDuration:0.2 delay:0.1 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _confirmBtn.top   = self.view.bottom;
        _confirmBtn.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _cancelBtn.top   = self.view.bottom;
        _cancelBtn.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
   
}


- (UIImage *)convertViewToImage {
    
    CGRect windowBounds = self.view.window.bounds;
    
    UIGraphicsBeginImageContextWithOptions(windowBounds.size, YES, 0.0);
    
    [self.view.window drawViewHierarchyInRect:windowBounds afterScreenUpdates:YES];
    UIImage *snapshot = UIGraphicsGetImageFromCurrentImageContext();
    
    UIGraphicsEndImageContext();

    
    UIGraphicsBeginImageContext(self.navigationController.view.bounds.size);
    [self.navigationController.view drawViewHierarchyInRect:self.navigationController.view.bounds afterScreenUpdates:YES];
    UIGraphicsEndImageContext();
    
    return snapshot;
}



#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    
    if([segue.identifier isEqualToString:@"toPop"]) {
        //NSLog(@"pop %@",_action.title);
        PopTextViewController * pop = [segue destinationViewController];
        pop.pushTitle = _pushTitle;
        pop.pushContent = _pushContent;
        pop.pushImage = _pushImage;
    }
}


@end
