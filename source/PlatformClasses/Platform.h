/*
 *  Platform.h
 *  GWEngine_1.0
 *
 *  Created by Kristofer Schlachter on 12/26/08.
 *  Copyright 2008 Gotham Wave Games, Inc. All rights reserved.
 *
 */

#ifdef TARGETIPHONE

#include "Platform_iPhone.h"
#endif
#ifdef TARGETMAC

#include "Platform_Mac.h"
#endif
#ifdef WIN32
	#include "Platform_Win32.h"
#endif
#ifdef __unix__
	#include "Platform_Unix.h"
#endif


#include "PlatformAll.h"

