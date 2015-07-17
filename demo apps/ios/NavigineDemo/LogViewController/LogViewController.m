//
//  LogViewController.m
//  Navigine_Demo
//
//  Created by Администратор on 19/06/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "LogViewController.h"

@interface LogViewController(){
  NSArray *filelist;
  NSFileManager *filemgr;
}
@property (nonatomic, strong) NavigineManager *navigineManager;
@property (nonatomic, strong) DebugHelper *debugHelper;

@end

@implementation LogViewController

- (id)initWithStyle:(UITableViewStyle)style
{
  self = [super initWithStyle:style];
  if (self) {
    // Custom initialization
  }
  return self;
}

-(void)viewDidLoad{
  [super viewDidLoad];
  self.navigineManager = [NavigineManager sharedManager];
  self.debugHelper = [DebugHelper sharedInstance];
  filelist = [NSArray array];
  filemgr = [NSFileManager defaultManager];
}


- (void)viewDidAppear:(BOOL)animated {
  [super viewDidAppear:animated];
}

- (void)viewWillAppear:(BOOL)animated {
  self.navigationItem.title = self.navigineManager.location.name;
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *path = [paths[0] stringByAppendingPathComponent:self.navigineManager.location.name];
  filelist = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:nil];
  [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
  [super viewWillDisappear:animated];
}

#pragma mark - UITableViewDelegate

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
  // Return the number of sections.
  if (filelist.count > 0) {
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
    return 1;
//
  } else {
    // Display a message when the table is empty
    UILabel *messageLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, self.view.bounds.size.width, self.view.bounds.size.height)];
    
    messageLabel.text = @"No data is currently available.\nPlease enter User Hash and download locations list";
    messageLabel.textColor = [UIColor blackColor];
    messageLabel.numberOfLines = 0;
    messageLabel.textAlignment = NSTextAlignmentCenter;
    [messageLabel sizeToFit];
    
    self.tableView.scrollEnabled = NO;
    self.tableView.backgroundView = messageLabel;
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
  }
  return 0;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  return [filelist count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
  static NSString *CellIdentifier = @"logCell";
  
  LogViewCell *cell = (LogViewCell *)[tableView dequeueReusableCellWithIdentifier: CellIdentifier
                                                                    forIndexPath :indexPath];
  cell.logfile.text = filelist[indexPath.row];
  
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *path = [[paths[0] stringByAppendingPathComponent:self.navigineManager.location.name] stringByAppendingPathComponent:filelist[indexPath.row]];
  NSDictionary *fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:nil];
  cell.filesize.text = [NSString stringWithFormat:@"%.1fKB", [[fileAttributes objectForKey:NSFileSize] longLongValue]/1024.];
  
  return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
  NSString *logfile = filelist[indexPath.row];
  if(![logfile hasSuffix:@"zip"]){
    self.debugHelper.navigateLogfile = filelist[indexPath.row];
    [self.navigationController popViewControllerAnimated:YES];
  }
}

@end
