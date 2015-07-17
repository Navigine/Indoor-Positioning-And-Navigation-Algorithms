//
//  LoaderInfo.h
//  Navigine_Demo
//
//  Created by Администратор on 21/05/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Location.h"

@interface LocationInfo : NSObject
@property (nonatomic, strong) Location *location;
@property (nonatomic) BOOL isSet;
@property (nonatomic) NSInteger serverVersion;
@property (nonatomic, strong) NSString *loadingProcess;
@end
