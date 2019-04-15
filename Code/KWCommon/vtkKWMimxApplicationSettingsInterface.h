/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxApplicationSettingsInterface.h,v $
Language:  C++
Date:      $Date: 2013/01/21 02:13:36 $
Version:   $Revision: 1.3 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkKWMimxApplicationSettingsInterface
// .SECTION Description
// Slicer specific ApplicationSettingsInterface. Provides a user
// interface to set/view application settings.
//

#ifndef __vtkKWMimxApplicationSettingsInterface_h
#define __vtkKWMimxApplicationSettingsInterface_h

//KWWidgets Headers
#include "vtkKWApplicationSettingsInterface.h"

// MIMX Headers
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWFrameWithLabel;
class vtkKWEntryWithLabel;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWCheckButton;
class vtkKWRadioButtonSet;
class vtkKWCheckButtonWithLabel;
class vtkKWScaleWithLabel;
class vtkKWSeparator;
class vtkKWEntryWithLabel;
class vtkKWScaleWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxApplicationSettingsInterface: public vtkKWApplicationSettingsInterface
{
public:
  static vtkKWMimxApplicationSettingsInterface* New();
  vtkTypeRevisionMacro(vtkKWMimxApplicationSettingsInterface,vtkKWApplicationSettingsInterface);
  vtkGetObjectMacro (FontSizeButtons, vtkKWRadioButtonSet );
  vtkGetObjectMacro (FontFamilyButtons, vtkKWRadioButtonSet );

  // Description:
  // Create the widget.
  virtual void Create();
  
  // Description:
  // Refresh the interface given the current value of the Window and its
  // views/composites/widgets.
  virtual void Update();

  // Description:
  // Set the default working directory for the program
  void SetWorkingDirectory( char *directoryName );
  
  // Description:
  // Callbacks for the Module application settings. Internal, do not use.
  
  // Description:
  // Callbacks for the Slicer font settings.
  virtual void SetFontFamilyCallback ( );
  virtual void SetFontSizeCallback ( );
  
  // Description:
  // Callbacks for the IA-FEMesh settings.
  virtual void AutoSaveModeCallback(int mode);
  virtual void AutoSaveScaleCallback(double value);
  virtual void AutoSaveDirectoryModeCallback(int mode);
  virtual void AutoSaveDirectoryCallback( );
  virtual void AverageElementLengthCallback(char *value);
  virtual void ABAQUSPrecisionCallback(double value);
  virtual void ImageResolutionCallback(char *value);
  
  
protected:
  vtkKWMimxApplicationSettingsInterface();
  ~vtkKWMimxApplicationSettingsInterface();

  
private:
  vtkKWMimxApplicationSettingsInterface(const vtkKWMimxApplicationSettingsInterface&); // Not implemented
  void operator=(const vtkKWMimxApplicationSettingsInterface&); // Not Implemented

  vtkKWFrameWithLabel *MimxSettingsFrame;
  vtkKWCheckButtonWithLabel *autoSaveButton;
  vtkKWScaleWithLabel *autoSaveScale;
  vtkKWCheckButtonWithLabel *workingDirButton;
  vtkKWLoadSaveButtonWithLabel *autoSaveDir;
  
  /* Font Configuration */
  vtkKWFrameWithLabel *FontSettingsFrame;
  vtkKWRadioButtonSet *FontSizeButtons;
  vtkKWRadioButtonSet *FontFamilyButtons;
  vtkKWSeparator *Separator;
  vtkKWEntryWithLabel *AverageElementLengthEntry;
  vtkKWScaleWithLabel *PropertyPrecisionScale;
  vtkKWEntryWithLabel *ImageResolutionEntry;
};


#endif
