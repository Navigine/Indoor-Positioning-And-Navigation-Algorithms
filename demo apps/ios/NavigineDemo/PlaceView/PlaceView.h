//
//  PlaceView.h
//  SVO
//
//  Created by Valentine on 27.07.14.
//  Copyright (c) 2014 Valentine. All rights reserved.
//

#import <UIKit/UIKit.h>

//#import "MapView.h"
#import "NavigineSDK.h"
#import "NavigineManager.h"

#import "AFNetworking.h"
#import "UIKit+AFNetworking.h"

@protocol MapViewDelegate <NSObject>

//- (CGPoint)getCoordinates;
- (NSString *)showType;
- (Venue *)routeToPlace;
//- (NSString *)getGate;

@end

@interface PlaceView : UIViewController <MapViewDelegate>{

}

+ (PlaceView *)sharedManager;

@property (weak, nonatomic) IBOutlet UIScrollView *sv;

@property (strong,nonatomic) Venue *venues;
@property (strong,nonatomic) NSString *fromType;

@property (weak, nonatomic) IBOutlet UIImageView *image;

@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
@property (weak, nonatomic) IBOutlet UILabel *statusLabel;
@property (weak, nonatomic) IBOutlet UITextView *descriptionLabel;

@property (weak, nonatomic) IBOutlet UIImageView *line;
@property (weak, nonatomic) IBOutlet UILabel *phoneLabel;
@property (weak, nonatomic) IBOutlet UIImageView *line2;
@property (weak, nonatomic) IBOutlet UIButton *callBtn;
@property (weak, nonatomic) IBOutlet UIButton *mapBtn;


@property (weak, nonatomic) IBOutlet UIView *blurView;
@property (weak, nonatomic) IBOutlet UILabel *blurText;
@property (weak, nonatomic) IBOutlet UIButton *blutBtnCancel;
@property (weak, nonatomic) IBOutlet UIButton *blurBtnRoute;

@end
