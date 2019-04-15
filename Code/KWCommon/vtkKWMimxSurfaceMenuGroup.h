/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSurfaceMenuGroup.h,v $
Language:  C++
Date:      $Date: 2013/01/15 23:05:54 $
Version:   $Revision: 1.8 $

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
// .NAME vtkKWMimxSurfaceMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxSurfaceMenuGroup_h
#define __vtkKWMimxSurfaceMenuGroup_h


// MIMX Headers
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWFileBrowserDialog;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxCreateSurfaceFromImageGroup;
class vtkKWMimxSaveSurfaceGroup;
class vtkKWMimxLoadSurfaceGroup;
class vtkKWMimxLoadSurfaceGroup;
class vtkKWMimxBooleanSurfaceGroup;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSurfaceMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxSurfaceMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSurfaceMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  // Description:
  // Launch GUI for loading a surface
  void LoadSurfaceCallback();
  
  // Description:
  // Launch GUI for saving a surface
  void SaveSurfaceCallback();
  
  // Description:
  // Launch GUI for deleting a surface
  void DeleteSurfaceCallback();

  // Description:
  // Launch GUI for performing boolean operations on a surface
  
  void BooleanOperationsCallback();

  // Description:
  // Launch GUI for creating a surface from an image
  void CreateSurfaceFromImageCallback();
  
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);
  void UpdateObjectLists();
  
  // Description:
  // Setting lists of all the objects for accessing
  vtkGetObjectMacro(LoadSurfaceGroup, vtkKWMimxLoadSurfaceGroup);
  
protected:
	vtkKWMimxSurfaceMenuGroup();
	~vtkKWMimxSurfaceMenuGroup();
  
	virtual void CreateWidget();
  
  vtkKWMimxLoadSurfaceGroup *LoadSurfaceGroup;
  vtkKWMimxSaveSurfaceGroup *SaveSurfaceGroup;
	vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
	vtkKWMimxCreateSurfaceFromImageGroup *CreateSurfaceFromImageGroup;
	vtkKWMimxBooleanSurfaceGroup *BooleanSurfaceGroup;

private:
  vtkKWMimxSurfaceMenuGroup(const vtkKWMimxSurfaceMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxSurfaceMenuGroup&); // Not implemented
};

#endif
