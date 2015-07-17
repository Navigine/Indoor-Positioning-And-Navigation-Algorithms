//
//  LocationTableViewCell.h
//  SVO
//
//  Created by Administrator on 17.06.14.
//  Copyright (c) 2014 Administrator. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LocationTableViewCell : UITableViewCell

@property (weak, nonatomic) IBOutlet UILabel *loadProcess;
@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet UIButton *btnDownload;
@property (weak, nonatomic) IBOutlet UILabel *serverVersion;

@end
