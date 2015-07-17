//
//  MapViewHelper.h
//  Navigine_Demo
//
//  Created by Администратор on 04/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "LoaderHelper.h"
#import "TBXML.h"

@protocol MapHelperDelegate;

@interface MapHelper : NSObject <UIWebViewDelegate, CBCentralManagerDelegate>

@property (nonatomic, weak) id <MapHelperDelegate> delegate;
@property (nonatomic, strong) NSMutableArray *webViewArray;
@property (nonatomic, strong) NSMutableDictionary *sublocId;
@property (nonatomic, strong) UIWebView *contentView;

+ (MapHelper *)sharedInstance;
@end

@protocol MapHelperDelegate <NSObject>
- (void) startNavigation;
- (void) stopNavigation;
- (void) changeCoordinates;
@end
