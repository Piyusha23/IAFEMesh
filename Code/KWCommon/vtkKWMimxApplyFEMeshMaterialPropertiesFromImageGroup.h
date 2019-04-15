/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.h,v $
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
// .NAME vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup -
// GUI for applying material properties calculated from an image.
// .SECTION Description
// This class will present the user with a user interface to define
// the properties for an element set based on an image. The conversion
// from CT number to modulus is performed based on user specified
// parameters.

#ifndef __vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup_h
#define __vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup_h


// MIMX Headers
#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWComboBoxWithLabel;
class vtkIntArray;
class vtkKWEntryWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMimxDefineElSetGroup;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void ApplyFEMeshMaterialPropertiesFromImageDoneCallback();
  int ApplyFEMeshMaterialPropertiesFromImageApplyCallback();
  void ApplyFEMeshMaterialPropertiesFromImageCancelCallback();
  void UpdateObjectLists();
  void FEMeshSelectionChangedCallback(const char *Selection);
  void ElementSetChangedCallback(const char *Selection);
  void DefineElementSetCallback();
  void BinCheckButtonCallback(int mode);
  void SpecifyRangeButtonCallback(int mode);
  int DefineConversionCallback();
  int ConversionApplyCallback();
  int ConversionCancelCallback();
  
protected:
	vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup();
	~vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup();
	virtual void CreateWidget();
  vtkKWComboBoxWithLabel *ImageListComboBox;
  vtkKWComboBoxWithLabel *FEMeshListComboBox;
  vtkKWComboBoxWithLabel *ElementSetComboBox;
  vtkKWEntryWithLabel *PoissonsRatioEntry;
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWPushButton *DefineElSetButton;
  vtkKWMimxDefineElSetGroup *DefineElementSetDialog;
  vtkKWCheckButton *BinCheckButton;
  vtkKWFrameWithLabel *BinFrame;
  vtkKWCheckButtonWithLabel *SpecifyRangeButton;
  vtkKWEntryWithLabel *MinimumBinEntry;
  vtkKWEntryWithLabel *MaximumBinEntry;
  vtkKWEntryWithLabel *NumOfBinsEntry;
  vtkKWFrameWithLabel *ImageConstantFrame;
  vtkKWEntryWithLabel *ImageConstantA;
  vtkKWEntryWithLabel *ImageConstantB;
  vtkKWEntryWithLabel *ImageConstantC;
  vtkKWTopLevel *ConversionDialog;
  vtkKWLabel *EquationLabel;
  vtkKWPushButton *DialogApplyButton;
  vtkKWPushButton *DialogCancelButton;
  vtkKWPushButton *DefineConversionButton;
  vtkKWMenuButtonWithLabel *ImageConversionType;
  char elementSetPrevious[256];
  
private:
  vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup(const vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup&); // Not implemented
  void operator=(const vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup&); // Not implemented
  
  char meshName[64];
  char elementSetName[64];
  double ConversionA;
  double ConversionB;
  double ConversionC;
  char ConversionType[64];
  
};

#endif

