/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewPropertiesGroup.h,v $
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
// .NAME vtkKWMimxViewPropertiesGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxViewPropertiesGroup_h
#define __vtkKWMimxViewPropertiesGroup_h

// KWWidget Headers
#include "vtkKWTopLevel.h"
#include "vtkKWMultiColumnListWithScrollbars.h"

// MIMX Headers
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWChangeColorButton;
class vtkKWFrameWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWThumbWheel;
class vtkLinkedListWrapper;
class vtkKWFrameWithScrollbar;
class vtkLinkedListWrapperTree;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWSpinBoxWithLabel;
class vtkKWScaleWithEntry;
class vtkMeshQualityClass;
class vtkKWCheckButtonWithLabel;
class vtkKWRange;
class vtkPlane;
class vtkPlaneWidget;
class vtkKWRadioButtonSet;
class vtkPlaneWidgetEventCallback;
class vtkKWCheckButton;
class vtkKWPushButton;
class vtkKWDialog;
class vtkKWLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxViewPropertiesGroup : public vtkKWTopLevel
{
public:
  static vtkKWMimxViewPropertiesGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxViewPropertiesGroup,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  virtual void Update();
  virtual void UpdateEnableState();
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(DoUndoTree, vtkLinkedListWrapperTree);
  vtkSetObjectMacro(MultiColumnList, vtkKWMultiColumnListWithScrollbars);
  vtkGetObjectMacro(MainFrame, vtkKWFrameWithLabel);
  vtkGetMacro(ElementsHighlighted,int);
  
  virtual void SetActorColor(double color[3]);
  virtual void SetOutlineColor(double color[3]);
  virtual void SetElementSetColor(double color[3]);
  void RepresentationCallback(int type);
  void SetElementSizeFromScaleCallback(double scale);
  void LineWidthCallback(double width);
  void OpacityCallback(double width);
  vtkSetObjectMacro(ObjectList, vtkLinkedListWrapper);
  void SetViewProperties();
  
  void DisplayModeCallback(int mode);
  void ElementSetVisibilityCallback(int index, int flag);
  void ElementSetDisplayPropertyCallback( );
  void SetMeshDisplayPropertyCallback();
  void SetElementSetList();
  void SetItemName(const char *item);
  void SetItemId(const char *id);
  int GetItemRow( );
  vtkSetMacro(SelectionRow, int);
  
protected:
	vtkKWMimxViewPropertiesGroup();
	~vtkKWMimxViewPropertiesGroup();
	virtual void CreateWidget();
  vtkKWMultiColumnListWithScrollbars *MultiColumnList;
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkLinkedListWrapper *ObjectList;
  
  vtkKWFrameWithLabel *MainFrame;
  vtkKWFrameWithLabel *PropertyFrame;
  vtkKWFrameWithLabel *ColorFrame;
  vtkKWFrameWithLabel *StyleFrame;
  vtkKWFrameWithLabel *NodeDataFrame;
  vtkKWFrameWithLabel *ElementDataFrame;
  vtkKWChangeColorButton *ActorColorButton;
  vtkKWChangeColorButton *ElementDataColorButton;
  vtkKWMenuButtonWithLabel *DisplayStyleMenuButton;
  vtkKWMenuButtonWithLabel *NodeDataMenuButton;
  vtkKWMenuButtonWithLabel *ElementDataMenuButton;
  vtkKWMenuButtonWithLabel *DisplayLabels;
  vtkKWSpinBoxWithLabel *LinewidthThumbwheel;
  vtkKWSpinBoxWithLabel *OpacityThumbwheel;
  vtkLinkedListWrapperTree *DoUndoTree;
  vtkKWFrameWithLabel *ElementSizeFrame;
  vtkKWSpinBoxWithLabel *ElementSizeScale;
  vtkMeshQualityClass *MeshQualityClass;
  vtkKWCheckButtonWithLabel *OverrideColorButton;
  vtkKWRange *ElementColorRange;
  vtkPlane *SavedImplicitPlane;
  vtkPlaneWidget *SavedPlaneWidget;
  int ElementsHighlighted;
  vtkKWFrameWithLabel *PlaneControlFrame;
  vtkKWCheckButton *InvertSelector;
  vtkKWCheckButton *EnablePlaneButton;
  vtkPlaneWidgetEventCallback *PlaneMoveCallback;
  vtkUnstructuredGrid *InInput;
  vtkKWDialog *ViewPropertiesDialog;
  vtkKWPushButton *CloseButton;
  vtkKWCheckButtonWithLabel *OutlineButton;
  vtkKWChangeColorButton *OutlineColorButton;
  vtkKWMenuButtonWithLabel *ColorMenuButton;
  vtkKWMenuButtonWithLabel *DisplayModeButton;
  vtkKWMultiColumnListWithScrollbars *ElementSetList;
  vtkKWLabel *ObjectNameLabel;
  
  int SelectionRow;
  bool inializedElementSetList;
  char ItemName[256];
  char ItemId[64];

private:
  vtkKWMimxViewPropertiesGroup(const vtkKWMimxViewPropertiesGroup&); // Not implemented
  void operator=(const vtkKWMimxViewPropertiesGroup&); // Not implemented
};

#endif

