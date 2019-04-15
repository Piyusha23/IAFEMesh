/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMenuOptionGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/01/05 20:40:12 $
Version:   $Revision: 1.3 $

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

#include "vtkKWMimxMenuOptionGroup.h"

#include <stdio.h>
#include <string.h>


#include <vtkObjectFactory.h>
#include <vtksys/SystemTools.hxx>


#include "vtkKWApplication.h"
#include <vtkKWFrameWithScrollbar.h>
#include <vtkKWPushButton.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxMenuOptionGroup);
vtkCxxRevisionMacro(vtkKWMimxMenuOptionGroup, "$Revision: 1.3 $");

//----------------------------------------------------------------------------
vtkKWMimxMenuOptionGroup::vtkKWMimxMenuOptionGroup()
{
  this->CancelButton = NULL;
  this->ApplyButton = NULL;
  this->MainFrameWithSlider = NULL;
  this->BaseFrame = NULL;
  this->ApplyCancelButtonFrame = NULL;
  
  this->Count = 0;
  this->MenuGroup = NULL;
  
  this->CancelStatus = 0;
  this->UseFrameWithSlider = 1;
  this->UseApplyButton = 1;
  this->UseCancelButton = 1;
  strcpy(this->objectSelectionPrevious,"");
}

//----------------------------------------------------------------------------
vtkKWMimxMenuOptionGroup::~vtkKWMimxMenuOptionGroup()
{
  if (this->CancelButton)
    this->CancelButton->Delete();
  if (this->ApplyButton) 
    this->ApplyButton->Delete();
  if (this->BaseFrame)
    this->BaseFrame->Delete();
  if (this->ApplyCancelButtonFrame) 
    this->ApplyCancelButtonFrame->Delete();
  if (this->MainFrameWithSlider) 
    this->MainFrameWithSlider->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::CreateWidget()
{
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();

  if (!this->BaseFrame)
    this->BaseFrame = vtkKWFrame::New();
  this->BaseFrame->SetParent( this->GetParent() );
  this->BaseFrame->Create();
  this->BaseFrame->SetBorderWidth(0);
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 0 -pady 0 -fill x", 
    this->BaseFrame->GetWidgetName());

  if ( UseFrameWithSlider == 1)
  {
    if (!this->MainFrameWithSlider)
      this->MainFrameWithSlider = vtkKWFrameWithScrollbar::New();
    this->MainFrameWithSlider->SetParent( this->BaseFrame );
    this->MainFrameWithSlider->Create();
    this->MainFrameWithSlider->SetBorderWidth(0);
    this->GetApplication()->Script(
      "pack %s -side top -anchor nw -padx 0 -pady 0 -fill x", 
      this->MainFrameWithSlider->GetWidgetName());
    this->MainFrame = this->MainFrameWithSlider->GetFrame();
  }
  else
  {
    this->MainFrame = this->BaseFrame;
  }
  
  if ( ( this->UseApplyButton ) || ( this->UseCancelButton ) )
  {
    if (!this->ApplyCancelButtonFrame)
      this->ApplyCancelButtonFrame = vtkKWFrame::New();
    this->ApplyCancelButtonFrame->SetParent( this->MainFrame );
    this->ApplyCancelButtonFrame->Create();
    this->GetApplication()->Script(
      "pack %s -side bottom -anchor nw -expand n -padx 5 -pady 6 -fill x", 
      this->ApplyCancelButtonFrame->GetWidgetName());
    
    if ( this->UseApplyButton )
    {
      if (!this->ApplyButton)
        this->ApplyButton = vtkKWPushButton::New();
      this->ApplyButton->SetParent(this->ApplyCancelButtonFrame);
      this->ApplyButton->Create();
      this->ApplyButton->SetReliefToFlat();
      this->ApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
      this->GetApplication()->Script(
                                     "pack %s -side left -anchor nw -expand y -padx 5 -pady 6", 
                                     this->ApplyButton->GetWidgetName());
    }
    
    if ( this->UseCancelButton )
    {
      if (!this->CancelButton)
        this->CancelButton = vtkKWPushButton::New();
      this->CancelButton->SetParent(this->ApplyCancelButtonFrame);
      this->CancelButton->Create();
      this->CancelButton->SetReliefToFlat();
      this->CancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
      this->GetApplication()->Script(
                                     "pack %s -side right -anchor ne -expand y -padx 5 -pady 6", 
                                     this->CancelButton->GetWidgetName());
    }
  }
  
}

//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::PackGroup()
{
  this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
                                 this->BaseFrame->GetWidgetName()); 
}

//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::UnpackGroup()
{
  this->GetApplication()->Script("pack forget %s", this->BaseFrame->GetWidgetName() );
}

//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::Update()
{
	this->UpdateEnableState();
}

//---------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "Base Frame" << std::endl;
  this->BaseFrame->PrintSelf(os,indent.GetNextIndent());
  
  os << indent << "UseFrameWithSlider" << this->UseFrameWithSlider << std::endl;
  if (this->UseFrameWithSlider)
  {
    os << indent << "Main Frame" << std::endl;
    this->MainFrameWithSlider->PrintSelf(os,indent.GetNextIndent());
  }
  
  os << indent << "Apply/Cancel Button Frame" << std::endl;
  this->ApplyCancelButtonFrame->PrintSelf(os,indent.GetNextIndent());
  
  os << indent << "Cancel Button" << std::endl;
  this->CancelButton->PrintSelf(os,indent.GetNextIndent());
  
  os << indent << "Apply Button" << std::endl;
  this->ApplyButton->PrintSelf(os,indent.GetNextIndent());
}
//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::SetUseApplyCancelButtons(int useButtons)
{
  this->UseCancelButton = useButtons;
  this->UseApplyButton = useButtons;
}

//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::SetUseApplyCancelButtonsOn( )
{
  this->SetUseApplyCancelButtons(1);
}

//----------------------------------------------------------------------------
void vtkKWMimxMenuOptionGroup::SetUseApplyCancelButtonsOff( )
{
  this->SetUseApplyCancelButtons(0);
}
//----------------------------------------------------------------------------
const char* vtkKWMimxMenuOptionGroup::ExtractFileName(const char* FName)
{
  std::string fileName = vtksys::SystemTools::GetFilenameWithoutExtension(FName);
  
  char *fileNameCopy = new char[fileName.length()+1];
  strcpy( fileNameCopy, fileName.c_str() );
  
  return fileNameCopy;
}
//----------------------------------------------------------------------------
