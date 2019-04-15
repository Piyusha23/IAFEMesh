/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxBooleanSurfaceGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/03/22 21:07:38 $
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

#include "vtkKWMimxBooleanSurfaceGroup.h"

/* VTK Header Files*/
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkClipPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>
#include <vtkBooleanOperationPolyDataFilter.h>

/* KWWidgets Header Files*/
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWPushButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWRenderWidget.h>

/* MIMX header files*/
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
//#include "vtkImplicitPolyData.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxBooleanSurfaceGroup);
vtkCxxRevisionMacro(vtkKWMimxBooleanSurfaceGroup, "$Revision: 1.2 $");


//----------------------------------------------------------------------------
vtkKWMimxBooleanSurfaceGroup::vtkKWMimxBooleanSurfaceGroup()
{
  this->ObjectListComboBox = NULL;
  this->ObjectListComboBox1 = NULL;
  this->SelectedSurfaceActor = vtkActor::New();
  this->SelectedSurfaceActor->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper = vtkPolyDataMapper::New();  
  this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
  this->Menubutton = NULL;
    
  this->Surface1 = NULL;
  this->Surface2 = NULL ;
  this->Clipper1 = NULL;
//  this->Clipper2 = NULL ;
  this->AppendSurfaces = NULL ;

  this->SelectedSurfaceActor1 = vtkActor::New();
  this->SelectedSurfaceActor1->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper1 = vtkPolyDataMapper::New();  
  this->SelectedSurfaceActor1->SetMapper(this->SelectedSurfaceMapper1);

  this->SelectedSurfaceActor2 = vtkActor::New();
  this->SelectedSurfaceActor2->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper2 = vtkPolyDataMapper::New();  
  this->SelectedSurfaceActor2->SetMapper(this->SelectedSurfaceMapper2);
}

//----------------------------------------------------------------------------
vtkKWMimxBooleanSurfaceGroup::~vtkKWMimxBooleanSurfaceGroup()
{
  if (this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if (this->ObjectListComboBox1)
     this->ObjectListComboBox1->Delete();
	if (this->SelectedSurfaceActor)
	  this->SelectedSurfaceActor->Delete();
  if (this->SelectedSurfaceMapper)
    this->SelectedSurfaceMapper->Delete();  
  if (this->Menubutton)
	  this->Menubutton->Delete();
  if (this->Surface1)
	  this->Surface1->Delete();
  if (this->Surface2)
	  this->Surface2->Delete();
  if (this->Clipper1)
	  this->Clipper1->Delete();
//  if (this->Clipper2)
	//  this->Clipper2->Delete();
  if (this->AppendSurfaces)
	  this->AppendSurfaces->Delete();
  if (this->SelectedSurfaceActor)
	  this->SelectedSurfaceActor->Delete();
  if (this->SelectedSurfaceActor1)
	  this->SelectedSurfaceActor1->Delete();
  if (this->SelectedSurfaceMapper)
	  this->SelectedSurfaceMapper->Delete();
  if (this->SelectedSurfaceMapper1)
	  this->SelectedSurfaceMapper1->Delete();
  if (this->SelectedSurfaceActor2)
    this->SelectedSurfaceActor2->Delete();
  if (this->SelectedSurfaceMapper2)
    this->SelectedSurfaceMapper2->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMimxBooleanSurfaceGroup::CreateWidget()
{
  if(this->IsCreated())
  {
  vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();

  // Select first surface -----------------------
  if (!this->ObjectListComboBox)
    this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();  
  this->ObjectListComboBox->SetParent(this->MainFrame);
  this->ObjectListComboBox->Create();
  this->ObjectListComboBox->SetWidth(20);
  this->ObjectListComboBox->SetLabelText("Surface 1: ");
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 2 -fill x", 
  this->ObjectListComboBox->GetWidgetName());			

  // Select second surface -----------------------
  if(!this->ObjectListComboBox1)
  this->ObjectListComboBox1 = vtkKWComboBoxWithLabel::New();  
  this->ObjectListComboBox1->SetParent(this->MainFrame);
  this->ObjectListComboBox1->Create();
  this->ObjectListComboBox1->SetWidth(20);
  this->ObjectListComboBox1->SetLabelText("Surface 2: ");
  this->ObjectListComboBox1->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox1->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 2 -fill x", 
  this->ObjectListComboBox1->GetWidgetName());			
  
  // Boolean Operation ---------------------------------------------
  if(!this->Menubutton)
  this->Menubutton = vtkKWMenuButtonWithLabel::New();  
  this->Menubutton->SetParent(this->MainFrame);
  this->Menubutton->Create();
  this->Menubutton->SetWidth(20);
  this->Menubutton->SetLabelText(" Options ");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Subtract:1-2 ",this, "AdditionSurfaceCallback");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Subtract:2-1",this, "SubtractionSurfaceCallback");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(		
		"Intersection",this, "IntersectionSurfaceCallback");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Union",this, "UnionSurfaceCallback");

  this->Menubutton->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 25 -fill x", 
    this->Menubutton->GetWidgetName());		

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "SaveVTKSurfaceApplyCallback");
  this->CancelButton->SetCommand(this, "SaveVTKSurfaceCancelCallback");
}

//----------------------------------------------------------------------------
void vtkKWMimxBooleanSurfaceGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxBooleanSurfaceGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxBooleanSurfaceGroup::SaveVTKSurfaceApplyCallback()
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
    // ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	//const char * decimate_filename = Menubutton->GetWidget()->GetValue() ;
	
	vtkPolyData *changed_surface = NULL ;
	changed_surface = AppendSurfaces->GetOutput();
		
	if(changed_surface)
		{
			this->AddSurfaceToDisplay(changed_surface, "Boolean", foundation_name, 1 , NULL);
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
	}
	return 0;
}
//----------------------------------------------------------------------------

void vtkKWMimxBooleanSurfaceGroup::SubtractionSurfaceCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();	
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	if(!strcmp(this->ObjectListComboBox1->GetWidget()->GetValue(),""))
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

	vtkKWComboBox *combobox1 = this->ObjectListComboBox1->GetWidget();
    const char *name1 = combobox1->GetValue();

	int num1 = combobox1->GetValueIndex(name1);
	if(num1 < 0 || num1 > combobox1->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox1->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
	vtkPolyData *polydata1 = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox1->GetValueIndex(name1)))->GetDataSet();

	/*vtkPolyData *polydata = vtkPolyData::SafeDownCast(this->SurfaceList
     ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
	vtkPolyData *polydata1 = vtkPolyData::SafeDownCast(this->SurfaceList
     ->GetItem(combobox1->GetValueIndex(name1)))->GetDataSet();*/

	/*Surface1 = vtkImplicitPolyData::New();
    Surface1->SetTolerance(1000);  
    Surface1->SetInput(polydata1);  

	Surface2 = vtkImplicitPolyData::New();
    Surface2->SetTolerance(1000);  
    Surface2->SetInput(polydata); */

	Surface1 = vtkSphereSource::New();
	Surface1->SetCenter(0,0,0);
	Surface1->Update();
	polydata = Surface1->GetOutput();

	Surface2 = vtkSphereSource::New();
	Surface2->SetCenter(0,0,0);
	Surface2->Update();
	polydata1 = Surface1->GetOutput();

	this->SelectedSurfaceMapper1->SetInputConnection(polydata->GetProducerPort());

	this->SelectedSurfaceActor1->SetMapper (this->SelectedSurfaceMapper1);
	this->SelectedSurfaceActor1->GetProperty()->SetColor(1,0,0);
//	this->SelectedSurfaceActor1->SetPosition(polydata->GetBounds()[1]-polydata->GetBounds()[0],0,0);

	//this->SelectedSurfaceMapper1->SetInputConnection(Clipper1->GetOutputPort());
	//this->SelectedSurfaceMapper1->ScalarVisibilityOff();

	this->SelectedSurfaceMapper2->SetInputConnection(polydata1->GetProducerPort());

	this->SelectedSurfaceActor2->SetMapper (this->SelectedSurfaceMapper2);
	this->SelectedSurfaceActor2->GetProperty()->SetColor(0,1,0);
//	this->SelectedSurfaceActor2->SetPosition(polydata1->GetBounds()[1]-polydata1->GetBounds()[0]),0,0);

	Clipper1 = vtkBooleanOperationPolyDataFilter :: New();
	Clipper1->SetOperationToDifference();
	Clipper1->SetInputConnection(0, polydata->GetProducerPort());
	Clipper1->SetInputConnection(1, polydata1->GetProducerPort());


    /*Clipper1 = vtkClipPolyData::New();
    Clipper1->SetInput(polydata1);
    Clipper1->SetClipFunction(Surface2);
    Clipper1->InsideOutOn(); // On for finger
    Clipper1->Update();*/

    /*Clipper2 = vtkClipPolyData::New();
    Clipper2->SetInput(polydata);
    Clipper2->SetClipFunction(Surface1);
    Clipper2->InsideOutOff();
    Clipper2->Update();*/

//	AppendSurfaces = vtkAppendPolyData::New();
//  AppendSurfaces->AddInput(Clipper1->GetOutput());
//  AppendSurfaces->AddInput(Clipper2->GetOutput());

	////////////////////////////////////////////////////////// added to correct the normals

   /*vtkPolyData *poly = vtkPolyData::New();  
   poly->SetPoints(Clipper1->GetOutput()->GetPoints());

   Clipper1->GetOutput()->BuildLinks();
   //vtkCellArray *polyarray = Clipper1->GetOutput()->GetPolys();

   vtkCellArray *localArray = vtkCellArray::New();
   for (int i = 0; i<Clipper1->GetOutput()->GetNumberOfCells(); i++ )
	{
		vtkIdType npts;
		vtkIdType *pts; 
		Clipper1->GetOutput()->GetCellPoints(i, npts, pts);		
		int temp = pts[2];
		pts[2] = pts[0];
		pts[0] = temp;	
		localArray->InsertNextCell(npts, pts);		
	}

	poly->SetPolys(localArray);
	localArray->Delete();

	AppendSurfaces = vtkAppendPolyData::New();
    AppendSurfaces->AddInput(Clipper2->GetOutput());
    AppendSurfaces->AddInput(poly);

////////////////////////////////////////////////////////// added to correct the normals


 //  this->SelectedSurfaceMapper->SetInput(Clipper1->GetOutput());
//    this->SelectedSurfaceMapper1->SetInput(Clipper2->GetOutput());*/
//	this->SelectedSurfaceMapper1->SetInputConnection(Clipper1->GetOutputPort());
//	this->SelectedSurfaceMapper1->ScalarVisibilityOff();
   // this->SelectedSurfaceMapper2->SetInput(polydata1->GetOutputPort());
	//this->SelectedSurfaceMapper2->ScalarVisibilityOff();
	
// added to remove the existing surfaces and add the final surface to the renderer...................


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

	int surface2Index = -1;
	vtkKWComboBox *combobox_surf2 = this->ObjectListComboBox1->GetWidget();
    const char *name_surf2 = combobox_surf2->GetValue();  	
	surface2Index = combobox_surf2->GetValueIndex(name_surf2);

	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface2Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

// added to remove the existing surfaces and add the final surface to the renderer end ..................

	this->SelectedSurfaceMapper->SetInputConnection(Clipper1->GetOutputPort());
	this->SelectedSurfaceMapper->ScalarVisibilityOff();
//	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
//	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
//	this->SelectedSurfaceActor2->SetMapper(this->SelectedSurfaceMapper2);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor2);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}

//----------------------------------------------------------------------------
// Surface intersection callback

void vtkKWMimxBooleanSurfaceGroup::IntersectionSurfaceCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();	
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	if(!strcmp(this->ObjectListComboBox1->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	// combobox 1 for surface 1
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	// combobox 2 for surface 2
	vtkKWComboBox *combobox1 = this->ObjectListComboBox1->GetWidget();
    const char *name1 = combobox1->GetValue();

	int num1 = combobox1->GetValueIndex(name1);
	if(num1 < 0 || num1 > combobox1->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox1->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
	vtkPolyData *polydata1 = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox1->GetValueIndex(name1)))->GetDataSet();

	Surface1 = vtkSphereSource::New();
	Surface1->SetCenter(0,0,0);
	Surface1->Update();
	polydata = Surface1->GetOutput();

	Surface2 = vtkSphereSource::New();
	Surface2->SetCenter(0,0,0);
	Surface2->Update();
	polydata1 = Surface1->GetOutput();

	/*Surface1 = vtkImplicitPolyData::New();
    Surface1->SetTolerance(250);  
    Surface1->SetInput(polydata1);  

	Surface2 = vtkImplicitPolyData::New();
    Surface2->SetTolerance(500);  
    Surface2->SetInput(polydata); */ 

	this->SelectedSurfaceMapper1->SetInputConnection(polydata->GetProducerPort());

	this->SelectedSurfaceActor1->SetMapper (this->SelectedSurfaceMapper1);
	this->SelectedSurfaceActor1->GetProperty()->SetColor(1,0,0);
	this->SelectedSurfaceActor1->SetPosition(polydata->GetBounds()[1]-polydata->GetBounds()[0],0,0);

	//this->SelectedSurfaceMapper1->SetInputConnection(Clipper1->GetOutputPort());
	//this->SelectedSurfaceMapper1->ScalarVisibilityOff();

	this->SelectedSurfaceMapper2->SetInputConnection(polydata1->GetProducerPort());

	this->SelectedSurfaceActor2->SetMapper (this->SelectedSurfaceMapper2);
	this->SelectedSurfaceActor2->GetProperty()->SetColor(0,1,0);
//	this->SelectedSurfaceActor2->SetPosition(polydata1->GetBounds()[1]-polydata1->GetBounds()[0]),0,0);

	Clipper1 = vtkBooleanOperationPolyDataFilter :: New();
	Clipper1->SetOperationToIntersection();
	Clipper1->SetInputConnection(0, polydata->GetProducerPort());
	Clipper1->SetInputConnection(1, polydata1->GetProducerPort());

   /* Clipper1 = vtkClipPolyData::New();
    Clipper1->SetInput(polydata1);
    Clipper1->SetClipFunction(Surface2);
    Clipper1->InsideOutOn(); // On for finger
    Clipper1->Update();

    Clipper2 = vtkClipPolyData::New();
    Clipper2->SetInput(polydata);
    Clipper2->SetClipFunction(Surface1);
    Clipper2->InsideOutOn();
    Clipper2->Update();*/

	/*AppendSurfaces = vtkAppendPolyData::New();
    AppendSurfaces->AddInput(Clipper1->GetOutput());
    AppendSurfaces->AddInput(Clipper2->GetOutput());

    this->SelectedSurfaceMapper->SetInput(Clipper1->GetOutput());
    this->SelectedSurfaceMapper1->SetInput(Clipper2->GetOutput());
    this->SelectedSurfaceMapper2->SetInput(AppendSurfaces->GetOutput());*/

	// added to remove the existing surfaces and add the final surface to the renderer...................

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

	int surface2Index = -1;
	vtkKWComboBox *combobox_surf2 = this->ObjectListComboBox1->GetWidget();
    const char *name_surf2 = combobox_surf2->GetValue();  	
	surface2Index = combobox_surf2->GetValueIndex(name_surf2);

	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface2Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

	this->SelectedSurfaceMapper->SetInputConnection(Clipper1->GetOutputPort());
	this->SelectedSurfaceMapper->ScalarVisibilityOff();
	this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
// added to remove the existing surfaces and add the final surface to the renderer end ..................	
//	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
//	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor2);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}
//----------------------------------------------------------------

void vtkKWMimxBooleanSurfaceGroup::AdditionSurfaceCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();	
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	if(!strcmp(this->ObjectListComboBox1->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	// combobox 1 for surface 1

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	// combobox 2 for surface 2

	vtkKWComboBox *combobox1 = this->ObjectListComboBox1->GetWidget();
    const char *name1 = combobox1->GetValue();

	int num1 = combobox1->GetValueIndex(name1);
	if(num1 < 0 || num1 > combobox1->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox1->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	vtkPolyData *polydata1 = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox1->GetValueIndex(name1)))->GetDataSet();

	Surface1 = vtkSphereSource::New();
	Surface1->SetCenter(0,0,0);
	Surface1->Update();
	polydata = Surface1->GetOutput();

	Surface2 = vtkSphereSource::New();
	Surface2->SetCenter(0,0,0);
	Surface2->Update();
	polydata1 = Surface1->GetOutput();

	/*Surface1 = vtkSphereSource::New();
	Surface1->SetCenter(0,0,0);
	Surface1->Update();
	polydata = Surface1->GetOutput();

	Surface2 = vtkSphereSource::New();
	Surface2->SetCenter(0,0,0);
	Surface2->Update();
	polydata1 = Surface1->GetOutput();*/

	/*Surface1 = vtkImplicitPolyData::New();
    Surface1->SetTolerance(250);  
    Surface1->SetInput(polydata1);  

	Surface2 = vtkImplicitPolyData::New();
    Surface2->SetTolerance(500);  
    Surface2->SetInput(polydata);  

	Clipper1 = vtkClipPolyData::New();
    Clipper1->SetInput(polydata1);
    Clipper1->SetClipFunction(Surface2);
    Clipper1->InsideOutOff(); // On for finger
    Clipper1->Update();

    Clipper2 = vtkClipPolyData::New();
    Clipper2->SetInput(polydata);
    Clipper2->SetClipFunction(Surface1);
    Clipper2->InsideOutOn();
    Clipper2->Update();*/

	this->SelectedSurfaceMapper1->SetInputConnection(polydata->GetProducerPort());

	this->SelectedSurfaceActor1->SetMapper (this->SelectedSurfaceMapper1);
	this->SelectedSurfaceActor1->GetProperty()->SetColor(1,0,0);
	this->SelectedSurfaceActor1->SetPosition(polydata->GetBounds()[1]-polydata->GetBounds()[0],0,0);

	//this->SelectedSurfaceMapper1->SetInputConnection(Clipper1->GetOutputPort());
	//this->SelectedSurfaceMapper1->ScalarVisibilityOff();

	this->SelectedSurfaceMapper2->SetInputConnection(polydata1->GetProducerPort());

	this->SelectedSurfaceActor2->SetMapper (this->SelectedSurfaceMapper2);
	this->SelectedSurfaceActor2->GetProperty()->SetColor(0,1,0);
	//this->SelectedSurfaceActor2->SetPosition(polydata1->GetBounds()[1]-polydata1->GetBounds()[0]),0,0);

	Clipper1 = vtkBooleanOperationPolyDataFilter :: New();
	Clipper1->SetOperationToUnion();
	Clipper1->SetInputConnection(0, polydata->GetProducerPort());
	Clipper1->SetInputConnection(1, polydata1->GetProducerPort());

//	AppendSurfaces = vtkAppendPolyData::New();
//  AppendSurfaces->AddInput(Clipper1->GetOutput());
//  AppendSurfaces->AddInput(Clipper2->GetOutput());

	////////////////////////////////////////////////////////// added to correct the normals

   /*vtkPolyData *poly = vtkPolyData::New();  
   poly->SetPoints(Clipper1->GetOutput()->GetPoints());

   Clipper1->GetOutput()->BuildLinks();
   //vtkCellArray *polyarray = Clipper1->GetOutput()->GetPolys();

   vtkCellArray *localArray = vtkCellArray::New();
   for (int i = 0; i<Clipper1->GetOutput()->GetNumberOfCells(); i++ )
	{
		vtkIdType npts;
		vtkIdType *pts ; 
		Clipper1->GetOutput()->GetCellPoints(i, npts, pts);		
		int temp = pts[2];
		pts[2] = pts[0];
		pts[0] = temp;	
		localArray->InsertNextCell(npts, pts);		
	}

	poly->SetPolys(localArray);
	localArray->Delete();

	AppendSurfaces = vtkAppendPolyData::New();
  AppendSurfaces->AddInput(polydata1);
  AppendSurfaces->AddInput(poly);
////////////////////////////////////////////////////////// added to correct the normals
//    this->SelectedSurfaceMapper->SetInput(Clipper1->GetOutput());
//    this->SelectedSurfaceMapper1->SetInput(Clipper2->GetOutput());
    this->SelectedSurfaceMapper2->SetInput(AppendSurfaces->GetOutput());*/

	// added to remove the existing surfaces and add the final surface to the renderer...................

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

	int surface2Index = -1;
	vtkKWComboBox *combobox_surf2 = this->ObjectListComboBox1->GetWidget();
    const char *name_surf2 = combobox_surf2->GetValue();  	
	surface2Index = combobox_surf2->GetValueIndex(name_surf2);

	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface2Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}


// added to remove the existing surfaces and add the final surface to the renderer end ..................	
	this->SelectedSurfaceMapper->SetInputConnection(Clipper1->GetOutputPort());
	this->SelectedSurfaceMapper->ScalarVisibilityOff();
	this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
//	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
//	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor2);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}

//----------------------------------------------------------------

void vtkKWMimxBooleanSurfaceGroup::UnionSurfaceCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();	
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	if(!strcmp(this->ObjectListComboBox1->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}

	// combobox 1 for surface 1

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	// combobox 2 for surface 2

	vtkKWComboBox *combobox1 = this->ObjectListComboBox1->GetWidget();
    const char *name1 = combobox1->GetValue();

	int num1 = combobox1->GetValueIndex(name1);
	if(num1 < 0 || num1 > combobox1->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox1->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	vtkPolyData *polydata1 = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
     ->GetItem(combobox1->GetValueIndex(name1)))->GetDataSet();

	/*Surface1 = vtkImplicitPolyData::New();
    Surface1->SetTolerance(250);  
    Surface1->SetInput(polydata1);  

	Surface2 = vtkImplicitPolyData::New();
    Surface2->SetTolerance(500);  
    Surface2->SetInput(polydata); */ 

	Surface1 = vtkSphereSource::New();
	Surface1->SetCenter(0,0,0);
	Surface1->Update();
	polydata = Surface1->GetOutput();

	Surface2 = vtkSphereSource::New();
	Surface2->SetCenter(0,0,0);
	Surface2->Update();
	polydata1 = Surface1->GetOutput();

   /* Clipper1 = vtkClipPolyData::New();
    Clipper1->SetInput(polydata1);
    Clipper1->SetClipFunction(Surface2);
    Clipper1->InsideOutOff(); // On for finger
    Clipper1->Update();

    Clipper2 = vtkClipPolyData::New();
    Clipper2->SetInput(polydata);
    Clipper2->SetClipFunction(Surface1);
    Clipper2->InsideOutOff();
    Clipper2->Update();*/

	this->SelectedSurfaceMapper1->SetInputConnection(polydata->GetProducerPort());

	this->SelectedSurfaceActor1->SetMapper (this->SelectedSurfaceMapper1);
	this->SelectedSurfaceActor1->GetProperty()->SetColor(1,0,0);
//	this->SelectedSurfaceActor1->SetPosition(polydata->GetBounds()[1]-polydata->GetBounds()[0],0,0);

	//this->SelectedSurfaceMapper1->SetInputConnection(Clipper1->GetOutputPort());
	//this->SelectedSurfaceMapper1->ScalarVisibilityOff();

	this->SelectedSurfaceMapper2->SetInputConnection(polydata1->GetProducerPort());

	this->SelectedSurfaceActor2->SetMapper (this->SelectedSurfaceMapper2);
	this->SelectedSurfaceActor2->GetProperty()->SetColor(0,1,0);
//	this->SelectedSurfaceActor2->SetPosition(polydata1->GetBounds()[1]-polydata1->GetBounds()[0]),0,0);

	Clipper1 = vtkBooleanOperationPolyDataFilter :: New();
	Clipper1->SetOperationToUnion();
	Clipper1->SetInputConnection(0, polydata->GetProducerPort());
	Clipper1->SetInputConnection(1, polydata1->GetProducerPort());

	/*AppendSurfaces = vtkAppendPolyData::New();
  AppendSurfaces->AddInput( polydata );
  AppendSurfaces->AddInput( polydata1 );

    this->SelectedSurfaceMapper->SetInput(Clipper1->GetOutput());
    this->SelectedSurfaceMapper1->SetInput(Clipper2->GetOutput());
    this->SelectedSurfaceMapper2->SetInput(AppendSurfaces->GetOutput());*/

	// added to remove the existing surfaces and add the final surface to the renderer...................

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

	int surface2Index = -1;
	vtkKWComboBox *combobox_surf2 = this->ObjectListComboBox1->GetWidget();
    const char *name_surf2 = combobox_surf2->GetValue();  	
	surface2Index = combobox_surf2->GetValueIndex(name_surf2);

	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface2Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}
	// added to remove the existing surfaces and add the final surface to the renderer end ..................	
	this->SelectedSurfaceMapper->SetInputConnection(Clipper1->GetOutputPort());
	this->SelectedSurfaceMapper->ScalarVisibilityOff();
	this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
	//this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	//this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor1);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor2);
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}

//-------------------------------------------------------------------------------
	void vtkKWMimxBooleanSurfaceGroup::PrintSelf(ostream& os, vtkIndent indent)
	{
		this->Superclass::PrintSelf(os,indent);
	}
//----------------------------------------------------------------------------
	void vtkKWMimxBooleanSurfaceGroup::SaveVTKSurfaceCancelCallback()
	{

//  this->MainFrame->UnpackChildren();

		this->UnpackGroup();
		this->MenuGroup->SetMenuButtonsEnabled(1);
		this->GetMimxMainWindow()->GetMainUserInterfacePanel()->SetEnabled(1);	
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor1);
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor2);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();

// added to add the existing surfaces back to the renderer----------------------------------------------

	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

	int surface2Index = -1;
	vtkKWComboBox *combobox_surf2 = this->ObjectListComboBox1->GetWidget();
    const char *name_surf2 = combobox_surf2->GetValue();  	
	surface2Index = combobox_surf2->GetValueIndex(name_surf2);

	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface2Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

	// added to add the existing surfaces back to the renderer----------------------------------------------

	Menubutton->GetWidget()->SetValue("");

	}
//------------------------------------------------------------------------------
	void vtkKWMimxBooleanSurfaceGroup::UpdateObjectLists()
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
		}


//--------------------------------------------------------------------------------
//// Surface 2

	this->ObjectListComboBox1->GetWidget()->DeleteAllValues();
	
//	int defaultItem = -1;
	for (int i = 0; i < this->SurfaceList->GetNumberOfItems(); i++)
	{
		ObjectListComboBox1->GetWidget()->AddValue(
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
    ObjectListComboBox1->GetWidget()->SetValue(
          this->SurfaceList->GetItem(defaultItem)->GetFileName());
  }

}
//--------------------------------------------------------------------------------	
//--------------------------------------------------------------------------------
void vtkKWMimxBooleanSurfaceGroup::SaveVTKSurfaceDoneCallback()
{
	if(this->SaveVTKSurfaceApplyCallback())
		this->SaveVTKSurfaceCancelCallback();
}
//---------------------------------------------------------------------------------
