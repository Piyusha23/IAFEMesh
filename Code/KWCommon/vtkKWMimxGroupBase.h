/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxGroupBase.h,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:58 $
Version:   $Revision: 1.6 $

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
// .NAME vtkKWMimxGroupBase - Base class for all the menu groups.
//
// .SECTION Description
// Base class that contains all the Set functions for the derived classes.
// Contains declaration for operation menu button which in turn launches
// GUI associated with the selected operations. 

#ifndef __vtkKWMimxGroupBase_h
#define __vtkKWMimxGroupBase_h

/* ITK Headers */
#include <itkImage.h>

/* VTK Headers */
#include <vtkCommand.h>

/* KWWidgets Headers */
#include <vtkKWCompositeWidget.h>

// MIMX Headers
#include "mimxCommonDefine.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkKWMimxViewProperties.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWMenuButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWPushButton;
class vtkLinkedListWrapper;
class vtkLinkedListWrapperTree;
class vtkKWComboBoxWithLabel;
class vtkKWMimxMainMenuGroup;
class vtkKWRenderWidget;
class vtkMimxEntryValueChangedCallback;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxGroupBase : public vtkKWCompositeWidget
{
public:
  static vtkKWMimxGroupBase* New();
  vtkTypeRevisionMacro(vtkKWMimxGroupBase,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  
  // Description:
  // Set functions to access all the lists
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FeatureList, vtkLinkedListWrapper);
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkSetObjectMacro(DoUndoTree, vtkLinkedListWrapperTree);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);
  
  // Description:
  // Get/Set functions to access the interface objects
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MainFrame, vtkKWFrame);
  vtkSetObjectMacro(ViewProperties, vtkKWMimxViewProperties);
  vtkSetObjectMacro(MenuGroup, vtkKWMimxMainMenuGroup);
  
  
  vtkSetMacro(Count, int);
  
  // Description:
  // This is used to add a new mesh to the display. This updates the memory
  // management used to store meshes. The mesh is the vtkUnstructuredGrid, the
  // namePrefix is the desired postfix for the mesh, the FoundationName is the
  // default name for the object, and elementSetName is the default element
  // set name if one does not exist
  void AddMeshToDisplay(vtkUnstructuredGrid *ugrid, const char *namePrefix,
                        const char *FoundationName, const char *elementSetName, vtkIdType Mod, const char *filePath);
  
  // Description:
  // This is used to add a new image to the display. The program will currently
  // support the following image types: Unsigned 8 bit, Signed 16 bit, and
  // Single precision Floating point
  void AddImageToDisplay(ImagePointerType image, const char *namePrefix,
                         const char *FoundationName, vtkIdType Mod, const char *filePath);
  
  // Description:
  // This is used to add a new surface to the display.
  void AddSurfaceToDisplay(vtkPolyData *surface, const char *namePrefix,
                           const char *foundationName, vtkIdType Mod, const char *filePath);
  
  // Description:
  // This is used to add a new contour/feature to the display.
  void AddFeatureToDisplay(vtkPolyData *feature, const char *namePrefix,
                           const char *foundationName, vtkIdType Mod, const char *filePath);
  
  // Description:
  // This is used to add a new Building block to the display.
  void AddBuildingBlockToDisplay(vtkUnstructuredGrid *ugrid, const char *namePrefix,
                                 const char *foundationName, vtkIdType Mod, const char *filePath);

  // Description:
  // Updated the ComboBox with all of the Building Block items that currently
  // exist in the interface.
  int UpdateBuildingBlockComboBox(vtkKWComboBox *combobox);
  
  // Description:
  // Updated the ComboBox with all of the Mesh items that currently
  // exist in the interface.
  int UpdateMeshComboBox(vtkKWComboBox *combobox);
  
  // Description:
  // Updated the ComboBox with all of the Surfaces that currently
  // exist in the interface.
  int UpdateSurfaceComboBox(vtkKWComboBox *combobox);
  
  // Description:
  // Updated the ComboBox with all of the Features that currently
  // exist in the interface.
  int UpdateFeatureComboBox(vtkKWComboBox *combobox);
  
  // Description:
  // Updated the ComboBox with all of the Images that currently
  // exist in the interface.
  int UpdateImageComboBox(vtkKWComboBox *combobox);
  
  // Description:
  // Updated the ComboBox with all of the Node sets that currently
  // exist in the mesh.
  //int UpdateNodeSetComboBox(vtkKWComboBox *combobox, vtkUnstructuredGrid *Mesh);
  
  // Description:
  // Updated the ComboBox with all of the Element sets that currently
  // exist in the mesh.
  //int UpdateElementSetComboBox(vtkKWComboBox *combobox, vtkUnstructuredGrid *Mesh);
  
  // Description:
  // Updated the ListBoxToListBox with all of the Node sets that currently
  // exist in the mesh.
  //int UpdateNodeSetListBoxToListBox(vtkKWListBoxToListBoxSelectionEditor *listbox,
  //                                  vtkUnstructuredGrid *Mesh);
  
  // Description:
  // Updated the ListBoxToListBox with all of the Surfaces that currently
  // exist in the interface.
  //int UpdateSurfaceListBoxToListBox(vtkKWListBoxToListBoxSelectionEditor *listbox);
  
  // Description:
  // Get the currently selected item in the ComboBox. It returns -1 if
  // no item has been selected
  //int GetComboBoxItem(vtkKWComboBox *combobox);
  
  int CancelStatus;
  
protected:
	vtkKWMimxGroupBase();
	virtual ~vtkKWMimxGroupBase();
	virtual void CreateWidget();
  vtkLinkedListWrapper *SurfaceList;
  vtkLinkedListWrapper *FeatureList;
  vtkLinkedListWrapper *BBoxList;
  vtkLinkedListWrapper *FEMeshList;
  vtkLinkedListWrapper *ImageList;
  vtkKWPushButton *DoneButton;
  vtkKWPushButton *ApplyButton;
  vtkKWPushButton *CancelButton;
  vtkKWFrame *MainFrame;

  vtkKWMimxMainWindow *MimxMainWindow;
  vtkKWMimxViewProperties *ViewProperties;
  vtkLinkedListWrapperTree *DoUndoTree;
  vtkKWMimxMainMenuGroup *MenuGroup;
  vtkIdType Count;  // to keep track of number of objects created during runtime 
  char objectSelectionPrevious[256];
  vtkMimxEntryValueChangedCallback *EntryChangedCallback;
  
private:
  vtkKWMimxGroupBase(const vtkKWMimxGroupBase&); // Not implemented
  void operator=(const vtkKWMimxGroupBase&); // Not implemented
};

#endif

