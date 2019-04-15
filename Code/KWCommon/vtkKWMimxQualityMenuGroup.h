/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxQualityMenuGroup.h,v $
Language:  C++
Date:      $Date: 2012/12/31 01:01:56 $
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
// .NAME vtkKWMimxQualityMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxQualityMenuGroup_h
#define __vtkKWMimxQualityMenuGroup_h


// MIMX Headers
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWFileBrowserDialog;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxImageViewProperties;
class vtkKWMimxEvaluateMeshQualityGroup;
class vtkKWMimxEditFEMeshLaplacianSmoothGroup;
class vtkKWMimxExportAbaqusFEMeshGroup;
class vtkKWMimxSaveMeshGroup;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxQualityMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxQualityMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxQualityMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void EvaluateMeshVolumeCallback();
  void EvaluateMeshEdgeCollapseCallback();
  void EvaluateMeshJacobianCallback();
  void EvaluateMeshSkewCallback();
  void EvaluateMeshAngleCallback();
  void EvaluateMeshCallback();
  void CreateMeshQualityGroup();
  void MeshImprovementCallback();
  void SmoothLaplacianFEMeshCallback();
  void SaveMeshCallback();
  void ExportAbaqusFEMeshCallback();
  
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  
  void UpdateObjectLists();
  
protected:
	vtkKWMimxQualityMenuGroup();
	~vtkKWMimxQualityMenuGroup();
	vtkKWFileBrowserDialog *FileBrowserDialog;
	vtkKWMimxSaveMeshGroup *SaveMeshGroup;
  vtkKWMimxExportAbaqusFEMeshGroup *ExportAbaqusFEMeshGroup;
	virtual void CreateWidget();
	
private:
  vtkKWMimxQualityMenuGroup(const vtkKWMimxQualityMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxQualityMenuGroup&); // Not implemented

  vtkKWMimxEvaluateMeshQualityGroup *EvaluateMeshInterface;
  vtkKWMimxEditFEMeshLaplacianSmoothGroup *FEMeshLaplacianSmoothGroup;

};

#endif
