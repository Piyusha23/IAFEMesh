/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveBBGroup.h,v $
Language:  C++
Date:      $Date: 2012/12/31 01:01:56 $
Version:   $Revision: 1.2 $

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
// .NAME vtkKWMimxSaveBBGroup - GUI for saving building blocks as unstructured grids.
// .SECTION Description
// The class creates the graphical user interface for saving of Building Blocks
// in VTK or VTK-XML file format. The Building Blocks are essentially 
// vtkUnstructuredGrids with field data used to define the mesh seed density.

#ifndef __vtkKWMimxSaveBBGroup_h
#define __vtkKWMimxSaveBBGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWComboBoxWithLabel;
class vtkKWCheckButtonWithLabel;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSaveBBGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxSaveBBGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveBBGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  // Description:
  // Call backs for push buttons
  void SaveBBCancelCallback();
  int SaveBBApplyCallback();
  
  // Description:
  // Updates the lists used for selection
  void UpdateObjectLists();
  
protected:
  vtkKWMimxSaveBBGroup();
  ~vtkKWMimxSaveBBGroup();
  virtual void CreateWidget();
  
  void SaveVTKBuildingBlock(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary);
  void SaveVTKXMLBuildingBlock(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary);
  
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWCheckButtonWithLabel *BinaryCheckButton;
  
private:
  vtkKWMimxSaveBBGroup(const vtkKWMimxSaveBBGroup&); // Not implemented
  void operator=(const vtkKWMimxSaveBBGroup&); // Not implemented
  
};

#endif


