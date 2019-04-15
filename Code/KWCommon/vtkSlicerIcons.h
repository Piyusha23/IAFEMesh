#ifndef __vtkSlicerIcons_h
#define __vtkSlicerIcons_h

// KWWidget Headers
#include "vtkKWObject.h"
#include "vtkKWResourceUtilities.h"
#include "vtkKWIcon.h"

// MIMX Headers
#include "vtkMimxKWCommonWin32Header.h"

// Description:
// This class is defined in anticipation of superclass methods that
// all Slicer Icons will use...

class VTK_MIMXKWCOMMON_EXPORT vtkSlicerIcons : public vtkKWObject
{
 public:
    static vtkSlicerIcons* New ( );
    vtkTypeMacro ( vtkSlicerIcons, vtkKWObject);
    void PrintSelf ( ostream& os, vtkIndent indent );

 protected:
    vtkSlicerIcons ( );
    virtual ~vtkSlicerIcons ( );
    vtkSlicerIcons (const vtkSlicerIcons& ); // Not implemented
    void operator = ( const vtkSlicerIcons& ); // Not implemented
    
 private:
    
};

#endif
