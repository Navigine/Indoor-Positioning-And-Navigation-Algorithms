//
//  ConsoleViewHelper.h
//  Navigine_Demo
//
//  Created by Администратор on 04/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LoaderHelper.h"

@interface DebugHelper : NSObject

@property (nonatomic, strong) LoaderHelper *loaderHelper;
@property (nonatomic, strong) NSString *saveLogfile;
@property (nonatomic, strong) NSString *navigateLogfile;

+(DebugHelper *) sharedInstance;

@end
