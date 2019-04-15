#include "vtkMimxKWCommonWin32Header.h"
#include "vtkMimxContourTraceWidget.h"

class VTK_MIMXKWCOMMON_EXPORT ManualTrace
{
public:
  enum TraceOrientation
    {
    Unknown  = 0,
    Axial    = 1,
    Coronal  = 2,
    Sagittal = 3
    };
  
  vtkMimxContourTraceWidget *m_ContourWidget;
  TraceOrientation m_Orientation;
  int m_SliceIndex;
  
  ManualTrace()
  {
  m_ContourWidget = vtkMimxContourTraceWidget::New();
  m_Orientation = Unknown;
  m_SliceIndex = -1;
  }
  
  ~ManualTrace() { m_ContourWidget->Delete(); }
  void SetOrientationAxial() {m_Orientation = Axial;}
  void SetOrientationCoronal() {m_Orientation = Coronal;}
  void SetOrientationSagittal() {m_Orientation = Sagittal;}
  int  GetOrientation() {return m_Orientation;}
  bool IsOrientationAxial()
  {
  if (m_Orientation == Axial)
    {
    return true;
    }
  else
    {
    return false;
    }
  }
  
  bool IsOrientationCoronal()
  {
  if (m_Orientation == Coronal)
    {
    return true;
    }
  else
    {
    return false;
    }
  }
  
  bool IsOrientationSagittal()
  {
  if (m_Orientation == Sagittal)
    {
    return true;
    }
  else
    {
    return false;
    }
  }
  
  void SetSliceIndex(int index) {m_SliceIndex = index; }
  int  GetSliceIndex() {return m_SliceIndex; }
  
};
