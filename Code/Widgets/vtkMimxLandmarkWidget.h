/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxLandmarkWidget.h,v $
Language:  C++

Date:      $Date: 2013/01/29 22:07:08 $
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
#ifndef __vtkMimxLandmarkWidget_h
#define __vtkMimxLandmarkWidget_h

#include "vtkAbstractWidget.h"
#include "vtkActorCollection.h"
#include "vtkMimxWidgetsWin32Header.h"

class vtkAbstractPropPicker;
class vtkActor;
class vtkActorCollection;
class vtkCellPicker;
class vtkDataSetMapper;
class vtkInteractorStyleRubberBandPick;
class vtkPolyData;
class vtkUnstructuredGrid;
class vtkPoints;
class vtkSource;

class VTK_MIMXWIDGETS_EXPORT vtkMimxLandmarkWidget : public vtkAbstractWidget
{
 public:
  // Description:
  // Instantiate the object.
  static vtkMimxLandmarkWidget *New();

  vtkTypeRevisionMacro(vtkMimxLandmarkWidget,vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
 // Enable the interaction
  virtual void SetEnabled(int);

  // Description:
  // Currently input should be a surface. It can easily be changed to any type of
  // data set.
  virtual void SetActor(vtkActor *Actor);
  virtual void SetActor2(vtkActor *Actor);
  virtual void SetActor3(vtkActor *Actor);
  void SetPicker(vtkAbstractPropPicker *Picker);
  void GetLandmark(vtkActor *actor, double point[3]);
  int AddLandmark(double point[3]);
  int GetNumberOfLandmarks();
  vtkActorCollection* GetActorCollection();
  void RemoveAllLandmarks();
  void SetVisibility(int i);
  int GetVisibility();
  void GetLandmarkIndex(double point[3], int index);
  void SetSphereRadius(double radius);
  double GetSphereRadius();

  void CreateDefaultRepresentation(){}
  vtkActorCollection *ActorCollection;
  vtkPolyData *SelectedActor;

protected:
  vtkMimxLandmarkWidget();
  ~vtkMimxLandmarkWidget();

  static void CtrlMouseMoveCallback           (vtkAbstractWidget* );
  static void CtrlLeftButtonDownCallback      (vtkAbstractWidget* );
  static void CtrlLeftButtonUpCallback        (vtkAbstractWidget* );
  static void CtrlRightButtonDownCallback     (vtkAbstractWidget* );
  static void CtrlRightButtonUpCallback       (vtkAbstractWidget* );
  static void CtrlMiddleButtonDownCallback    (vtkAbstractWidget* );
  static void CtrlMiddleButtonUpCallback      (vtkAbstractWidget* );
  static void CtrlMiddleButtonMoveCallback    (vtkAbstractWidget* );

   //BTX
  // Description:
  // Events invoked by this widget
  int WidgetEvent;
  enum WidgetEventIds 
    {
    Start = 0,
  Outside,
  CtrlLeftMouseButtonDown,
  CtrlLeftMouseButtonUp,
  CtrlRightMouseButtonDown,
  CtrlRightMouseButtonUp,
  CtrlMiddleMouseButtonDown,
  CtrlMiddleMouseButtonUp,
  CtrlMiddleMouseButtonMove
    };
  int State;
  enum WidgetState
  {
    Waiting,
	Selected,
    Moving,
    Move_Complete
  };

  //ETX
  vtkAbstractPropPicker *CellPicker;
  vtkCellPicker *LandmarkPicker;
  vtkActor *Actor;
  vtkActor *Actor2;
  vtkActor *Actor3;
  double sphere_radius;
  double LastPickPosition[3];
  int visibility;

private:
  vtkMimxLandmarkWidget(const vtkMimxLandmarkWidget&);  //Not implemented
  void operator=(const vtkMimxLandmarkWidget&);  //Not implemented
  
};

#endif

