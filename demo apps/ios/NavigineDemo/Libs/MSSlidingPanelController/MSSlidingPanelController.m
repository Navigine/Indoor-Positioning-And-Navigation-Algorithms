//  MSSlidingPanelController.m
//
// Copyright (c) 2014 Sébastien MICHOY
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer. Redistributions in binary
// form must reproduce the above copyright notice, this list of conditions and
// the following disclaimer in the documentation and/or other materials
// provided with the distribution. Neither the name of the nor the names of
// its contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#import "MSSlidingPanelController.h"
#import "NavigineDemoAppDelegate.h"

#pragma mark - Macros

#define MSSPStoryboardIDCenter  @"MSSPStoryboardIDCenter"
#define MSSPStoryboardIDLeft    @"MSSPStoryboardIDLeft"
#define MSSPStoryboardIDRight   @"MSSPStoryboardIDRight"

#pragma mark - Global variables

CGFloat     g_animationVelocity = 640;
NSUInteger  g_panelMaximumWidth = 280;

#pragma mark - Enumeration

/**
 *  These values are used to know where was the original panning touch.
 */
typedef NS_ENUM(NSUInteger, MSSPPanTouchLocation)
{
    /**
     *  The original panning touch began in the content part of the center view.
     */
    MSSPPanTouchLocationContent = MSSPOpenGestureModePanContent,
    
    /**
     *  The original panning touch began in the navigation bar of the center view.
     */
    MSSPPanTouchLocationNavBar = MSSPOpenGestureModePanNavBar,
};

#pragma mark - Interfaces

/**
 *  A view which manage the touches in function of center view interaction mode.
 */
@interface MSSlidingPanelCenterView : UIView

#pragma mark Access to the sliding panel controller
/** @name Access to the sliding panel controller */

/**
 *  The interaction mode with the center view controller.
 *
 *  By default, this value is `MSSPCenterViewInteractionNavBar`.
 */
@property (nonatomic, weak) MSSlidingPanelController    *slidingPanelController;

#pragma mark Manage center view's touches
/** @name Manage center view's touches */

/**
 *  Return the navigation bar object in view the view is included.
 *
 *  @param view The view.
 *
 *  @return The navigation bar object. It's value is `nil` if there is no superview which is a navigation bar.
 */
- (UINavigationBar *)navigationBarInSuperViewOfView:(UIView *)view;

@end

@interface MSSlidingPanelController () <UIGestureRecognizerDelegate>

#pragma mark Status bar
/** @name Status bar */

/**
 *  The color of the center view status bar.
 *
 *  By default, this value is clearColor.
 */
@property (nonatomic, strong)   UIColor                     *centerViewStatusBarColor;

/**
 *  A view which is above the status bar.
 */
@property (nonatomic, strong)   UIView                      *statusBarView;

#pragma mark Center view
/** @name Center view */

@property (nonatomic, strong)   MSSlidingPanelCenterView    *centerView;

#pragma mark Panels settings
/** @name Panels settings */

/**
 *  Set the maximum width of a panel.
 *
 *  @param panelMaximumWidth Panel's maximum width.
 *  @param side              The side. Must not be `MSSPSideDisplayedNone`.
 *  @param completion        A block object to be executed when the maximum width is set.
 */
- (void)setPanelMaximumWidth:(CGFloat)panelMaximumWidth forSide:(MSSPSideDisplayed)side withCompletion:(void (^)(void))completion;

#pragma mark Panels information
/** @name Panels information */

/**
 *  Return the panel controller for a side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 *
 *  @return The panel controller.
 */
- (UIViewController *)panelControllerForSide:(MSSPSideDisplayed)side;

/**
 *  Return the panel maximum width for a side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 *
 *  @return The panel maximum width.
 */
- (NSUInteger)panelMaximumWithForSide:(MSSPSideDisplayed)side;

/**
 *  Which panel is displayed.
 */
@property (nonatomic, assign)   MSSPSideDisplayed           sideDisplayed;

/**
 *  Return the status bar color for a side.
 *
 *  @param side The side.
 *
 *  @return The status bar color.
 */
- (UIColor *)statusBarColorForSide:(MSSPSideDisplayed)side;

/**
 *  Return if the status bar must be displayed smoothly for a side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 *
 *  @return YES if the status bar must be displayed smoothly, else NO.
 */
- (BOOL)statusBarDisplayedSmoothlyForSide:(MSSPSideDisplayed)side;

#pragma mark Tools
/** @name Tools */

/**
 *  Adjust the status bar color.
 */
- (void)adjustStatusBarColor;

/**
 *  Returns the animation duration for a given length.
 *
 *  @param length The length.
 *
 *  @return The duration.
 */
- (NSTimeInterval)animationDurationForLength:(CGFloat)length;

/**
 *  Set initial values to variables.
 */
- (void)commonSettings;

/**
 *  Find every navigation bar in a view.
 *
 *  @param navigationBarList A array which contains the list of every navigation bar found. Must be initialized.
 *  @param view              The view.
 */
- (void)fillNavigationBarsList:(NSMutableArray *)navigationBarList withSubviewsOfView:(UIView *)view;

/**
 *  The percentage visible of the current panel displayed.
 *
 *  @return The percentage.
 */
- (CGFloat)percentageVisibleOfDisplayedPanel;

#pragma mark Set center view and panels
/** @name Set center view and panels */

/**
 *  Set a panel controller for a side.
 *
 *  @param panelController The panel controller.
 *  @param side            The side. Must not be `MSSPSideDisplayedNone`.
 */
- (void)setPanelController:(UIViewController *)panelController forSide:(MSSPSideDisplayed)side;

#pragma mark Manage gestures
/** @name Manage gestures */

/**
 *  Ask the delegate if a gesture recognizer should receive an object representing a touch.
 *
 *  @param gestureRecognizer The gesture recognizer.
 *  @param touch             The touch.
 *
 *  @return YES (the default) to allow the gesture recognizer to examine the touch object, NO to prevent the gesture recognizer from seeing this touch object.
 */
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch;

/**
 *  Return the list of the possible closing gestures recognized.
 *
 *  @param gestureRecognizer The gesture recognizer.
 *  @param touch             The touch.
 *
 *  @return The closing gestures possibly recognized.
 */
- (MSSPCloseGestureMode)closeGestureModeWithGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer andTouch:(UITouch *)touch;

/**
 *  Verify if the touch is within the center view.
 *
 *  @param touch The touch
 *
 *  @return Return YES if the touch is whthin, else return NO.
 */
- (BOOL)isTouchInCenterView:(UITouch *)touch;

/**
 *  Verify if the touch is within an navigation bar.
 *
 *  @param touch              The touch.
 *  @param navigationBarsList The list of the navigation bar.
 *
 *  @return Return YES if the touch is within, else return NO.
 */
- (BOOL)isTouch:(UITouch *)touch inNavigationBars:(NSArray *)navigationBarsList;

/**
 *  Return the list of the possible opening gestures recognized.
 *
 *  @param gestureRecognizer The gesture recognizer.
 *  @param touch             The touch.
 *
 *  @return The opening gestures possibly recognized.
 */
- (MSSPOpenGestureMode)openGestureModeWithGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer andTouch:(UITouch *)touch;

/**
 *  Called when a panning is recognized.
 *
 *  @param panGestureRecognizer The gesture recognizer.
 */
- (void)panGestureRecognized:(UIPanGestureRecognizer *)panGestureRecognizer;

/**
 *  The panning recognizer of the center view.
 */
@property (nonatomic, strong)   UIPanGestureRecognizer      *panGestureRecognizer;

/**
 *  Verify if the new frame is authorized to have these values.
 *
 *  @param newCenterViewFrame The new center view frame.
 */
- (void)panGestureVerifyAuthorizationForNewCenterViewFrame:(CGRect *)newCenterViewFrame;

/**
 *  The original panning touch location : navigation bar or content.
 */
@property (nonatomic, assign)   MSSPPanTouchLocation        panTouchLocation;

/**
 *  The pan gesture translation translation.
 */
@property (nonatomic, assign)   CGPoint                     panTranslation;

/**
 *  Set the gesture recognizers.
 */
- (void)setGestureRecognizers;

/**
 *  Called when a tap is recognized.
 *
 *  @param tapGestureRecognizer The gesture recognizer.
 */
- (void)tapGestureRecognized:(UITapGestureRecognizer *)tapGestureRecognizer;

/**
 *  The tap recognizer of the center view.
 */
@property (nonatomic, strong)   UITapGestureRecognizer      *tapGestureRecognizer;

#pragma mark Load and unload panels
/** @name Load and unload panels */

/**
 *  Load the left panel.
 */
- (void)loadLeftPanel;

/**
 *  Load the panel corresponding to the side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 */
- (void)loadPanelForSide:(MSSPSideDisplayed)side;

/**
 *  Load the right panel.
 */
- (void)loadRightPanel;

/**
 *  Unload the left panel.
 */
- (void)unloadLeftPanel;

/**
 *  Unload the panel corresponding to the side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 */

/**
 *  Unload the panel corresponding to the side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 */
- (void)unloadPanelForSide:(MSSPSideDisplayed)side;

/**
 *  Unload the right panel.
 */
- (void)unloadRightPanel;

#pragma mark Actions
/** @name Actions */

/**
 *  Open the panel corresponding to the side.
 *
 *  @param side       The side. Must not be `MSSPSideDisplayedNone`.
 *  @param completion A block object to be executed when the panel is opened.
 */
- (void)openPanelSide:(MSSPSideDisplayed)side withCompletion:(void (^)(void))completion;

/**
 *  Open the panel corresponding to the side.
 *
 *  @param side                 The side. Must not be `MSSPSideDisplayedNone`.
 *  @param completion           A block object to be executed when the panel is opened.
 *  @param statusBarColorUpdate A value which indicate if the status bar color must be updated before the animation.
 */
- (void)openPanelSide:(MSSPSideDisplayed)side withCompletion:(void (^)(void))completion andStatusBarColorUpdate:(BOOL)statusBarColorUpdate;

#pragma mark Storyboard
/** @name Storyboard */

/**
 *  Sets views from storyboard.
 */
- (void)setViewsFromStoryboard;

@end

#pragma mark - Implementation

@implementation MSSlidingPanelCenterView

#pragma mark Manage center view's touches
/** @name Manage center view's touches */

/**
 *  Return the fartest descendant of the receiver that contains the specified point.
 *
 *  @param point The point.
 *  @param event The event.
 *
 *  @return The fartest descendant.
 */
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
    UIView  *hitView;
    
    hitView = [super hitTest:point withEvent:event];
    
    if ([[self slidingPanelController] sideDisplayed] == MSSPSideDisplayedLeft)
    {
        if ([[self slidingPanelController] leftPanelCenterViewInteractionMode] == MSSPCenterViewInteractionNone)
            return (nil);
        
        if ([[self slidingPanelController] leftPanelCenterViewInteractionMode] == MSSPCenterViewInteractionNavBar && ![self navigationBarInSuperViewOfView:hitView])
            return (nil);
    }
    else if ([[self slidingPanelController] sideDisplayed] == MSSPSideDisplayedRight)
    {
        if ([[self slidingPanelController] rightPanelCenterViewInteractionMode] == MSSPCenterViewInteractionNone)
            return (nil);
        
        if ([[self slidingPanelController] rightPanelCenterViewInteractionMode] == MSSPCenterViewInteractionNavBar && ![self navigationBarInSuperViewOfView:hitView])
            return (nil);
    }
    
    return (hitView);
}

/**
 *  Return the navigation bar object in view the view is included.
 *
 *  @param view The view.
 *
 *  @return The navigation bar object. It's value is `nil` if there is no superview which is a navigation bar.
 */
- (UINavigationBar *)navigationBarInSuperViewOfView:(UIView *)view
{
    UIView  *superView;
    
    if ([view isKindOfClass:[UINavigationBar class]])
        return ((UINavigationBar *) view);
    
    superView = [view superview];
    while (superView)
    {
        if ([superView isKindOfClass:[UINavigationBar class]])
            return ((UINavigationBar *) superView);
        
        superView = [superView superview];
    }
    
    return (nil);
}

@end

@implementation MSSlidingPanelController

#pragma mark Initialization
/** @name Initialization */

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @return The intialized centrer view controller.
 */
- (id)init
{
    self = [super init];
    
    if (self)
        [self commonSettings];
    
    return (self);
}

/**
 *  Returns an object initialized from data in a given unarchiver.
 *
 *  @param aDecoder An unarchiver object.
 *
 *  @return self, initialized using the data in decoder.
 */
- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    
    if (self)
        [self commonSettings];
    
    return (self);
}

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param nibNameOrNil   The nib name or nil.
 *  @param nibBundleOrNil The bundle name or nil.
 *
 *  @return The initialized centrer view controller.
 */
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    
    if (self)
        [self commonSettings];
    
    return (self);
}

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *
 *  @return The initialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController
{
    self = [super init];
    
    if (self)
    {
        [self commonSettings];
        [self setCenterViewController:centerViewController];
    }
    
    return (self);
}

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *  @param leftPanelController  The left panel controller.
 *
 *  @return The initialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController andLeftPanelController:(UIViewController *)leftPanelController
{
    self = [super init];
    
    if (self)
    {
        [self commonSettings];
        [self setCenterViewController:centerViewController];
        [self setLeftPanelController:leftPanelController];
    }
    
    return (self);
}

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *  @param rightPanelController The right panel controller.
 *
 *  @return The initialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController andRightPanelController:(UIViewController *)rightPanelController
{
    self = [super init];
    
    if (self)
    {
        [self commonSettings];
        [self setCenterViewController:centerViewController];
        [self setRightPanelController:rightPanelController];
    }
    
    return (self);
}

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *  @param leftPanelController  The left panel controller.
 *  @param rightPanelController The right panel controller.
 *
 *  @return The initialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController leftPanelController:(UIViewController *)leftPanelController andRightPanelController:(UIViewController *)rightPanelController
{
    self = [super init];
    
    if (self)
    {
        [self commonSettings];
        [self setCenterViewController:centerViewController];
        [self setLeftPanelController:leftPanelController];
        [self setRightPanelController:rightPanelController];
    }
    
    return (self);
}

#pragma mark View life cycle
/** @name View life cycle */

/**
 *  Creates the view that the controller manages.
 */
- (void)loadView
{
    CGRect  centerViewFrame;
    CGSize  windowSize;
    
    [super loadView];
    
    windowSize = [[UIScreen mainScreen] bounds].size;
    
    if ([self storyboard])
        [self setViewsFromStoryboard];
    
    [self setStatusBarView:[[UIView alloc] initWithFrame:CGRectMake(0, 0, windowSize.width, 20)]];
    [[self statusBarView] setBackgroundColor:[self centerViewStatusBarColor]];
    [[self statusBarView] setAutoresizingMask:(UIViewAutoresizingFlexibleWidth)];
    
    [self setCenterView:[[MSSlidingPanelCenterView alloc] initWithFrame:CGRectMake(0, 0, windowSize.width, windowSize.height)]];
    [[self centerView] setSlidingPanelController:self];
    [[[self centerViewController] view] setFrame:[[self centerView] frame]];
    [[[self centerViewController] view] setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
    [[self centerView] addSubview:[[self centerViewController] view]];
    [[self centerView] setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
    
    if ([[self parentViewController] isKindOfClass:[UITabBarController class]] &&
        [[self centerViewController] isKindOfClass:[UINavigationController class]] &&
        ![[(UITabBarController *)[self parentViewController] tabBar] isTranslucent])
    {
        centerViewFrame = [[[self centerViewController] view] frame];
        centerViewFrame.size.height += [[(UITabBarController *)[self parentViewController] tabBar] frame].size.height;
        [[[self centerViewController] view] setFrame:centerViewFrame];
    }
    
    [self setView:[[UIView alloc] initWithFrame:CGRectMake(0, 0, windowSize.width, windowSize.height)]];
    [[self view] addSubview:[self centerView]];
    [[self view] addSubview:[self statusBarView]];
    [[self view] setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
    
    [self setGestureRecognizers];
}

#pragma mark Tools
/** @name Tools */

/**
 *  Adjust the status bar color.
 */
- (void)adjustStatusBarColor
{
    UIColor *statusBarColor;
    CGFloat percentVisible;
    CGFloat statusBarCenterAlpha;
    CGFloat statusBarCenterBlue;
    CGFloat statusBarCenterGreen;
    CGFloat statusBarCenterRed;
    CGFloat statusBarPanelAlpha;
    CGFloat statusBarPanelBlue;
    CGFloat statusBarPanelGreen;
    CGFloat statusBarPanelRed;
    
    statusBarColor = [self statusBarColorForSide:[self sideDisplayed]];
    
    if ([self sideDisplayed] != MSSPSideDisplayedNone && [self statusBarDisplayedSmoothlyForSide:[self sideDisplayed]])
    {
        if (![[self centerViewStatusBarColor] getRed:&statusBarCenterRed green:&statusBarCenterGreen blue:&statusBarCenterBlue alpha:&statusBarCenterAlpha])
        {
            [[self centerViewStatusBarColor] getWhite:&statusBarCenterRed alpha:&statusBarCenterAlpha];
            
            statusBarCenterGreen = statusBarCenterRed;
            statusBarCenterBlue = statusBarCenterRed;
        }
        
        if (![statusBarColor getRed:&statusBarPanelRed green:&statusBarPanelGreen blue:&statusBarPanelBlue alpha:&statusBarPanelAlpha])
        {
            [statusBarColor getWhite:&statusBarPanelRed alpha:&statusBarPanelAlpha];
            
            statusBarPanelGreen = statusBarPanelRed;
            statusBarPanelBlue = statusBarPanelRed;
        }
        
        percentVisible = [self percentageVisibleOfDisplayedPanel];
        
        statusBarCenterRed += (statusBarPanelRed - statusBarCenterRed) * percentVisible;
        statusBarCenterGreen += (statusBarPanelGreen - statusBarCenterGreen) * percentVisible;
        statusBarCenterBlue += (statusBarPanelBlue - statusBarCenterBlue) * percentVisible;
        statusBarCenterAlpha += (statusBarPanelAlpha - statusBarCenterAlpha) * percentVisible;
        
        [[self statusBarView] setBackgroundColor:[UIColor colorWithRed:statusBarCenterRed green:statusBarCenterGreen blue:statusBarCenterBlue alpha:statusBarCenterAlpha]];
    }
    else
        [[self statusBarView] setBackgroundColor:statusBarColor];
}

/**
 *  Returns the animation duration for a given length.
 *
 *  @param length The length.
 *
 *  @return The duration.
 */
- (NSTimeInterval)animationDurationForLength:(CGFloat)length
{
    if (length <= 0)
        length *= -1;
    
    return (length / [self animationVelocity]);
}

/**
 *  Set initial values to variables.
 */
- (void)commonSettings
{
    _centerViewController = nil;
    [self setCenterViewStatusBarColor:[UIColor clearColor]];
    
    [self setLeftPanelCenterViewInteractionMode:MSSPCenterViewInteractionNavBar];
    [self setLeftPanelCloseGestureMode:MSSPCloseGestureModeAll];
    [self setLeftPanelController:nil];
    [self setLeftPanelMaximumWidth:g_panelMaximumWidth];
    [self setLeftPanelOpenGestureMode:MSSPOpenGestureModeAll];
    [self setLeftPanelStatusBarColor:[UIColor clearColor]];
    [self setLeftPanelStatusBarDisplayedSmoothly:NO];
    
    [self setRightPanelCenterViewInteractionMode:MSSPCenterViewInteractionNavBar];
    [self setRightPanelCloseGestureMode:MSSPCloseGestureModeAll];
    [self setRightPanelController:nil];
    [self setRightPanelMaximumWidth:g_panelMaximumWidth];
    [self setRightPanelOpenGestureMode:MSSPOpenGestureModeAll];
    [self setRightPanelStatusBarColor:[UIColor clearColor]];
    [self setRightPanelStatusBarDisplayedSmoothly:NO];
 
    [self setAnimationVelocity:g_animationVelocity];
    [self setSideDisplayed:MSSPSideDisplayedNone];
}

/**
 *  Find every navigation bar in a view.
 *
 *  @param navigationBarList A array which contains the list of every navigation bar found. Must be initialized.
 *  @param view              The view.
 */
- (void)fillNavigationBarsList:(NSMutableArray *)navigationBarList withSubviewsOfView:(UIView *)view
{
    UIView  *subview;
    
    if ([view isKindOfClass:[UINavigationBar class]])
        [navigationBarList addObject:view];
    
    for (subview in [view subviews])
        [self fillNavigationBarsList:navigationBarList withSubviewsOfView:subview];
}

/**
 *  The percentage visible of the current panel displayed.
 *
 *  @return The percentage.
 */
- (CGFloat)percentageVisibleOfDisplayedPanel
{
    if ([self sideDisplayed] == MSSPSideDisplayedLeft)
        return ([[self centerView] frame].origin.x / [self leftPanelMaximumWidth]);
    else if ([self sideDisplayed] == MSSPSideDisplayedRight)
        return ([[self centerView] frame].origin.x / - (NSInteger)[self rightPanelMaximumWidth]);
    
    return (0);
}

#pragma mark Set center view and panels
/** @name Set center view and panels */

/**
 *  Set the center view controller value.
 *
 *  @param centerViewController The center view controller. Must not be nil.
 */
- (void)setCenterViewController:(UIViewController *)centerViewController
{
    CGRect  frame;
    
    NSParameterAssert(centerViewController);
    
    if ([self isViewLoaded])
#ifndef __clang_analyzer__
        frame = [[[self centerViewController] view] frame];
#endif
    
    [[[self centerViewController] view] removeFromSuperview];
    [[self centerViewController] removeFromParentViewController];
    
    _centerViewController = centerViewController;
    
    [self addChildViewController:[self centerViewController]];
    [[self centerViewController] didMoveToParentViewController:self];
    
    if ([self isViewLoaded])
    {
#ifndef __clang_analyzer__
        [[[self centerViewController] view] setFrame:frame];
#endif
        [[[self centerViewController] view] setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
        [[self centerView] addSubview:[[self centerViewController] view]];
    }
}

/**
 *  Set the left panel controller value.
 *
 *  @param leftPanelController The left panel controller.
 */
- (void)setLeftPanelController:(UIViewController *)leftPanelController
{
    [self setPanelController:leftPanelController forSide:MSSPSideDisplayedLeft];
}

/**
 *  Set a panel controller for a side.
 *
 *  @param panelController The panel controller.
 *  @param side            The side. Must not be `MSSPSideDisplayedNone`.
 */
- (void)setPanelController:(UIViewController *)panelController forSide:(MSSPSideDisplayed)side
{
    __block BOOL    reloadPanel;
    void            (^setController)(void);
    
    NSParameterAssert(side != MSSPSideDisplayedNone);
    
    reloadPanel = NO;
    setController = ^(void)
    {
        [[self panelControllerForSide:side] removeFromParentViewController];
        
        if (side == MSSPSideDisplayedLeft)
            _leftPanelController = panelController;
        else
            _rightPanelController = panelController;
        
        if (!panelController)
            return ;
        
        [self addChildViewController:panelController];
        [[self panelControllerForSide:side] didMoveToParentViewController:self];
        
        if (reloadPanel)
            [self loadPanelForSide:side];
    };
    
    if ([self isViewLoaded] && [self sideDisplayed] == side)
    {
        if (!panelController)
        {
            [self closePanelWithCompletion:setController];
            return ;
        }
        else
        {
            [self unloadPanelForSide:side];
            reloadPanel = YES;
        }
    }
    
    setController();
}

/**
 *  Set the right panel controller value.
 *
 *  @param rightPanelController The right panel controller.
 */
- (void)setRightPanelController:(UIViewController *)rightPanelController
{
    [self setPanelController:rightPanelController forSide:MSSPSideDisplayedRight];
}

#pragma mark Panels settings
/** @name Panels settings */

/**
 *  Set the maximum width of the left panel.
 *
 *  @param leftPanelMaximumWidth Left panel's maximum width.
 */
- (void)setLeftPanelMaximumWidth:(CGFloat)leftPanelMaximumWidth
{
    [self setLeftPanelMaximumWidth:leftPanelMaximumWidth withCompletion:nil];
}

/**
 *  Set the maximum width of the left panel with.
 *
 *  @param maximumWidth Panel's maximum width.
 *  @param completion   A block object to be executed when the maximum width is set.
 */
- (void)setLeftPanelMaximumWidth:(CGFloat)leftPanelMaximumWidth withCompletion:(void (^)(void))completion
{
    [self setPanelMaximumWidth:leftPanelMaximumWidth forSide:MSSPSideDisplayedLeft withCompletion:completion];
}

/**
 *  Set the maximum width of a panel.
 *
 *  @param panelMaximumWidth Panel's maximum width.
 *  @param side              The side. Must not be `MSSPSideDisplayedNone`.
 *  @param completion        A block object to be executed when the maximum width is set.
 */
- (void)setPanelMaximumWidth:(CGFloat)panelMaximumWidth forSide:(MSSPSideDisplayed)side withCompletion:(void (^)(void))completion
{
    NSParameterAssert(side != MSSPSideDisplayedNone);
    
    if (panelMaximumWidth < 0)
        panelMaximumWidth = 0;
    
    if (panelMaximumWidth > [[UIScreen mainScreen] bounds].size.width)
        panelMaximumWidth = [[UIScreen mainScreen] bounds].size.width;
    
    if (side == MSSPSideDisplayedLeft)
        _leftPanelMaximumWidth = panelMaximumWidth;
    else
        _rightPanelMaximumWidth = panelMaximumWidth;
    
    if ([self sideDisplayed] == side)
        [self openPanelSide:side withCompletion:completion andStatusBarColorUpdate:NO];
    else if (completion)
        completion();
}

/**
 *  Set the maximum width of the right panel.
 *
 *  @param rightPanelMaximumWidth Right panel's maximum width.
 */
- (void)setRightPanelMaximumWidth:(CGFloat)rightPanelMaximumWidth
{
    [self setRightPanelMaximumWidth:rightPanelMaximumWidth withCompletion:nil];
}

/**
 *  Set the maximum width of the right panel with.
 *
 *  @param maximumWidth Panel's maximum width.
 *  @param completion   A block object to be executed when the maximum width is set.
 */
- (void)setRightPanelMaximumWidth:(CGFloat)rightPanelMaximumWidth withCompletion:(void (^)(void))completion
{
    [self setPanelMaximumWidth:rightPanelMaximumWidth forSide:MSSPSideDisplayedRight withCompletion:completion];
}

#pragma mark Panels information
/** @name Panels information */

/**
 *  Return the panel controller for a side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 *
 *  @return The panel controller.
 */
- (UIViewController *)panelControllerForSide:(MSSPSideDisplayed)side
{
    NSParameterAssert(side != MSSPSideDisplayedNone);
    
    if (side == MSSPSideDisplayedLeft)
        return ([self leftPanelController]);
    else
        return ([self rightPanelController]);
}

/**
 *  Return the panel maximum width for a side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 *
 *  @return The panel maximum width.
 */
- (NSUInteger)panelMaximumWithForSide:(MSSPSideDisplayed)side
{
    NSParameterAssert(side != MSSPSideDisplayedNone);
    
    if (side == MSSPSideDisplayedLeft)
        return ([self leftPanelMaximumWidth]);
    else
        return ([self rightPanelMaximumWidth]);
}

/**
 *  Return the status bar color for a side.
 *
 *  @param side The side.
 *
 *  @return The status bar color.
 */
- (UIColor *)statusBarColorForSide:(MSSPSideDisplayed)side
{
    if (side == MSSPSideDisplayedLeft)
        return ([self leftPanelStatusBarColor]);
    else if (side == MSSPSideDisplayedRight)
        return ([self rightPanelStatusBarColor]);
    
    return ([self centerViewStatusBarColor]);
}

/**
 *  Return if the status bar must be displayed smoothly for a side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 *
 *  @return YES if the status bar must be displayed smoothly, else NO.
 */
- (BOOL)statusBarDisplayedSmoothlyForSide:(MSSPSideDisplayed)side
{
    NSParameterAssert(side != MSSPSideDisplayedNone);
    
    if (side == MSSPSideDisplayedLeft)
        return ([self leftPanelStatusBarDisplayedSmoothly]);

    return ([self rightPanelStatusBarDisplayedSmoothly]);
}

#pragma mark Manage gestures
/** @name Manage gestures */

/**
 *  Ask the delegate if a gesture recognizer should receive an object representing a touch.
 *
 *  @param gestureRecognizer The gesture recognizer.
 *  @param touch             The touch.
 *
 *  @return YES (the default) to allow the gesture recognizer to examine the touch object, NO to prevent the gesture recognizer from seeing this touch object.
 */
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    MSSPOpenGestureMode openGestureMode;
    
    if ([self sideDisplayed] == MSSPSideDisplayedLeft)
        return (([self leftPanelCloseGestureMode] & [self closeGestureModeWithGestureRecognizer:gestureRecognizer andTouch:touch]) > 0);
    else if ([self sideDisplayed] == MSSPSideDisplayedRight)
        return (([self rightPanelCloseGestureMode] & [self closeGestureModeWithGestureRecognizer:gestureRecognizer andTouch:touch]) > 0);
    
    openGestureMode = [self openGestureModeWithGestureRecognizer:gestureRecognizer andTouch:touch];
    
    if ([self leftPanelController] && [self rightPanelController])
        return ((([self leftPanelOpenGestureMode] & openGestureMode) > 0) ||
                (([self rightPanelOpenGestureMode] & openGestureMode) > 0));
    else if ([self leftPanelController])
        return (([self leftPanelOpenGestureMode] & openGestureMode) > 0);
    else if ([self rightPanelController])
        return (([self rightPanelOpenGestureMode] & openGestureMode) > 0);
    
    return (NO);
}

/**
 *  Asks the delegate if two gesture recognizers should be allowed to recognize gestures simultaneously.
 *
 *  @param gestureRecognizer      An instance of a subclass of the abstract base class UIGestureRecognizer. This is the object sending the message to the delegate.
 *  @param otherGestureRecognizer An instance of a subclass of the abstract base class UIGestureRecognizer.
 *
 *  @return YES to allow both gestureRecognizer and otherGestureRecognizer to recognize their gestures simultaneously. The default implementation returns NO—no two gestures can be recognized simultaneously.
 */
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    if ([[self delegate] respondsToSelector:@selector(slidingPanelController:gestureRecognizer:shouldRecognizeSimultaneouslyWithGestureRecognizer:)])
        return ([[self delegate] slidingPanelController:self gestureRecognizer:gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:otherGestureRecognizer]);
    
    return (NO);
}

/**
 *  Return the list of the possible closing gestures recognized.
 *
 *  @param gestureRecognizer The gesture recognizer.
 *  @param touch             The touch.
 *
 *  @return The closing gestures possibly recognized.
 */
- (MSSPCloseGestureMode)closeGestureModeWithGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer andTouch:(UITouch *)touch
{
    MSSPCloseGestureMode    closeGestureMode;
    NSMutableArray          *navigationBarList;
    
    if (![self isTouchInCenterView:touch])
        return (MSSPCloseGestureModeNone);
    
    closeGestureMode = MSSPCloseGestureModeNone;
    navigationBarList = [[NSMutableArray alloc] init];
    [self fillNavigationBarsList:navigationBarList withSubviewsOfView:[self centerView]];
    
    if (gestureRecognizer == [self tapGestureRecognizer])
    {
        if ([self isTouch:touch inNavigationBars:navigationBarList])
            closeGestureMode |= MSSPCloseGestureModeTapNavBar;
        else
            closeGestureMode |= MSSPCloseGestureModeTapContent;
    }
    
    if (gestureRecognizer == [self panGestureRecognizer])
    {
        if ([self isTouch:touch inNavigationBars:navigationBarList])
        {
            closeGestureMode |= MSSPCloseGestureModePanNavBar;
            [self setPanTouchLocation:MSSPPanTouchLocationNavBar];
        }
        else
        {
            closeGestureMode |= MSSPCloseGestureModePanContent;
            [self setPanTouchLocation:MSSPPanTouchLocationContent];
        }
    }
    
    return (closeGestureMode);
}

/**
 *  Verify if the touch is within the center view.
 *
 *  @param touch The touch
 *
 *  @return Return YES if the touch is whthin, else return NO.
 */
- (BOOL)isTouchInCenterView:(UITouch *)touch
{
    CGRect  centerViewFrame;
    CGPoint touchPoint;
    
    touchPoint = [touch locationInView:[self view]];
    centerViewFrame = [[self centerView] frame];
    
    return (CGRectContainsPoint(centerViewFrame, touchPoint));
}

/**
 *  Verify if the touch is within an navigation bar.
 *
 *  @param touch              The touch.
 *  @param navigationBarsList The list of the navigation bar.
 *
 *  @return Return YES if the touch is within, else return NO.
 */
- (BOOL)isTouch:(UITouch *)touch inNavigationBars:(NSArray *)navigationBarsList
{
    UINavigationBar *navigationBar;
    CGRect          navigationBarFrame;
    CGPoint         touchPoint;
    
    touchPoint = [touch locationInView:[self view]];
    for (navigationBar in navigationBarsList)
    {
        navigationBarFrame = [navigationBar convertRect:[navigationBar bounds] toView:[self view]];

        if (CGRectContainsPoint(navigationBarFrame, touchPoint))
            return (YES);
    }
    
    return (NO);
}

/**
 *  Return the list of the possible opening gestures recognized.
 *
 *  @param gestureRecognizer The gesture recognizer.
 *  @param touch             The touch.
 *
 *  @return The opening gestures possibly recognized.
 */
- (MSSPOpenGestureMode)openGestureModeWithGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer andTouch:(UITouch *)touch
{
    MSSPOpenGestureMode     openGestureMode;
    NSMutableArray          *navigationBarList;
    
    if (![self isTouchInCenterView:touch])
        return (MSSPOpenGestureModeNone);
    
    openGestureMode = MSSPOpenGestureModeNone;
    navigationBarList = [[NSMutableArray alloc] init];
    [self fillNavigationBarsList:navigationBarList withSubviewsOfView:[self centerView]];
    
    if (gestureRecognizer == [self panGestureRecognizer])
    {
        if ([self isTouch:touch inNavigationBars:navigationBarList])
        {
            openGestureMode |= MSSPOpenGestureModePanNavBar;
            [self setPanTouchLocation:MSSPPanTouchLocationNavBar];
        }
        else
        {
            openGestureMode |= MSSPOpenGestureModePanContent;
            [self setPanTouchLocation:MSSPPanTouchLocationContent];
        }
    }
    
    return (openGestureMode);
}

/**
 *  Called when a panning is recognized.
 *
 *  @param panGestureRecognizer The gesture recognizer.
 */
- (void)panGestureRecognized:(UIPanGestureRecognizer *)panGestureRecognizer
{
    CGRect  newCenterViewFrame;
    CGFloat translationX;
    
    if ([panGestureRecognizer state] == UIGestureRecognizerStateBegan)
        [self setPanTranslation:CGPointZero];
    
    translationX = [panGestureRecognizer translationInView:[self view]].x - [self panTranslation].x;
    [self setPanTranslation:[panGestureRecognizer translationInView:[self view]]];
    
    newCenterViewFrame = [[self centerView] frame];
    newCenterViewFrame.origin.x += translationX;
    
    [self panGestureVerifyAuthorizationForNewCenterViewFrame:&newCenterViewFrame];
    [self adjustStatusBarColor];
    [[self centerView] setFrame:newCenterViewFrame];
    
    if ([panGestureRecognizer state] == UIGestureRecognizerStateEnded)
    {
        if ([self sideDisplayed] == MSSPSideDisplayedLeft)
        {
            if ([[self centerView] frame].origin.x <= [self leftPanelMaximumWidth] / 2)
                [self closePanel];
            else
                [self openLeftPanel];
        }
        else if ([self sideDisplayed] == MSSPSideDisplayedRight)
        {
            if ([[self centerView] frame].origin.x >= - (NSInteger)[self rightPanelMaximumWidth] / 2)
                [self closePanel];
            else
                [self openRightPanel];
        }
        else
            [self closePanel];
    }
}

/**
 *  Verify if the new frame is authorized to have these values.
 *
 *  @param newCenterViewFrame The new center view frame.
 */
- (void)panGestureVerifyAuthorizationForNewCenterViewFrame:(CGRect *)newCenterViewFrame
{
    if ([self leftPanelController] && newCenterViewFrame->origin.x > [self leftPanelMaximumWidth])
        newCenterViewFrame->origin.x = [self leftPanelMaximumWidth];
    else if (![self leftPanelController] && newCenterViewFrame->origin.x > 0)
        newCenterViewFrame->origin.x = 0;
    
    if ([self rightPanelController] && newCenterViewFrame->origin.x < - (NSInteger)[self rightPanelMaximumWidth])
        newCenterViewFrame->origin.x = - (NSInteger)[self rightPanelMaximumWidth];
    else if (![self rightPanelController] && newCenterViewFrame->origin.x < 0)
        newCenterViewFrame->origin.x = 0;
    
    if ([[self centerView] frame].origin.x <= 0 && newCenterViewFrame->origin.x > 0)
    {
        if ([[self delegate] respondsToSelector:@selector(slidingPanelController:hasClosedSide:)])
            [[self delegate] slidingPanelController:self hasClosedSide:[self sideDisplayed]];
        
        if (!([self leftPanelOpenGestureMode] & [self panTouchLocation]))
            newCenterViewFrame->origin.x = 0;
        else
        {
            [self loadLeftPanel];
            
            if ([[self delegate] respondsToSelector:@selector(slidingPanelController:beginsToBringOutSide:)])
                [[self delegate] slidingPanelController:self beginsToBringOutSide:[self sideDisplayed]];
        }
    }
    else if ([[self centerView] frame].origin.x >= 0 && newCenterViewFrame->origin.x < 0)
    {
        if ([[self delegate] respondsToSelector:@selector(slidingPanelController:hasClosedSide:)])
            [[self delegate] slidingPanelController:self hasClosedSide:[self sideDisplayed]];
        
        if (!([self rightPanelOpenGestureMode] & [self panTouchLocation]))
            newCenterViewFrame->origin.x = 0;
        else
        {
            [self loadRightPanel];
            
            if ([[self delegate] respondsToSelector:@selector(slidingPanelController:beginsToBringOutSide:)])
                [[self delegate] slidingPanelController:self beginsToBringOutSide:[self sideDisplayed]];
        }
    }
}

/**
 *  Set the gesture recognizers.
 */
- (void)setGestureRecognizers
{
    if (![self panGestureRecognizer])
    {
        [self setPanGestureRecognizer:[[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGestureRecognized:)]];
        [[self panGestureRecognizer] setDelegate:self];
        [[self panGestureRecognizer] setMinimumNumberOfTouches:1];
        [[self panGestureRecognizer] setMaximumNumberOfTouches:1];
    }
    
    if (![self tapGestureRecognizer])
    {
        [self setTapGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGestureRecognized:)]];
        [[self tapGestureRecognizer] setDelegate:self];
        [[self tapGestureRecognizer] setNumberOfTapsRequired:1];
        [[self tapGestureRecognizer] setNumberOfTouchesRequired:1];
    }
    
    [[self view] addGestureRecognizer:[self panGestureRecognizer]];
    [[self view] addGestureRecognizer:[self tapGestureRecognizer]];
}

/**
 *  Called when a tap is recognized.
 *
 *  @param tapGestureRecognizer The gesture recognizer.
 */
- (void)tapGestureRecognized:(UITapGestureRecognizer *)__unused tapGestureRecognizer
{
    [self closePanel];
}

#pragma mark Load and unload panels
/** @name Load and unload panels */

/**
 *  Load the left panel.
 */
- (void)loadLeftPanel
{
    [self loadPanelForSide:MSSPSideDisplayedLeft];
}

/**
 *  Load the panel corresponding to the side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 */
- (void)loadPanelForSide:(MSSPSideDisplayed)side
{
    UIViewController    *controller;
    CGRect              frame;
    
    if (side == MSSPSideDisplayedNone)
        return ;
    
    if (![self panelControllerForSide:side] || [self sideDisplayed] == side)
        return ;
    
    if ([self sideDisplayed] != MSSPSideDisplayedNone)
        [self unloadPanelForSide:[self sideDisplayed]];
    
    if (side == MSSPSideDisplayedLeft)
    {
        [[[self leftPanelController] view] setFrame:CGRectMake(0, 0, [self leftPanelMaximumWidth], [[self view] bounds].size.height)];
        [[[self leftPanelController] view] setAutoresizingMask:(UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleRightMargin)];
    }
    else
    {
        [[[self rightPanelController] view] setFrame:CGRectMake([[self view] bounds].size.width - [self rightPanelMaximumWidth], 0, [self rightPanelMaximumWidth], [[self view] bounds].size.height)];
        [[[self rightPanelController] view] setAutoresizingMask:(UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleLeftMargin)];
    }
    
    [[self view] addSubview:[[self panelControllerForSide:side] view]];
    [[self view] sendSubviewToBack:[[self panelControllerForSide:side] view]];
    
    if ([[self parentViewController] isKindOfClass:[UITabBarController class]] &&
        [controller isKindOfClass:[UINavigationController class]] &&
        ![[(UITabBarController *)[self parentViewController] tabBar] isTranslucent])
    {
        frame = [[controller view] frame];
        frame.size.height += [[(UITabBarController *)[self parentViewController] tabBar] frame].size.height;
        [[controller view] setFrame:frame];
    }
    
    [self setSideDisplayed:side];
}

/**
 *  Load the right panel.
 */
- (void)loadRightPanel
{
    [self loadPanelForSide:MSSPSideDisplayedRight];
}

/**
 *  Unload the left panel.
 */
- (void)unloadLeftPanel
{
    [self unloadPanelForSide:MSSPSideDisplayedLeft];
}

/**
 *  Unload the panel corresponding to the side.
 *
 *  @param side The side. Must not be `MSSPSideDisplayedNone`.
 */
- (void)unloadPanelForSide:(MSSPSideDisplayed)side
{
    if (side == MSSPSideDisplayedNone)
        return ;
    
    if (![self panelControllerForSide:side] || [self sideDisplayed] != side)
        return ;
    
    [[[self panelControllerForSide:side] view] removeFromSuperview];
    
    [self setSideDisplayed:MSSPSideDisplayedNone];
}

/**
 *  Unload the right panel.
 */
- (void)unloadRightPanel
{
    [self unloadPanelForSide:MSSPSideDisplayedRight];
}

#pragma mark Actions
/** @name Actions */

/**
 *  Close the opened panel.
 */
- (void)closePanel
{
    [self closePanelWithCompletion:nil];
}

/**
 *  Close the opened panel.
 *
 *  @param completion A block object to be executed when the panel is closed.
 */
- (void)closePanelWithCompletion:(void (^)(void))completion
{
    void    (^animationBlock)(void);
    CGFloat animationLength;
    CGSize  centerViewSize;
    void    (^completionBlock)(BOOL);
    
    if (![self centerViewController] || [self sideDisplayed] == MSSPSideDisplayedNone)
        return ;
    
    centerViewSize = [[self centerView] frame].size;

    animationBlock = ^()
    {
        if ([self statusBarDisplayedSmoothlyForSide:[self sideDisplayed]])
            [[self statusBarView] setBackgroundColor:[self statusBarColorForSide:MSSPSideDisplayedNone]];
        
        [[self centerView] setFrame:CGRectMake(0, 0, centerViewSize.width, centerViewSize.height)];
    };
    
    completionBlock = ^(BOOL finished)
    {
        if (finished)
        {
            if ([[self delegate] respondsToSelector:@selector(slidingPanelController:hasClosedSide:)])
                [[self delegate] slidingPanelController:self hasClosedSide:[self sideDisplayed]];
            
            [self unloadPanelForSide:[self sideDisplayed]];
            [self adjustStatusBarColor];
            
            if (completion)
                completion();
        }
    };
    
    if ([self sideDisplayed] == MSSPSideDisplayedLeft)
        animationLength = [[self centerView] frame].origin.x;
    else
        animationLength = -[[self centerView] frame].origin.x;
    
    [UIView animateWithDuration:[self animationDurationForLength:animationLength] animations:animationBlock completion:completionBlock];
}

/**
 *  Open the left panel.
 */
- (void)openLeftPanel
{
    [self openLeftPanelWithCompletion:nil];
}

/**
 *  Open the left panel.
 *
 *  @param completion A block object to be executed when the panel is opened.
 */
- (void)openLeftPanelWithCompletion:(void (^)(void))completion
{
    [self openPanelSide:MSSPSideDisplayedLeft withCompletion:completion];
}

/**
 *  Open the panel corresponding to the side.
 *
 *  @param side       The side. Must not be `MSSPSideDisplayedNone`.
 *  @param completion A block object to be executed when the panel is opened.
 */
- (void)openPanelSide:(MSSPSideDisplayed)side withCompletion:(void (^)(void))completion
{
    [self openPanelSide:side withCompletion:completion andStatusBarColorUpdate:YES];
}

/**
 *  Open the panel corresponding to the side.
 *
 *  @param side                 The side. Must not be `MSSPSideDisplayedNone`.
 *  @param completion           A block object to be executed when the panel is opened.
 *  @param statusBarColorUpdate A value which indicate if the status bar color must be updated before the animation.
 */
- (void)openPanelSide:(MSSPSideDisplayed)side withCompletion:(void (^)(void))completion andStatusBarColorUpdate:(BOOL)statusBarColorUpdate
{
    void                (^animationBlock)(void);
    CGSize              centerViewSize;
    void                (^completionBlock)(BOOL);
    void                (^openPanelBlock)();
    UIViewController    *panelController;
 
    NSParameterAssert(side != MSSPSideDisplayedNone);
    
    if (!(panelController = [self panelControllerForSide:side]))
        return ;
    
    centerViewSize = [[self centerView] frame].size;
    
    animationBlock = ^()
    {
        CGRect  frame;
        CGFloat x;
        
        if ([self statusBarDisplayedSmoothlyForSide:side])
            [[self statusBarView] setBackgroundColor:[self statusBarColorForSide:side]];
        
        if ([[panelController view] frame].size.width != [self panelMaximumWithForSide:side])
        {
            frame = [[panelController view] frame];
            frame.size.width = [self panelMaximumWithForSide:side];
            
            if (side == MSSPSideDisplayedRight)
                frame.origin.x = [[self centerView] frame].size.width - [self panelMaximumWithForSide:side];
                
            [[panelController view] setFrame:frame];
        }
        
        if (side == MSSPSideDisplayedLeft)
            x = [self panelMaximumWithForSide:side];
        else
            x = - (CGFloat)[self panelMaximumWithForSide:side];
        
        [[self centerView] setFrame:CGRectMake(x, 0, centerViewSize.width, centerViewSize.height)];
    };
    
    completionBlock = ^(BOOL finished)
    {
        if (finished)
        {
            if ([[self delegate] respondsToSelector:@selector(slidingPanelController:hasOpenedSide:)])
                [[self delegate] slidingPanelController:self hasOpenedSide:side];
            
            if (completion)
                completion();
        }
    };
    
    openPanelBlock = ^()
    {
        CGFloat animationLength;
        
        [self loadPanelForSide:side];
        
        if (statusBarColorUpdate)
            [self adjustStatusBarColor];
        
        if ([[self centerView] frame].origin.x == 0 && [[self delegate] respondsToSelector:@selector(slidingPanelController:beginsToBringOutSide:)])
            [[self delegate] slidingPanelController:self beginsToBringOutSide:side];
        
        if (side == MSSPSideDisplayedLeft)
            animationLength = [self leftPanelMaximumWidth] - [[self centerView] frame].origin.x;
        else
            animationLength = [self rightPanelMaximumWidth] + [[self centerView] frame].origin.x;
        
        [UIView animateWithDuration:[self animationDurationForLength:animationLength] animations:animationBlock completion:completionBlock];
    };
    
    if ([self sideDisplayed] != MSSPSideDisplayedNone && [self sideDisplayed] != side)
        [self closePanelWithCompletion:openPanelBlock];
    else
        openPanelBlock();
}

/**
 *  Open the right panel.
 */
- (void)openRightPanel
{
    [self openRightPanelWithCompletion:nil];
}

/**
 *  Open the right panel.
 *
 *  @param completion A block object to be executed when the panel is opened.
 */
- (void)openRightPanelWithCompletion:(void (^)(void))completion
{
    [self openPanelSide:MSSPSideDisplayedRight withCompletion:completion];
}

#pragma mark Storyboard
/** @name Storyboard */

/**
 *  Notifies the view controller that a segue is about to be performed.
 *
 *  @param segue  The segue object containing information about the view controllers involved in the segue.
 *  @param sender The object that initiated the segue. You might use this parameter to perform different actions based on which control (or other object) initiated the segue.
 */
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)__unused sender
{
    if ([segue.identifier isEqualToString:MSSPStoryboardIDCenter])
        [self setCenterViewController:[segue destinationViewController]];
    else if ([segue.identifier isEqualToString:MSSPStoryboardIDLeft])
        [self setLeftPanelController:[segue destinationViewController]];
    else if ([segue.identifier isEqualToString:MSSPStoryboardIDRight])
        [self setRightPanelController:[segue destinationViewController]];
}

/**
 *  Sets views from storyboard.
 */
- (void)setViewsFromStoryboard
{
    @try
    {
        [self performSegueWithIdentifier:MSSPStoryboardIDCenter sender:nil];
    }
    @catch (NSException *exception){}
    
    @try
    {
        [self performSegueWithIdentifier:MSSPStoryboardIDLeft sender:nil];
    }
    @catch (NSException *exception){}
    
    @try
    {
   //     [self performSegueWithIdentifier:MSSPStoryboardIDRight sender:nil];
    }
    @catch (NSException *exception){}
}

@end
