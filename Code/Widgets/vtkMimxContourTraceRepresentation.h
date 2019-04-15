
#ifndef __vtkMimxContourTraceRepresentation_h
#define __vtkMimxContourTraceRepresentation_h

#include "vtkMimxWidgetsWin32Header.h"
#include "vtkContourRepresentation.h"

class VTK_WIDGETS_EXPORT vtkMimxContourTraceRepresentation : public vtkContourRepresentation
{
  friend class vtkMimxContourTraceWidget;

public:
    static vtkMimxContourTraceRepresentation *New();
	vtkTypeRevisionMacro(vtkMimxContourTraceRepresentation,vtkContourRepresentation);

protected:
	vtkMimxContourTraceRepresentation();
	~vtkMimxContourTraceRepresentation();
};

#endif


