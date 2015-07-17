//
//  LoaderView.m
//  NavigineDemo
//
//  Created by Администратор on 13/01/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#import "LoaderView.h"


@interface LoaderView(){
  NSInteger isSetNewLocation;
}

@property (nonatomic,strong) LoaderHelper *loaderHelper;
@end


@implementation LoaderView

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
  
  self.view.backgroundColor = kColorFromHex(0xd2fffc);
  self.navigationController.navigationBar.barTintColor = kColorFromHex(0x00CDF9);
  self.navigationController.navigationBar.translucent = YES;
  self.tableView.backgroundColor = kColorFromHex(0xd2fffc);
  
  
  [self addLeftButton];
  [self addRightButton];
  [self addCenterButton];
  
  isSetNewLocation = 0;

  
  //initialize loader helper
  self.loaderHelper = [LoaderHelper sharedInstance];
  self.loaderHelper.loaderDelegate = self;
  CustomTabBarViewController *slide = (CustomTabBarViewController *)self.tabBarController;
  [slide setMapLoaderHelper:self];
  
  downloadingView = [[UIView alloc] initWithFrame:self.view.frame];
  [self.view addSubview:downloadingView];
  downloadingView.hidden = YES;
  
  indicator = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(downloadingView.centerX-27 ,200, 54, 54)];
  
  indicator.backgroundColor = kClearColor;
  indicator.color = kBlackColor;
  [indicator startAnimating];
  indicator.hidden = NO;
  
  downloadingLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, downloadingView.width, 54)];
  downloadingLabel.font = [UIFont fontWithName:@"Intro-Light" size:22.0f];
  downloadingLabel.text = @"";
  downloadingLabel.textAlignment = NSTextAlignmentCenter;
  downloadingLabel.top = indicator.bottom;
  
  isSetNewLocation = 0;
  isDownloadingLocation = NO;
  
  UILongPressGestureRecognizer *lpgr = [[UILongPressGestureRecognizer alloc]
                                        initWithTarget:self action:@selector(longPress:)];
  lpgr.minimumPressDuration = 1.0;
  [self.tableView addGestureRecognizer:lpgr];
  
  self.refreshControl = [[UIRefreshControl alloc] init];
  self.refreshControl.backgroundColor = [UIColor clearColor];
  self.refreshControl.tintColor = kBlackColor;
  [self.refreshControl addTarget:self
                          action:@selector(getLatestVersion)
                forControlEvents:UIControlEventValueChanged];
  

  [self.loaderHelper parseLocationList];
  [self setLocation:@""];
  [self.tableView reloadData];
}

-(void) viewWillAppear:(BOOL)animated{
  [super viewWillAppear:animated];
}

-(BOOL) textFieldShouldReturn:(UITextField *)textField{
  [textField resignFirstResponder];
  return YES;
}


- (void)addRightButton {
  /*
   
   UIImage *buttonImage = [UIImage imageNamed:@"btnAdd"];
   UIButton *aButton = [UIButton buttonWithType:UIButtonTypeCustom];
   [aButton setBackgroundImage:buttonImage forState:UIControlStateNormal];
   aButton.frame = CGRectMake(0.0, 0.0, buttonImage.size.width, buttonImage.size.height);
   UIBarButtonItem *aBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:aButton];
   [aButton addTarget:self action:@selector(addPressed:) forControlEvents:UIControlEventTouchUpInside];
   
   UIBarButtonItem *negativeSpacer = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
   [negativeSpacer setWidth:-17];
   
   [self.navigationItem setRightBarButtonItems:[NSArray arrayWithObjects:negativeSpacer,aBarButtonItem,nil] animated:YES];
   */
  return;
  
}

- (void)addLeftButton {
  
  UIImage *buttonImage = [UIImage imageNamed:@"btnMenu"];
  UIButton *leftButton = [UIButton buttonWithType:UIButtonTypeCustom];
  [leftButton setBackgroundImage:buttonImage forState:UIControlStateNormal];
  leftButton.frame = CGRectMake(0.0, 0.0, buttonImage.size.width,   buttonImage.size.height);
  UIBarButtonItem *aBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:leftButton];
  [leftButton addTarget:self action:@selector(menuPressed:)  forControlEvents:UIControlEventTouchUpInside];
  
  UIBarButtonItem *negativeSpacer = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
  [negativeSpacer setWidth:-17];
  
  [self.navigationItem setLeftBarButtonItems:[NSArray arrayWithObjects:negativeSpacer,aBarButtonItem,nil] animated:YES];
}

- (void)addCenterButton {
  
  UIView *buttonContainer = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 44)];
  buttonContainer.backgroundColor = [UIColor clearColor];
  
  NSString *loginText = @"USER HASH";
  UIButton *centerButton = [UIButton buttonWithType:UIButtonTypeCustom];
  [centerButton setFrame:CGRectMake(0, 0, 200, 44)];
  [centerButton setTitle:loginText forState:UIControlStateNormal];
  [centerButton setTitle:loginText forState:UIControlStateSelected];
  
  [centerButton addTarget:self action:@selector(userHashPressed:)  forControlEvents:UIControlEventTouchUpInside];
  
  [centerButton setShowsTouchWhenHighlighted:YES];
  [buttonContainer addSubview:centerButton];
  
  self.navigationItem.titleView = buttonContainer;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
  // Return the number of sections.
  if (self.loaderHelper.loadedLocations.count > 0) {
    self.tableView.scrollEnabled = YES;
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
    return 1;
    
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
  if (self.loaderHelper.loadedLocations.count > 0) {
    return self.loaderHelper.loadedLocations.count;
  }
  return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
  static NSString *CellIdentifier = @"cell";
  
  LocationInfo *currentLocCell = self.loaderHelper.loadedLocations[indexPath.row];
  LocationTableViewCell *cell = (LocationTableViewCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifier
                                                                                         forIndexPath:indexPath];
  
  //  cell.selectionStyle = UITableViewCellSelectionStyleNone;
  cell.titleLabel.textColor = kBlackColor;
  cell.titleLabel.highlightedTextColor = kGrayColor;
  
  NSString *localVersion;
  if (!currentLocCell.location.version) {
    localVersion = @" (?)";
  }
  else{
    localVersion = [NSString stringWithFormat:@" (v. %zd)",currentLocCell.location.version];
  }
  cell.titleLabel.text = [currentLocCell.location.name stringByAppendingString:localVersion];
  
  cell.serverVersion.textColor = kBlackColor;
  cell.serverVersion.highlightedTextColor = kGrayColor;
  
  if(currentLocCell.serverVersion != currentLocCell.location.version)
    cell.serverVersion.text = [NSString stringWithFormat:@"Version avaliable:%zd",currentLocCell.serverVersion];
  else
    cell.serverVersion.text = @"Version is up to date";
  
  cell.loadProcess.textColor = kBlackColor;
  cell.loadProcess.highlightedTextColor = kGrayColor;
  
  cell.loadProcess.text = currentLocCell.loadingProcess;
  if (currentLocCell.isSet == YES) {
    cell.backgroundColor = kColorFromHex(0xee4000);
  }
  else{
    cell.backgroundColor = [UIColor whiteColor];
  }
  
  return cell;
}

//-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
//  //Build a segue string based on the selected cell
//  NSString *segueString = @"detailLocationSegue";
//  static NSString *CellIdentifier = @"cell";
//
//  LocationInfo *currentLoc = self.loadedLocations[indexPath.row];
//  if(currentLoc.location.version){
//    [self performSegueWithIdentifier:segueString
//                            sender:currentLoc];
//  }
//}
//
//-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender{
//
//  LocationInfo *currentLoc = (LocationInfo *)sender;
//  if([segue.identifier isEqualToString:@"detailLocationSegue"]) {
//    //NSLog(@"pop %@",_action.title);
//    DetailLoaderView *pop = [segue destinationViewController];
//    pop.location = currentLoc.location;
//    pop.serverVersion = currentLoc.serverVersion;
//  }
//}


- (IBAction)addPressed:(id)sender {
  [self performSegueWithIdentifier:@"qrSegue" sender:nil];
}

- (IBAction)menuPressed:(id)sender {
  if(isDownloadingLocation == YES){
    [UIAlertView showWithTitle:@"Downloader busy" message:@"" cancelButtonTitle:@"OK"];
    return;
  }
  if(self.slidingPanelController.sideDisplayed == MSSPSideDisplayedLeft) {
    [self.slidingPanelController closePanel];
  }
  else {
    [self.slidingPanelController openLeftPanel];
  }
}

- (void)downloadLocationList :(NSString *)userHash{
  [self resignFirstResponder];
  [self.loaderHelper deleteAllLocations];
  UIImage *imageOfUnderlyingView = [self convertViewToImage];
  imageOfUnderlyingView = [imageOfUnderlyingView applyBlurWithRadius:20
                                                           tintColor:[UIColor colorWithWhite:1.0 alpha:0.2]
                                               saturationDeltaFactor:1.3
                                                           maskImage:nil];
  
  downloadingView.backgroundColor = [UIColor colorWithPatternImage:imageOfUnderlyingView];
  downloadingView.hidden = NO;
  [downloadingView addSubview:indicator];
  [downloadingView addSubview:downloadingLabel];
  
  if([userHash isEqual :@""] || !userHash){
    [UIAlertView showWithTitle:@"Can't load content" message:@"Incorrect UserHash" cancelButtonTitle:@"OK"];
    return;
  }
  if(isDownloadingLocation == YES){
    [UIAlertView showWithTitle:@"Can't load content" message:@"Downloader busy" cancelButtonTitle:@"OK"];
    return;
  }
  
  locationForDownload = @"";
  self.loaderHelper.userHash = userHash;
  [self.loaderHelper saveUserHashToFile:userHash];
  
  isDownloadingLocation = YES;
  [self.loaderHelper startDownloadProcess:locationForDownload :YES];
  
}


- (IBAction)userHashPressed:(id)sender {
  [UIAlertView showWithTitle:@"Enter User Hash"
                     message:nil
           cancelButtonTitle:@"Download"
           otherButtonTitles:[NSArray arrayWithObject:@"Cancel"]
           andTextFieldStyle:UIAlertViewStylePlainTextInput
            andTextFieldText:self.loaderHelper.userHash
                    tapBlock:^(UIAlertView *alertView, NSInteger buttonIndex) {
                      switch (buttonIndex) {
                        case 0:
                          [self downloadLocationList:[alertView textFieldAtIndex:0].text];
                          break;
                        case 1:
                          break;
                      }
                    }];
}

- (void)longPress:(UIGestureRecognizer *)gesture {
  if (gesture.state == UIGestureRecognizerStateBegan) {
    CGPoint translatedPoint = [(UIGestureRecognizer*)gesture locationInView:self.tableView];
    NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:translatedPoint];
    
    LocationInfo *currentLocation = self.loaderHelper.loadedLocations[indexPath.row];
    NSString *location = currentLocation.location.name;
    if(!currentLocation.location.version){
      NSString *str = [NSString stringWithFormat:@"Location '%@' cannot be selected!",location];
      [UIAlertView showWithTitle:str message:@"Please, download location first!" cancelButtonTitle:@"OK"];
    }
    else{
      [UIAlertView showWithTitle:[NSString stringWithFormat:@"Location '%@'",location]
                         message:nil
               cancelButtonTitle:@"Delete location"
               otherButtonTitles:[NSArray arrayWithObjects:@"Cancel", @"Set location", nil]
               andTextFieldStyle:UIAlertViewStyleDefault
                andTextFieldText:nil
                        tapBlock:^(UIAlertView *alertView, NSInteger buttonIndex) {
                          switch (buttonIndex) {
                            case 0:
                              [self deleteLocation:location];
                              break;
                            case 1:
                              break;
                            case 2:
                              [self setLocation:location];
                              break;
                              
                          }
                        }];
    }
    
  }
}

- (void)btnDownload:(id)sender {
  UIButton *downloadBtn = (UIButton *)sender;
  UITableView *owning = (UITableView*)[[downloadBtn superview] superview];
  
  NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:owning.center];
  LocationInfo *currentLocation = self.loaderHelper.loadedLocations[indexPath.row];
  
  if(isDownloadingLocation == YES){
    [UIAlertView showWithTitle:@"Can't load content" message:@"Downloader busy" cancelButtonTitle:@"OK"];
    return;
  }
  isDownloadingLocation = YES;
  [self.loaderHelper startDownloadProcess:currentLocation.location.name :YES];
  
}

-(void) changeDownloadingValue:(NSInteger)value{
  downloadingLabel.text = [NSString stringWithFormat:@"%zd%%",value];
  [self.tableView reloadData];
}

-(void)errorWhileDownloading:(NSInteger)error :(LocationInfo *)locationInfo{
  [self.refreshControl endRefreshing];
  [UIView animateWithDuration:0.3 animations:^{
    downloadingView.hidden = YES;
  } completion:^(BOOL finished) {
    [downloadingView removeAllSubviews];
  }];
  NSString *alertTitle = nil;
  if(![locationInfo.location.name isEqualToString:@""])
    alertTitle = [NSString stringWithFormat:@"Can't download '%@' location",locationInfo.location.name];
  else alertTitle = @"Can't downloadload location list";
  isDownloadingLocation = NO;
  switch (error) {
    case -1:
      [UIAlertView showWithTitle:alertTitle message:@"Check Internet Connection" cancelButtonTitle:@"OK"];
      break;
    case -2:
      [UIAlertView showWithTitle:alertTitle message:@"Check your userHash" cancelButtonTitle:@"OK"];
      break;
    default:
      break;
  }
  [self.tableView reloadData];
}

-(void)successfullDownloading:(LocationInfo *)locationInfo{
  [self.refreshControl endRefreshing];

  [UIView animateWithDuration:0.3 animations:^{
    downloadingView.hidden = YES;
  }completion:^(BOOL finished) {
    [downloadingView removeAllSubviews];
  }];
  
  isDownloadingLocation = NO;
  
  if(!locationInfo || [locationInfo.location.name isEqualToString:@""]){
    [self.loaderHelper parseLocationList];
    isSetNewLocation = 0;
  }
  else{
    NSInteger error = [self.loaderHelper setLocation:locationInfo.location.name];
    
    if(error == 1){
      NSString *str = [NSString stringWithFormat:@"Can't select location '%@'",locationInfo.location.name];
      [UIAlertView showWithTitle:str message:@"Archive is not valid" cancelButtonTitle:@"OK"];
      isSetNewLocation = 0;
      return;
    }
    locationInfo.serverVersion = locationInfo.location.version;
    isSetNewLocation = 1;
  }

  [self.tableView reloadData];
}

- (void)reloadData{
  // Reload table data
  [self.tableView reloadData];
  
  // End the refreshing
  if (self.refreshControl) {
    
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"MMM d, h:mm a"];
    NSString *title = [NSString stringWithFormat:@"Last update: %@", [formatter stringFromDate:[NSDate date]]];
    NSDictionary *attrsDictionary = [NSDictionary dictionaryWithObject:kBlackColor
                                                                forKey:NSForegroundColorAttributeName];
    NSAttributedString *attributedTitle = [[NSAttributedString alloc] initWithString:title attributes:attrsDictionary];
    self.refreshControl.attributedTitle = attributedTitle;
    
    NSUserDefaults *preferences = [NSUserDefaults standardUserDefaults];
    NSString *currentLevelKey = @"locationForSet";
    [preferences removeObjectForKey:currentLevelKey];
  }
}


- (void)setLocation :(NSString *)locationForSet{
  if(isDownloadingLocation == YES){
    NSString *str = [NSString stringWithFormat:@"Wait until location download"];
    [UIAlertView showWithTitle:str message:@"Downloader busy" cancelButtonTitle:@"OK"];
    return;
  }

  NSInteger error = [self.loaderHelper setLocation:locationForSet];
  
  if (error == 3)
    isSetNewLocation = 1;
  
  if (error == 0){
    isSetNewLocation = 1;
  }
  else{
    if(error == 1){
      NSString *str = [NSString stringWithFormat:@"Can't select location '%@'",locationForSet];
      [UIAlertView showWithTitle:str message:@"Archive is not valid" cancelButtonTitle:@"OK"];
    }
    isSetNewLocation = 0;
    return;
  }
  [self.tableView reloadData];
}

-(void) deleteLocation :(NSString *)locationForDelete{
  [self.loaderHelper deleteLocation:locationForDelete];
  [self.tableView reloadData];
}


- (void)getLatestVersion{
  if(isDownloadingLocation == YES){
    [UIAlertView showWithTitle:@"Can't load content" message:@"Downloader busy" cancelButtonTitle:@"OK"];
    return;
  }
  locationForDownload = @"";
  isDownloadingLocation = YES;
  [self.loaderHelper startDownloadProcess:locationForDownload :YES];
  
  [self performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
}


- (UIImage *)convertViewToImage {
  UIGraphicsBeginImageContext(self.navigationController.view.bounds.size);
  [self.navigationController.view drawViewHierarchyInRect:self.navigationController.view.bounds afterScreenUpdates:YES];
  UIImage *image2 = UIGraphicsGetImageFromCurrentImageContext();
  UIGraphicsEndImageContext();
  
  return image2;
}
@end