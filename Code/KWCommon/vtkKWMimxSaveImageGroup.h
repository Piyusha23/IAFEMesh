/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveImageGroup.h,v $
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
// .NAME vtkKWMimxSaveImageGroup - Save an ITK Image
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxSaveImageGroup_h
#define __vtkKWMimxSaveImageGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxImageMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;

class vtkMimxImageActor;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSaveImageGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxSaveImageGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveImageGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void SaveImageDoneCallback();
  void SaveImageCancelCallback();
  int SaveImageApplyCallback();
  void UpdateObjectLists();
  void SaveImage(char *FileName, vtkMimxImageActor *ImageActor);

protected:
  vtkKWMimxSaveImageGroup();
  ~vtkKWMimxSaveImageGroup();
  virtual void CreateWidget();
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWLoadSaveDialog *FileBrowserDialog;
private:
  vtkKWMimxSaveImageGroup(const vtkKWMimxSaveImageGroup&); // Not implemented
  void operator=(const vtkKWMimxSaveImageGroup&); // Not implemented
};

#endif


