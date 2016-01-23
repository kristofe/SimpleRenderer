/*
 *  TextureLoader.h
 *  iPhone
 *
 *  Created by Kristofer Schlachter on 8/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#import <UIKit/UIKit.h>
@interface TextureLoader : NSObject{
@public
	char* pathToImage;
	unsigned int** imgData;
	int* pwidth;
	int* pheight;
}

- (void) GetTextureThreaded;

@end