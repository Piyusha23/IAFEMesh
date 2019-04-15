/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxEditNodeElementNumbersGroup.h,v $
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
// .NAME vtkKWMimxEditNodeElementNumbersGroup - a taFEMeshed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxEditNodeElementNumbersGroup_h
#define __vtkKWMimxEditNodeElementNumbersGroup_h


// MIMX Headers
#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWCheckButtonWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxEditNodeElementNumbersGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxEditNodeElementNumbersGroup* New();
	vtkTypeRevisionMacro(vtkKWMimxEditNodeElementNumbersGroup,vtkKWMimxGroupBase);
	void PrintSelf(ostream& os, vtkIndent indent);
	virtual void Update();
	virtual void UpdateEnableState();

	vtkGetObjectMacro(NodeNumberEntry, vtkKWEntryWithLabel);
	vtkGetObjectMacro(ElementNumberEntry, vtkKWEntryWithLabel);
	vtkGetObjectMacro(NodeSetNameEntry, vtkKWEntryWithLabel);
	vtkGetObjectMacro(ElementSetNameEntry, vtkKWEntryWithLabel);

protected:
	vtkKWMimxEditNodeElementNumbersGroup();
	~vtkKWMimxEditNodeElementNumbersGroup();
	virtual void CreateWidget();
	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkKWEntryWithLabel *NodeNumberEntry;
	vtkKWEntryWithLabel *ElementNumberEntry;
	vtkKWEntryWithLabel *ElementSetNameEntry;
	vtkKWEntryWithLabel *NodeSetNameEntry;
	vtkKWCheckButtonWithLabel *NodeNumberCheckButton;
	vtkKWCheckButtonWithLabel *ElementNumberCheckButton;
	
private:
  vtkKWMimxEditNodeElementNumbersGroup(const vtkKWMimxEditNodeElementNumbersGroup&); // Not implemented
  void operator=(const vtkKWMimxEditNodeElementNumbersGroup&); // Not implemented
};

#endif

