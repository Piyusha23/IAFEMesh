/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxEditSurfaceGroup.cxx,v $
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

#include "vtkKWMimxEditSurfaceGroup.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkClipPolyData.h>
#include <vtkPlaneWidget.h>
#include <vtkCommand.h>
#include <vtkPolyDataWriter.h>
#include <vtkBoxWidget.h>
#include <vtkPlanes.h>
#include <vtkCamera.h>
#include <vtkMassProperties.h>
#include <vtkFillHolesFilter.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAppendPolyData.h>
#include <vtkCollection.h>
#include <vtkSphereWidget.h>
#include <vtkSphere.h>
#include <vtkRenderer.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

#include <vtkKWLoadSaveDialog.h>
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWPushButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWRenderWidget.h>

#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkMimxPatchSurfaceMesh.h"
#include "vtkMimxCubeAxesActor.h"
#include "vtkPatchSurfaceMeshUsingTPS.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxEditSurfaceGroup);
vtkCxxRevisionMacro(vtkKWMimxEditSurfaceGroup, "$Revision: 1.1 $");


//----------------------------------------------------------------------------
// callback for cutting plane
class vtkTIPWCallback : public vtkCommand
{
public:
	static vtkTIPWCallback *New() 
	{ return new vtkTIPWCallback; }
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkPlaneWidget *planeWidget =  reinterpret_cast<vtkPlaneWidget*>(caller);
		planeWidget->GetPlane(this->Plane);			
		
		//const char * SurfaceCut_filename = Edit_Surface_Group->Menubutton->GetWidget()->GetValue() ;			
		//vtkPolyData *changed_surface = NULL ;		
		vtkMassProperties *VolumeProperty = vtkMassProperties::New();	
		VolumeProperty->SetInput(Edit_Surface_Group->Clippolydata->GetOutput());
		VolumeProperty->GetVolume();	
		VolumeProperty->Update();
		this->Edit_Surface_Group->Entrybutton_FinVolume->GetWidget()->SetValueAsDouble(VolumeProperty->GetVolume());		
	}
	vtkTIPWCallback():Plane(0) {}
	vtkPlane *Plane;		
	vtkKWMimxEditSurfaceGroup *Edit_Surface_Group ;
	
};

// callback for box widget
class vtkClipBWCallback : public vtkCommand
{
public:
	static vtkClipBWCallback *New() 
	{ return new vtkClipBWCallback; }
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkBoxWidget *boxwidget = reinterpret_cast<vtkBoxWidget*>(caller);
		boxwidget->GetPlanes(this->Planes);			

		//const char * SurfaceCut_filename = Edit_Surface_Group1->Menubutton->GetWidget()->GetValue() ;			
		//vtkPolyData *changed_surface = NULL ;		
		vtkMassProperties *VolumeProperty = vtkMassProperties::New();	
		VolumeProperty->SetInput(Edit_Surface_Group1->Clippolydata1->GetOutput());
		VolumeProperty->GetVolume();	
		VolumeProperty->Update();
		this->Edit_Surface_Group1->Entrybutton_FinVolume->GetWidget()->SetValueAsDouble(VolumeProperty->GetVolume());
	}

	vtkClipBWCallback():Planes(0) {}
	vtkPlanes *Planes ;	
	vtkKWMimxEditSurfaceGroup *Edit_Surface_Group1 ;
};

class vtkClipSWCallback : public vtkCommand
{
public:
	static vtkClipSWCallback *New() 
	{ return new vtkClipSWCallback; }
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSphereWidget *spherewidget = reinterpret_cast<vtkSphereWidget*>(caller);
		spherewidget->GetSphere(this->Sphere);			
	}

	vtkClipSWCallback():Sphere(0) {}
	vtkSphere *Sphere ;	
	vtkKWMimxEditSurfaceGroup *Edit_Surface_Group ;
};
//////////// added to create multiple planes call back for 2 planes////////////////////////////////////////////////////

class vtkMPPlane1Callback : public vtkCommand
{
public:
	static vtkMPPlane1Callback *New() 
	{ return new vtkMPPlane1Callback; }
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkPlaneWidget *MPPlaneWidget1 =  reinterpret_cast<vtkPlaneWidget*>(caller);
		MPPlaneWidget1->GetPlane(this->MPPlane1);					
		//const char * SurfaceCut_filename = MPEdit_Surface_Group1->Menubutton->GetWidget()->GetValue() ;			
		//vtkPolyData *changed_surface = NULL ;		
	}
	vtkMPPlane1Callback():MPPlane1(0) {}
	vtkPlane *MPPlane1;		
	vtkKWMimxEditSurfaceGroup *MPEdit_Surface_Group1 ;
	
};

class vtkMPPlane2Callback : public vtkCommand
{
public:
	static vtkMPPlane2Callback *New() 
	{ return new vtkMPPlane2Callback; }
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkPlaneWidget *MPPlaneWidget2 =  reinterpret_cast<vtkPlaneWidget*>(caller);
		MPPlaneWidget2->GetPlane(this->MPPlane2);		
		
		//const char * SurfaceCut_filename = MPEdit_Surface_Group2->Menubutton->GetWidget()->GetValue() ;			
		//vtkPolyData *changed_surface = NULL ;				
	}
	vtkMPPlane2Callback():MPPlane2(0) {}
	vtkPlane *MPPlane2;		
	vtkKWMimxEditSurfaceGroup *MPEdit_Surface_Group2 ;
	
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


vtkKWMimxEditSurfaceGroup::vtkKWMimxEditSurfaceGroup()
{
  this->ObjectListComboBox = NULL;
  this->FileBrowserDialog = NULL;
  this->Plane = NULL;
  this->PlaneWidget = NULL;
  this->Clippolydata = NULL;
  this->SelectedSurfaceActor = vtkActor::New();
//  this->SelectedSurfaceActor->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper = vtkPolyDataMapper::New();
  this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
  this->PlaneCallback = vtkTIPWCallback::New();
  this->PlaneCallback->Edit_Surface_Group = this ;  
  this->Menubutton = NULL;
  this->PatchButton = NULL ;
  this->Patchsurface = NULL ;
  this->Planes = NULL;
  this->boxwidget = NULL;
  this->Clippolydata1 = NULL;
  this->SelectedSurfaceActor1 = vtkActor::New();
//  this->SelectedSurfaceActor1->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper1 = vtkPolyDataMapper::New();
  this->SelectedSurfaceActor1->SetMapper(this->SelectedSurfaceMapper1);
  this->BoxCallback = vtkClipBWCallback::New();
  this->SphereClipCallback = vtkClipSWCallback::New();
  this->SphereClipCallback->Edit_Surface_Group = this;
  this->BoxCallback->Edit_Surface_Group1 = this ;
  this->camera = NULL ;	
  this->InvertPlaneButton = NULL;
  this->xnormal = NULL ;
  this->ynormal = NULL ;
  this->znormal = NULL ;
  this->Normal_label_frame = NULL ;  
  this->axes = NULL ;
  this->AxesButton = NULL ;
  this->InvertPlaneFrame = NULL ;
  this->Entrybutton_IniVolume = NULL ;
  this->Entrybutton_FinVolume = NULL ;   
  this->InvertBoxButton = NULL ;
  this->InvertBoxFrame = NULL;
  this->Patchsurface_Plane_loop = NULL ;
  this->PatchButtonTPS = NULL ;
  this->append = NULL ;

  this->X_AngleEntrybutton = NULL ; 
  this->Y_AngleEntrybutton = NULL ; 
  this->Z_AngleEntrybutton = NULL ; 

  this->PlaneOrientationFrame = NULL ;

// multiple planes 
  this->MPPlaneCallback1 = vtkMPPlane1Callback::New();
  this->MPPlaneCallback1->MPEdit_Surface_Group1 = this ;  
  this->MPPlaneWidget1 = NULL;
  this->MPPlane1 = NULL ;
  this->MPClippolydata1 = NULL ;

  this->MPPlaneCallback2 = vtkMPPlane2Callback::New();
  this->MPPlaneCallback2->MPEdit_Surface_Group2 = this ;  
  this->MPPlaneWidget2 = NULL;
  this->MPPlane2 = NULL ;
  this->MPClippolydata2 = NULL ;

  this->MPSelectedSurfaceActor1 =vtkActor::New();
  this->MPSelectedSurfaceMapper1 = vtkPolyDataMapper::New();  
  this->MPSelectedSurfaceActor1->SetMapper(this->MPSelectedSurfaceMapper1);

  this->MPSelectedSurfaceActor2 =vtkActor::New();
  this->MPSelectedSurfaceMapper2 = vtkPolyDataMapper::New();  
  this->MPSelectedSurfaceActor2->SetMapper(this->MPSelectedSurfaceMapper2);

  this->MPPlaneOrientationFrame1 = NULL ;
  this->MPPlaneOrientationFrame2 = NULL ;

  this->MPX_AngleEntrybutton1 = NULL ;
  this->MPY_AngleEntrybutton1 = NULL ;
  this->MPZ_AngleEntrybutton1 = NULL ;

  this->MPX_AngleEntrybutton2 = NULL ;
  this->MPY_AngleEntrybutton2 = NULL ;
  this->MPZ_AngleEntrybutton2 = NULL ;

  this->MPappend = NULL ;
  this->SphereWidget = NULL;
  this->Sphere = NULL;
}
//----------------------------------------------------------------------------
vtkKWMimxEditSurfaceGroup::~vtkKWMimxEditSurfaceGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
  if (this->Plane)
	  this->Plane->Delete();
  if (this->PlaneWidget)
	  this->PlaneWidget->Delete();
  if (this->Clippolydata)
	  this->Clippolydata->Delete();	  
	  this->SelectedSurfaceActor->Delete();
      this->SelectedSurfaceMapper->Delete();
	  this->PlaneCallback->Delete();
  if (this->Menubutton)
      this->Menubutton->Delete();
  if(this->PatchButton)
	  this->PatchButton->Delete();
  if(this->Planes)
	  this->Planes->Delete();
  if(this->boxwidget)
	  this->boxwidget->Delete();
  if(this->Clippolydata1)
	  this->Clippolydata1->Delete();
	  this->SelectedSurfaceActor1->Delete();
	  this->SelectedSurfaceMapper1->Delete();
	  this->BoxCallback->Delete();
	  this->camera->Delete(); 
  if (this->InvertPlaneButton)
	  this->InvertPlaneButton->Delete();
  if(this->xnormal)
	  this->xnormal->Delete();
  if(this->ynormal)
	  this->ynormal->Delete();
  if(this->znormal)
	  this->znormal->Delete();
  if(this->Normal_label_frame)
	  this->Normal_label_frame->Delete();
  if(this->axes)
	  this->axes->Delete();
  if(this->AxesButton)
	  this->AxesButton->Delete();
  if(this->InvertPlaneFrame)
	  this->InvertPlaneFrame->Delete();
  if(this->Entrybutton_IniVolume)
	  this->Entrybutton_IniVolume->Delete();
  if(this->Entrybutton_FinVolume)
	  this->Entrybutton_FinVolume->Delete(); 
  if(this->InvertBoxButton)
	  this->InvertBoxButton->Delete();
  if(this->InvertBoxFrame)
	  this->InvertBoxFrame->Delete();

  if(this->Patchsurface_Plane_loop)
	  this->Patchsurface_Plane_loop->Delete();

  if(this->PatchButtonTPS)
	  this->PatchButtonTPS->Delete();
  if(this->append)
	  this->append->Delete();

  if(this->X_AngleEntrybutton)
	 this->X_AngleEntrybutton->Delete();

  if(this->Y_AngleEntrybutton)
	 this->Y_AngleEntrybutton->Delete();

  if(this->Z_AngleEntrybutton)
	 this->Z_AngleEntrybutton->Delete();

  if(this->PlaneOrientationFrame)
     this->PlaneOrientationFrame->Delete();
  
  if(this->MPPlaneWidget1)
     this->MPPlaneWidget1->Delete();

  if(this->MPPlane1)
     this->MPPlane1->Delete();

  if(this->MPClippolydata1)
     this->MPClippolydata1->Delete();

    if(this->MPPlaneWidget2)
     this->MPPlaneWidget2->Delete();

  if(this->MPPlane2)
     this->MPPlane2->Delete();

  if(this->MPClippolydata2)
     this->MPClippolydata2->Delete();

  if(this->MPPlaneOrientationFrame1)
	  this->MPPlaneOrientationFrame1->Delete();

  if(this->MPPlaneOrientationFrame2)
	  this->MPPlaneOrientationFrame2->Delete();

  if(this->MPX_AngleEntrybutton1)
	  this->MPX_AngleEntrybutton1->Delete();

  if(this->MPY_AngleEntrybutton1)
	  this->MPY_AngleEntrybutton1->Delete();

  if(this->MPZ_AngleEntrybutton1)
	  this->MPZ_AngleEntrybutton1->Delete();

  if(this->MPX_AngleEntrybutton2)
	  this->MPX_AngleEntrybutton1->Delete();

  if(this->MPY_AngleEntrybutton2)
	  this->MPY_AngleEntrybutton2->Delete();

  if(this->MPZ_AngleEntrybutton2)
	  this->MPZ_AngleEntrybutton2->Delete();

  if(this->MPappend)
	  this->MPappend->Delete();
  if(this->SphereWidget)
	  this->SphereWidget->Delete();
  if(this->Sphere)
	  this->Sphere->Delete();
  if(this->SphereClipCallback)
	  this->SphereClipCallback->Delete();
 }
//----------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::CreateWidget()
{
  if(this->IsCreated())
  {
  vtkErrorMacro("class already created");
  return;
  }

  this->Superclass::CreateWidget();

  if (!this->ObjectListComboBox)
    this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();  
  this->ObjectListComboBox->SetParent(this->MainFrame);
  this->ObjectListComboBox->Create();
  this->ObjectListComboBox->SetWidth(20);
  this->ObjectListComboBox->SetLabelText("Surface ");
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 2 -fill x", 
  this->ObjectListComboBox->GetWidgetName());			

  if(!this->Menubutton)
  this->Menubutton = vtkKWMenuButtonWithLabel::New();  
  this->Menubutton->SetParent(this->MainFrame);
  this->Menubutton->Create();
  this->Menubutton->SetWidth(20);
  this->Menubutton->SetLabelText(" Clip Function ");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Plane",this, "PlaneClipSurfaceCallback");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Box",this, "BoxClipSurfaceCallback");  
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Multiple-Planes",this, "MPClipSurfaceCallback"); 
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
	  "Sphere", this, "SphereClipSurfaceCallback");

  this->Menubutton->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 25 -fill x", 
  this->Menubutton->GetWidgetName());		

  if (!this->InvertPlaneFrame)	
  this->InvertPlaneFrame = vtkKWFrame::New();
  this->InvertPlaneFrame->SetParent(this->MainFrame);
  this->InvertPlaneFrame->Create();  
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 4 -fill x", 
  this->InvertPlaneFrame->GetWidgetName());
 
  if (!this->InvertPlaneButton)
  this->InvertPlaneButton = vtkKWCheckButtonWithLabel::New();
  this->InvertPlaneButton->SetParent(this->InvertPlaneFrame);
  this->InvertPlaneButton->Create();
  this->InvertPlaneButton->SetLabelText("  Invert View");
  this->InvertPlaneButton->GetWidget()->SetCommand(this, "InvertPlaneCallback");
  this->InvertPlaneButton->GetWidget()->SelectedStateOff();
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -padx 2 -pady 6",  
  this->InvertPlaneButton->GetWidgetName());  

  if (!this->InvertBoxFrame)	
  this->InvertBoxFrame = vtkKWFrame::New();
  this->InvertBoxFrame->SetParent(this->MainFrame);
  this->InvertBoxFrame->Create();  
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 4 -fill x", 
  this->InvertBoxFrame->GetWidgetName());

  if (!this->InvertBoxButton)
  this->InvertBoxButton = vtkKWCheckButtonWithLabel::New();
  this->InvertBoxButton->SetParent(this->InvertBoxFrame);
  this->InvertBoxButton->Create();
  this->InvertBoxButton->SetLabelText("  Inside Out");
  this->InvertBoxButton->GetWidget()->SetCommand(this, "InvertBoxCallback");
  this->InvertBoxButton->GetWidget()->SelectedStateOff();
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -padx 2 -pady 6",  
  this->InvertBoxButton->GetWidgetName());  

  if (!this->AxesButton)
  this->AxesButton = vtkKWCheckButtonWithLabel::New();
  this->AxesButton->SetParent(this->MainFrame);
  this->AxesButton->Create();
  this->AxesButton->SetLabelText("Axes On/Off");
  this->AxesButton->GetWidget()->SelectedStateOff();
  this->AxesButton->GetWidget()->SetCommand(this, "AxesStateCallback");  
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -expand y -padx 2 -pady 6 -fill x",  
  this->AxesButton->GetWidgetName());  

  if (!this->PatchButton)
  this->PatchButton = vtkKWCheckButtonWithLabel::New();
  this->PatchButton->SetParent(this->MainFrame);
  this->PatchButton->Create();
  this->PatchButton->SetLabelText("Fill Holes    ");
  this->PatchButton->GetWidget()->SelectedStateOff();
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -expand y -padx 2 -pady 6 -fill x",  
  this->PatchButton->GetWidgetName());  

  if (!this->PatchButtonTPS)
  this->PatchButtonTPS = vtkKWCheckButtonWithLabel::New();
  this->PatchButtonTPS->SetParent(this->MainFrame);
  this->PatchButtonTPS->Create();
  this->PatchButtonTPS->SetLabelText("Patch Using TPS    ");
  this->PatchButtonTPS->GetWidget()->SelectedStateOff();
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -expand y -padx 2 -pady 6 -fill x",  
  this->PatchButtonTPS->GetWidgetName());  

  if(!this->Entrybutton_IniVolume)
  this->Entrybutton_IniVolume = vtkKWEntryWithLabel::New();
  this->Entrybutton_IniVolume->SetParent(this->MainFrame);
  this->Entrybutton_IniVolume->Create();
  this->Entrybutton_IniVolume->GetWidget()->SetCommand(this,"VolumeChangedcallback");    
  this->Entrybutton_IniVolume->SetWidth(5); 
  this->Entrybutton_IniVolume->SetLabelText("Initial Volume");
  this->Entrybutton_IniVolume->SetLabelWidth(30);  
  this->Entrybutton_IniVolume->GetWidget()->SetStateToReadOnly();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 1 -pady 1 -fill x", 
  this->Entrybutton_IniVolume->GetWidgetName());

  if(!this->Entrybutton_FinVolume)
  this->Entrybutton_FinVolume = vtkKWEntryWithLabel::New();
  this->Entrybutton_FinVolume->SetParent(this->MainFrame);
  this->Entrybutton_FinVolume->Create();
  this->Entrybutton_FinVolume->GetWidget()->SetCommand(this,"VolumeChangedcallback");    
  this->Entrybutton_FinVolume->SetWidth(5); 
  this->Entrybutton_FinVolume->SetLabelText("Final Volume");
  this->Entrybutton_FinVolume->SetLabelWidth(30);  
  this->Entrybutton_FinVolume->GetWidget()->SetStateToReadOnly();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 1 -pady 1 -fill x", 
  this->Entrybutton_FinVolume->GetWidgetName());

  if (!this->Normal_label_frame)	
  this->Normal_label_frame = vtkKWFrameWithLabel::New();
  this->Normal_label_frame->SetParent(this->MainFrame);
  this->Normal_label_frame->Create();
  this->Normal_label_frame->SetLabelText("Normal Direction");
  this->Normal_label_frame->CollapseFrame( );
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
  this->Normal_label_frame->GetWidgetName());

  if (!this->PlaneOrientationFrame)	
  this->PlaneOrientationFrame = vtkKWFrameWithLabel::New();
  this->PlaneOrientationFrame->SetParent(this->MainFrame);
  this->PlaneOrientationFrame->Create();
  this->PlaneOrientationFrame->SetLabelText("Plane's Orientation wrt CS");
  this->PlaneOrientationFrame->CollapseFrame();  
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
  this->PlaneOrientationFrame->GetWidgetName());


  if(!this->xnormal)
  this->xnormal = vtkKWPushButton::New();  
  this->xnormal->SetParent(this->Normal_label_frame->GetFrame());
  this->xnormal->Create();
  this->xnormal->SetCommand(this, "X_NormalCallback");
  this->xnormal->SetText("X Normal");
  this->GetApplication()->Script(
  "pack %s -side left -anchor nw -expand 0 -padx 2 -pady 2 ", 
  this->xnormal->GetWidgetName());

  if(!this->ynormal)
  this->ynormal = vtkKWPushButton::New();  
  this->ynormal->SetParent(this->Normal_label_frame->GetFrame());
  this->ynormal->Create();
  this->ynormal->SetCommand(this, "Y_NormalCallback");
  this->ynormal->SetText("Y Normal");
  this->GetApplication()->Script(
      "pack %s -side left -anchor nw -expand 0 -padx 2 -pady 2 ", 
  this->ynormal->GetWidgetName());

  if(!this->znormal)
  this->znormal = vtkKWPushButton::New();  
  this->znormal->SetParent(this->Normal_label_frame->GetFrame());
  this->znormal->Create();
  this->znormal->SetCommand(this, "Z_NormalCallback");
  this->znormal->SetText("Z Normal");
  this->GetApplication()->Script(
      "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 2 ", 
  this->znormal->GetWidgetName());

  //////////////////////////////////////////////////////////////////////////////
  if(!this->X_AngleEntrybutton)
  this->X_AngleEntrybutton = vtkKWEntryWithLabel::New();
  this->X_AngleEntrybutton->SetParent(this->PlaneOrientationFrame->GetFrame());
  this->X_AngleEntrybutton->Create();
  this->X_AngleEntrybutton->GetWidget()->SetCommand(this,"XAnglechangedcallback");    
  this->X_AngleEntrybutton->SetWidth(5); 
//  this->X_AngleEntrybutton->SetLabelText("Y-X Plane");
  this->X_AngleEntrybutton->SetLabelText("Rot @ X Axis");
  this->X_AngleEntrybutton->SetLabelWidth(30);
  this->X_AngleEntrybutton->GetWidget()->SetValueAsInt(0);  
  this->X_AngleEntrybutton->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 ", 
  this->X_AngleEntrybutton->GetWidgetName());

  if(!this->Y_AngleEntrybutton)
  this->Y_AngleEntrybutton = vtkKWEntryWithLabel::New();
  this->Y_AngleEntrybutton->SetParent(this->PlaneOrientationFrame->GetFrame());
  this->Y_AngleEntrybutton->Create();
  this->Y_AngleEntrybutton->GetWidget()->SetCommand(this,"YAnglechangedcallback");    
  this->Y_AngleEntrybutton->SetWidth(5); 
//  this->Y_AngleEntrybutton->SetLabelText("Y-Z Plane");
  this->Y_AngleEntrybutton->SetLabelText("Rot @ Y Axis");
  this->Y_AngleEntrybutton->SetLabelWidth(30);
  this->Y_AngleEntrybutton->GetWidget()->SetValueAsInt(0);  
  this->Y_AngleEntrybutton->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand 0 -padx 1 -pady 1 ", 
  this->Y_AngleEntrybutton->GetWidgetName());

  if(!this->Z_AngleEntrybutton)
  this->Z_AngleEntrybutton = vtkKWEntryWithLabel::New();
  this->Z_AngleEntrybutton->SetParent(this->PlaneOrientationFrame->GetFrame());
  this->Z_AngleEntrybutton->Create();
  this->Z_AngleEntrybutton->GetWidget()->SetCommand(this,"ZAnglechangedcallback");      
  this->Z_AngleEntrybutton->SetWidth(5); 
//  this->Z_AngleEntrybutton->SetLabelText("Z-X Plane");
  this->Z_AngleEntrybutton->SetLabelText("Rot @ Z Axis");
  this->Z_AngleEntrybutton->SetLabelWidth(30);
  this->Z_AngleEntrybutton->GetWidget()->SetValueAsInt(0);  
  this->Z_AngleEntrybutton->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand 0 -padx 1 -pady 1 ", 
  this->Z_AngleEntrybutton->GetWidgetName());
  
  //////////////////////////////////////////////////////////////////////////////
  if (!this->MPPlaneOrientationFrame1)	
  this->MPPlaneOrientationFrame1 = vtkKWFrameWithLabel::New();
  this->MPPlaneOrientationFrame1->SetParent(this->MainFrame);
  this->MPPlaneOrientationFrame1->Create();
  this->MPPlaneOrientationFrame1->SetLabelText("Plane 1 Angle");
  this->MPPlaneOrientationFrame1->CollapseFrame();  
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
  this->MPPlaneOrientationFrame1->GetWidgetName());
  this->MPPlaneOrientationFrame1->Unpack();

  if (!this->MPPlaneOrientationFrame2)	
  this->MPPlaneOrientationFrame2 = vtkKWFrameWithLabel::New();
  this->MPPlaneOrientationFrame2->SetParent(this->MainFrame);
  this->MPPlaneOrientationFrame2->Create();
  this->MPPlaneOrientationFrame2->SetLabelText("Plane 2 Angle");
  this->MPPlaneOrientationFrame2->CollapseFrame();  
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
  this->MPPlaneOrientationFrame2->GetWidgetName());
  this->MPPlaneOrientationFrame2->Unpack();

// Added for the MP planes and angles entry 
  if(!this->MPX_AngleEntrybutton1)
  this->MPX_AngleEntrybutton1 = vtkKWEntryWithLabel::New();
  this->MPX_AngleEntrybutton1->SetParent(this->MPPlaneOrientationFrame1->GetFrame());
  this->MPX_AngleEntrybutton1->Create();
  this->MPX_AngleEntrybutton1->GetWidget()->SetCommand(this,"MP_XAnglechangedcallback1");      
  this->MPX_AngleEntrybutton1->SetWidth(5); 
//  this->MPX_AngleEntrybutton1->SetLabelText("X-Axis-Angle");

  this->MPX_AngleEntrybutton1->SetLabelText("Rot @ X Axis");
  this->MPX_AngleEntrybutton1->SetLabelWidth(30);
  this->MPX_AngleEntrybutton1->GetWidget()->SetValueAsInt(0);  
  this->MPX_AngleEntrybutton1->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 ", 
  this->MPX_AngleEntrybutton1->GetWidgetName());

  if(!this->MPY_AngleEntrybutton1)
  this->MPY_AngleEntrybutton1 = vtkKWEntryWithLabel::New();
  this->MPY_AngleEntrybutton1->SetParent(this->MPPlaneOrientationFrame1->GetFrame());
  this->MPY_AngleEntrybutton1->Create();
  this->MPY_AngleEntrybutton1->GetWidget()->SetCommand(this,"MP_YAnglechangedcallback1");      
  this->MPY_AngleEntrybutton1->SetWidth(5); 
//  this->MPY_AngleEntrybutton1->SetLabelText("Y-Axis-Angle");
  this->MPY_AngleEntrybutton1->SetLabelText("Rot @ Y Axis");
  this->MPY_AngleEntrybutton1->SetLabelWidth(30);
  this->MPY_AngleEntrybutton1->GetWidget()->SetValueAsInt(0);  
  this->MPY_AngleEntrybutton1->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 ", 
  this->MPY_AngleEntrybutton1->GetWidgetName());

  if(!this->MPZ_AngleEntrybutton1)
  this->MPZ_AngleEntrybutton1 = vtkKWEntryWithLabel::New();
  this->MPZ_AngleEntrybutton1->SetParent(this->MPPlaneOrientationFrame1->GetFrame());
  this->MPZ_AngleEntrybutton1->Create();
  this->MPZ_AngleEntrybutton1->GetWidget()->SetCommand(this,"MP_ZAnglechangedcallback1");      
  this->MPZ_AngleEntrybutton1->SetWidth(5); 
//  this->MPZ_AngleEntrybutton1->SetLabelText("Z-Axis-Angle");
  this->MPZ_AngleEntrybutton1->SetLabelText("Rot @ Z Axis");
  this->MPZ_AngleEntrybutton1->SetLabelWidth(30);
  this->MPZ_AngleEntrybutton1->GetWidget()->SetValueAsInt(0);  
  this->MPZ_AngleEntrybutton1->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 ", 
  this->MPZ_AngleEntrybutton1->GetWidgetName());



    if(!this->MPX_AngleEntrybutton2)
  this->MPX_AngleEntrybutton2 = vtkKWEntryWithLabel::New();
  this->MPX_AngleEntrybutton2->SetParent(this->MPPlaneOrientationFrame2->GetFrame());
  this->MPX_AngleEntrybutton2->Create();
  this->MPX_AngleEntrybutton2->GetWidget()->SetCommand(this,"MP_XAnglechangedcallback2");      
  this->MPX_AngleEntrybutton2->SetWidth(5); 
//  this->MPX_AngleEntrybutton2->SetLabelText("X-Axis-Angle");
  this->MPX_AngleEntrybutton2->SetLabelText("Rot @ X Axis");
  this->MPX_AngleEntrybutton2->SetLabelWidth(30);
  this->MPX_AngleEntrybutton2->GetWidget()->SetValueAsInt(0);  
  this->MPX_AngleEntrybutton2->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 ", 
  this->MPX_AngleEntrybutton2->GetWidgetName());

  if(!this->MPY_AngleEntrybutton2)
  this->MPY_AngleEntrybutton2 = vtkKWEntryWithLabel::New();
  this->MPY_AngleEntrybutton2->SetParent(this->MPPlaneOrientationFrame2->GetFrame());
  this->MPY_AngleEntrybutton2->Create();
  this->MPY_AngleEntrybutton2->GetWidget()->SetCommand(this,"MP_YAnglechangedcallback2");      
  this->MPY_AngleEntrybutton2->SetWidth(5); 
  //this->MPY_AngleEntrybutton2->SetLabelText("Y-Axis-Angle");
  this->MPY_AngleEntrybutton2->SetLabelText("Rot @ Y Axis");
  this->MPY_AngleEntrybutton2->SetLabelWidth(30);
  this->MPY_AngleEntrybutton2->GetWidget()->SetValueAsInt(0);  
  this->MPY_AngleEntrybutton2->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 ", 
  this->MPY_AngleEntrybutton2->GetWidgetName());

  if(!this->MPZ_AngleEntrybutton2)
  this->MPZ_AngleEntrybutton2 = vtkKWEntryWithLabel::New();
  this->MPZ_AngleEntrybutton2->SetParent(this->MPPlaneOrientationFrame2->GetFrame());
  this->MPZ_AngleEntrybutton2->Create();
  this->MPZ_AngleEntrybutton2->GetWidget()->SetCommand(this,"MP_ZAnglechangedcallback2");      
  this->MPZ_AngleEntrybutton2->SetWidth(5); 
//  this->MPZ_AngleEntrybutton2->SetLabelText("Z-Axis-Angle");
  this->MPZ_AngleEntrybutton2->SetLabelText("Rot @ Z Axis");
  this->MPZ_AngleEntrybutton2->SetLabelWidth(30);
  this->MPZ_AngleEntrybutton2->GetWidget()->SetValueAsInt(0);  
  this->MPZ_AngleEntrybutton2->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 ", 
  this->MPZ_AngleEntrybutton2->GetWidgetName());


  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "SaveVTKSurfaceApplyCallback");
  this->CancelButton->SetCommand(this, "SaveVTKSurfaceCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxEditSurfaceGroup::SaveVTKSurfaceApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return 0;
	}
    vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return 0;
	}

	vtkMimxSurfacePolyDataActor *OriginalActor = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)));
	const char *foundation_name = OriginalActor->GetFoundationName();

	//vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
	//->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	const char * SurfaceCut_filename = Menubutton->GetWidget()->GetValue() ;	

	vtkPolyData *changed_surface = NULL ;		

	if(!strcmp(SurfaceCut_filename,"Plane"))
		{
			if(Plane)
			changed_surface = Clippolydata->GetOutput();								
		}

	if(!strcmp(SurfaceCut_filename,"Box"))
		{
			changed_surface = Clippolydata1->GetOutput();								
		}

	else
		if(!strcmp(SurfaceCut_filename,"Sphere"))
		{
			changed_surface = Clippolydata->GetOutput();	
		}
	else 
		if(!strcmp(SurfaceCut_filename,"Multiple-Planes"))
		{
			MPappend = vtkAppendPolyData::New();
			MPappend->AddInput(MPClippolydata1->GetOutput());
			MPappend->AddInput(MPClippolydata2->GetOutput());
			MPappend->Update();
			changed_surface = MPappend->GetOutput();								
		}

	if(!changed_surface)
	
		return 0 ;

	if(!this->Patchsurface)

		Patchsurface = vtkFillHolesFilter::New();

	if(!strcmp(SurfaceCut_filename,"Plane"))
	{
		if(this->PatchButton->GetWidget()->GetSelectedState())
		{
			Patchsurface->SetInput(Clippolydata->GetOutput());	
			Patchsurface->SetHoleSize(10000.0); // added for fill holes filter
			Patchsurface->Update();
		}

//////////// used to write out patch loops ////////////////////////////////////
		if(this->PatchButtonTPS->GetWidget()->GetSelectedState())
		{
			Patchsurface_Plane_loop = vtkMimxPatchSurfaceMesh::New();
			Patchsurface_Plane_loop->SetInput(Clippolydata->GetOutput());			
			Patchsurface_Plane_loop->Update();
			Patchsurface_Plane_loop->GetContour(0);	

			int noc = Patchsurface_Plane_loop->GetNumberOfContours();
			append = NULL ; 

			append = vtkAppendPolyData::New();
			append->AddInput(changed_surface);
		
			vtkCollection *loopcollection = vtkCollection::New();

			for(int i = 0; i< noc; i++)
				{
					loopcollection->AddItem((vtkObject *) vtkPatchSurfaceMeshUsingTPS::New());				
				}

			for(int j = 0; j< noc ; j++)
			{
				vtkPatchSurfaceMeshUsingTPS *patchusingTPS = (vtkPatchSurfaceMeshUsingTPS *)loopcollection->GetItemAsObject(j);
				patchusingTPS->SetInput(Patchsurface_Plane_loop->GetContour(j));
				patchusingTPS->Update();
				append->AddInput(patchusingTPS->GetOutput());

				vtkPolyDataWriter *write = vtkPolyDataWriter::New();
				write->SetFileName("test_loop.vtk");
				write->SetInput(append->GetOutput());
				write->Write();

				patchusingTPS->Delete();

			}
			loopcollection->Delete();
		}

		if(this->PatchButton->GetWidget()->GetSelectedState())
		{
			this->SelectedSurfaceMapper->SetInput(Patchsurface->GetOutput());
		}
	
		else if(this->PatchButtonTPS->GetWidget()->GetSelectedState())

		this->SelectedSurfaceMapper->SetInput(append->GetOutput());			


//////////////////////////Added for appending the clipped surface and morphed surface end ////////////////////////

		else
		this->SelectedSurfaceMapper->SetInput(changed_surface);
		this->SelectedSurfaceMapper->Modified();
		this->SelectedSurfaceMapper->Update();		
	
	}
//	else // removed to accomodate multiple planes 
if(!strcmp(SurfaceCut_filename,"Box"))
	{
		Patchsurface->SetInput(Clippolydata1->GetOutput());
		Patchsurface->SetHoleSize(10000.0); // added for fill holes filter
		Patchsurface->Update();

//////////// used to write out patch loops ////////////////////////////////////

		Patchsurface_Box_loop = vtkMimxPatchSurfaceMesh::New();
		Patchsurface_Box_loop->SetInput(Clippolydata1->GetOutput());			
		Patchsurface_Box_loop->Update();
		Patchsurface_Box_loop->GetContour(0);	

// added for patching using TPS collection

		int noc1 = Patchsurface_Box_loop->GetNumberOfContours();
		append = NULL ;

		append = vtkAppendPolyData::New();
		append->AddInput(changed_surface);

		vtkCollection *loopcollection1 = vtkCollection::New();

		for(int i = 0; i< noc1; i++)
			{
				loopcollection1->AddItem((vtkObject *) vtkPatchSurfaceMeshUsingTPS::New());
			}

		for(int j = 0; j< noc1 ; j++)
		{
			vtkPatchSurfaceMeshUsingTPS *patchusingTPS1 = (vtkPatchSurfaceMeshUsingTPS *)loopcollection1->GetItemAsObject(j);
			patchusingTPS1->SetInput(Patchsurface_Box_loop->GetContour(j));
			patchusingTPS1->Update();
			append->AddInput(patchusingTPS1->GetOutput());		
			//patchusingTPS1->Delete();
		}
		append->Update();

		for(int j = 0; j< noc1 ; j++)
		{
			vtkPatchSurfaceMeshUsingTPS *patchusingTPS1 = (vtkPatchSurfaceMeshUsingTPS *)loopcollection1->GetItemAsObject(j);
			patchusingTPS1->Delete();
		}
		if(this->PatchButton->GetWidget()->GetSelectedState())
		{
			this->SelectedSurfaceMapper->SetInput(Patchsurface->GetOutput());
		}
	
		if(this->PatchButtonTPS->GetWidget()->GetSelectedState())

		this->SelectedSurfaceMapper->SetInput(append->GetOutput());			


//////////////////////////Added for appending the clipped surface and morphed surface end ////////////////////////
		else
		this->SelectedSurfaceMapper->SetInput(changed_surface);
		this->SelectedSurfaceMapper->Modified();
		this->SelectedSurfaceMapper->Update();		

		

/////////////////////////////////////////////////////////////////////////////////////////////

	}

// added for multiple planes ////////////

//else 
if(!strcmp(SurfaceCut_filename,"Multiple-Planes"))
{
		Patchsurface->SetInput(MPappend->GetOutput());
		Patchsurface->SetHoleSize(10000.0); // added for fill holes filter
		Patchsurface->Update();

		vtkPolyDataWriter *write1 = vtkPolyDataWriter::New();
			write1->SetFileName("multipleplanes_cut.vtk");
			write1->SetInput(Patchsurface->GetOutput());
			write1->Write();

//		MPappend = NULL ; 

		if(this->PatchButton->GetWidget()->GetSelectedState())
		{
			this->SelectedSurfaceMapper->SetInput(Patchsurface->GetOutput());
		}
	
		//if(this->PatchButtonTPS->GetWidget()->GetSelectedState())
		//{	
		//	append = vtkAppendPolyData::New();
		//	append->AddInput(changed_surface);
		//	append->AddInput(patchTPS_BC->GetOutput());
		//	append->Update();
		//	this->SelectedSurfaceMapper->SetInput(append->GetOutput());			
		//}

//////////////////////////Added for appending the clipped surface and morphed surface end ////////////////////////
		else
		this->SelectedSurfaceMapper->SetInput(changed_surface);

		vtkPolyDataWriter *write2 = vtkPolyDataWriter::New();
		write2->SetFileName("mp_unpatched.vtk");
		write2->SetInput(changed_surface);
		write2->Write();

		this->SelectedSurfaceMapper->Modified();
		this->SelectedSurfaceMapper->Update();		

/////////////////////////////////////////////////////////////////////////////////////////////

	}
// clipping with sphere widget
    if(!strcmp(SurfaceCut_filename,"Sphere"))
	{
		this->SelectedSurfaceMapper->SetInput(changed_surface);
		this->SelectedSurfaceMapper->Modified();
		this->SelectedSurfaceMapper->Update();		
	}

// added for multiple planes end 

	if(changed_surface)
	  {
		vtkPolyData *poly;

		if(this->PatchButton->GetWidget()->GetSelectedState()) // fill holes
		poly = Patchsurface->GetOutput();

		else if(this->PatchButtonTPS->GetWidget()->GetEnabled())

		{
		if(this->PatchButtonTPS->GetWidget()->GetSelectedState())// patch using tps
			poly = append->GetOutput();
		//changed_surface_actor->GetDataSet()->DeepCopy(Patchsurface->GetOutput());
		}
		else if(this->PatchButtonTPS->GetWidget()->GetSelectedState())//if(this->PatchButtonTPS->GetWidget()->GetEnabled())
		{
			if(this->PatchButtonTPS->GetWidget()->GetSelectedState())// patch using tps
			{
			//changed_surface_actor->GetDataSet()->DeepCopy(append->GetOutput());
			}
		}
		else
		poly = changed_surface;
		{
			//if(! this->PatchButton->GetWidget()->GetSelectedState() && ! this->PatchButtonTPS->GetWidget()->GetSelectedState())
			this->AddSurfaceToDisplay(changed_surface, "Edit", foundation_name, 1, NULL);
		}


// added to remove the original surface from the window and unselect the surface from the list-------------

		int surface1Index = -1;
		vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
		const char *name_surf1 = combobox_surf1->GetValue();  	
		surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  		this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
		this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

// added to remove the original surface from the window and unselect the surface from the list end -------------

	char surface_name[64]; 

	if(!strcmp(SurfaceCut_filename,"Sphere"))
		sprintf(surface_name, "SphereCut-");

	if(!strcmp(SurfaceCut_filename,"Plane"))
	sprintf(surface_name, "PlaneCut-");

	if(!strcmp(SurfaceCut_filename,"Box"))
	sprintf(surface_name, "BoxCut-");

	if(!strcmp(SurfaceCut_filename,"Multiple-Planes"))
	sprintf(surface_name, "MPCut-");

	this->AddSurfaceToDisplay(poly, surface_name, name, 1, NULL);

	this->UpdateObjectLists();
	}

    if(PlaneWidget)
  {
	PlaneWidget->SetEnabled(0);
	PlaneWidget->SetInteractor(NULL);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
	this->AxesButton->GetWidget()->SelectedStateOff();
	this->InvertPlaneButton->GetWidget()->SelectedStateOff();
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }
 
	if(SphereWidget)
	{
		SphereWidget->SetEnabled(0);
		SphereWidget->SetInteractor(NULL);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}

	if(boxwidget)
  {
	boxwidget->SetEnabled(0);  
	boxwidget->SetInteractor(NULL);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
	this->AxesButton->GetWidget()->SelectedStateOff();
	this->InvertBoxButton->GetWidget()->SelectedStateOff();
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }


// added for multiple planes plane 1

   if(MPPlaneWidget1)
  {
	MPPlaneWidget1->SetEnabled(0);  
	MPPlaneWidget1->SetInteractor(NULL);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
	this->AxesButton->GetWidget()->SelectedStateOff();	
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }

// added for multiple planes plane 2
      if(MPPlaneWidget2)
  {
	MPPlaneWidget2->SetEnabled(0);  
	MPPlaneWidget2->SetInteractor(NULL);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
	this->AxesButton->GetWidget()->SelectedStateOff();	
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor2);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }


// added for multiple planes plane 2 end 

	Patchsurface->Delete();
	Patchsurface = NULL ;
	
	

	//if(append )
	//{
	//	append->Delete();
	//	append = NULL ;
	//}
	
	return 0 ;
	

}
void vtkKWMimxEditSurfaceGroup::VolumeChangedcallback(const char *pr)
{	
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	VolumeProperty = vtkMassProperties::New();
	

	const char * SurfaceCut_filename = Menubutton->GetWidget()->GetValue() ;	
	vtkPolyData *changed_surface = NULL ;		
	if(!strcmp(SurfaceCut_filename,"Plane"))
		{
			if(Plane)
			changed_surface = Clippolydata->GetOutput();								
		}

	else

	if(!strcmp(SurfaceCut_filename,"Box"))
		{
			changed_surface = Clippolydata1->GetOutput();								
		}

	VolumeProperty->SetInput(polydata);
	VolumeProperty->GetVolume();	
	this->Entrybutton_IniVolume->GetWidget()->SetValueAsDouble(VolumeProperty->GetVolume());		
	return  ;
}

// CubeAxes actor On/Off button
void vtkKWMimxEditSurfaceGroup::AxesStateCallback(int state)
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Please Select Cutting Option");;
		return ;
	}

	if(state)
	{
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	axes = vtkMimxCubeAxesActor::New();
	axes->SetBounds(polydata->GetBounds());	
	double bds[6]; polydata->GetBounds(bds);
	axes->SetXAxisLabelRange(0,bds[1]-bds[0]);
	axes->SetYAxisLabelRange(0,bds[3]-bds[2]);
	axes->SetZAxisLabelRange(0,bds[5]-bds[4]);
	axes ->SetCamera (this->MimxMainWindow->GetRenderWidget()->GetRenderer()->GetActiveCamera());
    axes ->SetFlyModeToOuterEdges();
	axes->Modified();	
	this->MimxMainWindow->GetRenderWidget()->GetRenderer()->AddViewProp(axes);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}

	else
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();	
}

void vtkKWMimxEditSurfaceGroup::X_NormalCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Plane"))
		{
			callback->ErrorMessage("Choose Plane Option");
			return  ;
		}
		
	if(!this->PlaneWidget)
	PlaneWidget = vtkPlaneWidget::New();
	PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	PlaneWidget->SetPlaceFactor(0.5);	
	PlaneWidget->PlaceWidget();		
	PlaneWidget->SetNormal(1,0,0);
	PlaneWidget->SetHandleSize(0.005);
	PlaneWidget->AddObserver(vtkCommand::InteractionEvent,this->PlaneCallback);
//	PlaneWidget->SetEnabled(0);
	PlaneWidget->SetEnabled(1);
//	PlaneWidget->Modified();

	this->PlaneWidget->GetPlane(this->Plane);
	if(this->Clippolydata)
	{
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();	
	Clippolydata->Modified();	
	Clippolydata->Update();	
	}		
	
	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

void vtkKWMimxEditSurfaceGroup::Y_NormalCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Plane"))
	{
		callback->ErrorMessage("Choose Plane Option");
		return  ;
	}
	
	if(!this->PlaneWidget)
	PlaneWidget = vtkPlaneWidget::New();
	PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	PlaneWidget->SetPlaceFactor(0.5);	
	PlaneWidget->PlaceWidget();		
	PlaneWidget->SetNormal(0,1,0);
	PlaneWidget->SetHandleSize(0.005);
	PlaneWidget->AddObserver(vtkCommand::InteractionEvent,this->PlaneCallback);
//	PlaneWidget->SetEnabled(0);
	PlaneWidget->SetEnabled(1);
//	PlaneWidget->Modified();	

	this->PlaneWidget->GetPlane(this->Plane);
	if(this->Clippolydata)
	{
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();	
	Clippolydata->Modified();	
	Clippolydata->Update();	
	}
	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

void vtkKWMimxEditSurfaceGroup::Z_NormalCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Plane"))
		{
			callback->ErrorMessage("Choose Plane Option");
			return  ;
		}

	if(!this->PlaneWidget)
	PlaneWidget = vtkPlaneWidget::New();
	PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	PlaneWidget->SetPlaceFactor(0.5);	
	PlaneWidget->PlaceWidget();		
	PlaneWidget->SetNormal(0,0,1);
	PlaneWidget->SetHandleSize(0.005);
	PlaneWidget->AddObserver(vtkCommand::InteractionEvent,this->PlaneCallback);
//	PlaneWidget->SetEnabled(0);
	PlaneWidget->SetEnabled(1);
//	PlaneWidget->Modified();	

	this->PlaneWidget->GetPlane(this->Plane);
	if(this->Clippolydata)
	{
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();	
	Clippolydata->Modified();	
	Clippolydata->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//----------------------------------------------------------------------------
// Plane Clip Surface callback function body
void vtkKWMimxEditSurfaceGroup::PlaneClipSurfaceCallback()
{
	
	const char * box_filename = Menubutton->GetWidget()->GetValue() ;				
	if(!strcmp(box_filename,"Plane"))
		{
			this->xnormal->SetEnabled(1);
			this->ynormal->SetEnabled(1);
			this->znormal->SetEnabled(1);
		}
	this->GetApplication()->Script(
    "pack %s -side top -anchor n -padx 2 -pady 6",
    this->InvertPlaneButton->GetWidgetName());
	this->InvertBoxButton->Unpack();
	this->PatchButtonTPS->SetEnabled(1);
	this->PatchButtonTPS->GetWidget()->SelectedStateOff();
	

	// added to pack the plane orientation frame //////////

	this->MPPlaneOrientationFrame1->Unpack();
	this->MPPlaneOrientationFrame2->Unpack();

	this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x -before %s", 
	this->PlaneOrientationFrame->GetWidgetName(), this->ApplyButton->GetWidgetName());	

	this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x -before %s", 
	this->Normal_label_frame->GetWidgetName(), this->PlaneOrientationFrame->GetWidgetName());	
	
	// packing end /////////////////


	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

/////////// check if other widgets are active. If yes deactivete them and also the actors///////////////	
	if( boxwidget)
	{
	boxwidget->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
	}

	if( SphereWidget)
	{
		SphereWidget->SetEnabled(0);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
	}

	if( MPPlaneWidget1)
	{
	MPPlaneWidget1->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor1);
	}

	if( MPPlaneWidget2)
	{
	MPPlaneWidget2->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor2);
	}
/////////// check if other widgets are active. If yes deactivete them and also the actors///////////////
	
	if(!this->PlaneWidget)
	PlaneWidget = vtkPlaneWidget::New();
	PlaneWidget->KeyPressActivationOff();
	PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	PlaneWidget->SetPlaceFactor(0.5);
	PlaneWidget->SetInput(polydata);
	PlaneWidget->PlaceWidget();	
	PlaneWidget->SetEnabled(1);
	PlaneWidget->SetNormal(0,0,1);
	PlaneWidget->SetHandleSize(0.005);
	PlaneWidget->AddObserver(vtkCommand::InteractionEvent,this->PlaneCallback);

	if (!this->Plane)	
	Plane = vtkPlane::New();

	if(this->PlaneWidget)
	{
		this->PlaneWidget->GetPlane(this->Plane);	
	}

	if (!this->Clippolydata)	
	Clippolydata = vtkClipPolyData::New();
	Clippolydata->SetInput(polydata);
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();
	this->PlaneCallback->Plane = this->Plane ;
	Clippolydata->Modified();	
	Clippolydata->Update();	

	this->SelectedSurfaceMapper->SetInput(Clippolydata->GetOutput());
	this->SelectedSurfaceMapper->Update();
	this->SelectedSurfaceActor->SetMapper(SelectedSurfaceMapper);
	this->SelectedSurfaceActor->VisibilityOn();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();	
	
// added to remove the original surface from the window and unselect the surface from the list-------------
	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);	
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  		this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
		this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}
// added to remove the original surface from the window and unselect the surface from the list end -------

	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}
//----------------------------------------------------------------------------
// Box Clip Surface callback function body

void vtkKWMimxEditSurfaceGroup::BoxClipSurfaceCallback()
{
	
	this->PlaneOrientationFrame->Unpack();
	this->Normal_label_frame->Unpack();
	this->PatchButtonTPS->SetEnabled(1);
	this->PatchButtonTPS->GetWidget()->SelectedStateOff();

	this->MPPlaneOrientationFrame1->Unpack();
	this->MPPlaneOrientationFrame2->Unpack();

	const char * box_filename = Menubutton->GetWidget()->GetValue() ;				
	if(!strcmp(box_filename,"Box"))
		{
			this->xnormal->SetEnabled(0);
			this->ynormal->SetEnabled(0);
			this->znormal->SetEnabled(0);						
		}

//	this->PatchButtonTPS->SetEnabled(0);
//	this->PatchButtonTPS->GetWidget()->SelectedStateOff();
	this->InvertPlaneButton->Unpack();	

	this->GetApplication()->Script(
    "pack %s -side top -anchor n -padx 2 -pady 6",
	this->InvertBoxButton->GetWidgetName());

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

/////////// check if other widgets are active. If yes deactivete them and also the actors///////////////
	if( PlaneWidget)
	{
	PlaneWidget->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
	}

	if( SphereWidget)
	{
		SphereWidget->SetEnabled(0);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
	}

	if( MPPlaneWidget1)
	{
	MPPlaneWidget1->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor1);
	}

	if( MPPlaneWidget2)
	{
	MPPlaneWidget2->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor2);
	}

/////////// check if other widgets are active. If yes deactivete them and also the actors////////////////

	camera = vtkCamera::New();	
	camera->SetPosition(100,100,100);
	this->GetMimxMainWindow()->GetRenderWidget()->ResetCameraClippingRange();

    double aspect=400.0/300.0, planeequations[24];
    camera->GetFrustumPlanes(aspect, planeequations);
	
	if (!this->Planes)	
	Planes = vtkPlanes::New();
  
	double ped[24];   int i;
    for (i = 0; i < 24; ++i)
     {
      ped[i] = planeequations[i];
     }
     
	Planes->SetFrustumPlanes(ped);

	if(!this->boxwidget)
	boxwidget = vtkBoxWidget::New();	
	boxwidget->KeyPressActivationOff();
	boxwidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());	
	boxwidget->SetPlaceFactor(0.5);
	boxwidget->SetInput(polydata);
	boxwidget->PlaceWidget();	
	boxwidget->SetEnabled(1);	
	boxwidget->SetHandleSize(0.005);
	boxwidget->AddObserver(vtkCommand::InteractionEvent,this->BoxCallback);

	if (!this->Planes)	
	Planes = vtkPlanes::New();

	if(this->boxwidget)
	{
		this->boxwidget->GetPlanes(this->Planes);
	}

	if (!this->Clippolydata1)	
	Clippolydata1 = vtkClipPolyData::New();
	Clippolydata1->SetInput(polydata);
	Clippolydata1->SetClipFunction(Planes);	
	Clippolydata1->InsideOutOff();
	this->BoxCallback->Planes = this->Planes ; 			
	Clippolydata1->Modified();
	Clippolydata1->Update();	

	this->SelectedSurfaceMapper1->SetInput(Clippolydata1->GetOutput());	
	this->SelectedSurfaceMapper1->Update();
	this->SelectedSurfaceActor1->SetMapper(SelectedSurfaceMapper1);
	this->SelectedSurfaceActor1->VisibilityOn();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();	

// added to remove the original surface from the window and unselect the surface from the list-------------
	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);
	
	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);	
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}
// added to remove the original surface from the window and unselect the surface from the list end --------
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}
//----------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::InvertPlaneCallback( int mode )
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();	
	const char * invertplane_check = Menubutton->GetWidget()->GetValue() ;		
	if(strcmp(invertplane_check,"Plane"))
	return ;

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Please select surface and plane option");
		combobox->SetValue("");
		return ;
	}
	double normal[3] ;
	PlaneWidget->GetNormal(normal);
	this->PlaneWidget->SetNormal(-normal[0], -normal[1], -normal[2]);

	this->PlaneWidget->GetPlane(this->Plane);
	if(this->Clippolydata)
	{
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();	
	Clippolydata->Modified();	
	Clippolydata->Update();	
	}

	PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor()->Render();
	return ;
}
//----------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::InvertBoxCallback(int ibcb)
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();	
	const char * invertbox_check = Menubutton->GetWidget()->GetValue() ;		
	if(strcmp(invertbox_check,"Box"))
	return ;

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Please select surface and plane option");
		combobox->SetValue("");
		return ;
	}

	const char * invertbox_filename = Menubutton->GetWidget()->GetValue() ;	
	if(!strcmp(invertbox_filename,"Box"))
	  {		
		if (!this->Clippolydata1)	
		Clippolydata1 = vtkClipPolyData::New();
		if(this->InvertBoxButton->GetWidget()->GetSelectedState())
		Clippolydata1->InsideOutOn();
		else
		Clippolydata1->InsideOutOff();
		Clippolydata1->SetClipFunction(Planes);	
		Clippolydata1->Update();				
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	   }

	return;
}
//----------------------------------------------------------------------------

void vtkKWMimxEditSurfaceGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::SaveVTKSurfaceCancelCallback()
{ 

  this->CancelStatus = 1 ;
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor1);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor2);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  
  if(PlaneWidget)
  if(PlaneWidget->GetEnabled())
  {
	  if(PlaneWidget->GetInteractor()!= NULL)
	  {	  
		  PlaneWidget->SetEnabled(0);  
		  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
		  this->GetMimxMainWindow()->GetRenderWidget()->Render();
	  }
	  else

		PlaneWidget->SetInteractor(NULL);	
  }
  
  if(boxwidget)

  if(boxwidget->GetEnabled())
  {
	boxwidget->SetEnabled(0);  
	boxwidget->SetInteractor(NULL);	
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->axes);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
//	return ;
  }

  // added to add the existing surfaces back to the renderer--------------------------------------------

	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
	  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
		this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}	

// added to add the existing surfaces back to the renderer----------------------------------------------
	this->AxesButton->GetWidget()->SelectedStateOff();
	this->InvertPlaneButton->GetWidget()->SelectedStateOff();

	if(Patchsurface)
	{
		Patchsurface->Delete();
		Patchsurface = NULL ;
	}

	this->X_AngleEntrybutton->GetWidget()->SetValueAsInt(0);
	this->Y_AngleEntrybutton->GetWidget()->SetValueAsInt(0);
	this->Z_AngleEntrybutton->GetWidget()->SetValueAsInt(0);

	Menubutton->GetWidget()->SetValue("");

  if(MPPlaneWidget1)
  if(MPPlaneWidget1->GetEnabled())
  {
	  if(MPPlaneWidget1->GetInteractor()!= NULL)
	  {	  
		  MPPlaneWidget1->SetEnabled(0);  

 }
	  else

		MPPlaneWidget1->SetInteractor(NULL);	

  }

    if(MPPlaneWidget2)
  if(MPPlaneWidget2->GetEnabled())
  {
	  if(MPPlaneWidget2->GetInteractor()!= NULL)
	  {	  
		  MPPlaneWidget2->SetEnabled(0);  		  

	  }
	  else

		MPPlaneWidget2->SetInteractor(NULL);	
		
  }

  this->PatchButton->GetWidget()->SetSelectedState(0);

}
//------------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->SurfaceList->GetNumberOfItems(); i++)
	{
		ObjectListComboBox->GetWidget()->AddValue(
		this->SurfaceList->GetItem(i)->GetFileName());
	  		
		int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(i))->GetActor());

		if ( (defaultItem == -1) && ( viewedItem ) )
		{
		  defaultItem = i;
		}
	}
	
	if (defaultItem != -1)
  {
    ObjectListComboBox->GetWidget()->SetValue(
    this->SurfaceList->GetItem(defaultItem)->GetFileName());

//	this->VolumeChangedcallback(NULL);

  }
	
}
//--------------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::SaveVTKSurfaceDoneCallback()
{
	if(this->SaveVTKSurfaceApplyCallback())
	this->SaveVTKSurfaceCancelCallback();
}
//---------------------------------------------------------------------------------

// Callback for rotation about X axis
void vtkKWMimxEditSurfaceGroup::XAnglechangedcallback(const char *angle1)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Plane"))
		{
			return  ;
		}

	float theta =  this->X_AngleEntrybutton->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

//	sine   = sin((PI/2) + (theta*PI/180));				
//	cosine = cos((PI/2) + (theta*PI/180));

	  sine   = sin(theta*PI/180);				
	  cosine = cos(theta*PI/180);

	if(!this->PlaneWidget)

		PlaneWidget = vtkPlaneWidget::New();
		PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		PlaneWidget->SetPlaceFactor(0.5);	
		PlaneWidget->PlaceWidget();		
		PlaneWidget->SetNormal(0, cosine, sine );
		PlaneWidget->SetHandleSize(0.005);
		PlaneWidget->AddObserver(vtkCommand::InteractionEvent,this->PlaneCallback);
//		PlaneWidget->SetEnabled(0);
		PlaneWidget->SetEnabled(1);
//		PlaneWidget->Modified();		

	this->PlaneWidget->GetPlane(this->Plane);
	if(this->Clippolydata)
	{
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();	
	Clippolydata->Modified();	
	Clippolydata->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

// Rotation about Y axis
void vtkKWMimxEditSurfaceGroup::YAnglechangedcallback(const char *angle2)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}


	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Plane"))
		{
			return  ;
		}

	float theta =  this->Y_AngleEntrybutton->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

	sine   = sin(theta*PI/180);				
	cosine = cos(theta*PI/180);

	if(!this->PlaneWidget)
		
		PlaneWidget = vtkPlaneWidget::New();
		PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		PlaneWidget->SetPlaceFactor(0.5);	
		PlaneWidget->PlaceWidget();		
		PlaneWidget->SetNormal(sine, 0, cosine);
		PlaneWidget->SetHandleSize(0.005);
		PlaneWidget->AddObserver(vtkCommand::InteractionEvent,this->PlaneCallback);
//		PlaneWidget->SetEnabled(0);
		PlaneWidget->SetEnabled(1);
//		PlaneWidget->Modified();		

	this->PlaneWidget->GetPlane(this->Plane);
	if(this->Clippolydata)
	{
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();	
	Clippolydata->Modified();	
	Clippolydata->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}


void vtkKWMimxEditSurfaceGroup::ZAnglechangedcallback(const char *angle3)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}

	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Plane"))
		{
			return  ;
		}


	float theta =  this->Z_AngleEntrybutton->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

//	sine   = sin(theta*PI/180);				
//	cosine = cos(theta*PI/180);
	
	sine   = sin((PI/2) + (theta*PI/180));
	cosine = cos((PI/2) + (theta*PI/180));



	if(!this->PlaneWidget)

		PlaneWidget = vtkPlaneWidget::New();
		PlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		PlaneWidget->SetPlaceFactor(0.5);	
		PlaneWidget->PlaceWidget();		
		PlaneWidget->SetNormal(cosine, sine , 0 );
		PlaneWidget->SetHandleSize(0.005);
		PlaneWidget->AddObserver(vtkCommand::InteractionEvent,this->PlaneCallback);
//		PlaneWidget->SetEnabled(0);
		PlaneWidget->SetEnabled(1);
//		PlaneWidget->Modified();		

	this->PlaneWidget->GetPlane(this->Plane);
	if(this->Clippolydata)
	{
	Clippolydata->SetClipFunction(Plane);	
	Clippolydata->InsideOutOn();	
	Clippolydata->Modified();	
	Clippolydata->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();

}

/////////////// added multiple planes clip callback////////////////////////////
void vtkKWMimxEditSurfaceGroup::MPClipSurfaceCallback()
{

	this->InvertBoxButton->Unpack();
	this->InvertPlaneButton->Unpack();	

	this->PlaneOrientationFrame->Unpack();
	this->Normal_label_frame->Unpack();
	this->PatchButtonTPS->SetEnabled(0);
	this->PatchButtonTPS->GetWidget()->SelectedStateOff();


	this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x -before %s", 
	this->MPPlaneOrientationFrame2->GetWidgetName(), this->ApplyButton->GetWidgetName());	

	this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x -before %s", 
	this->MPPlaneOrientationFrame1->GetWidgetName(), this->MPPlaneOrientationFrame2->GetWidgetName());	


	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

/////////// check if other widgets are active. If yes deactivete them and also the actors////////////////
	if( PlaneWidget)
	{
	PlaneWidget->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
	}

	if( SphereWidget)
	{
		SphereWidget->SetEnabled(0);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
	}

	if( boxwidget)
	{
	boxwidget->SetEnabled(0);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
	}
  
/////////// check if other widgets are active. If yes deactivete them and also the actors////////////////
	
	if(!this->MPPlaneWidget1)
	MPPlaneWidget1 = vtkPlaneWidget::New();
	MPPlaneWidget1->KeyPressActivationOff();
	MPPlaneWidget1->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	MPPlaneWidget1->SetPlaceFactor(0.5);
	MPPlaneWidget1->SetInput(polydata);
	MPPlaneWidget1->PlaceWidget();	
	MPPlaneWidget1->SetEnabled(1);
	MPPlaneWidget1->SetNormal(0.29,0,-0.966);
	MPPlaneWidget1->SetHandleSize(0.005);
	MPPlaneWidget1->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback1);

	if(!this->MPPlaneWidget2)
	MPPlaneWidget2 = vtkPlaneWidget::New();
	MPPlaneWidget2->KeyPressActivationOff();
	MPPlaneWidget2->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	MPPlaneWidget2->SetPlaceFactor(0.5);
	MPPlaneWidget2->SetInput(polydata);
	MPPlaneWidget2->PlaceWidget();	
	MPPlaneWidget2->SetEnabled(1);
	MPPlaneWidget2->SetNormal(0,0,1);
	MPPlaneWidget2->SetHandleSize(0.005);
	MPPlaneWidget2->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback2);

	if (!this->MPPlane1)	
	MPPlane1 = vtkPlane::New();

	if(this->MPPlaneWidget1)
	{
		this->MPPlaneWidget1->GetPlane(this->MPPlane1);	
	}

	if (!this->MPPlane2)	
	MPPlane2 = vtkPlane::New();

	if(this->MPPlaneWidget2)
	{
		this->MPPlaneWidget2->GetPlane(this->MPPlane2);	
	}

	if (!this->MPClippolydata1)	
	MPClippolydata1 = vtkClipPolyData::New();
	MPClippolydata1->SetInput(polydata);
	MPClippolydata1->SetClipFunction(MPPlane1);	
	MPClippolydata1->InsideOutOn();
	this->MPPlaneCallback1->MPPlane1 = this->MPPlane1 ;
	MPClippolydata1->Modified();	
	MPClippolydata1->Update();	

	if (!this->MPClippolydata2)	
	MPClippolydata2 = vtkClipPolyData::New();
	MPClippolydata2->SetInput(polydata);
	MPClippolydata2->SetClipFunction(MPPlane2);	
	MPClippolydata2->InsideOutOn();
	this->MPPlaneCallback2->MPPlane2 = this->MPPlane2 ;
	MPClippolydata2->Modified();	
	MPClippolydata2->Update();	

	this->MPSelectedSurfaceMapper1->SetInput(MPClippolydata1->GetOutput());
	this->MPSelectedSurfaceMapper1->Update();
	this->MPSelectedSurfaceActor1->SetMapper(MPSelectedSurfaceMapper1);
	this->MPSelectedSurfaceActor1->VisibilityOn();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();	
	
	this->MPSelectedSurfaceMapper2->SetInput(MPClippolydata2->GetOutput());
	this->MPSelectedSurfaceMapper2->Update();
	this->MPSelectedSurfaceActor2->SetMapper(MPSelectedSurfaceMapper2);
	this->MPSelectedSurfaceActor2->VisibilityOn();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();	

// added to remove the original surface from the window and unselect the surface from the list-------------
	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->MPSelectedSurfaceActor1);	
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->MPSelectedSurfaceActor2);	
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  		this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
		this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();

	}
// added to remove the original surface from the window and unselect the surface from the list end -------

	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->MPSelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->MPSelectedSurfaceActor2);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}
//----------------------------------------------------------------------------
// Plane 1, angle 1 rotation about X axis

void vtkKWMimxEditSurfaceGroup::MP_XAnglechangedcallback1(const char *mpxangle1)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Multiple-Planes"))
		{
			return  ;
		}

	float theta =  this->MPX_AngleEntrybutton1->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

	sine   = sin(theta*PI/180);				
	cosine = cos(theta*PI/180);

	if(!this->MPPlaneWidget1)

		MPPlaneWidget1 = vtkPlaneWidget::New();
		MPPlaneWidget1->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		MPPlaneWidget1->SetPlaceFactor(0.5);	
		MPPlaneWidget1->PlaceWidget();		
		MPPlaneWidget1->SetNormal(0, cosine , sine );
		MPPlaneWidget1->SetHandleSize(0.005);
		MPPlaneWidget1->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback1);		
//		MPPlaneWidget1->SetEnabled(0);
		MPPlaneWidget1->SetEnabled(1);
//		MPPlaneWidget1->Modified();		

	this->MPPlaneWidget1->GetPlane(this->MPPlane1);	
	if(this->MPClippolydata1)
	{
	MPClippolydata1->SetClipFunction(MPPlane1);	
//	MPClippolydata1->InsideOutOn();	
	MPClippolydata1->InsideOutOff();	
	MPClippolydata1->Modified();	
	MPClippolydata1->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//// plane 1, angle 2
void vtkKWMimxEditSurfaceGroup::MP_YAnglechangedcallback1(const char *mpyangle1)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Multiple-Planes"))
		{
			return  ;
		}

	float theta =  this->MPY_AngleEntrybutton1->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

	sine   = sin(theta*PI/180);				
	cosine = cos(theta*PI/180);

	if(!this->MPPlaneWidget1)

		MPPlaneWidget1 = vtkPlaneWidget::New();
		MPPlaneWidget1->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		MPPlaneWidget1->SetPlaceFactor(0.5);	
		MPPlaneWidget1->PlaceWidget();		
		MPPlaneWidget1->SetNormal(sine, 0, cosine);
		MPPlaneWidget1->SetHandleSize(0.005);
		MPPlaneWidget1->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback1);
//		MPPlaneWidget1->SetEnabled(0);
		MPPlaneWidget1->SetEnabled(1);
//		MPPlaneWidget1->Modified();		

	this->MPPlaneWidget1->GetPlane(this->MPPlane1);	
	if(this->MPClippolydata1)
	{
	MPClippolydata1->SetClipFunction(MPPlane1);	
//	MPClippolydata1->InsideOutOn();	
	MPClippolydata1->InsideOutOff();	
	MPClippolydata1->Modified();	
	MPClippolydata1->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}


//// plane 1, angle 3
void vtkKWMimxEditSurfaceGroup::MP_ZAnglechangedcallback1(const char *mpzangle1)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Multiple-Planes"))
		{
			return  ;
		}

	float theta =  this->MPZ_AngleEntrybutton1->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

//	sine   = sin(theta*PI/180);				
//	cosine = cos(theta*PI/180);

	sine   = sin((PI/2) + (theta*PI/180));
	cosine = cos((PI/2) + (theta*PI/180));

	if(!this->MPPlaneWidget1)

		MPPlaneWidget1 = vtkPlaneWidget::New();
		MPPlaneWidget1->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		MPPlaneWidget1->SetPlaceFactor(0.5);	
		MPPlaneWidget1->PlaceWidget();		
		MPPlaneWidget1->SetNormal(cosine, sine, 0);
		MPPlaneWidget1->SetHandleSize(0.005);
		MPPlaneWidget1->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback1);
//		MPPlaneWidget1->SetEnabled(0);
		MPPlaneWidget1->SetEnabled(1);
//		MPPlaneWidget1->Modified();		

	this->MPPlaneWidget1->GetPlane(this->MPPlane1);	
	if(this->MPClippolydata1)
	{
	MPClippolydata1->SetClipFunction(MPPlane1);	
//	MPClippolydata1->InsideOutOn();	
	MPClippolydata1->InsideOutOff();	
	MPClippolydata1->Modified();	
	MPClippolydata1->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/// plane 1, angle 1

void vtkKWMimxEditSurfaceGroup::MP_XAnglechangedcallback2(const char *mpxangle2)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Multiple-Planes"))
		{
			return  ;
		}

	float theta =  this->MPX_AngleEntrybutton2->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

	sine   = sin(theta*PI/180);				
	cosine = cos(theta*PI/180);

	if(!this->MPPlaneWidget2)

		MPPlaneWidget2 = vtkPlaneWidget::New();
		MPPlaneWidget2->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		MPPlaneWidget2->SetPlaceFactor(0.5);	
		MPPlaneWidget2->PlaceWidget();		
		MPPlaneWidget2->SetNormal(0,cosine, sine );
		MPPlaneWidget2->SetHandleSize(0.005);
		MPPlaneWidget2->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback2);
//		MPPlaneWidget2->SetEnabled(0);
		MPPlaneWidget2->SetEnabled(1);
//		MPPlaneWidget2->Modified();		

	this->MPPlaneWidget2->GetPlane(this->MPPlane2);	
	if(this->MPClippolydata2)
	{
	MPClippolydata2->SetClipFunction(MPPlane2);	
	MPClippolydata2->InsideOutOn();	
	MPClippolydata2->Modified();	
	MPClippolydata2->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//// plane 1, angle 2
void vtkKWMimxEditSurfaceGroup::MP_YAnglechangedcallback2(const char *mpyangle2)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Multiple-Planes"))
		{
			return  ;
		}

	float theta =  this->MPY_AngleEntrybutton2->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

	sine   = sin(theta*PI/180);				
	cosine = cos(theta*PI/180);

	if(!this->MPPlaneWidget2)

		MPPlaneWidget2 = vtkPlaneWidget::New();
		MPPlaneWidget2->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		MPPlaneWidget2->SetPlaceFactor(0.5);	
		MPPlaneWidget2->PlaceWidget();		
		MPPlaneWidget2->SetNormal(sine, 0, cosine );
		MPPlaneWidget2->SetHandleSize(0.005);
		MPPlaneWidget2->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback2);
//		MPPlaneWidget2->SetEnabled(0);
		MPPlaneWidget2->SetEnabled(1);
//		MPPlaneWidget2->Modified();		

	this->MPPlaneWidget2->GetPlane(this->MPPlane2);	
	if(this->MPClippolydata2)
	{
	MPClippolydata2->SetClipFunction(MPPlane2);	
	MPClippolydata2->InsideOutOn();	
	MPClippolydata2->Modified();	
	MPClippolydata2->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//// plane 1, angle 3
void vtkKWMimxEditSurfaceGroup::MP_ZAnglechangedcallback2(const char *mpyangle2)
{
	if(CancelStatus)
	{
		CancelStatus =  0;
		return ;
	}
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return ;
	}
	
	const char * MenuButtonName = Menubutton->GetWidget()->GetValue() ;	
	if(strcmp(MenuButtonName,"Multiple-Planes"))
		{
			return  ;
		}

	float theta =  this->MPZ_AngleEntrybutton2->GetWidget()->GetValueAsDouble();
	#define PI 3.14159265 
	float sine, cosine ;

//	sine   = sin(theta*PI/180);				
//	cosine = cos(theta*PI/180);

	sine   = sin((PI/2) + (theta*PI/180));
	cosine = cos((PI/2) + (theta*PI/180));

	if(!this->MPPlaneWidget2)

		MPPlaneWidget2 = vtkPlaneWidget::New();
		MPPlaneWidget2->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		MPPlaneWidget2->SetPlaceFactor(0.5);	
		MPPlaneWidget2->PlaceWidget();		
		MPPlaneWidget2->SetNormal(cosine, sine, 0);
		MPPlaneWidget2->SetHandleSize(0.005);
		MPPlaneWidget2->AddObserver(vtkCommand::InteractionEvent,this->MPPlaneCallback2);
//		MPPlaneWidget2->SetEnabled(0);
		MPPlaneWidget2->SetEnabled(1);
//		MPPlaneWidget2->Modified();		

	this->MPPlaneWidget2->GetPlane(this->MPPlane2);	
	if(this->MPClippolydata2)
	{
	MPClippolydata2->SetClipFunction(MPPlane2);	
	MPClippolydata2->InsideOutOn();	
	MPClippolydata2->Modified();	
	MPClippolydata2->Update();	
	}

	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->Render();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//------------------------------------------------------------------------------------------
void vtkKWMimxEditSurfaceGroup::SphereClipSurfaceCallback()
{
	//const char * box_filename = Menubutton->GetWidget()->GetValue() ;				

	this->InvertBoxButton->Unpack();
	this->PatchButtonTPS->SetEnabled(1);
	this->PatchButtonTPS->GetWidget()->SelectedStateOff();


	// added to pack the plane orientation frame //////////

	this->MPPlaneOrientationFrame1->Unpack();
	this->MPPlaneOrientationFrame2->Unpack();


	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
		->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
	
	double *center = polydata->GetCenter();
	/////////// check if other widgets are active. If yes deactivete them and also the actors///////////////	
	if( boxwidget)
	{
		boxwidget->SetEnabled(0);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
	}
	if(PlaneWidget)
	{
		PlaneWidget->SetEnabled(0);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
	}
	if( MPPlaneWidget1)
	{
		MPPlaneWidget1->SetEnabled(0);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor1);
	}

	if( MPPlaneWidget2)
	{
		MPPlaneWidget2->SetEnabled(0);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->MPSelectedSurfaceActor2);
	}
	/////////// check if other widgets are active. If yes deactivete them and also the actors///////////////

	if(!this->SphereWidget)
		SphereWidget = vtkSphereWidget::New();
	SphereWidget->KeyPressActivationOff();
	SphereWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	SphereWidget->SetCenter(center);
	SphereWidget->SetInput(polydata);
	SphereWidget->PlaceWidget();	
	SphereWidget->SetEnabled(1);
	//SphereWidget->SetHandleSize(0.005);
	SphereWidget->AddObserver(vtkCommand::InteractionEvent,this->SphereClipCallback);

	if (!this->Sphere)	
		this->Sphere = vtkSphere::New();

	if(this->Sphere)
	{
		this->SphereWidget->GetSphere(this->Sphere);	
	}

	this->SphereClipCallback->Sphere = this->Sphere ; 			

	if (!this->Clippolydata)	
		Clippolydata = vtkClipPolyData::New();
	Clippolydata->SetInput(polydata);
	Clippolydata->SetClipFunction(this->Sphere);	
	//Clippolydata->InsideOutOn();
	Clippolydata->Modified();	
	Clippolydata->Update();	

	this->SelectedSurfaceMapper->SetInput(Clippolydata->GetOutput());
	this->SelectedSurfaceMapper->Update();
	this->SelectedSurfaceActor->SetMapper(SelectedSurfaceMapper);
	this->SelectedSurfaceActor->VisibilityOn();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();	

	// added to remove the original surface from the window and unselect the surface from the list-------------
	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
	const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);	
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
		this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
		this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}
	// added to remove the original surface from the window and unselect the surface from the list end -------

	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}
//----------------------------------------------------------------------------
