//
//  PlaceView.m
//  SVO
//
//  Created by Valentine on 27.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import "PlaceView.h"

NSString *type;
Venue *venues;

static PlaceView * sharedManager;


@interface PlaceView ()

@end

@implementation PlaceView

+ (PlaceView *)sharedManager {
    
    if (nil != sharedManager) {
        return sharedManager;
    }
    
    static dispatch_once_t pred;        // Lock
    dispatch_once(&pred, ^{             // This code is called at most once per app
        sharedManager = [super allocWithZone:nil];
        
    });
    
    return sharedManager;
}

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
    
    self.view.backgroundColor = kColorFromHex(0x31363d);
    
    _nameLabel.text = _venues.name.uppercaseString;
    _nameLabel.font = [UIFont fontWithName:@"Intro-Book" size:20.0f];

    _statusLabel.text = @"Open";
    _statusLabel.textColor = kColorFromHex(0x9CE852);
    _statusLabel.font = [UIFont fontWithName:@"Intro-Book" size:12.0f];
    
    _descriptionLabel.text = _venues.descript;
    _descriptionLabel.font = [UIFont fontWithName:@"Intro-Book" size:12.0f];
    [_descriptionLabel sizeToFit];
    
    _callBtn.titleLabel.font = [UIFont fontWithName:@"Intro-Book" size:12.0f];
    _mapBtn.titleLabel.font  = [UIFont fontWithName:@"Intro-Book" size:14.0f];
    
    _line.top  = _descriptionLabel.bottom + 13.0f;
    
    _phoneLabel.text = _venues.phone;
    _phoneLabel.font = [UIFont fontWithName:@"Intro-Book" size:18.0f];
    
    _phoneLabel.top = _line.bottom + 13.0f;
    _line2.top      = _phoneLabel.bottom + 13.0f;
    _callBtn.top = _line.bottom + 8.0f;

    _mapBtn.top = _line2.bottom + 25.0f;
    
    
    _image.alpha = 0.0f;
    
    [_image setImageWithURLRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:_venues.image]] placeholderImage:nil
                               success:^(NSURLRequest *request, NSHTTPURLResponse *response, UIImage *image) {
                                   
                                   _image.image = image;
                                 
                                   [UIView animateWithDuration:0.5 animations:^{
                                       _image.alpha = 1;
                                   }];
                                   
    }
    failure:^(NSURLRequest *request, NSHTTPURLResponse *response, NSError *error) {
                                   //NSLog(@"Image error: %@", error);
    }];

}


- (void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.navigationController setNavigationBarHidden:YES animated:animated];
    
    [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleDefault animated:YES];
    
}


- (void) viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.navigationController setNavigationBarHidden:NO animated:animated];
    
   [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleLightContent animated:YES];
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (Venue *)routeToPlace {
    return  venues;
}

- (NSString *)showType {
    return type;
}


- (void)scrollViewDidScroll:(UIScrollView *)scroll {
    
    CGFloat y = scroll.contentOffset.y;
    
    if(y<0) {
        y = y*(-1);
        _image.frame = CGRectMake(0, 0 - y, _image.width, 160.0f + y);
    }
}

- (IBAction)backPressed:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)callPressed:(id)sender {
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"telprompt://%@",_venues.phone]];
    [[UIApplication sharedApplication] openURL:url];
}

- (IBAction)routePressed:(id)sender {
    
    
    [self showConfirmationView];


}

- (IBAction)showPressed:(id)sender {/*
    CustomTabBarViewController *slide = (CustomTabBarViewController *)self.tabBarController;
    [slide setMapWithDelegate:self];*/
    type = @"show";
 
    if([_fromType isEqualToString:@"map"]) {
        venues = _venues;
        [self dismissViewControllerAnimated:YES completion:nil];
    }
    
}



- (void)showConfirmationView {
    
    _blurText.top        = self.view.bottom;
    _blurText.alpha      = 0;
    _blurBtnRoute.top    = self.view.bottom;
    _blurBtnRoute.alpha  = 0;
    _blutBtnCancel.top   = self.view.bottom;
    _blutBtnCancel.alpha = 0;
    _blurView.height     = 568.0f;
    _blurView.hidden     = NO;
    
    [self.navigationController.view insertSubview:_blurView aboveSubview:self.navigationController.view];
    
    UIImage *imageOfUnderlyingView = [self convertViewToImage];
    imageOfUnderlyingView = [imageOfUnderlyingView applyBlurWithRadius:20
                                                             tintColor:[UIColor colorWithWhite:1.0 alpha:0.2]
                                                 saturationDeltaFactor:1.3
                                                             maskImage:nil];
    
    _blurView.backgroundColor = [UIColor colorWithPatternImage:imageOfUnderlyingView];
    
    [UIView animateWithDuration:0.3 animations:^{
        _blurView.alpha = 1;
    } completion:^(BOOL finished) {
        [UIView animateWithDuration:0.3 delay:0 options:UIViewAnimationOptionCurveEaseOut animations:^{
            _blurText.top   = 120.0;
            _blurText.alpha = 1.0;
        } completion:^(BOOL finished) {
            
        }];
        
        [UIView animateWithDuration:0.2 delay:0.1 options:UIViewAnimationOptionCurveEaseOut animations:^{
            _blurBtnRoute.top   = 197.0;
            _blurBtnRoute.alpha = 1.0;
        } completion:^(BOOL finished) {
            
        }];
        
        [UIView animateWithDuration:0.2 delay:0.2 options:UIViewAnimationOptionCurveEaseOut animations:^{
            _blutBtnCancel.top   = 346.0;
            _blutBtnCancel.alpha = 1.0;
        } completion:^(BOOL finished) {
            
        }];
        
        
    }];
    
    
    
}

- (IBAction)makRoutePressed:(id)sender {
    
    [UIView animateWithDuration:0.3 delay:0.1 options:UIViewAnimationOptionCurveEaseIn animations:^{
        _blurText.top   = self.view.bottom;
        _blurText.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0.1 options:UIViewAnimationOptionCurveEaseIn animations:^{
        _blurBtnRoute.top   = self.view.bottom;
        _blurBtnRoute.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0 options:UIViewAnimationOptionCurveEaseIn animations:^{
        _blutBtnCancel.top   = self.view.bottom;
        _blutBtnCancel.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    
    [UIView animateWithDuration:0.3 delay:0.5 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _blurView.alpha = 0;
    } completion:^(BOOL finished) {
        _blurView.hidden = NO;
        //CustomTabBarViewController *slide = (CustomTabBarViewController *)self.tabBarController;
        //[slide setMapWithDelegate:self];
        type = @"route";
        
        if([_fromType isEqualToString:@"map"]) {
            venues = _venues;
            [self dismissViewControllerAnimated:YES completion:nil];
            //[self.navigationController popViewControllerAnimated:YES];
        }
    }];
    
    
    
    
    
}

- (IBAction)cancelPressed:(id)sender {
    
    
    [UIView animateWithDuration:0.3 delay:0.1 options:UIViewAnimationOptionCurveEaseIn animations:^{
        _blurText.top   = self.view.bottom;
        _blurText.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0.1 options:UIViewAnimationOptionCurveEaseIn animations:^{
        _blurBtnRoute.top   = self.view.bottom;
        _blurBtnRoute.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    [UIView animateWithDuration:0.2 delay:0 options:UIViewAnimationOptionCurveEaseIn animations:^{
        _blutBtnCancel.top   = self.view.bottom;
        _blutBtnCancel.alpha = 0.0;
    } completion:^(BOOL finished) {
        
    }];
    
    
    
    [UIView animateWithDuration:0.3 delay:0.5 options:UIViewAnimationOptionCurveEaseOut animations:^{
        _blurView.alpha = 0;
    } completion:^(BOOL finished) {
        _blurView.hidden = NO;
    }];
}

- (UIImage *)convertViewToImage {
    UIGraphicsBeginImageContext(self.navigationController.view.bounds.size);
    [self.navigationController.view drawViewHierarchyInRect:self.navigationController.view.bounds afterScreenUpdates:YES];
    UIImage *image2 = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image2;
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
