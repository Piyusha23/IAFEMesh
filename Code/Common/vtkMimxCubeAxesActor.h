/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMimxCubeAxesActor.h,v $
  Language:  C++
  Date:      $Date: 2013/03/16 22:16:55 $
  Version:   $Revision: 1.1 $
  Thanks:    Kathleen Bonnell, B Division, Lawrence Livermore Nat'l Laboratory 

Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserve  
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.
=========================================================================*/
// .NAME vtkMimxCubeAxesActor - create a  plot of a bounding box edges - 
// used for navigation
// .SECTION Description
// vtkMimxCubeAxesActor is a composite actor that draws axes of the 
// bounding box of an input dataset. The axes include labels and titles
// for the x-y-z axes. The algorithm selects which axes to draw based
// on the user-defined 'fly' mode.  (STATIC is default).
// 'STATIC' constructs axes from all edges of the bounding box.
// 'CLOSEST_TRIAD' consists of the three axes x-y-z forming a triad that 
// lies closest to the specified camera. 
// 'FURTHEST_TRIAD' consists of the three axes x-y-z forming a triad that 
// lies furthest from the specified camera. 
// 'OUTER_EDGES' is constructed from edges that are on the "exterior" of the 
// bounding box, exterior as determined from examining outer edges of the 
// bounding box in projection (display) space. 
// 
// To use this object you must define a bounding box and the camera used
// to render the vtkMimxCubeAxesActor. You can optionally turn on/off labels,
// ticks, gridlines, and set tick location, number of labels, and text to
// use for axis-titles.  A 'corner offset' can also be set.  This allows 
// the axes to be set partially away from the actual bounding box to perhaps
// prevent overlap of labels between the various axes.
//
// The Bounds instance variable (an array of six doubles) is used to determine
// the bounding box.
// 
// .SECTION See Also
// vtkActor vtkAxisActor vtkMimxCubeAxesActor2D

#ifndef __vtkMimxCubeAxesActor_h
#define __vtkMimxCubeAxesActor_h

#define VTK_FLY_OUTER_EDGES     0
#define VTK_FLY_CLOSEST_TRIAD   1
#define VTK_FLY_FURTHEST_TRIAD  2
#define VTK_FLY_STATIC_TRIAD    3
#define VTK_FLY_STATIC_EDGES    4

#define VTK_TICKS_INSIDE        0
#define VTK_TICKS_OUTSIDE       1
#define VTK_TICKS_BOTH          2

#include "vtkActor.h"
#include "vtkMimxCommonWin32Header.h"

class vtkAxisActor;
class vtkCamera;

class VTK_MIMXCOMMON_EXPORT vtkMimxCubeAxesActor : public vtkActor
{
public:
  vtkTypeRevisionMacro(vtkMimxCubeAxesActor,vtkActor);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Instantiate object with label format "6.3g" and the number of labels 
  // per axis set to 3.
  static vtkMimxCubeAxesActor *New();
  
  // Description:
  // Draw the axes as per the vtkProp superclass' API.
  virtual int RenderOpaqueGeometry(vtkViewport*);
  virtual int RenderTranslucentGeometry(vtkViewport *) {return 0;}

  // Description:
  // Explicitly specify the region in space around which to draw the bounds.
  // The bounds is used only when no Input or Prop is specified. The bounds
  // are specified according to (xmin,xmax, ymin,ymax, zmin,zmax), making
  // sure that the min's are less than the max's.
  vtkSetVector6Macro(Bounds,double);
  double *GetBounds();
  void GetBounds(double& xmin, double& xmax, double& ymin, double& ymax, 
                 double& zmin, double& zmax);
  void GetBounds(double bounds[6]);

  // Description:
  // Set/Get the camera to perform scaling and translation of the 
  // vtkMimxCubeAxesActor.
  virtual void SetCamera(vtkCamera*);
  vtkGetObjectMacro(Camera,vtkCamera);

  // Description:
  // Specify a mode to control how the axes are drawn: either static, 
  // closest triad, furthest triad or outer edges in relation to the 
  // camera position.
  vtkSetClampMacro(FlyMode, int, VTK_FLY_OUTER_EDGES, VTK_FLY_STATIC_EDGES);
  vtkGetMacro(FlyMode, int);
  void SetFlyModeToOuterEdges()
    {this->SetFlyMode(VTK_FLY_OUTER_EDGES);};
  void SetFlyModeToClosestTriad()
    {this->SetFlyMode(VTK_FLY_CLOSEST_TRIAD);};
  void SetFlyModeToFurthestTriad()
    {this->SetFlyMode(VTK_FLY_FURTHEST_TRIAD);};
  void SetFlyModeToStaticTriad()
    {this->SetFlyMode(VTK_FLY_STATIC_TRIAD);};
  void SetFlyModeToStaticEdges()
    {this->SetFlyMode(VTK_FLY_STATIC_EDGES);};

  // Description:
  // Set/Get the labels for the x, y, and z axes. By default, 
  // use "X-Axis", "Y-Axis" and "Z-Axis".
  vtkSetStringMacro(XTitle);
  vtkGetStringMacro(XTitle);
  vtkSetStringMacro(XUnits);
  vtkGetStringMacro(XUnits);
  vtkSetStringMacro(YTitle);
  vtkGetStringMacro(YTitle);
  vtkSetStringMacro(YUnits);
  vtkGetStringMacro(YUnits);
  vtkSetStringMacro(ZTitle);
  vtkGetStringMacro(ZTitle);
  vtkSetStringMacro(ZUnits);
  vtkGetStringMacro(ZUnits);

  // Description:
  // Set/Get the format with which to print the labels on each of the
  // x-y-z axes.
  vtkSetStringMacro(XLabelFormat);
  vtkGetStringMacro(XLabelFormat);
  vtkSetStringMacro(YLabelFormat);
  vtkGetStringMacro(YLabelFormat);
  vtkSetStringMacro(ZLabelFormat);
  vtkGetStringMacro(ZLabelFormat);
  
  // Description:
  // Set/Get the inertial factor that controls how often (i.e, how
  // many renders) the axes can switch position (jump from one axes 
  // to another).
  vtkSetClampMacro(Inertia, int, 1, VTK_LARGE_INTEGER);
  vtkGetMacro(Inertia, int);

  // Description:
  // Specify an offset value to "pull back" the axes from the corner at
  // which they are joined to avoid overlap of axes labels. The 
  // "CornerOffset" is the fraction of the axis length to pull back.
  vtkSetMacro(CornerOffset, double);
  vtkGetMacro(CornerOffset, double);

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Turn on and off the visibility of each axis.
  vtkSetMacro(XAxisVisibility,int);
  vtkGetMacro(XAxisVisibility,int);
  vtkBooleanMacro(XAxisVisibility,int);
  vtkSetMacro(YAxisVisibility,int);
  vtkGetMacro(YAxisVisibility,int);
  vtkBooleanMacro(YAxisVisibility,int);
  vtkSetMacro(ZAxisVisibility,int);
  vtkGetMacro(ZAxisVisibility,int);
  vtkBooleanMacro(ZAxisVisibility,int);

  // Description:
  // Turn on and off the visibility of labels for each axis.
  vtkSetMacro(XAxisLabelVisibility,int);
  vtkGetMacro(XAxisLabelVisibility,int);
  vtkBooleanMacro(XAxisLabelVisibility,int);

  vtkSetMacro(YAxisLabelVisibility,int);
  vtkGetMacro(YAxisLabelVisibility,int);
  vtkBooleanMacro(YAxisLabelVisibility,int);

  vtkSetMacro(ZAxisLabelVisibility,int);
  vtkGetMacro(ZAxisLabelVisibility,int);
  vtkBooleanMacro(ZAxisLabelVisibility,int);

  // Description:
  // Turn on and off the visibility of ticks for each axis.
  vtkSetMacro(XAxisTickVisibility,int);
  vtkGetMacro(XAxisTickVisibility,int);
  vtkBooleanMacro(XAxisTickVisibility,int);

  vtkSetMacro(YAxisTickVisibility,int);
  vtkGetMacro(YAxisTickVisibility,int);
  vtkBooleanMacro(YAxisTickVisibility,int);

  vtkSetMacro(ZAxisTickVisibility,int);
  vtkGetMacro(ZAxisTickVisibility,int);
  vtkBooleanMacro(ZAxisTickVisibility,int);

  // Description:
  // Turn on and off the visibility of minor ticks for each axis.
  vtkSetMacro(XAxisMinorTickVisibility,int);
  vtkGetMacro(XAxisMinorTickVisibility,int);
  vtkBooleanMacro(XAxisMinorTickVisibility,int);

  vtkSetMacro(YAxisMinorTickVisibility,int);
  vtkGetMacro(YAxisMinorTickVisibility,int);
  vtkBooleanMacro(YAxisMinorTickVisibility,int);

  vtkSetMacro(ZAxisMinorTickVisibility,int);
  vtkGetMacro(ZAxisMinorTickVisibility,int);
  vtkBooleanMacro(ZAxisMinorTickVisibility,int);

  vtkSetMacro(DrawXGridlines,int);
  vtkGetMacro(DrawXGridlines,int);
  vtkBooleanMacro(DrawXGridlines,int);

  vtkSetMacro(DrawYGridlines,int);
  vtkGetMacro(DrawYGridlines,int);
  vtkBooleanMacro(DrawYGridlines,int);

  vtkSetMacro(DrawZGridlines,int);
  vtkGetMacro(DrawZGridlines,int);
  vtkBooleanMacro(DrawZGridlines,int);

  // Description:
  // Set/Get the location of ticks marks. 
  vtkSetClampMacro(TickLocation, int, VTK_TICKS_INSIDE, VTK_TICKS_BOTH);
  vtkGetMacro(TickLocation, int);

  void SetTickLocationToInside(void) 
    { this->SetTickLocation(VTK_TICKS_INSIDE); };
  void SetTickLocationToOutside(void) 
    { this->SetTickLocation(VTK_TICKS_OUTSIDE); };
  void SetTickLocationToBoth(void) 
    { this->SetTickLocation(VTK_TICKS_BOTH); };

  void SetLabelScaling(bool, int, int, int);  


/////// added for custom axis numbering ////////////////
//Description:
  // Set the label range for vtkAxisActor for the X Axis
  void SetXAxisLabelRange(double minValue, double maxValue);
  //Description:
  // Set the label range for vtkAxisActor for the Y Axis
  void SetYAxisLabelRange(double minValue, double maxValue);
  //Description:
  // Set the label range for vtkAxisActor for the Z Axis
  void SetZAxisLabelRange(double minValue, double maxValue);

  // Description:
  // Set/Get whether the XAxis Label ranges are overridden.
  vtkGetMacro(UseAxisRangeForXLabels, int);
  vtkSetMacro(UseAxisRangeForXLabels, int);
  vtkBooleanMacro(UseAxisRangeForXLabels, int);

  // Description:
  // Set/Get whether the YAxis Label ranges are overridden.
  vtkGetMacro(UseAxisRangeForYLabels, int);
  vtkSetMacro(UseAxisRangeForYLabels, int);
  vtkBooleanMacro(UseAxisRangeForYLabels, int);

  // Description:
  // Set/Get whether the ZAxis Label ranges are overridden.
  vtkGetMacro(UseAxisRangeForZLabels, int);
  vtkSetMacro(UseAxisRangeForZLabels, int);
  vtkBooleanMacro(UseAxisRangeForZLabels, int);

/////// added for axis custom axis numbering  end ////////////////


  // Description:
  // Shallow copy of a KatCubeAxesActor.
  void ShallowCopy(vtkMimxCubeAxesActor *actor);

protected:
  vtkMimxCubeAxesActor();
  ~vtkMimxCubeAxesActor();

  int LabelExponent(double min, double max);
  int Digits(double min, double max);
  double MaxOf(double, double);
  double MaxOf(double, double, double, double);
  double FFix(double);
  double FSign(double, double);

  double       Bounds[6]; //Define bounds explicitly

  vtkCamera *Camera;
  int FlyMode;
 
  // to control all axes  
  // [0] always for 'Major' axis during non-static fly modes.
  vtkAxisActor *XAxes[4];
  vtkAxisActor *YAxes[4];
  vtkAxisActor *ZAxes[4];

  char *XTitle;
  char *XUnits;
  char *YTitle;
  char *YUnits;
  char *ZTitle;
  char *ZUnits;

  char *ActualXLabel;
  char *ActualYLabel;
  char *ActualZLabel;

  int TickLocation;

  int XAxisVisibility;
  int YAxisVisibility;
  int ZAxisVisibility;

  int XAxisTickVisibility;
  int YAxisTickVisibility;
  int ZAxisTickVisibility;

  int XAxisMinorTickVisibility;
  int YAxisMinorTickVisibility;
  int ZAxisMinorTickVisibility;

  int XAxisLabelVisibility;
  int YAxisLabelVisibility;
  int ZAxisLabelVisibility;

  int DrawXGridlines;
  int DrawYGridlines;
  int DrawZGridlines;

  char  *XLabelFormat;
  char  *YLabelFormat;
  char  *ZLabelFormat;
  double CornerOffset;
  int   Inertia;
  int   RenderCount;
  int   InertiaLocs[3];
  
  int RenderSomething;
///////////////////////////////////////////////////////////////
  int UseAxisRangeForXLabels;
  int UseAxisRangeForYLabels;
  int UseAxisRangeForZLabels;

  double UserXLabelRange[2];
  double UserYLabelRange[2];
  double UserZLabelRange[2];

///////////////////////////////////////////////////////////////



  
private:
  vtkMimxCubeAxesActor(const vtkMimxCubeAxesActor&); // Not implemented
  void operator=(const vtkMimxCubeAxesActor&); // Not implemented

  vtkSetStringMacro(ActualXLabel);
  vtkSetStringMacro(ActualYLabel);
  vtkSetStringMacro(ActualZLabel);

  vtkTimeStamp BuildTime;
  int LastXPow;
  int LastYPow;
  int LastZPow;
  int UserXPow;
  int UserYPow;
  int UserZPow;
  bool AutoLabelScaling;
  int LastXAxisDigits;
  int LastYAxisDigits;
  int LastZAxisDigits;
  double LastXRange[2];
  double LastYRange[2];
  double LastZRange[2];
  int   LastFlyMode;

  int   RenderAxesX[4];
  int   RenderAxesY[4];
  int   RenderAxesZ[4];
  int   NumberOfAxesX;
  int   NumberOfAxesY;
  int   NumberOfAxesZ;

  bool MustAdjustXValue;
  bool MustAdjustYValue;
  bool MustAdjustZValue;
  bool ForceXLabelReset;
  bool ForceYLabelReset;
  bool ForceZLabelReset;

  // various helper methods
  void  TransformBounds(vtkViewport *viewport, const double bounds[6], 
                        double pts[8][3]);
  void  AdjustAxes(double bounds[6],
                   double xCoords[4][6], double yCoords[4][6], 
                   double zCoords[4][6],
                   double xRange[2], double yRange[2], double zRange[2]);

  bool  ComputeTickSize(double bounds[6]);
  void  AdjustValues(const double bounds[6]);
  void  AdjustRange(const double bounds[6]);
  void  BuildAxes(vtkViewport *);
  void  DetermineRenderAxes(vtkViewport *);
  void  SetNonDependentAttributes(void);
  void  BuildLabels(vtkAxisActor *axes[4]);
  void  AdjustTicksComputeRange(vtkAxisActor *axes[4]);

////////////////////////////////////////////////////////////////////////
 void ComputeLabelValuesForSpecifiedRange(double inRange[2],
                                           double& majorStartValue,
                                           double& majorDelta,
                                           double& minorStartValue,
                                           double& minorDelta);

///////////////////////////////////////////////////////////////////////
  // hide the superclass' ShallowCopy() from the user and the compiler.
  void ShallowCopy(vtkProp *prop) { this->vtkProp::ShallowCopy( prop ); };
};


#endif

