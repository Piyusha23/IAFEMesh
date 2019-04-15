/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxTransformSurfaceGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/16 22:16:55 $
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

// Filter takes in surafce data ( *.stl or *.vtk format) and a transform 
// can be applied either by interacdtively moving the box or by manually
// entering the Translation/Rotation and Scaling values. The Output surface
// can be saved in both vtk and stl formats.



#ifndef __vtkKWMimxTransformSurfaceGroup_h
#define __vtkKWMimxTransformSurfaceGroup_h


#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkActor;
class vtkMimxBoxWidget;
class vtkBWCallback;
class vtkMatrix4x4 ;
class vtkPolyDataMapper;
class vtkTransform;
class vtkTransformPolyDataFilter ;

class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;
class vtkKWMenuButtonWithLabel;
class vtkKWMatrixWidgetWithLabel ;
class vtkKWCheckButtonWithLabel;
class vtkKWPushButton;
 
class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxTransformSurfaceGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxTransformSurfaceGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxTransformSurfaceGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void SaveVTKSurfaceDoneCallback();
  void TransformSurfaceCancelCallback();
  int TransformSurfaceApplyCallback();
  void UpdateObjectLists();
  void TransformSurfaceCallback(const char *entry);
  void MatrixValuesTrans(int row, int col , const char *entry) ; 
  void MatrixValuesScale(int row, int col , const char *entry) ; 
  void MatrixValuesRotate(int row, int col , const char *entry) ; 
  void MatrixValues_changed(int row, int col , const char *entry);
  void MatrixValuesReset();

  void BWStateCallback(int state);


protected:
	vtkKWMimxTransformSurfaceGroup();
	~vtkKWMimxTransformSurfaceGroup();

	vtkMimxBoxWidget *boxWidget;
	vtkTransform *Transform;
	vtkBWCallback *TransformCallback;
	vtkPolyDataMapper *SelectedSurfaceMapper;
	vtkActor *SelectedSurfaceActor;
	vtkMatrix4x4 *matrix ;

	vtkTransformPolyDataFilter *transpoly ;
	vtkKWCheckButtonWithLabel *BWVisiCheckButton;

	vtkKWPushButton *ResetTransButton;


	virtual void CreateWidget();

	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkKWLoadSaveDialog *FileBrowserDialog;
	vtkKWMatrixWidgetWithLabel *matrix_widget1;
	vtkKWMatrixWidgetWithLabel *matrix_widget2;
	vtkKWMatrixWidgetWithLabel *matrix_widget3;
	char  previousselection[256];
	char  currentselection[256];
	double previousval[3];
	double previousvaltrans[3];
	double previousvalscale[3];

	double originalcenter[3];

	
private:
  vtkKWMimxTransformSurfaceGroup(const vtkKWMimxTransformSurfaceGroup&); 
  void operator=(const vtkKWMimxTransformSurfaceGroup&); 
};

#endif

