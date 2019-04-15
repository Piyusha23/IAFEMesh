/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/01/21 02:13:36 $
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
#include <stdio.h>
#include <string.h>

#include "vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.h"

// ITK Headers
#include "itkImage.h"

// VTK Headers
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkIntArray.h>
#include <vtkMimxSurfacePolyDataActor.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkMimxUnstructuredGridActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkMimxImageActor.h>
#include <vtkMimxApplyImageBasedMaterialProperties.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkObjectFactory.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

//KWWidget Headers
#include <vtkKWApplication.h>
#include <vtkKWFileBrowserDialog.h>
#include <vtkKWEvent.h>
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWLabel.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWNotebook.h>
#include <vtkKWOptions.h>
#include <vtkKWRenderWidget.h>
#include <vtkKWTkUtilities.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWPushButton.h>
#include <vtkKWMimxMainUserInterfacePanel.h>
#include <vtkKWMessageDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWIcon.h>

// MIMX Headers
#include "vtkMimxMeshActor.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxDefineElSetGroup.h"
#include "vtkMimxRebinMaterialProperty.h"
#include "vtkLinkedListWrapper.h"
#include "Resources/mimxElementSetDfns.h"
#include "Resources/mimxBin.h"
#include "Resources/mimxLabelEquation.h"
#include "Resources/mimxButtonEquation.h"



//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup);
vtkCxxRevisionMacro(vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup, "$Revision: 1.3 $");


//----------------------------------------------------------------------------
vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup()
{
  this->FEMeshListComboBox = NULL;
  this->ImageListComboBox = NULL;
  this->ElementSetComboBox = NULL;
  this->PoissonsRatioEntry = NULL;
  /*
  this->ViewFrame = NULL;
  this->ViewPropertyButton = NULL;
  this->ViewLegendButton = NULL;
  this->ClippingPlaneMenuButton = NULL;
  */
  this->ComponentFrame = NULL;
  this->DefineElSetButton = NULL;
  this->DefineElementSetDialog = NULL;
  
  this->BinCheckButton = NULL;
  this->BinFrame = NULL;
  this->SpecifyRangeButton = NULL;
  this->MinimumBinEntry = NULL;
  this->MaximumBinEntry = NULL;
  this->NumOfBinsEntry = NULL;
  this->ImageConstantFrame = NULL;
  this->ImageConstantA = NULL;
  this->ImageConstantB = NULL;
  this->ImageConstantC = NULL;
  this->ConversionDialog = NULL;
  this->EquationLabel = NULL;
  this->DialogApplyButton = NULL;
  this->DialogCancelButton = NULL;
  this->DefineConversionButton = NULL;
  this->ImageConversionType = NULL;
  strcpy(this->elementSetPrevious, "");
  this->ConversionA = 0.0;
  this->ConversionB = 2875.0;
  this->ConversionC = 3.0;
  strcpy(this->ConversionType, "Average");
}

//----------------------------------------------------------------------------
vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::~vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup()
{
  if(this->FEMeshListComboBox)
     this->FEMeshListComboBox->Delete();
  if(this->ImageListComboBox)
    this->ImageListComboBox->Delete();
  if(this->ElementSetComboBox)
	  this->ElementSetComboBox->Delete();
  if(this->PoissonsRatioEntry)
	  this->PoissonsRatioEntry->Delete();	  
	/*
	if(this->ViewFrame)
	  this->ViewFrame->Delete();  
	if(this->ViewPropertyButton)
	  this->ViewPropertyButton->Delete();
  if(this->ViewLegendButton)
	  this->ViewLegendButton->Delete();
  if(this->ClippingPlaneMenuButton)
	  this->ClippingPlaneMenuButton->Delete();
	*/
	if (this->ComponentFrame)
	  this->ComponentFrame->Delete();
	if (this->DefineElSetButton)
	  this->DefineElSetButton->Delete();
	if (this->DefineElementSetDialog)
	  this->DefineElementSetDialog->Delete();
	
	if (this->BinCheckButton)
	  this->BinCheckButton->Delete();
	if (this->BinFrame)
	  this->BinFrame->Delete();
	if (this->SpecifyRangeButton)
	  this->SpecifyRangeButton->Delete();
	if (this->MinimumBinEntry)
	  this->MinimumBinEntry->Delete();
	if (this->MaximumBinEntry)
	  this->MaximumBinEntry->Delete();
	if (this->NumOfBinsEntry)
	  this->NumOfBinsEntry->Delete();
	if(this->ImageConstantA)
		this->ImageConstantA->Delete();
	if(this->ImageConstantB)
		this->ImageConstantB->Delete();
	if(this->ImageConstantC)
		this->ImageConstantC->Delete();
	if(this->ImageConstantFrame)
		this->ImageConstantFrame->Delete();
  if(this->ConversionDialog)
		this->ConversionDialog->Delete();
  if(this->EquationLabel)
		this->EquationLabel->Delete();
	if(this->DialogApplyButton)
		this->DialogApplyButton->Delete();
	if(this->DialogCancelButton)
		this->DialogCancelButton->Delete();
	if(this->DefineConversionButton)
		this->DefineConversionButton->Delete();
	if(this->ImageConversionType)
		this->ImageConversionType->Delete();			
 }
//----------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::CreateWidget()
{
  if(this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();
  
 
  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Assign Material Properties (Image)");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 0 -fill x", 
    this->MainFrame->GetWidgetName());

  if ( !this->ComponentFrame )
    this->ComponentFrame = vtkKWFrameWithLabel::New();
  this->ComponentFrame->SetParent( this->MainFrame );
  this->ComponentFrame->Create();
  this->ComponentFrame->SetLabelText("Mesh & Image Assignments");
  this->ComponentFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ComponentFrame->GetWidgetName());
  
  if(!this->FEMeshListComboBox)	
     this->FEMeshListComboBox = vtkKWComboBoxWithLabel::New();
  FEMeshListComboBox->SetParent(this->ComponentFrame->GetFrame());
  FEMeshListComboBox->Create();
  this->FEMeshListComboBox->GetWidget()->SetCommand(
	  this, "FEMeshSelectionChangedCallback");
  FEMeshListComboBox->SetLabelText("Mesh : ");
  FEMeshListComboBox->SetLabelWidth( 15 );
  FEMeshListComboBox->GetWidget()->ReadOnlyOn();
  FEMeshListComboBox->GetWidget()->SetBalloonHelpString("Mesh for element set selection");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    FEMeshListComboBox->GetWidgetName());

  if(!this->ImageListComboBox)	
    this->ImageListComboBox = vtkKWComboBoxWithLabel::New();
  ImageListComboBox->SetParent(this->ComponentFrame->GetFrame());
  ImageListComboBox->Create();
  ImageListComboBox->SetLabelText("Image : ");
  ImageListComboBox->SetLabelWidth(15);
  ImageListComboBox->GetWidget()->ReadOnlyOn();
  ImageListComboBox->GetWidget()->SetBalloonHelpString("Image to be used for defining material properties");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ImageListComboBox->GetWidgetName());

  vtkKWIcon *defineElSetIcon = vtkKWIcon::New();
  defineElSetIcon->SetImage(    image_mimxElementSetDfns, 
                                image_mimxElementSetDfns_width, 
                                image_mimxElementSetDfns_height, 
                                image_mimxElementSetDfns_pixel_size); 
                          
  if ( !this->DefineElSetButton )
    this->DefineElSetButton = vtkKWPushButton::New();
  this->DefineElSetButton->SetParent(this->MainFrame);
  this->DefineElSetButton->Create();
  this->DefineElSetButton->SetCommand(this, "DefineElementSetCallback");
  this->DefineElSetButton->SetImageToIcon( defineElSetIcon );
  this->DefineElSetButton->SetReliefToFlat( );
  this->DefineElSetButton->SetBalloonHelpString("Define image conversion parameters");
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -padx 2 -pady 6", 
        this->DefineElSetButton->GetWidgetName());
        
  
  if (!this->ElementSetComboBox)	
	  this->ElementSetComboBox = vtkKWComboBoxWithLabel::New();
  ElementSetComboBox->SetParent(this->MainFrame);
  ElementSetComboBox->Create();
  ElementSetComboBox->GetWidget()->SetCommand(this, "ElementSetChangedCallback");
  ElementSetComboBox->SetLabelText("Element Set : ");
  ElementSetComboBox->SetLabelWidth(15);
  ElementSetComboBox->GetWidget()->ReadOnlyOn();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  ElementSetComboBox->GetWidgetName());

  // Poisson's ratio
  if (!this->PoissonsRatioEntry)
	  this->PoissonsRatioEntry = vtkKWEntryWithLabel::New();
  this->PoissonsRatioEntry->SetParent(this->MainFrame);
  this->PoissonsRatioEntry->Create();
  //this->PoissonsRatioEntry->SetWidth(4);
  this->PoissonsRatioEntry->SetLabelText("Poisson's Ratio : ");
  this->PoissonsRatioEntry->SetLabelWidth(15);
  this->PoissonsRatioEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->PoissonsRatioEntry->GetWidgetName());

  //
  vtkKWIcon *equationIcon = vtkKWIcon::New();
  equationIcon->SetImage(   image_mimxButtonEquation, 
                            image_mimxButtonEquation_width, 
                            image_mimxButtonEquation_height, 
                            image_mimxButtonEquation_pixel_size); 
                                  
  if ( !this->DefineConversionButton )
    this->DefineConversionButton = vtkKWPushButton::New();
  this->DefineConversionButton->SetParent(this->MainFrame);
  this->DefineConversionButton->Create();
  this->DefineConversionButton->SetCommand(this, "DefineConversionCallback");
  this->DefineConversionButton->SetImageToIcon( equationIcon );
  //this->DefineConversionButton->SetText( "E = a + bp^c" );
  this->DefineConversionButton->SetReliefToFlat( );
  this->DefineConversionButton->SetBalloonHelpString("Image intensity to elastic modulus conversion parameters");
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -padx 2 -pady 6", 
        this->DefineConversionButton->GetWidgetName());
          
  
  vtkKWIcon *binIcon = vtkKWIcon::New();
  binIcon->SetImage(    image_mimxBin, 
                        image_mimxBin_width, 
                        image_mimxBin_height, 
                        image_mimxBin_pixel_size); 
                                
  if ( !this->BinCheckButton )
    this->BinCheckButton = vtkKWCheckButton::New();
  this->BinCheckButton->SetParent( this->MainFrame );
  this->BinCheckButton->Create();
  this->BinCheckButton->SetBorderWidth( 2 );
  this->BinCheckButton->SetImageToIcon( binIcon );
  this->BinCheckButton->SetSelectImageToIcon( binIcon );
  this->BinCheckButton->SetCommand(this, "BinCheckButtonCallback");
  this->BinCheckButton->SetBalloonHelpString("Activate/deactivate material property binning");
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 2 -pady 6", 
   this->BinCheckButton->GetWidgetName());
  
  if ( !this->BinFrame )
    this->BinFrame = vtkKWFrameWithLabel::New();
  this->BinFrame->SetParent( this->MainFrame );
  this->BinFrame->Create();
  this->BinFrame->SetLabelText("Bin Options");
  this->BinFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->BinFrame->GetWidgetName());

  if (!this->NumOfBinsEntry)
	  this->NumOfBinsEntry = vtkKWEntryWithLabel::New();
  this->NumOfBinsEntry->SetParent(this->BinFrame->GetFrame());
  this->NumOfBinsEntry->Create();
  this->NumOfBinsEntry->SetLabelText("Number of Bins : ");
  this->NumOfBinsEntry->SetLabelWidth(20);
  this->NumOfBinsEntry->GetWidget()->SetValueAsInt(16);
  this->NumOfBinsEntry->GetWidget()->SetRestrictValueToInteger();
  this->NumOfBinsEntry->SetEnabled(0);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->NumOfBinsEntry->GetWidgetName());
	  
  if ( !this->SpecifyRangeButton )
    this->SpecifyRangeButton = vtkKWCheckButtonWithLabel::New();
  this->SpecifyRangeButton->SetParent( this->BinFrame->GetFrame() );
  this->SpecifyRangeButton->Create();
  this->SpecifyRangeButton->SetLabelText("Specify Range ");
  this->SpecifyRangeButton->SetLabelWidth(20);
  this->SpecifyRangeButton->SetLabelPositionToRight();
  this->SpecifyRangeButton->GetWidget()->SetCommand(this, "SpecifyRangeButtonCallback");
  this->SpecifyRangeButton->SetEnabled(0);
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 2 -pady 6", 
   this->SpecifyRangeButton->GetWidgetName());

  if (!this->MinimumBinEntry)
	  this->MinimumBinEntry = vtkKWEntryWithLabel::New();
  this->MinimumBinEntry->SetParent(this->BinFrame->GetFrame());
  this->MinimumBinEntry->Create();
  this->MinimumBinEntry->SetLabelText("Minimum E : ");
  this->MinimumBinEntry->SetLabelWidth(20);
  this->MinimumBinEntry->GetWidget()->SetValueAsDouble(0.0);
  this->MinimumBinEntry->GetWidget()->SetRestrictValueToDouble();
  this->MinimumBinEntry->SetEnabled(0);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->MinimumBinEntry->GetWidgetName());

  if (!this->MaximumBinEntry)
	  this->MaximumBinEntry = vtkKWEntryWithLabel::New();
  this->MaximumBinEntry->SetParent(this->BinFrame->GetFrame());
  this->MaximumBinEntry->Create();
  this->MaximumBinEntry->SetLabelText("Maximum E : ");
  this->MaximumBinEntry->SetLabelWidth(20);
  this->MaximumBinEntry->GetWidget()->SetValueAsDouble(0.0);
  this->MaximumBinEntry->GetWidget()->SetRestrictValueToDouble();
  this->MaximumBinEntry->SetEnabled(0);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->MaximumBinEntry->GetWidgetName());

#if 0
  if (!this->ViewFrame)
    this->ViewFrame = vtkKWFrameWithLabel::New();
  this->ViewFrame->SetParent( this->MainFrame->GetFrame() );
  this->ViewFrame->Create();
  this->ViewFrame->SetLabelText("View");
  this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
              this->ViewFrame->GetWidgetName() );    
  this->ViewFrame->CollapseFrame();
  
  if (!this->ViewPropertyButton)
    this->ViewPropertyButton = vtkKWCheckButtonWithLabel::New();
  this->ViewPropertyButton->SetParent(this->ViewFrame->GetFrame());
  this->ViewPropertyButton->Create();
  this->ViewPropertyButton->GetWidget()->SetCommand(this, "ViewMaterialPropertyCallback");
  this->ViewPropertyButton->GetWidget()->SetText("View Properties");
//  this->ViewPropertyButton->GetWidget()->SetEnabled( 0 );
  this->GetApplication()->Script(
        "grid %s -row 0 -column 0 -sticky ne -padx 2 -pady 2", 
        this->ViewPropertyButton->GetWidgetName());

  if (!this->ViewLegendButton)
    this->ViewLegendButton = vtkKWCheckButtonWithLabel::New();
  this->ViewLegendButton->SetParent(this->ViewFrame->GetFrame());
  this->ViewLegendButton->Create();
  this->ViewLegendButton->GetWidget()->SetCommand(this, "ViewPropertyLegendCallback");
  this->ViewLegendButton->GetWidget()->SetText("View Legend");
 // this->ViewLegendButton->GetWidget()->SetEnabled( 0 );
  this->GetApplication()->Script(
        "grid %s -row 0 -column 1 -sticky ne -padx 2 -pady 2", 
        this->ViewLegendButton->GetWidgetName());
  
  if(!this->ClippingPlaneMenuButton)	
		this->ClippingPlaneMenuButton = vtkKWMenuButtonWithLabel::New();
	this->ClippingPlaneMenuButton->SetParent(this->ViewFrame->GetFrame());
	this->ClippingPlaneMenuButton->Create();
	this->ClippingPlaneMenuButton->SetBorderWidth(0);
	this->ClippingPlaneMenuButton->SetReliefToGroove();
//	this->ClippingPlaneMenuButton->GetWidget()->SetEnabled( 0 );
	this->ClippingPlaneMenuButton->SetLabelText("Clipping Plane :");
	this->GetApplication()->Script(
	  "grid %s -row 1 -column 0 -sticky ne -padx 2 -pady 2", 
		this->ClippingPlaneMenuButton->GetWidgetName());
	this->ClippingPlaneMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Off",this, "ClippingPlaneCallback 1");
	this->ClippingPlaneMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"On",this, "ClippingPlaneCallback 2");
  this->ClippingPlaneMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Invert",this, "ClippingPlaneCallback 3");
	this->ClippingPlaneMenuButton->GetWidget()->SetValue("Off");
#endif	
	
  this->ApplyButton->SetParent( this->MainFrame );
  this->ApplyButton->Create();
  this->ApplyButton->SetReliefToFlat();
  this->ApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
  //this->ApplyButton->SetText("Apply");
  this->ApplyButton->SetCommand(this, "ApplyFEMeshMaterialPropertiesFromImageApplyCallback");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand y -padx 5 -pady 2", 
	  this->ApplyButton->GetWidgetName());

  this->CancelButton->SetParent( this->MainFrame );
  this->CancelButton->Create();
  this->CancelButton->SetReliefToFlat();
  this->CancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
  //this->CancelButton->SetText("Cancel");
  this->CancelButton->SetCommand(this, "ApplyFEMeshMaterialPropertiesFromImageCancelCallback");
  this->GetApplication()->Script(
    "pack %s -side right -anchor ne -expand y -padx 5 -pady 2", 
    this->CancelButton->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::
	ApplyFEMeshMaterialPropertiesFromImageApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

  if(!strcmp(this->FEMeshListComboBox->GetWidget()->GetValue(),""))
  {
	callback->ErrorMessage("Please select a mesh");
	return 0;
  }
    if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
  {
	  callback->ErrorMessage("Please load/select an image");
	  return 0;
	}
    vtkKWComboBox *combobox = this->FEMeshListComboBox->GetWidget();
    const char *name = combobox->GetValue();
  strcpy(this->meshName, name);
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Please choose a valid mesh");
		combobox->SetValue("");
		return 0;
	}

	const char *elementsetname = this->ElementSetComboBox->GetWidget()->GetValue();
	if(!strcmp(elementsetname,""))
	{
		callback->ErrorMessage("Please choose a valid element set");
		return 0;
	}
	strcpy(this->elementSetName, elementsetname);

	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex(name)));

	vtkUnstructuredGrid *ugrid = vtkMimxMeshActor::SafeDownCast(this->FEMeshList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	typedef itk::Image<signed short, 3>  ImageType;
	ImageType::Pointer itkimage;

	combobox = this->ImageListComboBox->GetWidget();
	name = combobox->GetValue();

	num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Please choose a valid image");
		combobox->SetValue("");
		return 0;
	}

	float poissonsratio = this->PoissonsRatioEntry->GetWidget()->GetValueAsDouble();
	if(poissonsratio < -1.0 || poissonsratio > 0.5)
	{
		callback->ErrorMessage("Poisson's ratio should be beetween -1.0 and 0.5");
		return 0;
	}
  vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
	itkimage = imageActor->GetITKImage();

	//
	char young[256];
	strcpy(young, elementsetname);
	strcat(young, "_Constant_Youngs_Modulus");

	vtkDoubleArray *youngarray = vtkDoubleArray::SafeDownCast(
		ugrid->GetFieldData()->GetArray(young));

	if (youngarray)
	{
		vtkKWMessageDialog *Dialog = vtkKWMessageDialog::New();
		Dialog->SetStyleToYesNo();
		Dialog->SetApplication(this->GetApplication());
		Dialog->Create();
		Dialog->SetTitle("Your Attention Please!");
		Dialog->SetText("A user-defined material property has already been assigned to this element set. Would you like to overwrite?");
		Dialog->Invoke();
		if(Dialog->GetStatus() == vtkKWMessageDialog::StatusCanceled)
		{
			Dialog->Delete();
			return 1;
		}
		Dialog->Delete();
		ugrid->GetFieldData()->RemoveArray( young );
	}
	
	std::string checkPropName;
  checkPropName = elementsetname;
  checkPropName += "_Image_Based_Material_Property";
  
  youngarray = vtkDoubleArray::SafeDownCast( ugrid->GetCellData()->GetArray(checkPropName.c_str()) );
	if ( youngarray )
	{
		vtkKWMessageDialog *Dialog = vtkKWMessageDialog::New();
		Dialog->SetStyleToYesNo();
		Dialog->SetApplication(this->GetApplication());
		Dialog->Create();
		Dialog->SetTitle("Your Attention Please!");
		Dialog->SetText("An image-based material property has already been assigned to this element set. Would you like to overwrite?");
		Dialog->Invoke();
		if(Dialog->GetStatus() == vtkKWMessageDialog::StatusCanceled)
		{
			Dialog->Delete();
			return 1;
		}
		Dialog->Delete();
		ugrid->GetCellData()->RemoveArray( checkPropName.c_str() );
	}
	
  /* 
   * Assign Material Properties to a Mesh based on an image.
   * This is done in the Image IJK Space - Requires that we transform
   * the Mesh (RAS coordinates) to IJK space. The image is in LPS coordinates
   * and we simply remove the direction cosines, origin, spacing for 
   * the assignment. The mesh and image are transformed back after the material 
   * property assignment.
   */
  
  /* Make the Image Physical space and IJK space Correspond */
  double cosineArray[9];
	ImageType::PointType origin = itkimage->GetOrigin();
	ImageType::SpacingType spacing = itkimage->GetSpacing();
  ImageType::DirectionType direction = itkimage->GetDirection();
	
	ImageType::SpacingType tmpSpacing;
	tmpSpacing[0] = tmpSpacing[1] = tmpSpacing[2] = 1.0;
	ImageType::PointType tmpOrigin;
	tmpOrigin[0] = tmpOrigin[1] = tmpOrigin[2] = 0.0;
  ImageType::DirectionType tmpDirection;
  tmpDirection.Fill(0);
  tmpDirection[0][0] = 1.0; tmpDirection[1][1] = 1.0; tmpDirection[2][2] = 1.0;
	itkimage->SetSpacing( tmpSpacing );
	itkimage->SetOrigin( tmpOrigin );
  itkimage->SetDirection( tmpDirection );
  
  /* Transform the mesh from RAS to Image IJK space */
  vtkTransform *RasToIjkTransform = imageActor->GetRASToIJKTransform();
	vtkTransformFilter *transformFilter = vtkTransformFilter::New();
  transformFilter->SetInput( ugrid );
  transformFilter->SetTransform( RasToIjkTransform );
  transformFilter->Update( );
  
	vtkMimxApplyImageBasedMaterialProperties *applymatprops = 
		vtkMimxApplyImageBasedMaterialProperties::New();
	//applymatprops->SetInput(ugrid);
  applymatprops->SetInput( transformFilter->GetUnstructuredGridOutput() );
	applymatprops->SetITKImage(itkimage);
	callback->SetState(0);
	applymatprops->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
	applymatprops->SetElementSetName( this->ElementSetComboBox->GetWidget()->GetValue() );
	applymatprops->SetConstantA( this->ConversionA );
  applymatprops->SetConstantB( this->ConversionB );
	applymatprops->SetConstantC( this->ConversionC );
	if (strcmp(this->ConversionType, "Average") == 0)
	{
  	applymatprops->SetIntensityCalculationMode(vtkMimxApplyImageBasedMaterialProperties::AVERAGE);
	}
	else if (strcmp(this->ConversionType, "Median") == 0)
	{
  	applymatprops->SetIntensityCalculationMode(vtkMimxApplyImageBasedMaterialProperties::MEDIAN);
	}
	else
	{
  	applymatprops->SetIntensityCalculationMode(vtkMimxApplyImageBasedMaterialProperties::MAXIMUM);
	}

	applymatprops->Update();
  
  /* Reset the Image Information */
	imageActor->GetITKImage()->SetSpacing( spacing );
	imageActor->GetITKImage()->SetOrigin( origin );
  imageActor->GetITKImage()->SetDirection( direction );
  
  
	if (!callback->GetState())
	{
		ugrid->GetFieldData()->RemoveArray(young);
    youngarray = vtkDoubleArray::SafeDownCast( applymatprops->GetOutput()->GetCellData()->GetArray(checkPropName.c_str()) );
    ugrid->GetCellData()->AddArray( youngarray );
  
		meshActor->StoreImageBasedMaterialProperty( this->ElementSetComboBox->GetWidget()->GetValue() );
		meshActor->StoreConstantPoissonsRatio( this->ElementSetComboBox->GetWidget()->GetValue(), poissonsratio);
	}
	applymatprops->RemoveObserver(callback);
	applymatprops->Delete();

	if(!this->BinCheckButton->GetSelectedState())	return 1;
	int numberofbins = this->NumOfBinsEntry->GetWidget()->GetValueAsInt();

	if(numberofbins <= 1)
	{
		callback->ErrorMessage("Number of Bins Should always be > 1. If the number of bins = 1 then apply material property as constant" );
		return 0;
	}

	const char *elsetname = this->ElementSetComboBox->GetWidget()->GetValue();

	if(!strcmp(elsetname,""))
	{
		callback->ErrorMessage("Choose valid Element Set");
		return 0;
	}
	strcpy(this->elementSetName, elsetname);

	char str[256];
	strcpy(str, elsetname);
	strcat(str, "_Image_Based_Material_Property");

	vtkDoubleArray *matarray = vtkDoubleArray::SafeDownCast(
		ugrid->GetCellData()->GetArray(str));
	if(!matarray)
	{
		callback->ErrorMessage("Image based material properties do not exist for binning. Calculate material properties first");
		return 0;
	}

	float min = this->MinimumBinEntry->GetWidget()->GetValueAsDouble();
	float max = this->MaximumBinEntry->GetWidget()->GetValueAsDouble();
	int minmaxstate = this->SpecifyRangeButton->GetWidget()->GetSelectedState();
	if(minmaxstate)
	{
		if(min >= max)
		{
			callback->ErrorMessage("Invalid minimum and maximum values");
			return 0;		
		}
	}
	strcat(str, "_ReBin");
	matarray = vtkDoubleArray::SafeDownCast(ugrid->GetCellData()->GetArray(str));
	if(matarray)
	{
		vtkKWMessageDialog *Dialog = vtkKWMessageDialog::New();
		Dialog->SetStyleToYesNo();
		Dialog->SetApplication(this->GetApplication());
		Dialog->Create();
		Dialog->SetTitle("Your Attention Please!");
		Dialog->SetText("A user-defined material property has already been assigned to this element set. Would you like to overwrite this data?");
		Dialog->Invoke();
		if(Dialog->GetStatus() == vtkKWMessageDialog::StatusCanceled)
		{
			Dialog->Delete();
			return 1;
		}
		Dialog->Delete();
	}
	//
	vtkMimxRebinMaterialProperty *rebinmatprops = 
		vtkMimxRebinMaterialProperty::New();
	rebinmatprops->SetInput(ugrid);
	rebinmatprops->SetElementSetName(elsetname);
	rebinmatprops->SetNumberOfHistogramBins(numberofbins);
	callback->SetState(0);
	rebinmatprops->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
	if(minmaxstate)
	{
		rebinmatprops->SetComputeMinBin(0);
		rebinmatprops->SetBinLowerBound(min);
		rebinmatprops->SetComputeMaxBin(0);
		rebinmatprops->SetBinUpperBound(max);
	}

	rebinmatprops->Update();
	if (!callback->GetState())
	{
		ugrid->Initialize();
		ugrid->DeepCopy(rebinmatprops->GetOutput());
		meshActor->StoreImageBasedMaterialPropertyReBin(elsetname);
	}
	else
	{
		rebinmatprops->RemoveObserver(callback);
		rebinmatprops->Delete();
		return 0;
	}

  this->BinCheckButton->SetEnabled( 0 );
  this->DefineConversionButton->SetEnabled( 0 );
  this->PoissonsRatioEntry->SetEnabled( 0 );
  this->BinFrame->SetEnabled( 0 );
  this->ApplyButton->SetEnabled( 0 );

  this->GetMimxMainWindow()->SetStatusText("Assigned Material Properties");
  
	return 0;
}
//----------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::ApplyFEMeshMaterialPropertiesFromImageCancelCallback()
{
	this->CancelStatus = 1;
	this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName());
	this->MenuGroup->SetMenuButtonsEnabled(1);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
	  this->CancelStatus = 0;
}
//------------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::UpdateObjectLists()
{
  this->FEMeshListComboBox->GetWidget()->DeleteAllValues();
  strcpy(this->objectSelectionPrevious,"");
  strcpy(this->elementSetPrevious, "");
  int defaultItem = -1;
  for (int i = 0; i < this->FEMeshList->GetNumberOfItems(); i++)
  {
	  FEMeshListComboBox->GetWidget()->AddValue(
		  this->FEMeshList->GetItem(i)->GetFileName());
		  
		vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->FEMeshList->GetItem(i));  
    bool viewedItem = meshActor->GetMeshVisibility();
    
    if ( (defaultItem == -1) && ( viewedItem ) )
		{
		  defaultItem = i;
		}
  }
  if ((defaultItem == -1) && (this->FEMeshList->GetNumberOfItems() > 0))
    defaultItem = this->FEMeshList->GetNumberOfItems()-1;
    
  if (defaultItem != -1)
  {
    FEMeshListComboBox->GetWidget()->SetValue(
          this->FEMeshList->GetItem(defaultItem)->GetFileName());
    /* Insert the Element Set Name */
  }
  
  this->ImageListComboBox->GetWidget()->DeleteAllValues();
  
  defaultItem = -1;
  for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
  {
	  ImageListComboBox->GetWidget()->AddValue(
		  this->ImageList->GetItem(i)->GetFileName());
		int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
			this->ImageList->GetItem(i)->GetActor());
    if ( (defaultItem == -1) && ( viewedItem ) )
		{
		  defaultItem = i;
		}
  }
  if ((defaultItem == -1) && (this->ImageList->GetNumberOfItems() > 0))
    defaultItem = this->ImageList->GetNumberOfItems() - 1;
    
  if (defaultItem != -1)
  {
    ImageListComboBox->GetWidget()->SetValue(
          this->ImageList->GetItem(defaultItem)->GetFileName());
  }
  this->FEMeshSelectionChangedCallback(this->FEMeshListComboBox->GetWidget()->GetValue());
}
//------------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::
	ApplyFEMeshMaterialPropertiesFromImageDoneCallback()
{
	if(this->ApplyFEMeshMaterialPropertiesFromImageApplyCallback())
		this->ApplyFEMeshMaterialPropertiesFromImageCancelCallback();
}
//---------------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::
	FEMeshSelectionChangedCallback(const char *Selection)
{
	if(this->CancelStatus)	return;
	if(!this->FEMeshList->GetNumberOfItems())
	{
		this->DefineElSetButton->SetStateToDisabled();
		return;
	}
	if(!strcmp(this->objectSelectionPrevious, Selection))	return;
	
	strcpy(this->objectSelectionPrevious, Selection); 

	vtkUnstructuredGrid *ugrid = vtkMimxMeshActor::SafeDownCast(this->FEMeshList->
		GetItem(FEMeshListComboBox->GetWidget()->GetValueIndex(Selection)))->GetDataSet();

	this->ElementSetComboBox->GetWidget()->DeleteAllValues();
	int i;
	vtkStringArray *strarray = vtkStringArray::SafeDownCast(
		ugrid->GetFieldData()->GetAbstractArray("Element_Set_Names"));

	int numarrrays = strarray->GetNumberOfValues();

	for (i=0; i<numarrrays; i++)
	{
		this->ElementSetComboBox->GetWidget()->AddValue(
			strarray->GetValue(i));
	}
	this->ElementSetComboBox->GetWidget()->SetValue( strarray->GetValue(0) );
	this->ElementSetChangedCallback(strarray->GetValue(0));
}
//-------------------------------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::
	ElementSetChangedCallback(const char *Selection)
{
	if(this->CancelStatus)	return;
	if(!strcmp(Selection,""))
	{
		return;
	}

	if(!strcmp(this->FEMeshListComboBox->GetWidget()->GetValue(),""))
	{
		return;
	}
	if(!strcmp(this->elementSetPrevious, Selection))	return;
	strcpy(this->elementSetPrevious, Selection);
	this->DefineElSetButton->SetStateToNormal();
	char poisson[256];
	strcpy(poisson, Selection);
	strcat(poisson, "_Constant_Poissons_Ratio");

	vtkKWComboBox *combobox = this->FEMeshListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		return;
	}

	vtkUnstructuredGrid *ugrid = vtkMimxMeshActor::SafeDownCast(this->FEMeshList
		->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	vtkFloatArray *Nuarray = vtkFloatArray::SafeDownCast(
		ugrid->GetFieldData()->GetArray(poisson));	

	float poissonsratio;
	if (Nuarray)
	{
		poissonsratio = Nuarray->GetValue(0);
		this->PoissonsRatioEntry->GetWidget()->SetValueAsDouble(poissonsratio);
	}
	else
	{
		this->PoissonsRatioEntry->GetWidget()->SetValue("");
	}
	
	this->BinCheckButton->SetEnabled( 1 );
  this->DefineConversionButton->SetEnabled( 1 );
  this->PoissonsRatioEntry->SetEnabled( 1 );
  this->BinFrame->SetEnabled( 1 );
  this->ApplyButton->SetEnabled( 1 );
}
#if 0
//-------------------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::ViewMaterialPropertyCallback( int mode )
{
  vtkKWComboBox *combobox = this->FEMeshListComboBox->GetWidget();
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
           this->FEMeshList->GetItem(combobox->GetValueIndex( 
		   this->FEMeshListComboBox->GetWidget()->GetValue())));         
  if ( mode )
  {
   // char scalarName[256];
   // strcpy(scalarName, this->ElementSetComboBox->GetWidget()->GetValue());
	  //strcat(scalarName, "_Image_Based_Material_Property");
	  //meshActor->SetMeshScalarName( scalarName );
	  //meshActor->SetMeshScalarVisibility(true);
    meshActor->SetElementSetScalarName(this->ElementSetComboBox->GetWidget()->GetValue(), "Youngs_Modulus");
    meshActor->SetElementSetScalarVisibility(this->ElementSetComboBox->GetWidget()->GetValue(), true);
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }
  else
  {
	  //meshActor->SetMeshScalarVisibility(false);
    meshActor->SetElementSetScalarVisibility(this->ElementSetComboBox->GetWidget()->GetValue(), false);
    this->ViewPropertyLegendCallback(0);
    this->ViewLegendButton->GetWidget()->SetSelectedState( 0 );
    this->GetMimxMainWindow()->GetRenderWidget()->Render(); 
  }
}

//-------------------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::ViewPropertyLegendCallback( int mode )
{
  vtkKWComboBox *combobox = this->FEMeshListComboBox->GetWidget();
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
           this->FEMeshList->GetItem(combobox->GetValueIndex( combobox->GetValue() )));
  const char *elsetName = this->ElementSetComboBox->GetWidget()->GetValue();
          
  if ( mode )
  {
    meshActor->SetElementSetLegendVisibility(elsetName, true);
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }
  else
  {
    meshActor->SetElementSetLegendVisibility(elsetName, false);
    this->GetMimxMainWindow()->GetRenderWidget()->Render(); 
  }
}

//-------------------------------------------------------------------------------------
int vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::ClippingPlaneCallback( int mode )
{
	vtkKWComboBox *combobox = this->FEMeshListComboBox->GetWidget();
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex( combobox->GetValue())));
	const char *elsetName = this->ElementSetComboBox->GetWidget()->GetValue();
	if(!strcmp(elsetName,""))	return 0;
	if (mode == 1)
	{
		meshActor->DisableElementSetCuttingPlane(elsetName);
	}
	else if (mode == 2)
	{
		meshActor->EnableElementSetCuttingPlane(elsetName);
		meshActor->SetInvertCuttingPlane( false );
	}
	else
	{
		meshActor->EnableElementSetCuttingPlane(elsetName);
		meshActor->SetInvertCuttingPlane( true );  
	}
	this->GetMimxMainWindow()->GetRenderWidget()->Render(); 
	return 1;
}
#endif
//----------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::DefineElementSetCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	
	if(!strcmp(this->FEMeshListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Mesh must be selection");
		return;
	}

  vtkKWComboBox *combobox = this->FEMeshListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Invalid Mesh was selected");
		combobox->SetValue("");
		return;
	}
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex(name)));
		
	if (!this->DefineElementSetDialog)
	{
	  this->DefineElementSetDialog = vtkKWMimxDefineElSetGroup::New();
	  this->DefineElementSetDialog->SetApplication( this->GetApplication() );
	  this->DefineElementSetDialog->SetMimxMainWindow( this->GetMimxMainWindow() );
	  this->DefineElementSetDialog->Create();
	}
	this->DefineElementSetDialog->SetMeshActor( meshActor );
	this->DefineElementSetDialog->SetElementSetCombobox(this->ElementSetComboBox->GetWidget());
	this->DefineElementSetDialog->Display();
}
//----------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::BinCheckButtonCallback(int mode)
{
  if ( mode )
  {
    this->SpecifyRangeButton->SetEnabled(1);
    this->NumOfBinsEntry->SetEnabled(1);
    if ( this->SpecifyRangeButton->GetWidget()->GetSelectedState() )
    {
      this->MinimumBinEntry->SetEnabled(1);
      this->MaximumBinEntry->SetEnabled(1);
    }
  }
  else
  {
    this->SpecifyRangeButton->SetEnabled(0);
    this->NumOfBinsEntry->SetEnabled(0);
    this->MinimumBinEntry->SetEnabled(0);
    this->MaximumBinEntry->SetEnabled(0);
  }
}
//----------------------------------------------------------------------------
void vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::SpecifyRangeButtonCallback(int mode)
{
  if ( mode )
  {
    this->MinimumBinEntry->SetEnabled(1);
    this->MaximumBinEntry->SetEnabled(1);
  }
  else
  {
    this->MinimumBinEntry->SetEnabled(0);
    this->MaximumBinEntry->SetEnabled(0);
  }
}
//----------------------------------------------------------------------------
int vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::DefineConversionCallback()
{
  if (!this->ConversionDialog)
  {
    this->ConversionDialog = vtkKWTopLevel::New();
    this->ConversionDialog->SetApplication( this->GetApplication() );
    this->ConversionDialog->Create();
    this->ConversionDialog->SetResizable(1, 1);
    this->ConversionDialog->SetSize(275, 200);
    this->ConversionDialog->SetMinimumSize(275, 200);
    this->ConversionDialog->ModalOff( );
    this->ConversionDialog->SetMasterWindow(this->GetMimxMainWindow());
  }
  this->ConversionDialog->SetTitle( "Image-Based Elastic Modulus" );
  
    
  if (!this->EquationLabel)
  {
    vtkKWIcon *equationIcon = vtkKWIcon::New();
    equationIcon->SetImage(    image_mimxLabelEquation, 
                                  image_mimxLabelEquation_width, 
                                  image_mimxLabelEquation_height, 
                                  image_mimxLabelEquation_pixel_size); 
                                
    this->EquationLabel = vtkKWLabel::New();  
    this->EquationLabel->SetParent( this->ConversionDialog );
    this->EquationLabel->Create();
    this->EquationLabel->SetImageToIcon( equationIcon );
    //this->EquationLabel->SetText( "E = a + bp^c" );
    this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 2",
                this->EquationLabel->GetWidgetName() );    
  }
  
  if (!this->ImageConstantA)
  {
	  this->ImageConstantA = vtkKWEntryWithLabel::New();
    this->ImageConstantA->SetParent( this->ConversionDialog );
    this->ImageConstantA->Create();
    this->ImageConstantA->SetLabelText("a : ");
    this->ImageConstantA->SetLabelWidth(4);
    this->ImageConstantA->GetWidget()->SetRestrictValueToDouble();
    this->GetApplication()->Script(
	    "pack %s -side top -anchor nw -padx 2 -pady 6 -fill x", 
	    this->ImageConstantA->GetWidgetName());
  }
  this->ImageConstantA->GetWidget()->SetValueAsDouble( this->ConversionA );
  
  if (!this->ImageConstantB)
  {
	  this->ImageConstantB = vtkKWEntryWithLabel::New();
  this->ImageConstantB->SetParent( this->ConversionDialog );
  this->ImageConstantB->Create();
  this->ImageConstantB->SetLabelText("b : ");
  this->ImageConstantB->SetLabelWidth(4);
  this->ImageConstantB->SetWidth(10);
  this->ImageConstantB->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -padx 2 -pady 6 -fill x", 
	  this->ImageConstantB->GetWidgetName());
  }
  this->ImageConstantB->GetWidget()->SetValueAsDouble( this->ConversionB );
  
  if (!this->ImageConstantC)
	{
	  this->ImageConstantC = vtkKWEntryWithLabel::New();
    this->ImageConstantC->SetParent( this->ConversionDialog );
    this->ImageConstantC->Create();
    this->ImageConstantC->SetLabelText("c : ");
    this->ImageConstantC->SetLabelWidth(4);
    this->ImageConstantC->SetWidth(10);
    this->ImageConstantC->GetWidget()->SetRestrictValueToDouble();
    this->GetApplication()->Script(
  	  "pack %s -side top -anchor nw -padx 2 -pady 6 -fill x", 
  	  this->ImageConstantC->GetWidgetName());
  }
  this->ImageConstantC->GetWidget()->SetValueAsDouble( this->ConversionC );
  
  if (!this->ImageConversionType)
	{
	  this->ImageConversionType = vtkKWMenuButtonWithLabel ::New();
    this->ImageConversionType->SetParent( this->ConversionDialog );
    this->ImageConversionType->Create();
    this->ImageConversionType->SetLabelText("Density Calculation : ");
    //this->ImageConversionType->SetLabelWidth(4);
    //this->ImageConversionType->SetWidth(10);
    //this->ImageConversionType->GetWidget()->SetRestrictValueToDouble();
    this->ImageConversionType->GetWidget()->GetMenu()->AddRadioButton( "Average" );
    this->ImageConversionType->GetWidget()->GetMenu()->AddRadioButton( "Median" );
    this->ImageConversionType->GetWidget()->GetMenu()->AddRadioButton( "Maximum" );
    this->GetApplication()->Script(
  	  "pack %s -side top -anchor nw -padx 2 -pady 6 -fill x", 
  	  this->ImageConversionType->GetWidgetName());
  }
  this->ImageConversionType->GetWidget()->SetValue( this->ConversionType );
  
  
  
   
  if (!this->DialogApplyButton)
  {
    this->DialogApplyButton = vtkKWPushButton::New();
    this->DialogApplyButton->SetParent( this->ConversionDialog );
    this->DialogApplyButton->Create();
    this->DialogApplyButton->SetReliefToFlat();
    this->DialogApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
    this->DialogApplyButton->SetCommand(this, "ConversionApplyCallback");
    this->GetApplication()->Script("pack %s -side left -anchor nw -padx 2 -pady 2",
                  this->DialogApplyButton->GetWidgetName() ); 
  }
  
  if (!this->DialogCancelButton)
  {
    this->DialogCancelButton = vtkKWPushButton::New();
    this->DialogCancelButton->SetParent( this->ConversionDialog );
    this->DialogCancelButton->Create();
    this->DialogCancelButton->SetReliefToFlat();
    this->DialogCancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
    this->DialogCancelButton->SetCommand(this, "ConversionCancelCallback");
    this->GetApplication()->Script("pack %s -side right -anchor ne -padx 2 -pady 2",
                  this->DialogCancelButton->GetWidgetName() ); 
  }
  
  this->ConversionDialog->Display();
  return 1;
}
//----------------------------------------------------------------------------
int vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::ConversionApplyCallback()
{
  std::string constA = this->ImageConstantA->GetWidget()->GetValue();
	std::string constB = this->ImageConstantB->GetWidget()->GetValue();
	std::string constC = this->ImageConstantC->GetWidget()->GetValue();

	if ( ( constA == "" ) || ( constB == "" ) || ( constC == "" ) )
	{
		vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
		callback->ErrorMessage("Image parameter entries cannot be empty");
		return 0;
	}
	
  this->ConversionA = this->ImageConstantA->GetWidget()->GetValueAsDouble( );
  this->ConversionB = this->ImageConstantB->GetWidget()->GetValueAsDouble( );
  this->ConversionC = this->ImageConstantC->GetWidget()->GetValueAsDouble( );
  strcpy(this->ConversionType, this->ImageConversionType->GetWidget()->GetValue() );
  this->ConversionDialog->Withdraw( );

  return 1;
}
//----------------------------------------------------------------------------
int vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::ConversionCancelCallback()
{
  this->ConversionDialog->Withdraw( );

  return 1;
}
//-------------------------------------------------------------------------------------
