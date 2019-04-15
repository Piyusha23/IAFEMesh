/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: mimxCoRegisterBSpline.cxx,v $
Language:  C++
Date:      $Date: 2012/12/07 19:08:59 $
Version:   $Revision: 1.1.1.1 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <fstream>

#include <itkImage.h>
#include <metaCommand.h>
#include <itkOrientImageFilter.h>
#include <itkImageFileReader.h>
#include <itkVersorRigid3DTransformOptimizer.h>
#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkBSplineDeformableTransform.h>
#include <itkVersorRigid3DTransform.h>
#include <itkLBFGSBOptimizer.h>
#include <itkTimeProbesCollectorBase.h>
#include <itkTransformFactory.h>
#include <itkTransformFileWriter.h>
#include <itkTransformFileReader.h>



//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::LBFGSBOptimizer   OptimizerType;
  typedef const OptimizerType   *OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( ! itk::IterationEvent().CheckEvent( &event ) )
        {
        return;
        }
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << /*optimizer->GetCurrentPosition() <<*/ std::endl;
    }
};


/****************************************************************
 Program: mimxCoRegisterBspline
 
 Purpose: Co-register two image datasets using a b-spline 
          transform

****************************************************************/



int main (int argc, char **argv)
{
  MetaCommand command;

  command.SetOption("MovingImageFilename","m",false,"Moving Image");
  command.AddOptionField("MovingImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("FixedImageFilename","f",false,"Fixed Image");
  command.AddOptionField("FixedImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("InputTransform","t",false,"Starting Rigid Transform");
  command.AddOptionField("InputTransform","filename",MetaCommand::STRING,false,"");
  
  command.SetOption("OutputFilename","o",false,"Output Transform");
  command.AddOptionField("OutputFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("iterations","n",false,"# of iterations [1000]");
  command.AddOptionField("iterations","iterations",MetaCommand::INT,false,"1000");
  
  command.SetOption("grid","g",false,"Grid Size [12 12 12]");
  command.AddOptionField("grid","xsize",MetaCommand::INT,false,"12");
  command.AddOptionField("grid","ysize",MetaCommand::INT,false,"12");
  command.AddOptionField("grid","zsize",MetaCommand::INT,false,"12");
  
  command.SetOption("border","b",false,"Border Size [3 3 3]");
  command.AddOptionField("border","xsize",MetaCommand::INT,false,"3");
  command.AddOptionField("border","ysize",MetaCommand::INT,false,"3");
  command.AddOptionField("border","zsize",MetaCommand::INT,false,"3");

  command.SetOption("boundx","bx",false,"Bound X dimension deformations [0=unbounded, 1=use lower bound, 2=use lower and upper bounds, 3=use upper bound");
  command.AddOptionField("boundx","type",MetaCommand::INT,false,"0");

  command.SetOption("boundy","by",false,"Bound Y dimension deformations [0=unbounded, 1=use lower bound, 2=use lower and upper bounds, 3=use upper bound");
  command.AddOptionField("boundy","type",MetaCommand::INT,false,"0");

  command.SetOption("boundz","bz",false,"Bound Z dimension deformations [0=unbounded, 1=use lower bound, 2=use lower and upper bounds, 3=use upper bound");
  command.AddOptionField("boundz","type",MetaCommand::INT,false,"0");


  command.SetOption("xLowerBound","xlb",false,"Lower Bound X dimension deformations");
  command.AddOptionField("xLowerBound","bound",MetaCommand::FLOAT,false,"-12");

  command.SetOption("xUpperBound","xub",false,"Upper Bound X dimension deformations");
  command.AddOptionField("xUpperBound","bound",MetaCommand::FLOAT,false,"12");

  command.SetOption("yLowerBound","ylb",false,"Lower Bound Y dimension deformations");
  command.AddOptionField("yLowerBound","bound",MetaCommand::FLOAT,false,"-12");

  command.SetOption("yUpperBound","yub",false,"Upper Bound Y dimension deformations");
  command.AddOptionField("yUpperBound","bound",MetaCommand::FLOAT,false,"12");

  command.SetOption("zLowerBound","zlb",false,"Lower Bound Z dimension deformations");
  command.AddOptionField("zLowerBound","bound",MetaCommand::FLOAT,false,"-12");

  command.SetOption("zUpperBound","zub",false,"Upper Bound Z dimension deformations");
  command.AddOptionField("zUpperBound","bound",MetaCommand::FLOAT,false,"12");
  

  command.SetOption("corrections","c",false,"Number of Corrections [12]");
  command.AddOptionField("corrections","number",MetaCommand::INT,false,"12");
  
  command.SetOption("evaluations","e",false,"Number of Evaluations [500]");
  command.AddOptionField("evaluations","number",MetaCommand::INT,false,"500");
    
  command.SetOption("histogram","h",false,"Number of Histogram Bins [50]");
  command.AddOptionField("histogram","bins",MetaCommand::INT,false,"50");
  
  command.SetOption("scale","s",false,"Spatial Sample Scale [100]");
  command.AddOptionField("scale","scale",MetaCommand::INT,false,"100");
  
  command.SetOption("convergence","cf",false,"Convergence Factor [10000000]");
  command.AddOptionField("convergence","factor",MetaCommand::INT,false,"10000000");
  
  command.SetOption("tolerance","gt",false,"Gradient Tolerance [0.0001]");
  command.AddOptionField("tolerance","tolerance",MetaCommand::INT,false,"0.0001");

  command.SetOption("index","z",false,"Moving Image 4D Index [0]");
  command.AddOptionField("index","index",MetaCommand::INT,false,"0");
  
  command.SetOption("reorient","r",false,"Reorient anatomical image [0=Off, 1=On]");
  command.AddOptionField("reorient","option",MetaCommand::INT,false,"0");
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  std::string MovingImageFilename = command.GetValueAsString("MovingImageFilename","filename");
  std::string FixedImageFilename = command.GetValueAsString("FixedImageFilename","filename");
  std::string InputTransform = command.GetValueAsString("InputTransform","filename");
  std::string OutputFilename = command.GetValueAsString("OutputFilename","filename");  
      
  int Iterations = command.GetValueAsInt("iterations","iterations");
  int GridXSize = command.GetValueAsInt("grid","xsize");
  int GridYSize = command.GetValueAsInt("grid","ysize");
  int GridZSize = command.GetValueAsInt("grid","zsize");
  int BorderXSize = command.GetValueAsInt("border","xsize");
  int BorderYSize = command.GetValueAsInt("border","ysize");
  int BorderZSize = command.GetValueAsInt("border","zsize");
  int NumberOfCorrections = command.GetValueAsInt("corrections","number");
  int NumberOfEvaluations = command.GetValueAsInt("evaluations","number");
  int HistogramBins = command.GetValueAsInt("histogram","bins");  
  int SpatialScale = command.GetValueAsInt("scale","scale");  
  float Convergence = command.GetValueAsFloat("convergence","factor");
  float Tolerance = command.GetValueAsFloat("tolerance","tolerance");
  int MovingImageIndex = command.GetValueAsInt("index","index");
  int BoundXDeformations = command.GetValueAsInt("boundx","type");
  int BoundYDeformations = command.GetValueAsInt("boundy","type");
  int BoundZDeformations = command.GetValueAsInt("boundz","type");
  float xLowerBound = command.GetValueAsFloat("xLowerBound","bound");
  float yLowerBound = command.GetValueAsFloat("yLowerBound","bound");
  float zLowerBound = command.GetValueAsFloat("zLowerBound","bound");
  float xUpperBound = command.GetValueAsFloat("xUpperBound","bound");
  float yUpperBound = command.GetValueAsFloat("yUpperBound","bound");
  float zUpperBound = command.GetValueAsFloat("zUpperBound","bound");
  int OrientAnatomicalImage = command.GetValueAsInt("reorient","option");
  
  std::cout << "Moving Image: " <<  MovingImageFilename << std::endl; 
  std::cout << "Fixed Image: " <<  FixedImageFilename << std::endl; 
  std::cout << "Output Transform: " <<  OutputFilename << std::endl; 
  std::cout << "Input Transform: " <<  InputTransform << std::endl; 
  std::cout << "Grid X Size: " << GridXSize <<std::endl;
  std::cout << "Grid Y Size: " << GridYSize <<std::endl;
  std::cout << "Grid Z Size: " << GridZSize <<std::endl;
  std::cout << "Border X Size: " << BorderXSize <<std::endl;
  std::cout << "Border Y Size: " << BorderYSize <<std::endl;
  std::cout << "Border Z Size: " << BorderZSize <<std::endl;
  std::cout << "Corrections: " << NumberOfCorrections <<std::endl;
  std::cout << "Evaluations: " << NumberOfEvaluations <<std::endl;
  std::cout << "Histogram: " << HistogramBins <<std::endl;
  std::cout << "Scale: " << SpatialScale <<std::endl;  
  std::cout << "Convergence: " << Convergence <<std::endl;  
  std::cout << "Tolerance: " << Tolerance <<std::endl;  
  std::cout << "Iterations: " << Iterations <<std::endl;
  std::cout << "Index: " << MovingImageIndex <<std::endl;
  std::cout << "Bound X: " << BoundXDeformations <<std::endl;
  std::cout << "Bound Y: " << BoundYDeformations <<std::endl;
  std::cout << "Bound Z: " << BoundZDeformations <<std::endl;
  std::cout << "X Bound: " << xLowerBound << " " << xUpperBound <<std::endl;
  std::cout << "Y Bound: " << yLowerBound << " " << yUpperBound <<std::endl;
  std::cout << "Z Bound: " << zLowerBound << " " << zUpperBound <<std::endl;
  std::cout << "Reorient Anatomical Image: " << OrientAnatomicalImage <<std::endl;   
  //exit(1);
  
  
  
  
  typedef itk::Image<signed short, 3> ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;

  /* Read the Moving Image */
  ImageReaderType::Pointer movingImageReader =  ImageReaderType::New();
  movingImageReader->SetFileName( MovingImageFilename.c_str() );
  try 
    {     
    movingImageReader->Update();  
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    throw;
    }

  typedef itk::OrientImageFilter< ImageType, ImageType>  OrientFilterType;
  ImageType::Pointer movingImage;
  if ( OrientAnatomicalImage )
    {
    OrientFilterType::Pointer orientImageFilter = OrientFilterType::New();
    orientImageFilter->SetInput( movingImageReader->GetOutput() );
    orientImageFilter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    orientImageFilter->UseImageDirectionOn();
    orientImageFilter->Update();
    movingImage = orientImageFilter->GetOutput();
    ImageType::PointType  fixedOrigin  = movingImage->GetOrigin();
    fixedOrigin.Fill(0);
    movingImage->SetOrigin( fixedOrigin );
    }
  else
    {
    movingImage = movingImageReader->GetOutput();
    }


  /* Read the Fixed Image */
  ImageReaderType::Pointer fixedImageReader =  ImageReaderType::New();
  fixedImageReader->SetFileName( FixedImageFilename.c_str() );
  try 
    {     
    fixedImageReader->Update();  
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    throw;
    }

  ImageType::Pointer fixedImage;
  if ( OrientAnatomicalImage )
    {
    OrientFilterType::Pointer orientImageFilter = OrientFilterType::New();
    orientImageFilter->SetInput( fixedImageReader->GetOutput() );
    orientImageFilter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    orientImageFilter->UseImageDirectionOn();
    orientImageFilter->Update();
    fixedImage = orientImageFilter->GetOutput();
    ImageType::PointType  fixedOrigin  = fixedImage->GetOrigin();
    fixedOrigin.Fill(0);
    fixedImage->SetOrigin( fixedOrigin );
    }
  else
    {
    fixedImage = fixedImageReader->GetOutput();
    }

  /* Now Setup the Registration */
  ImageType::SizeType fixedImageSize = fixedImage->GetBufferedRegion().GetSize();
  const unsigned int numberOfSamples = fixedImage->GetBufferedRegion().GetNumberOfPixels() / 
                                       SpatialScale;


  typedef itk::ImageRegistrationMethod< ImageType, ImageType > RegistrationType;
  RegistrationType::Pointer   registration  = RegistrationType::New();

  typedef itk::LBFGSBOptimizer                OptimizerType;
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  registration->SetOptimizer(     optimizer     );

  typedef itk::MattesMutualInformationImageToImageMetric<
                ImageType,
                ImageType >                 MetricType;
  MetricType::Pointer         metric      = MetricType::New();
  metric->SetNumberOfHistogramBins( HistogramBins );
  metric->SetNumberOfSpatialSamples( numberOfSamples );
  metric->ReinitializeSeed( 76926294 );
  registration->SetMetric(        metric        );

  typedef itk:: LinearInterpolateImageFunction<
        ImageType,
        double          >                    InterpolatorType;
  InterpolatorType::Pointer  interpolator  = InterpolatorType::New();
  registration->SetInterpolator(  interpolator  );

  static const unsigned int SpaceDimension = 3;
  static const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;
  typedef itk::BSplineDeformableTransform<
               CoordinateRepType,
               SpaceDimension,
               SplineOrder >              TransformType;
  TransformType::Pointer finalTransform = TransformType::New( );
  registration->SetTransform( finalTransform );



  registration->SetFixedImage(  fixedImage   );
  registration->SetMovingImage(   movingImage   );
  registration->SetFixedImageRegion( fixedImage->GetBufferedRegion() );

  /*** Setup the B-SPline Parameters ***/
  TransformType::RegionType    bsplineRegion;
  TransformType::SizeType      gridSizeOnImage;
  TransformType::SizeType      gridBorderSize;
  TransformType::SizeType      totalGridSize;

  gridSizeOnImage[0] = GridXSize;
  gridSizeOnImage[1] = GridYSize;
  gridSizeOnImage[2] = GridZSize;
  gridBorderSize[0]  = BorderXSize;    // Border for spline order = 3 ( 1 lower, 2 upper )
  gridBorderSize[1]  = BorderYSize;    // Border for spline order = 3 ( 1 lower, 2 upper )
  gridBorderSize[2]  = BorderZSize;    // Border for spline order = 3 ( 1 lower, 2 upper )

  totalGridSize = gridSizeOnImage + gridBorderSize;
  bsplineRegion.SetSize( totalGridSize );

  TransformType::SpacingType spacing = fixedImage->GetSpacing();
  TransformType::OriginType origin = fixedImage->GetOrigin();

  for (unsigned int r=0; r<3; r++)
    {
    spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  /
                         static_cast<double>(gridSizeOnImage[r] - 1) );
    origin[r]  -=  spacing[r];
    }

  finalTransform->SetGridSpacing( spacing );
  finalTransform->SetGridOrigin( origin );
  finalTransform->SetGridRegion( bsplineRegion );

  /* Fix this */
  if ( InputTransform.length() > 0 )
    {
    std::cout << "Using Bulk Transform: " << InputTransform << std::endl;
    itk::TransformFileReader::Pointer transformReader =  itk::TransformFileReader::New();
    transformReader->SetFileName( InputTransform.c_str() );
    try 
      {
      transformReader->Update( );  
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      throw;
      }

    typedef itk::VersorRigid3DTransform< double >     BulkTransformType;
    BulkTransformType::Pointer bulkTransform = BulkTransformType::New();

    std::string readTransformType = (transformReader->GetTransformList()->back())->GetTransformTypeAsString();
    if ( strcmp(readTransformType.c_str(),"VersorRigid3DTransform_double_3_3") == 0)
      {
      bulkTransform->SetIdentity();
      bulkTransform->SetParameters(
                (*transformReader->GetTransformList()->begin())->GetParameters() );
      bulkTransform->SetFixedParameters(
                (*transformReader->GetTransformList()->begin())->GetFixedParameters() );
      }
    else
      {
      std::cout << "Error: Invalid Bulk Transform Type! " << std::endl;
      std::cout << "Only the VersorRigid3DTransform_double_3_3 transform is currently supported." << std::endl;
      }
    finalTransform->SetBulkTransform(   bulkTransform   );
    }

  const unsigned int numberOfParameters = finalTransform->GetNumberOfParameters();
  TransformType::ParametersType parameters( numberOfParameters );
  parameters.Fill( 0.0 );

  finalTransform->SetParameters( parameters );
  registration->SetInitialTransformParameters( finalTransform->GetParameters() );

  OptimizerType::BoundSelectionType boundSelect( finalTransform->GetNumberOfParameters() );
  OptimizerType::BoundValueType     upperBound( finalTransform->GetNumberOfParameters() );
  OptimizerType::BoundValueType     lowerBound( finalTransform->GetNumberOfParameters() );


  /* User Specifies the Deformation Bounds in X,Y,Z */
  /*    Default is unbounded                        */
  for ( int i=0; i<boundSelect.size(); i+=3)
    {
    boundSelect[i+0] = BoundXDeformations;
    boundSelect[i+1] = BoundYDeformations;
    boundSelect[i+2] = BoundZDeformations;
    lowerBound[i+0]  = xLowerBound;
    lowerBound[i+1]  = yLowerBound;
    lowerBound[i+2]  = zLowerBound;
    upperBound[i+0]  = xUpperBound;
    upperBound[i+1]  = yUpperBound;
    upperBound[i+2]  = zUpperBound;
    }


  optimizer->SetBoundSelection( boundSelect );
  optimizer->SetUpperBound( upperBound );
  optimizer->SetLowerBound( lowerBound );

  optimizer->SetCostFunctionConvergenceFactor( Convergence );
  optimizer->SetProjectedGradientTolerance( Tolerance );
  optimizer->SetMaximumNumberOfIterations( Iterations );
  optimizer->SetMaximumNumberOfEvaluations( NumberOfEvaluations );
  optimizer->SetMaximumNumberOfCorrections( NumberOfCorrections );

  // Add a time probe
  itk::TimeProbesCollectorBase collector;
  std::cout << std::endl << "Starting Registration" << std::endl;

  // Create the Command observer and register it with the registration filter.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  try
    {
    collector.Start( "Registration" );
    registration->StartRegistration();
    collector.Stop( "Registration" );
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    throw;
    }

  OptimizerType::ParametersType finalParameters =
            registration->GetLastTransformParameters();

  collector.Report();

  /* This call is required to copy the parameters */
  finalTransform->SetParametersByValue( finalParameters );

  std::cout << finalTransform << std::endl;

  itk::TransformFileWriter::Pointer transformWriter =  itk::TransformFileWriter::New();
  transformWriter->SetFileName( OutputFilename.c_str() );
  if ( InputTransform.length() > 0 )
    {
    transformWriter->AddTransform( finalTransform->GetBulkTransform( ) );
    }
  transformWriter->AddTransform( finalTransform );
  try 
    {
    transformWriter->Update( );  
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    throw;
    }

  exit(0);

}    
