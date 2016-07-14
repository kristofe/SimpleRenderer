//
//  OpenGL.h
//  VolumeRenderIOS
//
//  Created by Kristofer Schlachter on 5/15/14.
//
//

//#ifndef OpenGLHelper_H
//#define OpenGLHelper_H

#define GLFW_INCLUDE_COREARB
/*
#ifdef TARGETIPHONE
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
    #include "GLFW/glfw3.h"
    #include "glutil.h"
#elif TARGETMAC
*/
#ifdef __APPLE__
//    #include <GL/glew.h>
    #include "GLFW/glfw3.h"
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
    //#include "gl_desktop_util.h"
    #include "glutil.h"
#elif WIN32
    #include <GL/glew.h>
    #include "GLFW/glfw3.h"
    #include "glutil.h"
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
    #include "GLFW/glfw3.h"
    #include "glutil.h"
#endif



//#endif //OpenGLHelper_H
