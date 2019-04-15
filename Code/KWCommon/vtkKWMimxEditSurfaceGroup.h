/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxEditSurfaceGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/16 22:16:55 $
Version:   $Revision: 1.1 $

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
// .NAME vtkKWMimxEditSurfaceGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxEditSurfaceGroup_h
#define __vtkKWMimxEditSurfaceGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;

// added for plane cut
class vtkPlane;
class vtkPlaneWidget;
class vtkClipPolyData;
class vtkActor;
class vtkPolyDataMapper;
class vtkTIPWCallback;
class vtkKWMenuButtonWithLabel;

class vtkKWCheckButtonWithLabel;
class vtkKWPushButton ;
class vtkMimxPatchSurfaceMesh ;
class vtkMimxCubeAxesActor ;
class vtkFillHolesFilter ;

// added for box..........

class vtkPlanes ;
class vtkBoxWidget ;
class vtkSphere;
class vtkSphereWidget;
class vtkClipBWCallback ;
class vtkClipSWCallback;
class vtkCamera ;
class vtkKWEntryWithLabel;
class vtkMassProperties ;
class vtkKWPushButton ;
class vtkKWFrameWithLabel ;
class vtkKWFrame ;
class vtkPatchSurfaceMeshUsingTPS ;
class vtkAppendPolyData ;
class vtkKWScale ;

class vtkMPPlane1Callback;
class vtkMPPlane2Callback;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxEditSurfaceGroup : public vtkKWMimxMenuOptionGroup
{
public:
	static vtkKWMimxEditSurfaceGroup* New();
	vtkTypeRevisionMacro(vtkKWMimxEditSurfaceGroup,vtkKWMimxMenuOptionGroup);
	void PrintSelf(ostream& os, vtkIndent indent);
	virtual void Update();
	virtual void UpdateEnableState();
	void SaveVTKSurfaceDoneCallback();
	void SaveVTKSurfaceCancelCallback();
	int SaveVTKSurfaceApplyCallback();
	void UpdateObjectLists();
	void PlaneClipSurfaceCallback();
	void SphereClipSurfaceCallback();
	void BoxClipSurfaceCallback();
	void InvertPlaneCallback(int mode);
	void VolumeChangedcallback(const char *);
	void X_NormalCallback();
	void Y_NormalCallback();
	void Z_NormalCallback();
	void AxesStateCallback(int state);
	void InvertBoxCallback(int mode1);

	void XAnglechangedcallback(const char *);
	void YAnglechangedcallback(const char *);
	void ZAnglechangedcallback(const char *);
	
	// added

	void MPClipSurfaceCallback();
	void MP_XAnglechangedcallback1(const char *);
	void MP_YAnglechangedcallback1(const char *);
	void MP_ZAnglechangedcallback1(const char *);

	void MP_XAnglechangedcallback2(const char *);
	void MP_YAnglechangedcallback2(const char *);
	void MP_ZAnglechangedcallback2(const char *);



	vtkClipPolyData *Clippolydata1;
	vtkClipPolyData *Clippolydata;
	vtkKWMenuButtonWithLabel *Menubutton;
	vtkKWEntryWithLabel *Entrybutton_IniVolume;
	vtkKWEntryWithLabel *Entrybutton_FinVolume;

protected:
	vtkKWMimxEditSurfaceGroup();
	~vtkKWMimxEditSurfaceGroup();
	// added for plane
	vtkPlane *Plane;
	vtkPlaneWidget *PlaneWidget;
	vtkPolyDataMapper *SelectedSurfaceMapper;
	vtkActor *SelectedSurfaceActor;
	vtkTIPWCallback *PlaneCallback;
	vtkFillHolesFilter *Patchsurface;
	vtkMimxPatchSurfaceMesh *Patchsurface_Plane_loop;
	vtkMimxPatchSurfaceMesh *Patchsurface_Box_loop;
	vtkKWCheckButtonWithLabel *PatchButton;
	vtkKWCheckButtonWithLabel *PatchButtonTPS;
	vtkPatchSurfaceMeshUsingTPS *patchTPS ;
	vtkPatchSurfaceMeshUsingTPS *patchTPS_BC ;

	
// added for box

	vtkPlanes *Planes;
	vtkBoxWidget *boxwidget ;
	vtkPolyDataMapper *SelectedSurfaceMapper1;
	vtkActor *SelectedSurfaceActor1;
	vtkClipBWCallback *BoxCallback ;
	vtkClipSWCallback *SphereClipCallback;
	vtkCamera *camera ;
	vtkKWCheckButtonWithLabel *InvertPlaneButton;
	vtkKWCheckButtonWithLabel *InvertBoxButton;
	vtkMassProperties *VolumeProperty ;
	vtkKWPushButton *xnormal ;
	vtkKWPushButton *ynormal ;
	vtkKWPushButton *znormal ;
	vtkKWFrameWithLabel *Normal_label_frame ;
	vtkKWFrame *InvertPlaneFrame ;
	vtkKWFrame *InvertBoxFrame ;
	vtkMimxCubeAxesActor *axes ;
	vtkKWCheckButtonWithLabel *AxesButton;
	vtkAppendPolyData *append ;

	vtkKWEntryWithLabel *X_AngleEntrybutton;
	vtkKWEntryWithLabel *Y_AngleEntrybutton;
	vtkKWEntryWithLabel *Z_AngleEntrybutton;
	
	vtkKWFrameWithLabel *PlaneOrientationFrame ;

	vtkKWFrameWithLabel *MPPlaneOrientationFrame1 ;
	vtkKWFrameWithLabel *MPPlaneOrientationFrame2 ;

	vtkKWEntryWithLabel *MPX_AngleEntrybutton1;
	vtkKWEntryWithLabel *MPY_AngleEntrybutton1;
	vtkKWEntryWithLabel *MPZ_AngleEntrybutton1;

	vtkKWEntryWithLabel *MPX_AngleEntrybutton2;
	vtkKWEntryWithLabel *MPY_AngleEntrybutton2;
	vtkKWEntryWithLabel *MPZ_AngleEntrybutton2;

	vtkAppendPolyData *MPappend ;


	vtkMPPlane1Callback *MPPlaneCallback1;
	vtkPlane *MPPlane1;
	vtkSphereWidget *SphereWidget;
	vtkPlaneWidget *MPPlaneWidget1;
	vtkClipPolyData *MPClippolydata1;


	vtkMPPlane2Callback *MPPlaneCallback2;
	vtkPlane *MPPlane2;
	vtkPlaneWidget *MPPlaneWidget2;
	vtkClipPolyData *MPClippolydata2;



	vtkPolyDataMapper *MPSelectedSurfaceMapper1;
	vtkActor *MPSelectedSurfaceActor1;

	vtkPolyDataMapper *MPSelectedSurfaceMapper2;
	vtkActor *MPSelectedSurfaceActor2;


	
	virtual void CreateWidget();
	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkKWLoadSaveDialog *FileBrowserDialog;
	vtkSphere *Sphere;

private:
  vtkKWMimxEditSurfaceGroup(const vtkKWMimxEditSurfaceGroup&); // Not implemented
void operator=(const vtkKWMimxEditSurfaceGroup&); // Not implemented
 };

#endif

