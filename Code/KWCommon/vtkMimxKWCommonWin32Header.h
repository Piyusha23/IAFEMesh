#ifndef __vtkMimxKWCommonWin32Header_h
        #define __vtkMimxKWCommonWin32Header_h

        #include <vtkMimxKWCommonConfigure.h>

        #if defined(WIN32) && !defined(VTKSLICER_STATIC)
                #if defined(mimxKWCOMMON_EXPORTS)
                        #define VTK_MIMXKWCOMMON_EXPORT __declspec( dllexport ) 
                #else
                        #define VTK_MIMXKWCOMMON_EXPORT __declspec( dllimport ) 
                #endif
        #else
                #define VTK_MIMXKWCOMMON_EXPORT 
        #endif
#endif
