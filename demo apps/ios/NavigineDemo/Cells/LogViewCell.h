//
//  LogViewCell.h
//  Navigine_Demo
//
//  Created by Администратор on 19/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LogViewCell : UITableViewCell

@property (nonatomic, weak) IBOutlet UILabel* logfile;
@property (nonatomic, weak) IBOutlet UILabel* filesize;

@end
