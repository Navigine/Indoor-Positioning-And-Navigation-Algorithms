//
//  Sublocation.h
//  NavigineSDK
//
//  Created by Администратор on 27/04/15.
//  Copyright (c) 2015 Navigine. All rights reserved.
//

#ifndef NavigineSDK_Sublocation_h
#define NavigineSDK_Sublocation_h

#endif

@interface Sublocation :NSObject{
}

@property (nonatomic) NSInteger id;
@property (nonatomic,strong)   NSString  *name;

@property (nonatomic,strong)   NSString  *svgFile;
@property (nonatomic,strong)   NSString  *pngFile;
@property (nonatomic,strong)   NSString  *jpgFile;

@property (nonatomic,strong)   NSData    *svgImage;
@property (nonatomic,strong)   NSData    *pngImage;
@property (nonatomic,strong)   NSData    *jpgImage;

@property (nonatomic) float     width;
@property (nonatomic) float     height;
@property (nonatomic) float     azimuth;
@property (nonatomic) double    gpsLatitude;
@property (nonatomic) double    gpsLongitude;

@property (nonatomic,strong) NSString    *archiveFile;

-(NSArray *)getGpsCoordinates: (float)x :(float)y;
@end