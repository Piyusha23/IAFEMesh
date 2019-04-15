/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxTransformSurfaceGroup.cxx,v $
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

#include "vtkKWMimxTransformSurfaceGroup.h"

//VTK Headers
#include <vtkActor.h>
#include <vtkMimxBoxWidget.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkRenderer.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//KWWidget Headers
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWCheckButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWMatrixWidget.h>
#include <vtkKWMatrixWidgetWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWRenderWidget.h>

//MIMX Headers
#include "vtkMimxErrorCallback.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxTransformSurfaceGroup);
vtkCxxRevisionMacro(vtkKWMimxTransformSurfaceGroup, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
// Callback for the boxwidget used to transform surfaces.
class vtkBWCallback : public vtkCommand
{
public:
	static vtkBWCallback *New() 
	{ return new vtkBWCallback; }
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkMimxBoxWidget *boxWidget = reinterpret_cast<vtkMimxBoxWidget*>(caller);
		boxWidget->GetTransform(this->Transform);

		this->Actor->VisibilityOn();
		this->Actor->SetUserTransform(this->Transform);
		this->Actor->Modified();
		this->matrix_values_changed->MatrixValues_changed(0,0,NULL);				

	}

		vtkBWCallback():Transform(0),Actor(0) {}
		vtkTransform *Transform;
		vtkActor     *Actor;
		vtkKWMimxTransformSurfaceGroup *matrix_values_changed ;		
		vtkMimxSurfacePolyDataActor *surfpolyactor ;
		
};


vtkKWMimxTransformSurfaceGroup::vtkKWMimxTransformSurfaceGroup()
{
  this->ObjectListComboBox = NULL;
  this->FileBrowserDialog = NULL;
  this->SelectedSurfaceActor = vtkActor::New();
  this->SelectedSurfaceActor->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper = vtkPolyDataMapper::New();
  this->SelectedSurfaceMapper->ScalarVisibilityOff();
  this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
  this->boxWidget = vtkMimxBoxWidget::New();
  this->Transform = vtkTransform::New();
  this->TransformCallback = vtkBWCallback::New() ;
  this->TransformCallback->matrix_values_changed = this;
  this->CancelStatus = 0;
  this->matrix = vtkMatrix4x4::New();
  this->matrix_widget1 = vtkKWMatrixWidgetWithLabel::New();
  this->matrix_widget2 = vtkKWMatrixWidgetWithLabel::New();
  this->matrix_widget3 = vtkKWMatrixWidgetWithLabel::New();

//  previousvaltrans[0] = 0;
//  previousvaltrans[1] = 0;
//  previousvaltrans[2] = 0;

  strcpy(previousselection,"");  

  this->BWVisiCheckButton = NULL ;

  this->ResetTransButton = NULL ;

}
//----------------------------------------------------------------------------
vtkKWMimxTransformSurfaceGroup::~vtkKWMimxTransformSurfaceGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
  if (this->boxWidget)
	  this->boxWidget->Delete();
  if(this->Transform)
	  this->Transform->Delete();
  if(this->TransformCallback)
	  this->TransformCallback->Delete();
	  this->SelectedSurfaceActor->Delete();
	  this->SelectedSurfaceMapper->Delete();  
  if(this->matrix)
	  this->matrix->Delete();
  if(this->matrix_widget1)
	  this->matrix_widget1->Delete();
  if(this->matrix_widget2)
	  this->matrix_widget2->Delete();
  if(this->matrix_widget3)
	  this->matrix_widget3->Delete();   

  if(this->BWVisiCheckButton)
	  this->BWVisiCheckButton->Delete();

  if(this->ResetTransButton)
	  this->ResetTransButton->Delete();
}
//----------------------------------------------------------------------------

void vtkKWMimxTransformSurfaceGroup::CreateWidget()
{
  if(this->IsCreated())
  {
  vtkErrorMacro("class already created");
  return;
  }

  this->Superclass::CreateWidget();
  

  if(!this->ObjectListComboBox)
    this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();  
  this->ObjectListComboBox->SetParent(this->MainFrame);
  this->ObjectListComboBox->Create();
  this->ObjectListComboBox->SetWidth(20);
  this->ObjectListComboBox->SetLabelText("Surface ");
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox->GetWidget()->SetStateToNormal();
  this->ObjectListComboBox->GetWidget()->SetCommand(this, "TransformSurfaceCallback");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 5 -fill x", 
  this->ObjectListComboBox->GetWidgetName());			


    if (!this->BWVisiCheckButton)
  this->BWVisiCheckButton = vtkKWCheckButtonWithLabel::New();
  this->BWVisiCheckButton->SetParent(this->MainFrame);
  this->BWVisiCheckButton->Create();
  this->BWVisiCheckButton->SetLabelText("Box On/Off");
  this->BWVisiCheckButton->GetWidget()->SelectedStateOff();
  this->BWVisiCheckButton->GetWidget()->SetCommand(this, "BWStateCallback");  
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -expand y -padx 2 -pady 6 -fill x",  
  this->BWVisiCheckButton->GetWidgetName());  


  // 3 matrix widgets , one each for translation, scaling , rotation   
  // translation
  if(!this->matrix_widget1)
  this->matrix_widget1 = vtkKWMatrixWidgetWithLabel::New();
  this->matrix_widget1->SetParent(this->MainFrame);
  this->matrix_widget1->Create();
  this->matrix_widget1->GetWidget()->SetElementChangedCommand(this,"MatrixValuesTrans");
  this->matrix_widget1->SetBorderWidth(2);
  this->matrix_widget1->SetReliefToGroove();
  this->matrix_widget1->SetPadX(3);
  this->matrix_widget1->SetPadY(3);  
  this->matrix_widget1->GetWidget()->SetNumberOfRows(1);
  this->matrix_widget1->GetWidget()->SetNumberOfColumns(3);  
  this->matrix_widget1->SetLabelText("Translation ( X , Y , Z)");  
  this->matrix_widget1->SetLabelPositionToTop();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 60 -pady 0 -fill x", 
  this->matrix_widget1->GetWidgetName());
  this->matrix_widget1->UpdateEnableState();

// scaling
  if(!this->matrix_widget2)
  this->matrix_widget2 = vtkKWMatrixWidgetWithLabel::New();
  this->matrix_widget2->SetParent(this->MainFrame);
  this->matrix_widget2->Create();
  this->matrix_widget2->GetWidget()->SetElementChangedCommand(this,"MatrixValuesScale");  
  this->matrix_widget2->SetBorderWidth(2);
  this->matrix_widget2->SetReliefToGroove();
  this->matrix_widget2->SetPadX(3);
  this->matrix_widget2->SetPadY(3);  
  this->matrix_widget2->GetWidget()->SetNumberOfRows(1);
  this->matrix_widget2->GetWidget()->SetNumberOfColumns(3);  
  this->matrix_widget2->SetLabelText("Scaling    ");      
  this->matrix_widget2->SetLabelPositionToTop();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 60 -pady 0 -fill x", 
  this->matrix_widget2->GetWidgetName());
  this->matrix_widget2->UpdateEnableState();

  // rotation
  if(!this->matrix_widget3)
  this->matrix_widget3 = vtkKWMatrixWidgetWithLabel::New();
  this->matrix_widget3->SetParent(this->MainFrame);
  this->matrix_widget3->Create();
  this->matrix_widget3->GetWidget()->SetElementChangedCommand(this,"MatrixValuesRotate");
  this->matrix_widget3->SetBorderWidth(2);
  this->matrix_widget3->SetReliefToGroove();
  this->matrix_widget3->SetPadX(3);
  this->matrix_widget3->SetPadY(3);  
  this->matrix_widget3->GetWidget()->SetNumberOfRows(1);
  this->matrix_widget3->GetWidget()->SetNumberOfColumns(3);  
  this->matrix_widget3->SetLabelText("Rotation (Rx , Ry, Rz)  ");    
  this->matrix_widget3->SetLabelPositionToTop();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 60 -pady 0 -fill x", 
  this->matrix_widget3->GetWidgetName());
  this->matrix_widget3->UpdateEnableState();

    if ( !this->ResetTransButton )
    this->ResetTransButton = vtkKWPushButton::New();
  this->ResetTransButton->SetParent(this->MainFrame);
  this->ResetTransButton->Create();
  this->ResetTransButton->SetCommand(this, "MatrixValuesReset");  
  this->ResetTransButton->SetText("Reset");
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -padx 2 -pady 6", 
  this->ResetTransButton->GetWidgetName());

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "TransformSurfaceApplyCallback");
  this->CancelButton->SetCommand(this, "TransformSurfaceCancelCallback");
  
}
// Transform callback. Takes in a surface. applies transform using boxwidget.
// Box widget can be interactively moved around and the resulting transform is 
//  applied to the underlying surface.
void vtkKWMimxTransformSurfaceGroup:: TransformSurfaceCallback(const char *entry)
{		
	if(this->CancelStatus)
	{
		this->CancelStatus = 0;
		return ;
	}

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
		if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{	
		return ;
	}

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();


	strcpy(previousselection,name);

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	vtkActor *finalActor = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
	->GetItem(combobox->GetValueIndex(name)))->GetActor();


	vtkPolyData *finalPolyData = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
	->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	
	//vtkMimxSurfacePolyDataActor *current = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
	//	->GetItem(combobox->GetValueIndex(name)));


// added to show initial values 

	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,0,0);
	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,1,0);
	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,2,0);		


	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,0,1);
	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,1,1);
	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,2,1);		

	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,0,0);
	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,1,0);
	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,2,0);		

	previousvaltrans[0] = 0;
	previousvaltrans[1] = 0;
	previousvaltrans[2] = 0;

	previousval[0] = 0;
	previousval[1] = 0;
	previousval[2] = 0;

	
///////////////// Added to change the orientation of the box widget and match with the oringial///////
///////////////// shape and dimensions////////////////////////////////////////////////////////////////

	vtkTransform *tmpTrans = vtkTransform::New();
	vtkMatrix4x4 *mat44 = vtkMatrix4x4::New();
  
  //VAM - FIX ME - Need to support this infrastructure
	//mat44->DeepCopy(vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(combobox->GetValueIndex(name)))->GetOrignalTransform()->GetMatrix());
	//VAM - END FIX ME
  tmpTrans->SetMatrix(mat44);
	mat44->Delete();

	vtkTransform *invTrans = vtkTransform::New();
	invTrans->DeepCopy(tmpTrans);
	invTrans->Inverse();
	
	vtkTransformFilter *trans = vtkTransformFilter::New();
	trans->SetInput( finalPolyData );
	trans->SetTransform(invTrans);
	trans->Update();	

	this->SelectedSurfaceMapper->SetInput(vtkPolyData::SafeDownCast(trans->GetOutput()));
	this->SelectedSurfaceMapper->Modified();
	this->SelectedSurfaceMapper->Update();
	
////////////////////////////////////////////////

	float x[3], y[3], z[3];
	this->boxWidget->SetInteractor( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor() );
	this->boxWidget->SetPlaceFactor(1);
	this->boxWidget->SetHandleSize(0.001);
	this->boxWidget->SetProp3D(this->SelectedSurfaceActor);
	this->boxWidget->PlaceWidget(trans->GetOutput()->GetBounds());			

	ofstream outfile;
	outfile.open ("bounds.txt");
  
  //VAM - FIX ME - Need to support this infrastructure
	vtkMatrix4x4* transMatrix;
  //transMatrix = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(combobox->GetValueIndex(name)))
  //   ->GetOrignalTransform()->GetMatrix();
	//VAM - END FIX ME	
		for (int i=0;i<4;i++)
		{
		  for (int j=0;j<4;j++)
		  {
  		  double element = transMatrix->GetElement(i, j);
  		  outfile << element << std::endl;		  
		  }
		}
		double bounds[6];
		trans->GetOutput()->GetBounds(bounds);
		for(int i = 0; i< 6; i++)
		{
			outfile << bounds[i]<< endl;		
		}
		outfile.close();

	this->boxWidget->SetTransform(tmpTrans);
	this->SelectedSurfaceActor->SetUserTransform(tmpTrans);
	
	if(this->boxWidget->GetEnabled())
	{
		boxWidget->SetEnabled(0);
	}
	

	this->boxWidget->SetInput(finalPolyData); // use this to show the original surface after applying the transform
	this->boxWidget->SetInputActor(finalActor);
	this->boxWidget->SetEnabled(1);	
	this->TransformCallback->Transform = this->Transform ;
	this->TransformCallback->Actor = this->SelectedSurfaceActor;	
	this->boxWidget->AddObserver(vtkCommand::InteractionEvent,this->TransformCallback);
	this->boxWidget->GetTransform(this->Transform);
	this->Transform->GetPosition(x); 
	this->Transform->GetScale(y);
	this->Transform->GetOrientation(z); 
	
//finalActor->SetUserTransform(vtkTransform::SafeDownCast(this->Transform->GetInverse()));
	
// Translation
//
//	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,0,x[0]);
//	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,1,x[1]);
//	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,2,x[2]);
//
//// Scaling
//	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,0,y[0]);
//	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,1,y[1]);
//	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,2,y[2]);
//
//// Rotation
//	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,0,z[0]);
//	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,1,z[1]);
//	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,2,z[2]);

	double * xcenter = this->boxWidget->GetCenter(); // added instead of get position
	this->originalcenter[0] = xcenter[0];
	this->originalcenter[1] = xcenter[1];
	this->originalcenter[2] = xcenter[2];

	previousvaltrans[0] = 0;
	previousvaltrans[1] = 0;
	previousvaltrans[2] = 0;

	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);

	this->SelectedSurfaceMapper->Modified();
	this->SelectedSurfaceMapper->Update();
	this->SelectedSurfaceActor->Modified();	

	this->GetMimxMainWindow()->GetRenderWidget()->Render();

}

// Translation matrix widget
void vtkKWMimxTransformSurfaceGroup:: MatrixValuesTrans(int row, int col , const char *entry)
{
//	double x[3]; // removed april 8
	double diff1[3];	
	if(!this->boxWidget)
	return;
	if(!this->boxWidget->GetEnabled())
	return;
	this->boxWidget->GetTransform(this->Transform);
	
	//double * x = this->boxWidget->GetCenter(); // added instead of get position: gives the center of the bw

//	this->Transform->GetPosition(x);  // removed april 8
//	diff1[0] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,0)-x[0];
//	diff1[1] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,1)-x[1];
//	diff1[2] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,2)-x[2];	

	diff1[0] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,0)-previousvaltrans[0];
	diff1[1] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,1)-previousvaltrans[1];
	diff1[2] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,2)-previousvaltrans[2];

	this->Transform->Translate(diff1); // removed april 8	
	this->Transform->Update();

// added
	previousvaltrans[0] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,0);
	previousvaltrans[1] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,1);
	previousvaltrans[2] = this->matrix_widget1->GetWidget()->GetElementValueAsDouble(0,2);
// added end 

	this->boxWidget->SetTransform(this->Transform);
	this->boxWidget->Modified();	

	this->SelectedSurfaceActor->SetUserTransform(this->Transform); // added april 9 to update the actor 
	this->SelectedSurfaceActor->Modified(); // added april 9


	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->SelectedSurfaceMapper->Modified();
	this->SelectedSurfaceMapper->Update();
	this->SelectedSurfaceActor->Modified();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
// Scaling matrix widget
void vtkKWMimxTransformSurfaceGroup:: MatrixValuesScale(int row, int col , const char *entry)
{
	double y[3];
	double diff2[3];
	if(!this->boxWidget)
	return;
	if(!this->boxWidget->GetEnabled())
	return;	
	this->boxWidget->GetTransform(this->Transform);	
	this->Transform->GetScale(y); 
	diff2[0] = this->matrix_widget2->GetWidget()->GetElementValueAsDouble(0,0)/y[0];
	diff2[1] = this->matrix_widget2->GetWidget()->GetElementValueAsDouble(0,1)/y[1];
	diff2[2] = this->matrix_widget2->GetWidget()->GetElementValueAsDouble(0,2)/y[2];

///////// added april 9 
	double xmean, ymean, zmean ;
	double bounds1[6];
	this->SelectedSurfaceMapper->GetInput()->GetBounds(bounds1);
	xmean = (bounds1[0]+bounds1[1])/2;
	ymean = (bounds1[2]+bounds1[3])/2;
	zmean = (bounds1[4]+bounds1[5])/2;
	this->Transform->Translate(xmean, ymean, zmean);

	this->Transform->Scale(diff2);

	this->Transform->Translate(-xmean, -ymean, -zmean);

	this->Transform->Update();

	//previousval[0] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,0);
	//previousval[1] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,1);
	//previousval[2] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,2);
	previousvalscale[0] = this->matrix_widget2->GetWidget()->GetElementValueAsDouble(0,0);
	previousvalscale[1] = this->matrix_widget2->GetWidget()->GetElementValueAsDouble(0,1);
	previousvalscale[2] = this->matrix_widget2->GetWidget()->GetElementValueAsDouble(0,2);

///////// added april 9  end //////////////

//	this->Transform->Scale(diff2); removed april 9
//	this->Transform->Update();		removed april 9
	this->boxWidget->SetTransform(this->Transform);
	this->boxWidget->Modified();	
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->SelectedSurfaceMapper->Modified();
	this->SelectedSurfaceMapper->Update();
	this->SelectedSurfaceActor->SetUserTransform(this->Transform); 
	this->SelectedSurfaceActor->Modified();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
// Rotation matrix widget
void vtkKWMimxTransformSurfaceGroup:: MatrixValuesRotate(int row, int col , const char *entry)
{
	double z[3];
	double diff3[3];		
	if(!this->boxWidget)
	return;
	if(!this->boxWidget->GetEnabled())
	return;	
	this->boxWidget->GetTransform(this->Transform);	
	this->Transform->GetOrientation(z);
	
	diff3[0] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,0)-previousval[0];
	diff3[1] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,1)-previousval[1];
	diff3[2] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,2)-previousval[2];	
	
/////////
	double xmean, ymean, zmean ;
	double bounds1[6];
	this->SelectedSurfaceMapper->GetInput()->GetBounds(bounds1);
	xmean = (bounds1[0]+bounds1[1])/2;
	ymean = (bounds1[2]+bounds1[3])/2;
	zmean = (bounds1[4]+bounds1[5])/2;
	this->Transform->Translate(xmean, ymean, zmean);
/////////
	this->Transform->RotateY(diff3[1]);
	this->Transform->RotateX(diff3[0]);	
	this->Transform->RotateZ(diff3[2]);
////////
	this->Transform->Translate(-xmean, -ymean, -zmean);
////////

	this->Transform->Update();
	previousval[0] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,0);
	previousval[1] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,1);
	previousval[2] = this->matrix_widget3->GetWidget()->GetElementValueAsDouble(0,2);

	this->boxWidget->SetTransform(this->Transform);
	this->boxWidget->Modified();	

	this->SelectedSurfaceActor->SetUserTransform(this->Transform); // added april 9 to update the actor 
	this->SelectedSurfaceActor->Modified(); // added april 9

	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->SelectedSurfaceMapper->Modified();
	this->SelectedSurfaceMapper->Update();
	this->SelectedSurfaceActor->Modified();	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

// This widget displays the values for the transformation matrix

void vtkKWMimxTransformSurfaceGroup:: MatrixValues_changed(int row, int col ,const char *entry)
{
	
	float y[3], z[3]; // x[3]
	this->boxWidget->GetTransform(this->Transform);						

	double * x = this->boxWidget->GetCenter(); // added instead of get position
	
	double differential[3] ;

	differential[0] = x[0]-originalcenter[0];
	differential[1] = x[1]-originalcenter[1];
	differential[2] = x[2]-originalcenter[2];
	
//	this->Transform->GetPosition(x); 
	this->Transform->GetScale(y);
	this->Transform->GetOrientation(z);	

// Translation		
	previousvaltrans[0] = differential[0];
	previousvaltrans[1] = differential[1];
	previousvaltrans[2] = differential[2];

//	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,0,x[0]);
//	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,1,x[1]);
//	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,2,x[2]);

	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,0,differential[0]);
	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,1,differential[1]);
	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,2,differential[2]);

//// Scaling
	previousvalscale[0] = y[0];
	previousvalscale[1] = y[1];
	previousvalscale[2] = y[2];

	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,0,y[0]);
	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,1,y[1]);
	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,2,y[2]);

//// Orientation
	previousval[0] = z[0];
	previousval[1] = z[1];
	previousval[2] = z[2];

	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,0,z[0]);
	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,1,z[1]);
	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,2,z[2]);		
}

void vtkKWMimxTransformSurfaceGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxTransformSurfaceGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxTransformSurfaceGroup::TransformSurfaceApplyCallback()
{
	this->CancelStatus = 1;

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

	vtkPolyData *polydata = this->SelectedSurfaceMapper->GetInput();
	transpoly = vtkTransformPolyDataFilter::New();
	transpoly->SetInput(polydata);
	transpoly->SetTransform(this->Transform);
	transpoly->Update();

	vtkPolyData *changed_surface = NULL ;

	changed_surface = transpoly->GetOutput() ;			

	boxWidget->SetEnabled(0);
	boxWidget->SetInteractor(NULL);	
	boxWidget->SetInput(NULL);


	if(changed_surface)
	{
		this->AddSurfaceToDisplay(changed_surface, "", foundation_name, 1, NULL );

		/*
		vtkMimxSurfacePolyDataActor *changed_surface_actor = vtkMimxSurfacePolyDataActor::New();
		changed_surface_actor->SetFoundationName(foundation_name);
		changed_surface_actor->GetDataSet()->DeepCopy(changed_surface);
		changed_surface_actor->Modified();
		changed_surface_actor->GetOrignalTransform()->DeepCopy(this->Transform); // added 

		this->SurfaceList->AppendItem(changed_surface_actor);
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(changed_surface_actor->GetActor());			
        */

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

		char surface_name[64]; 
		sprintf(surface_name, "Transform-");
		this->AddSurfaceToDisplay(changed_surface, surface_name, name, 1 , NULL);
		this->UpdateObjectLists();
	}

		//boxWidget->SetEnabled(0);
		//boxWidget->SetInteractor(NULL);	
		//boxWidget->SetInputActor(NULL);

	//// translation
	//this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,0,0);
	//this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,1,0);
	//this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,2,0);

	////// Scaling
	//this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,0,1);
	//this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,1,1);
	//this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,2,1);

	////// Orientation
	//this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,0,0);
	//this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,1,0);
	//this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,2,0);


    this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);	    
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return 0;
}

void vtkKWMimxTransformSurfaceGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "ObjectListComboBox: " << this->ObjectListComboBox << std::endl;
  os << indent << "FileBrowserDialog: " << this->FileBrowserDialog << std::endl;
  os << indent << "SelectedSurfaceActor: " << this->SelectedSurfaceActor << std::endl;
  os << indent << "SelectedSurfaceMapper: " << this->SelectedSurfaceMapper << std::endl;
  os << indent << "boxWidget: " << this->boxWidget << std::endl;
  os << indent << "Transform: " << this->Transform << std::endl;
  os << indent << "TransformCallback: " << this->TransformCallback << std::endl;
  os << indent << "CancelStatus: " << this->CancelStatus << std::endl;
  os << indent << "matrix: " << this->matrix << std::endl;
  os << indent << "matrix_widget1: " << this->matrix_widget1 << std::endl;
  os << indent << "matrix_widget2: " << this->matrix_widget2 << std::endl;
  os << indent << "matrix_widget3: " << this->matrix_widget3 << std::endl;
  os << indent << "previousselection: " << this->previousselection << std::endl;
  os << indent << "BWVisiCheckButton: " << this->BWVisiCheckButton << std::endl;
  os << indent << "ResetTransButton: " << this->ResetTransButton << std::endl;
}
//----------------------------------------------------------------------------
void vtkKWMimxTransformSurfaceGroup::TransformSurfaceCancelCallback()
{
  this->CancelStatus = 1;
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);

	if (num == -1)
	  return ;
	//vtkActor *finalActor = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    //->GetItem(combobox->GetValueIndex(name)))->GetActor();

	vtkPolyData *finalPolyData = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
	->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
	finalPolyData->Modified();
	finalPolyData->Update();

  this->GetMimxMainWindow()->GetRenderWidget()->Render();  
  boxWidget->SetEnabled(0);
  boxWidget->SetInteractor(NULL);
	boxWidget->SetInput(NULL);

	this->boxWidget->GetSelectedFaceProperty()->SetOpacity(0.000001);
	this->boxWidget->GetOutlineProperty()->SetOpacity(0.000001);
	this->boxWidget->GetSelectedOutlineProperty()->SetOpacity(0.000001);
	this->BWVisiCheckButton->GetWidget()->SelectedStateOff();



 //   // translation
	//this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,0,0);
	//this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,1,0);
	//this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,2,0);

	////// Scaling
	//this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,0,1);
	//this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,1,1);
	//this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,2,1);

	////// Orientation
	//this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,0,0);
	//this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,1,0);
	//this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,2,0);


}
//------------------------------------------------------------------------------
void vtkKWMimxTransformSurfaceGroup::UpdateObjectLists()
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
	this->TransformSurfaceCallback(NULL);
	
}
//--------------------------------------------------------------------------------
void vtkKWMimxTransformSurfaceGroup::SaveVTKSurfaceDoneCallback()
{
	if(this->TransformSurfaceApplyCallback())
	this->TransformSurfaceCancelCallback();
}
//---------------------------------------------------------------------------------

void vtkKWMimxTransformSurfaceGroup::BWStateCallback(int state)
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Please select Transform");;
		return ;
	}
	if(state)
	{	
		this->boxWidget->GetSelectedFaceProperty()->SetOpacity(1.0);
		this->boxWidget->GetOutlineProperty()->SetOpacity(1.0);
		this->boxWidget->GetSelectedOutlineProperty()->SetOpacity(1.0);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}

	else
	{		
		this->boxWidget->GetSelectedFaceProperty()->SetOpacity(0.000001);
		this->boxWidget->GetOutlineProperty()->SetOpacity(0.000001);
		this->boxWidget->GetSelectedOutlineProperty()->SetOpacity(0.000001);	
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}
//		this->GetMimxMainWindow()->GetRenderWidget()->Render();	
}





// used to reset the transform 

void vtkKWMimxTransformSurfaceGroup:: MatrixValuesReset()
{
	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,1,1);
	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,0,1);	
	this->matrix_widget2->GetWidget()->SetElementValueAsDouble(0,2,1);	

	MatrixValuesScale(0,0,NULL);


	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,1,0.0);
	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,0,0.0);	
	this->matrix_widget3->GetWidget()->SetElementValueAsDouble(0,2,0.0);	

	MatrixValuesRotate(0,0,NULL);

	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,0,0.0);
	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,1,0.0);
	this->matrix_widget1->GetWidget()->SetElementValueAsDouble(0,2,0.0);

	MatrixValuesTrans(0,0,NULL);

}
