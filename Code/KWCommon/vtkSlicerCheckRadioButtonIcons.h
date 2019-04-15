#ifndef __vtkSlicerCheckRadioButtonIcons_h
#define __vtkSlicerCheckRadioButtonIcons_h

// KWWidget Headers
#include "vtkKWObject.h"
#include "vtkKWResourceUtilities.h"
#include "vtkKWIcon.h"

// SLICER Headers
#include "vtkSlicerIcons.h"
#include "./Resources/vtkSlicerCheckRadioButton_ImageData.h"

// MIMX Headers
#include "vtkMimxKWCommonWin32Header.h"

class VTK_MIMXKWCOMMON_EXPORT vtkSlicerCheckRadioButtonIcons : public vtkSlicerIcons
{
 public:
  static vtkSlicerCheckRadioButtonIcons* New();
  vtkTypeRevisionMacro (vtkSlicerCheckRadioButtonIcons, vtkSlicerIcons);
  void PrintSelf ( ostream& os, vtkIndent indent);
  vtkGetObjectMacro ( DisabledIcon, vtkKWIcon );
  vtkGetObjectMacro ( DeselectIcon, vtkKWIcon );
  vtkGetObjectMacro ( SelectCheckIcon, vtkKWIcon );
  vtkGetObjectMacro ( SelectRadioIcon, vtkKWIcon );
  
  virtual void AssignImageDataToIcons ( );
  
 protected:
  vtkSlicerCheckRadioButtonIcons ( );
  virtual ~vtkSlicerCheckRadioButtonIcons ( );
  
  vtkKWIcon *DeselectIcon;
  vtkKWIcon *SelectCheckIcon;
  vtkKWIcon *SelectRadioIcon;
  vtkKWIcon *DisabledIcon;

 private:
  vtkSlicerCheckRadioButtonIcons (const vtkSlicerCheckRadioButtonIcons&); // Not implemented
  void operator = (const vtkSlicerCheckRadioButtonIcons&); // Not implemented

};
#endif
