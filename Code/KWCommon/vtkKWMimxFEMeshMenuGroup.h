/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxFEMeshMenuGroup.h,v $
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

#ifndef __vtkKWMimxFEMeshMenuGroup_h
#define __vtkKWMimxFEMeshMenuGroup_h


// MIMX Headers
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWMimxCreateBBFromBoundsGroup;
class vtkKWMimxCreateBBMeshSeedGroup;
class vtkKWMimxEditBBGroup;
class vtkKWMimxCreateFEMeshFromBBGroup;
class vtkKWMimxViewProperties;
class vtkKWMimxSaveMeshGroup;
class vtkKWMimxEditBBMeshSeedGroup;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxEditFEMeshLaplacianSmoothGroup;
class vtkKWMimxCreateExtrudeFEMeshGroup;
class vtkMimxUnstructuredGridActor;
class vtkKWMimxEditElementSetNumbersGroup;
class vtkKWMimxEditNodeSetNumbersGroup;

class vtkKWMimxMergeMeshGroup;
class vtkKWMimxExportAbaqusFEMeshGroup;
class vtkKWMimxMergeBBGroup;
class vtkKWMimxCreateElementSetPickFaceGroup;
class vtkMimxUnstructuredGridActor;
class vtkKWMimxEditBBMeshSeedGroup;
class vtkKWMimxLoadMeshGroup;

class vtkUnstructuredGrid;
class vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup;
class vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup;
class vtkKWMimxCreateFEMeshNodeSetGroup;
class vtkKWMimxCreateFEMeshElementSetGroup;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxFEMeshMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxFEMeshMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxFEMeshMenuGroup,vtkKWMimxMainMenuGroup);
  
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  // Description:
  // Launch GUI for loading a Mesh
  void LoadMeshCallback();
  
  // Description:
  // Launch GUI for creating a Mesh
  void CreateFEMeshCallback();
  
  // Description:
  // Launch GUI for deleting a Mesh
  void DeleteFEMeshCallback();
  
  // Description:
  // Launch GUI for editing a Mesh
  void EditFEMeshCallback();
  
  // Description:
  // Launch GUI for creating a Mesh from a building block and surface
  void CreateFEMeshFromBBCallback();
  
  // Description:
  // Launch GUI for saving a Mesh
  void SaveMeshCallback();
  
  // Description:
  // Launch GUI for smooting a Mesh
  void SmoothLaplacianFEMeshCallback();
  
  // Description:
  // Launch GUI for extruding a Mesh
  void CreateFEMeshFromSurfaceExtrusionCallback();
  
  // Description:
  // Launch GUI for editing a Mesh element numbers
  void EditElementNumbersCallback();
  
  // Description:
  // Launch GUI for loading a Mesh node numbers
  void EditNodeNumbersCallback();
  
  // Description:
  // Launch GUI for merging two Meshes
  void CreateFEMeshFromMergingCallback();
  
  // Description:
  // Launch GUI for exprting a Mesh
  void ExportFEMeshCallback();
  
  // Description:
  // Launch GUI for loading a Mesh in Abaqus format
  void ExportAbaqusFEMeshCallback();
  
  
  // Description:
  // Launch GUI for assining material properties based on an image
  void EditImageBasedMatPropsCallback();
  
  // Description:
  // Launch GUI for defining Mesh element sets
  void EditCreateElementSetPickFaceCallback();
  
  // Description:
  // Launch GUI for creating a Rigid Body mesh
  void CreateFEMeshFromSurfaceExtractionCallback();
  
  // Description:
  // Launch GUI for defining Mesh node sets
  void CreateFEMeshNodeSetCallback();
  
  // Description:
  // Launch GUI for defining Mesh element sets
  void CreateFEMeshElementSetCallback();
  
  // Description:
  // Launch GUI for editing Mesh seeds
  void EditBBMeshSeedCallback();
  
  // Description:
  // Set the various object types
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
 
  // Description:
  // Get the Load Mesh Group to access load functions
  vtkGetObjectMacro(LoadMeshGroup, vtkKWMimxLoadMeshGroup);
  
protected:
	vtkKWMimxFEMeshMenuGroup();
	~vtkKWMimxFEMeshMenuGroup();

	virtual void CreateWidget();
  
  vtkKWMimxLoadMeshGroup *LoadMeshGroup;
  vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
  vtkKWMimxEditFEMeshLaplacianSmoothGroup *FEMeshLaplacianSmoothGroup;
  vtkKWMimxCreateExtrudeFEMeshGroup *CreateExtrudeFEMeshGroup;
  vtkKWMimxEditElementSetNumbersGroup *EditElementSetNumbersGroup;
  vtkKWMimxEditNodeSetNumbersGroup *EditNodeSetNumbersGroup;
  vtkKWMimxMergeMeshGroup *MergeMeshGroup;
  vtkKWMimxExportAbaqusFEMeshGroup *ExportAbaqusFEMeshGroup;
  vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup *ImageMatPropGroup;
  vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup *FEMeshFromSurfaceExtractionGroup;
  vtkKWMimxCreateElementSetPickFaceGroup *CreateElementSetPickFaceGroup;
  vtkKWMimxCreateFEMeshNodeSetGroup *CreateFEMeshNodeSetGroup;
  vtkKWMimxCreateFEMeshElementSetGroup *CreateFEMeshElementSetGroup;
  vtkKWMimxEditBBMeshSeedGroup *EditBBMeshSeedGroup;
  vtkKWMimxCreateFEMeshFromBBGroup *FEMeshFromBB;
  vtkKWMimxSaveMeshGroup *SaveMeshGroup;
  
  
  void UpdateObjectLists();
  int CheckCellTypesInUgrid(vtkUnstructuredGrid *);

private:
  vtkKWMimxFEMeshMenuGroup(const vtkKWMimxFEMeshMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxFEMeshMenuGroup&); // Not implemented
};

#endif
