/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxBBMenuGroup.h,v $
Language:  C++
Date:      $Date: 2013/01/15 23:05:54 $
Version:   $Revision: 1.5 $

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
// .NAME vtkKWMimxFEMeshMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxBBMenuGroup_h
#define __vtkKWMimxBBMenuGroup_h


// MIMX Headers
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWMimxCreateBBFromBoundsGroup;
class vtkKWMimxCreateBBMeshSeedGroup;
class vtkKWMimxEditBBGroup;
class vtkKWMimxCreateFEMeshFromBBGroup;
class vtkKWMimxViewProperties;
class vtkKWMimxSaveBBGroup;
class vtkKWMimxEditBBMeshSeedGroup;
class vtkKWMimxSaveVTKFEMeshGroup;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxEditFEMeshLaplacianSmoothGroup;
class vtkKWMimxCreateExtrudeFEMeshGroup;
class vtkMimxUnstructuredGridActor;
class vtkKWMimxNodeElementNumbersGroup;
class vtkKWMimxMergeMeshGroup;
class vtkKWMimxExportAbaqusFEMeshGroup;
class vtkKWMimxMergeBBGroup;
class vtkKWMimxCreateElementSetPickFaceGroup;
class vtkMimxUnstructuredGridActor;
class vtkKWMimxLoadBBGroup;
class vtkUnstructuredGrid;
class vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup;
class vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup;
class vtkKWMimxSurfaceMenuGroup;
class vtkKWMimxCreateBBFromRubberBandPickGroup;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxBBMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxBBMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxBBMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  
  // Description:
  // Launch GUI for loading a Building Block
  void LoadBBCallback();
  
  // Description:
  // Launch GUI for Creating a Building Block
  void CreateBBCallback();
  
  // Description:
  // Launch GUI for deleting a Building Block
  void DeleteBBCallback();
  
  // Description:
  // Launch GUI for Creating a Building Block from a surface
  void CreateBBFromBoundsCallback();
  
  // Description:
  // Launch GUI for specifying Block mesh seeding
  void CreateBBMeshSeedCallback();
  
  // Description:
  // Launch GUI for editing Block mesh seeding
  void EditBBMeshSeedCallback();
  
  // Description:
  // Launch GUI for editing a Building Block 
  void EditBBCallback();
  
  // Description:
  // Launch GUI for saving a Building Block
  void SaveBBCallback();
  
  // Description:
  // Launch GUI for merging Building Blocks
  void CreateBBFromMergingCallback();
  
  // Description:
  // Launch GUI for creating a Building Block from a rubberband line
  void CreateBBFromRubberBandPickCallback();
  
  // Description:
  // Set the various object types
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);
  vtkSetObjectMacro(SurfaceMenuGroup, vtkKWMimxSurfaceMenuGroup);
  vtkGetObjectMacro(SurfaceMenuGroup, vtkKWMimxSurfaceMenuGroup);
  
  // Description:
  // Get the Load Building Block Group to access load functions
  vtkGetObjectMacro(LoadBBGroup, vtkKWMimxLoadBBGroup);
  
protected:
	vtkKWMimxBBMenuGroup();
	~vtkKWMimxBBMenuGroup();

  vtkKWMimxLoadBBGroup *LoadBBGroup;
  vtkKWMimxCreateBBFromBoundsGroup *CreateBBFromBounds;
  vtkKWMimxCreateBBMeshSeedGroup *CreateBBMeshSeed;
  vtkKWMimxEditBBGroup *EditBB;
  vtkKWMimxSaveBBGroup *SaveBBGroup;
  vtkKWMimxEditBBMeshSeedGroup *EditBBMeshSeedGroup;
  vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
  vtkKWMimxMergeBBGroup *MergeBBGroup;
  vtkKWMimxCreateBBFromRubberBandPickGroup *CreateBBFromRubberBandPickGroup;
  virtual void CreateWidget();

  void UpdateObjectLists();
  int CheckCellTypesInUgrid(vtkUnstructuredGrid *);


  vtkKWMimxSurfaceMenuGroup *SurfaceMenuGroup;

private:
  vtkKWMimxBBMenuGroup(const vtkKWMimxBBMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxBBMenuGroup&); // Not implemented
};

#endif
