//  MSViewControllerSlidingPanel.m
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

#import "MSViewControllerSlidingPanel.h"

#pragma mark - Implementation

@implementation UIViewController (MSSlidingPanel)

#pragma mark Getting other related view controllers
/** @name Getting other related view controllers */

/**
 *  The nearest ancestor in the view controller hierarchy that is a sliding controller.
 *
 *  If the receiver or one of its ancestors is a child of a sliding controller, this property contains the owning sliding controller. This property is nil if the view controller is not embedded inside a navigation controller.
 */
- (MSSlidingPanelController *)slidingPanelController
{
    UIViewController    *parentViewController;
    
    parentViewController = [self parentViewController];
    while (parentViewController)
    {
        if ([parentViewController isKindOfClass:[MSSlidingPanelController class]])
            return ((MSSlidingPanelController *) parentViewController);
        
        parentViewController = [parentViewController parentViewController];
    }
    
    return (nil);
}

@end
