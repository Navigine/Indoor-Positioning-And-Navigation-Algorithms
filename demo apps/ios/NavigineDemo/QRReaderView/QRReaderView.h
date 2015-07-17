//
//  QRReaderView.h
//  NavigineDemo
//
//  Created by Администратор on 04/02/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#ifndef NavigineDemo_QRReaderView_h
#define NavigineDemo_QRReaderView_h

#endif

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "ZBarSDK.h"

@interface QRReaderView : UIViewController <ZBarReaderDelegate>{
}

@property (nonatomic, retain) IBOutlet UIImageView *resultImage;
//@property (nonatomic, retain) IBOutlet UITextView *resultText;
- (IBAction)scanButtonTapped;

@end