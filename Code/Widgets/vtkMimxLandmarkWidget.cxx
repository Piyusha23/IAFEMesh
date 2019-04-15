/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxLandmarkWidget.cxx,v $
Language:  C++

Date:      $Date: 2013/01/29 03:10:48 $
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
#include "vtkMimxLandmarkWidget.h"

#include <vtkCallbackCommand.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkEvent.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkWidgetCallbackMapper.h> 
#include <vtkWidgetEventTranslator.h>

vtkCxxRevisionMacro(vtkMimxLandmarkWidget, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMimxLandmarkWidget);

//----------------------------------------------------------------------
vtkMimxLandmarkWidget::vtkMimxLandmarkWidget()
{
  //Add sphere landmark
  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::LeftButtonPressEvent,
            vtkEvent::ControlModifier, 0, 1, NULL,
			vtkMimxLandmarkWidget::CtrlLeftMouseButtonDown,
			this, vtkMimxLandmarkWidget::CtrlLeftButtonDownCallback);

  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::LeftButtonReleaseEvent,
			vtkEvent::ControlModifier, 0, 1, NULL,
            vtkMimxLandmarkWidget::CtrlLeftMouseButtonUp,
            this, vtkMimxLandmarkWidget::CtrlLeftButtonUpCallback);

  //Remove sphere landmark
  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::RightButtonPressEvent,
			vtkEvent::ControlModifier, 0, 1, NULL,
            vtkMimxLandmarkWidget::CtrlRightMouseButtonDown,
            this, vtkMimxLandmarkWidget::CtrlRightButtonDownCallback);

  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::RightButtonReleaseEvent,
			vtkEvent::ControlModifier, 0, 1, NULL,
            vtkMimxLandmarkWidget::CtrlRightMouseButtonUp,
            this, vtkMimxLandmarkWidget::CtrlRightButtonUpCallback);
  
  //Select and move sphere landmark
  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::MiddleButtonPressEvent,
			vtkEvent::ControlModifier, 0, 1, NULL,
            vtkMimxLandmarkWidget::CtrlMiddleMouseButtonDown,
            this, vtkMimxLandmarkWidget::CtrlMiddleButtonDownCallback);

  this->CallbackMapper->SetCallbackMethod(
          vtkCommand::MouseMoveEvent,
		  vtkEvent::ControlModifier, 0, 1, NULL,
          vtkMimxLandmarkWidget::CtrlMiddleMouseButtonMove,
          this, vtkMimxLandmarkWidget::CtrlMiddleButtonMoveCallback);

  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::MiddleButtonReleaseEvent,
			vtkEvent::ControlModifier, 0, 1, NULL,
            vtkMimxLandmarkWidget::CtrlMiddleMouseButtonUp,
            this, vtkMimxLandmarkWidget::CtrlMiddleButtonUpCallback);


  this->Actor = NULL;
  this->Actor2 = NULL;
  this->Actor3 = NULL;
  this->ActorCollection = vtkActorCollection::New();
  this->SelectedActor = vtkPolyData::New();
  this->CellPicker = NULL;
  this->State = vtkMimxLandmarkWidget::Waiting;
 
  //initialize sphere radius if not done elsewhere
  this->sphere_radius = NULL;
  this->visibility = NULL;
}

//----------------------------------------------------------------------
vtkMimxLandmarkWidget::~vtkMimxLandmarkWidget()
{

	int numItems = this->ActorCollection->GetNumberOfItems();
	for (int i=0; i<numItems; i++)
	{
		vtkActor *actor = vtkActor::SafeDownCast(this->ActorCollection->GetItemAsObject(i));
		actor->Delete();
	}
	this->ActorCollection->Delete();
	this->SelectedActor->Delete();
}

//----------------------------------------------------------------------
void vtkMimxLandmarkWidget::SetEnabled(int enabling)
{  
  if ( enabling ) //----------------
    {
    vtkDebugMacro(<<"Enabling widget");
  
    if ( this->Enabled ) //already enabled, just return
      {
      return;
      }
  
    if ( ! this->Interactor )
      {
      vtkErrorMacro(<<"The interactor must be set prior to enabling the widget");
      return;
      }

    int X=this->Interactor->GetEventPosition()[0];
    int Y=this->Interactor->GetEventPosition()[1];
  
    if ( ! this->CurrentRenderer )
      {
      this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(X,Y));
  
      if (this->CurrentRenderer == NULL)
        {
        return;
        }

      }
  
    // We're ready to enable
    this->Enabled = 1;
  
    // listen for the events found in the EventTranslator
    if ( ! this->Parent )
      {
      this->EventTranslator->AddEventsToInteractor(this->Interactor,
        this->EventCallbackCommand,this->Priority);
      }
    else
      {
      this->EventTranslator->AddEventsToParent(this->Parent,
        this->EventCallbackCommand,this->Priority);
      }
        
	if ( !this->Interactor )
	{
		vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
		return;
	}

    }
  
  else //disabling------------------
    {
    vtkDebugMacro(<<"Disabling widget");
  
    if ( ! this->Enabled ) //already disabled, just return
      {
      return;
      }
  
    this->Enabled = 0;
  
    // don't listen for events any more
    if ( ! this->Parent )
      {
      this->Interactor->RemoveObserver(this->EventCallbackCommand);
      }
    else
      {
      this->Parent->RemoveObserver(this->EventCallbackCommand);
      }
    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->SetCurrentRenderer(NULL);
    }
  
  // Should only render if there is no parent
  if ( this->Interactor && !this->Parent )
    {
    this->Interactor->Render();
    }
}
//--------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::CtrlLeftButtonDownCallback(vtkAbstractWidget *w)
{
	vtkMimxLandmarkWidget *self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);

	//int X = self->Interactor->GetEventPosition()[0];
	//int Y = self->Interactor->GetEventPosition()[1];

	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
	self->Interactor->Render();
}
//----------------------------------------------------------------------
void vtkMimxLandmarkWidget::CtrlLeftButtonUpCallback(vtkAbstractWidget *w)
{
  vtkMimxLandmarkWidget *self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);

  vtkMimxLandmarkWidget *Self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);
  int X = Self->Interactor->GetEventPosition()[0];
  int Y = Self->Interactor->GetEventPosition()[1];

  if (!Self->CurrentRenderer || !Self->CurrentRenderer->IsInViewport(X, Y))
  {
	  Self->WidgetEvent = vtkMimxLandmarkWidget::Outside;
	  return;
  }

  vtkAssemblyPath *path;
  vtkCellPicker *cPicker = vtkCellPicker::SafeDownCast(Self->CellPicker);
  cPicker->AddPickList(self->Actor);
  cPicker->PickFromListOn();
  cPicker->SetTolerance(0.005);
  cPicker->Pick(X,Y,0.0,self->CurrentRenderer);
  path = cPicker->GetPath();
  if(path !=NULL)
  {
	  int PickedCell = cPicker->GetCellId();
	  if(PickedCell > -1)
	  {
		double *pickedLoc = cPicker->GetPickPosition();	
		vtkSphereSource *sphereSource = vtkSphereSource::New();
		sphereSource->SetCenter(pickedLoc);
		sphereSource->SetRadius(self->sphere_radius);
		sphereSource->Update();
		vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
		mapper->SetInput(sphereSource->GetOutput());
		vtkActor *actor = vtkActor::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
		self->CurrentRenderer->AddActor(actor);
		self->ActorCollection->AddItem(actor);
		sphereSource->Delete();
		mapper->Delete();
	  }
  }
  
  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();
  self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  self->Interactor->Render();

}
//--------------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::CtrlRightButtonUpCallback(vtkAbstractWidget *w)
{
	vtkMimxLandmarkWidget *self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);

	//int X = self->Interactor->GetEventPosition()[0];
	//int Y = self->Interactor->GetEventPosition()[1];

	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
	self->Interactor->Render();

}
//----------------------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::CtrlRightButtonDownCallback(vtkAbstractWidget *w)
{
	vtkMimxLandmarkWidget *Self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);
	int X = Self->Interactor->GetEventPosition()[0];
	int Y = Self->Interactor->GetEventPosition()[1];

	if (!Self->CurrentRenderer || !Self->CurrentRenderer->IsInViewport(X, Y) || !Self->ActorCollection->GetLastItem())
	{
		Self->WidgetEvent = vtkMimxLandmarkWidget::Outside;
		return;
	}

	vtkAssemblyPath *path;
	vtkCellPicker *spherePicker = vtkCellPicker::New();
	Self->ActorCollection->InitTraversal();

	//Add all spheres to pick list
	for (int i = 0; i < Self->ActorCollection->GetNumberOfItems(); i++)
	{
		spherePicker->AddPickList(Self->ActorCollection->GetNextActor());

	}
	spherePicker->PickFromListOn();
	spherePicker->SetTolerance(0.005);
	spherePicker->Pick(X,Y,0.0, Self->CurrentRenderer);
	path = spherePicker->GetPath();

	if(path !=NULL)
	{
		int PickedCell = spherePicker->GetCellId();
		if(PickedCell > -1)
		{
			//Remove sphere from renderer and from pick list
			Self->CurrentRenderer->RemoveActor(spherePicker->GetActor());
			Self->ActorCollection->RemoveItem(spherePicker->GetActor());
		}
	}
	else
	{
		spherePicker->Delete();
		return;
	}

	Self->EventCallbackCommand->SetAbortFlag(1);
	Self->StartInteraction();
	Self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	Self->Interactor->Render();
}
//----------------------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::CtrlMiddleButtonDownCallback(vtkAbstractWidget *w)
{
	vtkMimxLandmarkWidget *Self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);
	int X = Self->Interactor->GetEventPosition()[0];
	int Y = Self->Interactor->GetEventPosition()[1];

	if (!Self->CurrentRenderer || !Self->CurrentRenderer->IsInViewport(X, Y) || !Self->ActorCollection->GetLastItem())
	{
		Self->WidgetEvent = vtkMimxLandmarkWidget::Outside;
		return;
	}

	vtkAssemblyPath *path;
	vtkCellPicker *spherePicker = vtkCellPicker::New();
	Self->ActorCollection->InitTraversal();

	//Add all spheres to pick list
	for (int i = 0; i < Self->ActorCollection->GetNumberOfItems(); i++)
	{
		spherePicker->AddPickList(Self->ActorCollection->GetNextActor());

	}
	//Pick sphere
	spherePicker->PickFromListOn();
	spherePicker->SetTolerance(0.005);
	spherePicker->Pick(X,Y,0.0, Self->CurrentRenderer);
	path = spherePicker->GetPath();

	if(path !=NULL)
	{
		int PickedCell = spherePicker->GetCellId();
		if(PickedCell > -1)
		{
			//Select sphere
			Self->SelectedActor = vtkPolyData::SafeDownCast(spherePicker->GetActor()->GetMapper()->GetInput());
			spherePicker->GetActor()->GetMapper()->GetCenter(Self->LastPickPosition);
		}
	}
	else
	{
		spherePicker->Delete();
		return;
	}
	Self->State = vtkMimxLandmarkWidget::Selected;
	Self->EventCallbackCommand->SetAbortFlag(1);
	Self->StartInteraction();
	Self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	Self->Interactor->Render();

}
//----------------------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::CtrlMiddleButtonMoveCallback(vtkAbstractWidget *w)
{
	vtkMimxLandmarkWidget *Self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);
	if(Self->State == vtkMimxLandmarkWidget::Selected || Self->State == vtkMimxLandmarkWidget::Moving)
	{
		int X = Self->Interactor->GetEventPosition()[0];
		int Y = Self->Interactor->GetEventPosition()[1];

		//Ensure you're within the area for selection
		if (!Self->CurrentRenderer || !Self->CurrentRenderer->IsInViewport(X, Y) || !Self->ActorCollection->GetLastItem())
		{
			Self->WidgetEvent = vtkMimxLandmarkWidget::Outside;
			return;
		}

		//Variables for picked positions
		double pick_point[4] = {0,0,0};
		double focal_point[3] = {0,0,0};
		double temp_point[3] = {0,0,0};
		double z = 0;

		Self->ComputeWorldToDisplay(Self->LastPickPosition[0], Self->LastPickPosition[1], Self->LastPickPosition[2], focal_point);
		z = focal_point[2];
		Self->ComputeDisplayToWorld(double(X), double(Y), z, pick_point);

		vtkPoints *sphere_points = Self->SelectedActor->GetPoints();
		
		// Assign sphere points to new position
		for(int i=0; i<sphere_points->GetNumberOfPoints(); i++)
		{
			sphere_points->GetPoint(i,temp_point);
			temp_point[0]+=(pick_point[0]-Self->LastPickPosition[0]);
			temp_point[1]+=(pick_point[1]-Self->LastPickPosition[1]);
			temp_point[2]+=(pick_point[2]-Self->LastPickPosition[2]);
			sphere_points->SetPoint(i,temp_point);
		}
		
		sphere_points->Modified();

		Self->SelectedActor->SetPoints(sphere_points);
		Self->SelectedActor->Update();

		Self->LastPickPosition[0] = pick_point[0];
		Self->LastPickPosition[1] = pick_point[1];
		Self->LastPickPosition[2] = pick_point[2];

		Self->State = vtkMimxLandmarkWidget::Moving;
		Self->EventCallbackCommand->SetAbortFlag(1);
		Self->StartInteraction();
		Self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
		Self->Interactor->Render();
	}
}
//----------------------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::CtrlMiddleButtonUpCallback(vtkAbstractWidget *w)
{
	vtkMimxLandmarkWidget *Self = reinterpret_cast<vtkMimxLandmarkWidget*>(w);
	if(Self->State == vtkMimxLandmarkWidget::Selected || Self->State == vtkMimxLandmarkWidget::Moving)
	{
		int X = Self->Interactor->GetEventPosition()[0];
		int Y = Self->Interactor->GetEventPosition()[1];

		if (!Self->CurrentRenderer || !Self->CurrentRenderer->IsInViewport(X, Y) || !Self->ActorCollection->GetLastItem())
		{
			Self->WidgetEvent = vtkMimxLandmarkWidget::Outside;
			return;
		}
		//Variables for picked positions
		double pick_point[4] = {0,0,0};
		double focal_point[3] = {0,0,0};
		double temp_point[3] = {0,0,0};
		double z = 0;

		Self->ComputeWorldToDisplay(Self->LastPickPosition[0], Self->LastPickPosition[1], Self->LastPickPosition[2], focal_point);
		z = focal_point[2];
		Self->ComputeDisplayToWorld(double(X), double(Y), z, pick_point);

		vtkPoints *sphere_points = Self->SelectedActor->GetPoints();

		vtkIdType cellId;
		double dist = 9999999;
		double dist1;
		double dist2;
		double dist3;
		int subId;
		double closestPoint[3] = {0,0,0};
		double closestPoint1[3] = {0,0,0};
		double closestPoint2[3] = {0,0,0};
		double closestPoint3[3] = {0,0,0};
		
		//Determine which plane is closest to point for selection
		if(Self->Actor != NULL)
		{
			vtkCellLocator *selectOnPlane1 = vtkCellLocator::New();
			selectOnPlane1->SetDataSet(Self->Actor->GetMapper()->GetInputAsDataSet());
			selectOnPlane1->BuildLocator();
			selectOnPlane1->FindClosestPoint(pick_point,closestPoint1,cellId,subId,dist1);

			if(dist1 < dist)
			{
				closestPoint[0] = closestPoint1[0];
				closestPoint[1] = closestPoint1[1];
				closestPoint[2] = closestPoint1[2];
				dist = dist1;
			}
		}
		if(Self->Actor2 != NULL)
		{
			vtkCellLocator *selectOnPlane2 = vtkCellLocator::New();
			selectOnPlane2->SetDataSet(Self->Actor2->GetMapper()->GetInputAsDataSet());
			selectOnPlane2->BuildLocator();
			selectOnPlane2->FindClosestPoint(pick_point,closestPoint2,cellId,subId,dist2);

			if(dist2 < dist)
			{
				closestPoint[0] = closestPoint2[0];
				closestPoint[1] = closestPoint2[1];
				closestPoint[2] = closestPoint2[2];
				dist = dist2;
			}
		}
		if(Self->Actor3 != NULL)
		{
			vtkCellLocator *selectOnPlane3 = vtkCellLocator::New();
			selectOnPlane3->SetDataSet(Self->Actor3->GetMapper()->GetInputAsDataSet());
			selectOnPlane3->BuildLocator();
			selectOnPlane3->FindClosestPoint(pick_point,closestPoint3,cellId,subId,dist3);
			if(dist3 < dist)
			{
				closestPoint[0] = closestPoint3[0];
				closestPoint[1] = closestPoint3[1];
				closestPoint[2] = closestPoint3[2];
				dist = dist3;
			}
		}
		
		// Assign sphere points to new position
		for(int i=0; i<sphere_points->GetNumberOfPoints(); i++)
		{
			sphere_points->GetPoint(i,temp_point);
			temp_point[0]+=(closestPoint[0]-Self->LastPickPosition[0]);
			temp_point[1]+=(closestPoint[1]-Self->LastPickPosition[1]);
			temp_point[2]+=(closestPoint[2]-Self->LastPickPosition[2]);
			sphere_points->SetPoint(i,temp_point);
		}
		
		sphere_points->Modified();

		Self->SelectedActor->SetPoints(sphere_points);
		Self->SelectedActor->Update();

		Self->LastPickPosition[0] = closestPoint[0];
		Self->LastPickPosition[1] = closestPoint[1];
		Self->LastPickPosition[2] = closestPoint[2];

		Self->State = vtkMimxLandmarkWidget::Move_Complete;
		Self->EventCallbackCommand->SetAbortFlag(1);
		Self->StartInteraction();
		Self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
		Self->Interactor->Render();
	}
}
//--------------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);  
}
//---------------------------------------------------------------------
//Set the individual plane actors
void vtkMimxLandmarkWidget::SetActor(vtkActor *InputActor)
{
	this->Actor = InputActor;
}
//---------------------------------------------------------------------
//Set the individual plane actors
void vtkMimxLandmarkWidget::SetActor2(vtkActor *InputActor)
{
	this->Actor2 = InputActor;
}
//---------------------------------------------------------------------
//Set the individual plane actors
void vtkMimxLandmarkWidget::SetActor3(vtkActor *InputActor)
{
	this->Actor3 = InputActor;
}
//--------------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::SetPicker(vtkAbstractPropPicker *PPicker)
{
	this->CellPicker = PPicker;
}
//----------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::GetLandmark(vtkActor *actor, double point[3])
{
	actor->GetMapper()->GetInput()->GetCenter(point);
}
//----------------------------------------------------------------------------------------
int vtkMimxLandmarkWidget::AddLandmark(double point[3])
{
	//Add a sphere from input point as center
	int index = this->ActorCollection->GetNumberOfItems();

	vtkSphereSource *sphere = vtkSphereSource::New();
	sphere->SetCenter(point);
	sphere->SetRadius(this->sphere_radius);
	sphere->Update();

	vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
	mapper->SetInput(sphere->GetOutput());

	vtkActor *sphereactor = vtkActor::New();
	sphereactor->SetMapper(mapper);
	sphereactor->GetProperty()->SetColor(0.0, 1.0, 0.0);

	this->CurrentRenderer->AddActor(sphereactor);
	this->ActorCollection->AddItem(sphereactor);

	//Update the window
	this->Interactor->Render();

	return index;
}
//----------------------------------------------------------------------------------------
int vtkMimxLandmarkWidget::GetNumberOfLandmarks()
{
	return this->ActorCollection->GetNumberOfItems();
}
//----------------------------------------------------------------------------------------
vtkActorCollection* vtkMimxLandmarkWidget::GetActorCollection()
{
	return this->ActorCollection;
}
//----------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::GetLandmarkIndex(double point[3], int index)
{
	//Input a sphere center point and output the index for that sphere actor

	//Go to beginning of list of sphere actors
	this->ActorCollection->InitTraversal();
	
	vtkActor *actor = vtkActor::New();

	//Loop through all spheres and test whether input point matches any sphere center points by testing distance
	for (int i = 0; i < this->ActorCollection->GetNumberOfItems(); i++)
	{
		double temp_center[3] = {0,0,0};
		actor->GetMapper()->GetInput()->GetCenter(temp_center);

		if(vtkMath::Distance2BetweenPoints(point,temp_center) == 0)
		{
			index = i;
		}
	}
}
//----------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::RemoveAllLandmarks()
{
	//Go to beginning of list of sphere actors
	this->ActorCollection->InitTraversal();
	
	//Loop through all spheres
	for (int i = 0; i < this->ActorCollection->GetNumberOfItems(); i++)
	{
		this->CurrentRenderer->RemoveActor(ActorCollection->GetNextActor());
	}
	
	//Remove all items from actor list
	this->ActorCollection->RemoveAllItems();

	//Update the window
	this->Interactor->Render();
}
//----------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::SetVisibility(int v)
{
	this->visibility = v;
	
	if(this->ActorCollection->GetNumberOfItems()>0)
	{
		//Turn off visibility with 0 and on visibility with 1
		switch(this->visibility)
		{
			case 0:
				//Go to beginning of list of sphere actors
				this->ActorCollection->InitTraversal();
				
				//Loop through all spheres and remove them from visibility
				for (int i = 0; i < this->ActorCollection->GetNumberOfItems(); i++)
				{
					this->CurrentRenderer->RemoveActor(ActorCollection->GetNextActor());
				}
				this->Interactor->Render();
				break;

			case 1:
				//Go to beginning of list of sphere actors
				this->ActorCollection->InitTraversal();
				
				//Loop through all spheres and add them to visibility
				for (int i = 0; i < this->ActorCollection->GetNumberOfItems(); i++)
				{
					this->CurrentRenderer->AddActor(ActorCollection->GetNextActor());
				}
				this->Interactor->Render();
				break;
		}
	}
}
//----------------------------------------------------------------------------------------
void vtkMimxLandmarkWidget::SetSphereRadius(double radius)
{
	//Record the size for future spheres
	this->sphere_radius = radius;
}
//----------------------------------------------------------------------------------------
double vtkMimxLandmarkWidget::GetSphereRadius()
{
	//Return the size for future spheres
	return this->sphere_radius;
}
//----------------------------------------------------------------------------------------
int vtkMimxLandmarkWidget::GetVisibility()
{
	return this->visibility;
	
}
