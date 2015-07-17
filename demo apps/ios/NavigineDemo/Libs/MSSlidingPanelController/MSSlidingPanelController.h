//  MSSlidingPanelController.h
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

#import <UIKit/UIKit.h>

#pragma mark - Enumerations & options

/**
 *  These values are used to indicate the interaction mode with the center view.
 */
typedef NS_ENUM(NSUInteger, MSSPCenterViewInteraction)
{
    /**
     *  There is no interaction with the center view.
     */
    MSSPCenterViewInteractionNone,
    
    /**
     *  The user can interact only with the navigation bar of the center view.
     *  If there is no navigation bar, no interaction with the center view are possible.
     */
    MSSPCenterViewInteractionNavBar,
    
    /**
     *  The user can interact view all the center view.
     */
    MSSPCenterViewInteractionFullView,
};

/**
 *  These options are used to set the gestures to close a panel.
 */
typedef NS_OPTIONS(NSUInteger, MSSPCloseGestureMode)
{
    /**
     *  No gesture are used.
     */
    MSSPCloseGestureModeNone        =   0,
    
    /**
     *  The user can slide the content part of the central view.
     */
    MSSPCloseGestureModePanContent  =   1 << 0,
    
    /**
     *  The user can slide the nav bar of the central view.
     */
    MSSPCloseGestureModePanNavBar   =   1 << 1,
    
    /**
     *  The user can tap the content part of the central view.
     */
    MSSPCloseGestureModeTapContent  =   1 << 2,
    
    /**
     *  The user can tap the nav bar of the central view.
     */
    MSSPCloseGestureModeTapNavBar   =   1 << 3,
    
    /**
     *  The user can use every previous gestures.
     */
    MSSPCloseGestureModeAll         =   MSSPCloseGestureModePanContent  |
                                        MSSPCloseGestureModePanNavBar   |
                                        MSSPCloseGestureModeTapContent  |
                                        MSSPCloseGestureModeTapNavBar,
};

/**
 *  These options are used to set the gestures to open a panel.
 */
typedef NS_OPTIONS(NSUInteger, MSSPOpenGestureMode)
{
    /**
     *  No gesture are used.
     */
    MSSPOpenGestureModeNone         =   0,
    
    /**
     *  The user can slide the content part of the central view.
     */
    MSSPOpenGestureModePanContent   =   1 << 0,
    
    /**
     *  The user can slide the nav bar of the central view.
     */
    MSSPOpenGestureModePanNavBar    =   1 << 1,
    
    /**
     *  The user can use every previous gestures.
     */
    MSSPOpenGestureModeAll          =   MSSPOpenGestureModePanContent   |
                                        MSSPOpenGestureModePanNavBar,
};

/**
 *  These values are used to know which panel side is currently displayed.
 */
typedef NS_ENUM(NSUInteger, MSSPSideDisplayed)
{
    /**
     *  No panel is displayed.
     */
    MSSPSideDisplayedNone,
    
    /**
     *  The left panel is displayed.
     */
    MSSPSideDisplayedLeft,
    
    /**
     *  The right panel is displayed.
     */
    MSSPSideDisplayedRight,
};

#pragma mark - Protocol declaration

@protocol MSSlidingPanelControllerDelegate;

#pragma mark - Interface

/**
 *  The MSSlidingPanelController provides a class which allow to display sliding panels.
 *  
 *  The sliding panel controller allows to set two panels: one on the left, and one on the right. However, only one panel can be displayed at the same time.
 *  If you want to use the Storyboard, use custom segues with the following identifiers: MSSPStoryboardIDCenter, MSSPStoryboardIDLeft, MSSPStoryboardIDRight.
 */
@interface MSSlidingPanelController : UIViewController

#pragma mark Initialization
/** @name Initialization */

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *
 *  @return The intialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController;

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *  @param leftPanelController  The left panel controller.
 *
 *  @return The intialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController andLeftPanelController:(UIViewController *)leftPanelController;

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *  @param rightPanelController The right panel controller.
 *
 *  @return The intialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController andRightPanelController:(UIViewController *)rightPanelController;

/**
 *  Initialize and return a new sliding panel controller.
 *
 *  @param centerViewController The center view controller. Must not be `nil`.
 *  @param leftPanelController  The left panel controller.
 *  @param rightPanelController The right panel controller.
 *
 *  @return The intialized centrer view controller.
 */
- (id)initWithCenterViewController:(UIViewController *)centerViewController leftPanelController:(UIViewController *)leftPanelController andRightPanelController:(UIViewController *)rightPanelController;

#pragma mark Center view settings
/** @name Center view settings */

/**
 *  The center view controller.
 *  
 *  By default, this value is `nil`.
 */
@property (nonatomic, strong)           UIViewController                        *centerViewController;

#pragma mark Left panel settings
/** @name Left panel settings */

/**
 *  The interaction mode with the center view controller when the left panel is open.
 *
 *  By default, this value is `MSSPCenterViewInteractionNavBar`.
 */
@property (nonatomic, assign)           MSSPCenterViewInteraction               leftPanelCenterViewInteractionMode;

/**
 *  The gestures which allow to close the left panel.
 *
 *  By default, this value is `MSSPCloseGestureModeAll`.
 */
@property (nonatomic, assign)           MSSPCloseGestureMode                    leftPanelCloseGestureMode;

/**
 *  The left panel controller.
 *
 *  By default, this value is `nil`.
 */
@property (nonatomic, strong)           UIViewController                        *leftPanelController;

/**
 *  The maximum width of the left panel.
 *
 *  By default, this value is 280.
 */
@property (nonatomic, assign)           CGFloat                                 leftPanelMaximumWidth;

/**
 *  The gestures which allow to open the left panel.
 *
 *  By default, this value is `MSSPOpenGestureModeAll`.
 */
@property (nonatomic, assign)           MSSPOpenGestureMode                     leftPanelOpenGestureMode;

/**
 *  The color of the left panel status bar.
 *
 *  By default, this value is clearColor.
 */
@property (nonatomic, strong)           UIColor                                 *leftPanelStatusBarColor;

/**
 *  Indicate if the left status bar color will be displayed smoothly.
 *
 *  By default, this value is `NO`.
 */
@property (nonatomic, assign)           BOOL                                    leftPanelStatusBarDisplayedSmoothly;

/**
 *  Set the maximum width of the left panel with.
 *
 *  @param maximumWidth Panel's maximum width.
 *  @param completion   A block object to be executed when the maximum width is set.
 */
- (void)setLeftPanelMaximumWidth:(CGFloat)leftPanelMaximumWidth withCompletion:(void (^)(void))completion;

#pragma mark Right panel settings
/** @name Right panel settings */

/**
 *  The interaction mode with the center view controller when the right panel is open.
 *
 *  By default, this value is `MSSPCenterViewInteractionNavBar`.
 */
@property (nonatomic, assign)           MSSPCenterViewInteraction               rightPanelCenterViewInteractionMode;

/**
 *  The gestures which allow to close the right panel.
 *
 *  By default, this value is `MSSPCloseGestureModeAll`.
 */
@property (nonatomic, assign)           MSSPCloseGestureMode                    rightPanelCloseGestureMode;

/**
 *  The right panel controller.
 *
 *  By default, this value is `nil`.
 */
@property (nonatomic, strong)           UIViewController                        *rightPanelController;

/**
 *  The maximum width of the right panel.
 *
 *  By default, this value is 280.
 */
@property (nonatomic, assign)           CGFloat                                 rightPanelMaximumWidth;

/**
 *  The gestures which allow to open the right panel.
 *
 *  By default, this value is `MSSPOpenGestureModeAll`.
 */
@property (nonatomic, assign)           MSSPOpenGestureMode                     rightPanelOpenGestureMode;

/**
 *  The color of the right panel status bar.
 *
 *  By default, this value is clearColor.
 */
@property (nonatomic, strong)           UIColor                                 *rightPanelStatusBarColor;

/**
 *  Indicate if the right status bar color will be displayed smoothly.
 *
 *  By default, this value is `NO`.
 */
@property (nonatomic, assign)           BOOL                                    rightPanelStatusBarDisplayedSmoothly;

/**
 *  Set the maximum width of the right panel with.
 *
 *  @param maximumWidth Panel's maximum width.
 *  @param completion   A block object to be executed when the maximum width is set.
 */
- (void)setRightPanelMaximumWidth:(CGFloat)rightPanelMaximumWidth withCompletion:(void (^)(void))completion;

#pragma mark Global settings
/** @name Global settings */

/**
 *  The animation velocity. It indicates of how many pixels per seconds must be move the panel.
 *
 *  By default, this value is 640.
 */
@property (nonatomic, assign)           CGFloat                                 animationVelocity;

/**
 *  The sliding panel controller delegate.
 */
@property (nonatomic, strong)           id <MSSlidingPanelControllerDelegate>   delegate;

#pragma mark Get information about panels
/** @name Get information about panels */

/**
 *  Which panel is displayed.
 */
@property (nonatomic, assign, readonly) MSSPSideDisplayed                       sideDisplayed;

#pragma mark Actions
/** @name Actions */

/**
 *  Close the opened panel.
 */
- (void)closePanel;

/**
 *  Close the opened panel.
 *
 *  @param completion A block object to be executed when the panel is closed.
 */
- (void)closePanelWithCompletion:(void (^)(void))completion;

/**
 *  Open the left panel.
 */
- (void)openLeftPanel;

/**
 *  Open the left panel.
 *
 *  @param completion A block object to be executed when the panel is opened.
 */
- (void)openLeftPanelWithCompletion:(void (^)(void))completion;

/**
 *  Open the right panel.
 */
- (void)openRightPanel;

/**
 *  Open the right panel.
 *
 *  @param completion A block object to be executed when the panel is opened.
 */
- (void)openRightPanelWithCompletion:(void (^)(void))completion;

@end

#pragma mark - Protocol

@protocol MSSlidingPanelControllerDelegate <NSObject>

@optional

/**
 *  Tells the delegate that the specified side begins to bring out.
 *
 *  @param panelController The panel controller.
 *  @param side            The side.
 */
- (void)slidingPanelController:(MSSlidingPanelController *)panelController beginsToBringOutSide:(MSSPSideDisplayed)side;

/**
 *  Tells the delegate that the specified side has been closed.
 *
 *  @param panelController The panel controller.
 *  @param side            The side.
 */
- (void)slidingPanelController:(MSSlidingPanelController *)panelController hasClosedSide:(MSSPSideDisplayed)side;

/**
 *  Tells the delegate that the specified side has been opened.
 *
 *  @param panelController The panel controller.
 *  @param side            The side.
 */
- (void)slidingPanelController:(MSSlidingPanelController *)panelController hasOpenedSide:(MSSPSideDisplayed)side;

/**
 *  Asks the delegate if two gesture recognizers should be allowed to recognize gestures simultaneously.
 *  WARNING : Be careful with this method. Some behaviors can be unexpected.
 *
 *  @param panelController        The panel controller.
 *  @param gestureRecognizer      An instance of a subclass of the abstract base class UIGestureRecognizer. This is gesture recognizer of the panel controller.
 *  @param otherGestureRecognizer An instance of a subclass of the abstract base class UIGestureRecognizer.
 *
 *  @return YES to allow both gestureRecognizer and otherGestureRecognizer to recognize their gestures simultaneously. The default implementation returns NO—no two gestures can be recognized simultaneously.
 */
- (BOOL)slidingPanelController:(MSSlidingPanelController *)panelController gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer;

@end
