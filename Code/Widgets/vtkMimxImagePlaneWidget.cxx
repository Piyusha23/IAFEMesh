/*=========================================================================
 
 Program:   MIMX Meshing Toolkit
 Module:    $RCSfile: vtkMimxImagePlaneWidget.cxx,v $
 Language:  C++
 Date:      $Date: 2013/01/31 14:51:36 $
 Version:   $Revision: 1.4 $
 
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

#include "vtkMimxImagePlaneWidget.h"
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkTextActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkAssemblyPath.h>
#include <vtkCollectionIterator.h>
#include <vtkAssemblyNode.h>
#include <vtkAbstractPropPicker.h>


//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkMimxImagePlaneWidget, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkMimxImagePlaneWidget);

//----------------------------------------------------------------------------
vtkActor* vtkMimxImagePlaneWidget::GetTexturePlaneActor()
{
	return this->TexturePlaneActor;
}

//----------------------------------------------------------------------------
vtkAbstractPropPicker* vtkMimxImagePlaneWidget::GetPlanePicker()
{
	return this->PlanePicker;
}

//----------------------------------------------------------------------------
vtkMimxImagePlaneWidget::vtkMimxImagePlaneWidget() 
{
  vtkMatrix4x4* transformMatrix = vtkMatrix4x4::New();
  transformMatrix->Identity();
  
  this->IJKToLPSTransform = vtkTransform::New();
  this->IJKToLPSTransform->SetMatrix( transformMatrix );
}

//----------------------------------------------------------------------------
vtkMimxImagePlaneWidget::~vtkMimxImagePlaneWidget() 
{

}


void vtkMimxImagePlaneWidget::SetIJKToLPSTransform(vtkTransform *transform)
{
  this->IJKToLPSTransform->SetMatrix( transform->GetMatrix() );
}

//----------------------------------------------------------------------------
void vtkMimxImagePlaneWidget::OnChar()
{
  vtkRenderWindowInteractor *i = this->Interactor;
  
  // Toggle X-axis On/Off
  if (i->GetKeyCode() == 'x' && this->PlaneOrientation == 0)
    {
    if (this->GetEnabled())
      {
      this->SetEnabled(0);
      }
    else
      {
      this->SetEnabled(1);
      }
    }
  
  // Toggle Y-axis On/Off
  if (i->GetKeyCode() == 'y' && this->PlaneOrientation == 1)
    {
    if (this->GetEnabled())
      {
      this->SetEnabled(0);
      }
    else
      {
      this->SetEnabled(1);
      }
    }
  
  // Toggle Z-axis On/Off
  if (i->GetKeyCode() == 'z' && this->PlaneOrientation == 2)
    {
    if (this->GetEnabled())
      {
      this->SetEnabled(0);
      }
    else
      {
      this->SetEnabled(1);
      }
    }
  
  //std::cout << "Got Key: " << i->GetKeyCode() << std::endl;
  if ( i->GetKeyCode() == 'r' || i->GetKeyCode() == 'R' )
    {
    if ( i->GetShiftKey() || i->GetControlKey() )
      {
      this->SetWindowLevel( this->OriginalWindow, this->OriginalLevel );
      double wl[2] = { this->CurrentWindow, this->CurrentLevel };
      
      this->EventCallbackCommand->SetAbortFlag(1);
      this->InvokeEvent(vtkCommand::ResetWindowLevelEvent, wl);
      }
    else
      {
      this->Interactor->GetInteractorStyle()->OnChar();
      }
    }
  else
    {
    this->Interactor->GetInteractorStyle()->OnChar();
    }
}



//----------------------------------------------------------------------------
void vtkMimxImagePlaneWidget::ManageTextDisplay()
{
  if ( !this->DisplayText )
  {
    return;
  }

  if ( this->State == vtkImagePlaneWidget::WindowLevelling )
  {
    sprintf(this->TextBuff,"Window, Level: ( %g, %g )",
    this->CurrentWindow, this->CurrentLevel );
  }
  else if ( this->State == vtkImagePlaneWidget::Cursoring )
  {
    if ( this->CurrentImageValue == VTK_DOUBLE_MAX )
    {
      sprintf(this->TextBuff,"Off Image");
    }
    else
    {
      double lpsPoint[3];
      this->IJKToLPSTransform->TransformPoint(this->CurrentCursorPosition, lpsPoint);
      sprintf(this->TextBuff,"( %4.2f, %4.2f, %4.2f ): %g",
        lpsPoint[0],
        lpsPoint[1],
        lpsPoint[2],this->CurrentImageValue);
    }
  }

  this->TextActor->SetInput(this->TextBuff);
  this->TextActor->Modified();
}

//----------------------------------------------------------------------------
void vtkMimxImagePlaneWidget::StartCursor()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];
  
  // Okay, make sure that the pick is in the current renderer
  if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
  {
    this->State = vtkImagePlaneWidget::Outside;
    return;
  }
  
  // Okay, we can process this. If anything is picked, then we
  // can start pushing the plane.
  vtkAssemblyPath *path;
  this->PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->PlanePicker->GetPath();
  
  int found = 0;
  int i;
  if ( path != 0 )
  {
    // Deal with the possibility that we may be using a shared picker
    vtkCollectionSimpleIterator sit;
    path->InitTraversal(sit);
    vtkAssemblyNode *node;
    for ( i = 0; i < path->GetNumberOfItems() && !found ; i++ )
    {
      node = path->GetNextNode(sit);
      if ( node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor) )
      {
        found = 1;
      }
    }
  }
  
  if( ! found || path == 0 )
  {
    this->State = vtkImagePlaneWidget::Outside;
    this->HighlightPlane(0);
    this->ActivateCursor(0);
    this->ActivateText(0);
    return;
  }
  else
  {
    this->State = vtkImagePlaneWidget::Cursoring;
    this->HighlightPlane(1);
    this->ActivateCursor(1);
    this->ActivateText(1);
    this->UpdateCursor(X,Y);
    this->ManageTextDisplay();
  }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,0);
  this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkMimxImagePlaneWidget::OnMouseMove()
{
  // See whether we're active
  //
  if ( this->State == vtkImagePlaneWidget::Outside ||
      this->State == vtkImagePlaneWidget::Start )
  {
    return;
  }
  
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];
  
  // Do different things depending on state
  // Calculations everybody does
  //
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z, vpn[3];
  
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if ( ! camera )
  {
    return;
  }
  
  // Compute the two points defining the motion vector
  //
  this->ComputeWorldToDisplay(this->LastPickPosition[0], 
                              this->LastPickPosition[1],
                              this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];
  
  this->ComputeDisplayToWorld(
                              double(this->Interactor->GetLastEventPosition()[0]),
                              double(this->Interactor->GetLastEventPosition()[1]),
                              z, prevPickPoint);
  
  this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);
  
  if ( this->State == vtkImagePlaneWidget::WindowLevelling )
  {
    this->WindowLevel(X,Y);
    this->ManageTextDisplay();
  }
  else if ( this->State == vtkImagePlaneWidget::Pushing )
  {
    this->Push(prevPickPoint, pickPoint);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if ( this->State == vtkImagePlaneWidget::Spinning )
  {
    this->Spin(prevPickPoint, pickPoint);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if ( this->State == vtkImagePlaneWidget::Rotating )
  {
    camera->GetViewPlaneNormal(vpn);
    this->Rotate(prevPickPoint, pickPoint, vpn);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if ( this->State == vtkImagePlaneWidget::Scaling )
  {
    this->Scale(prevPickPoint, pickPoint, X, Y);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if ( this->State == vtkImagePlaneWidget::Moving )
  {
    this->Translate(prevPickPoint, pickPoint);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if ( this->State == vtkImagePlaneWidget::Cursoring )
  {
    this->UpdateCursor(X,Y);
    this->ManageTextDisplay();
  }
  
  // Interact, if desired
  //
  this->EventCallbackCommand->SetAbortFlag(1);
  
  if ( this->State == vtkImagePlaneWidget::WindowLevelling )
  {
    double wl[2] = { this->CurrentWindow, this->CurrentLevel };
    this->InvokeEvent(vtkCommand::WindowLevelEvent,wl);
  }
  else
  {
    this->InvokeEvent(vtkCommand::InteractionEvent,0);
  }
  
  this->Interactor->Render();
}
