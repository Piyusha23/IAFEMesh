#ifndef __vtkMimxContourTraceWidget_h
#define __vtkMimxContourTraceWidget_h

#include "vtkAbstractWidget.h"
#include "vtkContourWidget.h"
#include "vtkMimxWidgetsWin32Header.h"
#include "vtkActor.h"

class vtkContourRepresentation;
class vtkPolyData;
class vtkIdList;

class VTK_MIMXWIDGETS_EXPORT vtkMimxContourTraceWidget : public vtkContourWidget
{
public:
  static vtkMimxContourTraceWidget *New();
  vtkTypeRevisionMacro(vtkMimxContourTraceWidget,vtkAbstractWidget);
  void AddPolyData(vtkPolyData *InputPolyData);
  int IdentifyCell(double x, double y, double z, vtkPolyData *InputPolyData);
  void IsolateSliceContours();
  void AppendCell(vtkPolyData *InputPolyData);
  void AddPositionInfo(double sliceLoc, int axisview);
  void ModifyContour(vtkPolyData *InputPolyData);
  int cp(vtkPolyData *inputPoly, double point[3]);
  void AddSlicePolyData(vtkPolyData *InputPolyData);
  void AddSliceActor(vtkActor *InputActor);
  void UpdateVisibleSliceContours();

protected:
  vtkMimxContourTraceWidget();
  ~vtkMimxContourTraceWidget();

  static void SButtonDownCallback		(vtkAbstractWidget* );
  static void DButtonDownCallback		(vtkAbstractWidget* );

  vtkPolyData *Compiled_Contours;
  vtkPolyData *Isolated_Contours;
  vtkPolyData *Slice_Contours;

  vtkActor *SliceActor;

  int sliceposition;
  int axis;

   //BTX
  int WidgetEvent;
  enum WidgetEventIds 
    {
    Start = 15,
  Outside,
  SButtonDown,
  DButtonDown,
    };
  //ETX


private:
  vtkMimxContourTraceWidget(const vtkMimxContourTraceWidget&);  //Not implemented
  void operator=(const vtkMimxContourTraceWidget&);  //Not implemented
};

#endif