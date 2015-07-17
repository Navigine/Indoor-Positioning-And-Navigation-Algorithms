//
//  AFPickerView.m
//  PickerView
//
//  Created by Fraerman Arkady on 24.11.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "AFPickerView.h"

@implementation AFPickerView

#pragma mark - Synthesization

@synthesize dataSource;
@synthesize delegate;
@synthesize selectedRow = currentRow;
@synthesize rowFont = _rowFont;
@synthesize rowIndent = _rowIndent;




#pragma mark - Custom getters/setters

- (void)setSelectedRow:(int)selectedRow
{
  if (selectedRow >= rowsCount)
    return;
  
  currentRow = selectedRow;
  [contentView setContentOffset:CGPointMake(0.0, 39.0 * currentRow) animated:NO];
}




- (void)setRowFont:(UIFont *)rowFont
{
  _rowFont = rowFont;
  
  for (UILabel *aLabel in visibleViews)
  {
    aLabel.font = _rowFont;
  }
  
  for (UILabel *aLabel in recycledViews)
  {
    aLabel.font = _rowFont;
  }
}




- (void)setRowIndent:(CGFloat)rowIndent
{
  _rowIndent = rowIndent;
  
  for (UILabel *aLabel in visibleViews)
  {
    CGRect frame = aLabel.frame;
    frame.origin.x = _rowIndent;
    frame.size.width = self.frame.size.width - _rowIndent;
    aLabel.frame = frame;
  }
  
  for (UILabel *aLabel in recycledViews)
  {
    CGRect frame = aLabel.frame;
    frame.origin.x = _rowIndent;
    frame.size.width = self.frame.size.width - _rowIndent;
    aLabel.frame = frame;
  }
}




#pragma mark - Initialization

- (id)initWithFrame:(CGRect)frame
{
  self = [super initWithFrame:frame];
  if (self)
  {
    // setup
    [self setup];
    
    // backgound
    UIImageView *bacground = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"pickerBackground.png"]];
    bacground.alpha = 0.5f;
    [self addSubview:bacground];
    
    // glass
    UIImage *glassImage = [UIImage imageNamed:@"pickerGlass.png"];
    glassImageView = [[UIImageView alloc] initWithFrame:CGRectMake(0.0, 76.0, glassImage.size.width, glassImage.size.height)];
    glassImageView.image = glassImage;
    [self addSubview:glassImageView];
    
    // content
    contentView = [[UIScrollView alloc] initWithFrame:CGRectMake(0.0, 0.0, frame.size.width, frame.size.height)];
    contentView.showsHorizontalScrollIndicator = NO;
    contentView.showsVerticalScrollIndicator = NO;
    contentView.delegate = self;
    [self addSubview:contentView];
    [self bringSubviewToFront:contentView];
    
    UITapGestureRecognizer *tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(didTap:)];
    [contentView addGestureRecognizer:tapRecognizer];
    
    
    // shadows
    //        UIImageView *shadows = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"pickerShadows.png"]];
    //        [self addSubview:shadows];
    
    
  }
  return self;
}




- (void)setup
{
  _rowFont = [UIFont boldSystemFontOfSize:24.0];
  _rowIndent = 30.0;
  
  currentRow = 0;
  rowsCount = 0;
  visibleViews = [[NSMutableSet alloc] init];
  recycledViews = [[NSMutableSet alloc] init];
}




#pragma mark - Buisness

- (void)reloadData
{
  // empry views
  currentRow = 0;
  rowsCount = 0;
  
  for (UIView *aView in visibleViews)
    [aView removeFromSuperview];
  
  for (UIView *aView in recycledViews)
    [aView removeFromSuperview];
  
  visibleViews = [[NSMutableSet alloc] init];
  recycledViews = [[NSMutableSet alloc] init];
  
  rowsCount = [dataSource numberOfRowsInPickerView:self];
  if (rowsCount) {
    [delegate pickerView:self didSelectRow:0];
    [contentView setContentOffset:CGPointMake(0.0, 0.0) animated:NO];
    contentView.contentSize = CGSizeMake(contentView.frame.size.width, 39.0 * rowsCount + 4 * 39.0);
    [self tileViews];
  }
}




- (void)determineCurrentRow
{
  CGFloat delta = contentView.contentOffset.y;
  int position = round(delta / 39.0);
  currentRow = position;
  [contentView setContentOffset:CGPointMake(0.0, 39.0 * position) animated:YES];
  [delegate pickerView:self didSelectRow:currentRow];
}




- (void)didTap:(id)sender
{
  UITapGestureRecognizer *tapRecognizer = (UITapGestureRecognizer *)sender;
  CGPoint point = [tapRecognizer locationInView:self];
  int steps = floor(point.y / 39) - 2;
  [self makeSteps:steps];
}




- (void)makeSteps:(int)steps
{
  if (steps == 0 || steps > 2 || steps < -2)
    return;
  
  [contentView setContentOffset:CGPointMake(0.0, 39.0 * currentRow) animated:NO];
  
  int newRow = currentRow + steps;
  if (newRow < 0 || newRow >= rowsCount)
  {
    if (steps == -2)
      [self makeSteps:-1];
    else if (steps == 2)
      [self makeSteps:1];
    
    return;
  }
  
  currentRow = currentRow + steps;
  [contentView setContentOffset:CGPointMake(0.0, 39.0 * currentRow) animated:YES];
  [delegate pickerView:self didSelectRow:currentRow];
}




#pragma mark - recycle queue

- (UIView *)dequeueRecycledView
{
  UIView *aView = [recycledViews anyObject];
  
  if (aView)
    [recycledViews removeObject:aView];
  return aView;
}



- (BOOL)isDisplayingViewForIndex:(NSUInteger)index
{
  BOOL foundPage = NO;
  for (UIView *aView in visibleViews)
  {
    int viewIndex = aView.frame.origin.y / 39.0 - 2;
    if (viewIndex == index)
    {
      foundPage = YES;
      break;
    }
  }
  return foundPage;
}




- (void)tileViews
{
  // Calculate which pages are visible
  CGRect visibleBounds = contentView.bounds;
  int firstNeededViewIndex = floorf(CGRectGetMinY(visibleBounds) / 39.0) - 2;
  int lastNeededViewIndex  = floorf((CGRectGetMaxY(visibleBounds) / 39.0)) - 2;
  firstNeededViewIndex = MAX(firstNeededViewIndex, 0);
  lastNeededViewIndex  = MIN(lastNeededViewIndex, rowsCount - 1);
  
  // Recycle no-longer-visible pages
  for (UIView *aView in visibleViews)
  {
    int viewIndex = aView.frame.origin.y / 39 - 2;
    if (viewIndex < firstNeededViewIndex || viewIndex > lastNeededViewIndex)
    {
      [recycledViews addObject:aView];
      [aView removeFromSuperview];
    }
  }
  
  [visibleViews minusSet:recycledViews];
  
  // add missing pages
  for (int index = firstNeededViewIndex; index <= lastNeededViewIndex; index++)
  {
    if (![self isDisplayingViewForIndex:index])
    {
      UILabel *label = (UILabel *)[self dequeueRecycledView];
      
      if (label == nil)
      {
        label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 39, 39.0)];
        label.textAlignment = NSTextAlignmentCenter;
        label.backgroundColor = [UIColor clearColor];
        label.font = self.rowFont;
        label.textColor = RGBACOLOR(0.0, 0.0, 0.0, 0.75);
      }
      
      [self configureView:label atIndex:index];
      [contentView addSubview:label];
      [visibleViews addObject:label];
    }
  }
}




- (void)configureView:(UIView *)view atIndex:(NSUInteger)index
{
  UILabel *label = (UILabel *)view;
  label.text = [dataSource pickerView:self titleForRow:index];
  CGRect frame = label.frame;
  frame.origin.y = 39.0 * index + 78.0;
  label.frame = frame;
}




#pragma mark - UIScrollViewDelegate

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
  [self tileViews];
}




- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{
  if (!decelerate)
    [self determineCurrentRow];
}




- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
  [self determineCurrentRow];
}

@end
