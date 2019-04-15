/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageManualSegmentGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/11 01:30:10 $
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
#ifndef __vtkKWMimxImageManualSegmentGroup_h
#define __vtkKWMimxImageManualSegmentGroup_h

#include <list>

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"
#include "ManualTrace.h"

class vtkImageActor;
class vtkKWComboBoxWithLabel;
class vtkKWRadioButton;
class vtkKWPushButton;
class vtkKWMenuButton;
class vtkKWScale;
class vtkContourWidget;
class vtkMimxImagePlaneWidget;
class vtkOrientedGlyphContourRepresentation;
class vtkPolyDataPointPlacer;
class vtkPolyDataMapper;
class vtkPoints;
class vtkMimxContourTraceWidget;
class vtkInteractorObserver;
class vtkMimxFeaturePolyDataActor;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxImageManualSegmentGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxImageManualSegmentGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxImageManualSegmentGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  //Callbacks for GUI interaction
  void CancelCallback();
  void AddContourCallback();
  void NewContourCallback();
  void RemoveContoursCallback();
  void CopyContourCallback(int shift);
  void AxisCallback();
  void SliderCallback(double);
  void ViewCallback(int axis, bool resetCamera);
  void UpdateSliceContoursCallback(bool displayContours);
  void InitializeActorCallback();
  void SetFeatureCallback(const char *value);
  void GetActiveContourPolyData(vtkPolyData *contour);
  void ReplaceInterfaceContourWithEditedPolyData();
  vtkPolyData* CreateEmptyContourPolyData(int plane);
  int GetFeaturePlane(vtkPolyData *feature);
  int GetTracePlane();
  void SetImageCallback(const char *value);
  void UpdateFeatureList();
  void UpdateImageList();
  void CopySliceContours(int fromSlice, int toSlice);
  void ResetActiveTrace();
  void ConvertFeatureToTrace(vtkMimxFeaturePolyDataActor *actor);
  
  
  // Methods to Enable/Disable the Tracing Interaction
  void EnableTracingInteractor();
  void DisableTracingInteractor();
  
  
protected:
  vtkKWMimxImageManualSegmentGroup();
  ~vtkKWMimxImageManualSegmentGroup();
  virtual void CreateWidget();
  ManualTrace* CreateNewEmptyContour(int axis, int sliceIndex, double sliceLocation);
    
  //KWWidget Interface Widgets
  vtkKWFrameWithLabel *ImageListFrame;
  vtkKWComboBoxWithLabel *ImageListComboBox;
  vtkKWComboBoxWithLabel *FeatureListComboBox;
  vtkKWPushButton *NewContourButton;
  vtkKWFrameWithLabel *AxisFrame;
  vtkKWRadioButton *XAxis;
  vtkKWRadioButton *YAxis;
  vtkKWRadioButton *ZAxis;
  vtkKWFrameWithLabel *AddRemoveButtonFrame;
  vtkKWPushButton *AddContourButton;
  vtkKWPushButton *RemoveContourButton;
  vtkKWFrameWithLabel *CopyButtonFrame;
  vtkKWPushButton *Copy2PrevContourButton;
  vtkKWPushButton *Copy2NextContourButton;
  vtkKWScale *SliceScale;
  vtkKWFrameWithLabel *ScaleFrame;
  
  
  //VTK Objects for Tracing
  vtkMimxImagePlaneWidget *ImageActorWidgetX;
  vtkMimxImagePlaneWidget *ImageActorWidgetY;
  vtkMimxImagePlaneWidget *ImageActorWidgetZ;

  vtkInteractorObserver *DefaultInteractorStyle;
  vtkInteractorObserver *TraceInteractorStyle;

  
private:
  vtkKWMimxImageManualSegmentGroup(const vtkKWMimxImageManualSegmentGroup&); // Not implemented
  void operator=(const vtkKWMimxImageManualSegmentGroup&); // Not implemented
  
  //BTX
  std::list<ManualTrace*> EditTraceList;
  std::string ActiveActorName;
  std::string ActiveTraceName;
  int TracingAxis;
  //ETX
};

#endif

