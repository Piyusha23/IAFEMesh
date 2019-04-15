/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveSceneGroup.h,v $
Language:  C++
Date:      $Date: 2013/01/15 23:05:54 $
Version:   $Revision: 1.1 $

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
// .NAME vtkKWMimxSaveSceneGroup - A pop up window for creation of element sets.
//
// .SECTION Description
// The window contains GUI for element set creation. The mesh from which element sets
// need to be created need to be specified. 5 different element selection types exist.
// 1) Select elements through, 2) Select elements on the surface, 3) Select visible 
// elements, 4) Select elements belonging to a building-block face and 5) Select single
// element. The widget used for the interaction is vtkMimxCreateElementSetWidgetFEMesh.
// Two selection modes exist. Full and Partial which is determined through the opacity
// value of the input mesh. If the opacity value is 1.0 the mode is full mesh, else 
// partial mesh. One more option that exists is Editing mode with add and subtract as
// sub options. In Full mesh, Add mode will add elements chosen to the existing selection
// but for partial mesh, Add mode will retain only the chosen elements and remove other
// elements from selection. In subtract mode, the chosen elements are removed from the 
// selection.
//
// .SECTION See Also
// vtkMimxCreateElementSetWidgetFEMesh

#ifndef __vtkKWMimxSaveSceneGroup_h
#define __vtkKWMimxSaveSceneGroup_h

#include "vtkKWTopLevel.h"

#include "vtkLinkedListWrapper.h"

#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainWindow.h"


class vtkPolyData;
class vtkKWMessageDialog;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWPushButton;
class vtkMimxImageActor;
class vtkKWLoadSaveButton;

class vtkKWMimxSaveSceneGroup : public vtkKWTopLevel
{
public:
  static vtkKWMimxSaveSceneGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveSceneGroup,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  virtual void Update();
  virtual void UpdateEnableState();
 
  vtkGetObjectMacro(ApplyButton, vtkKWPushButton);

  // Description:
  vtkSetMacro(WithdrawOnApply, int);
  vtkGetMacro(WithdrawOnApply, int);

  // Description:
  // Apply button callback for creation of element set
  int ApplyButtonCallback();

  // Description:
  // Super class method to close the window and deselect all
  // the buttons selected.
  virtual void Withdraw();

  void UpdateObjectLists();

  // Description:
  // Set all the list  and window access
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);

  vtkSetObjectMacro(MainNotebook, vtkKWMimxMainNotebook);
  
 /* SceneWriterType::Pointer GetSceneWriter()
  {
	  return SceneWriter;
  }*/

  void SaveImage(const char *FileName, vtkMimxImageActor *ImageActor);

  void SaveSTLSurface(const char *fileName, vtkPolyData *polyData, bool binary);
  void SaveVTKSurface(const char *fileName, vtkPolyData *polyData, bool binary);
  void SaveVTKXMLSurface(const char *fileName, vtkPolyData *polyData, bool binary);
  void SaveVTKBuildingBlock(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary);
  void SaveVTKXMLBuildingBlock(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary);
  void WriteXMLUnstructuredGrid(const char *, vtkUnstructuredGrid *, bool binary);
  void WriteUnstructuredGrid(const char *, vtkUnstructuredGrid *, bool binary);
  void SetSceneDirectory();
  
protected:
  vtkKWMimxSaveSceneGroup();
  ~vtkKWMimxSaveSceneGroup();
  virtual void CreateWidget();

  vtkKWMimxMainWindow *MimxMainWindow;
  vtkKWMimxMainNotebook *MainNotebook;
  
  vtkKWPushButton *CancelButton;
  vtkKWPushButton *ApplyButton;
  vtkKWLoadSaveButton *SetDirButton;
  vtkLinkedListWrapper *SurfaceList;
  vtkLinkedListWrapper *BBoxList;
  vtkLinkedListWrapper *FEMeshList;
  vtkLinkedListWrapper *ImageList;
  
  vtkKWMultiColumnListWithScrollbars *MultiColumnList;

  vtkKWMessageDialog *Dialog;

private:
  vtkKWMimxSaveSceneGroup(const vtkKWMimxSaveSceneGroup&); // Not implemented
  void operator=(const vtkKWMimxSaveSceneGroup&); // Not implemented
  
  int WithdrawOnApply;
};

#endif


