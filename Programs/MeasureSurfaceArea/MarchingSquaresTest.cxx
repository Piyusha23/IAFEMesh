#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkMarchingSquares.h>
#include <vtkStripper.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkContourWidget.h>
#include <vtkContourRepresentation.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkRendererCollection.h>
#include <vtkLookupTable.h>
#include <vtkLinearContourLineInterpolator.h>
#include <vtkPolyDataWriter.h>
#include <vtkDecimatePolylineFilter.h>
#include <vtkHyperOctreeContourFilter.h>
#include <vtkHyperOctreeAlgorithm.h>

int main (int, char *[])
{

    // Make primitive image
    vtkSmartPointer<vtkImageData> tempImage =
      vtkSmartPointer<vtkImageData>::New();
    tempImage->SetDimensions( 10, 10, 1 );
    tempImage->SetScalarTypeToUnsignedChar();
    tempImage->AllocateScalars();

    // Make view of image
    unsigned char *ptr = (unsigned char*)tempImage->GetScalarPointer();
    // Clear to black
    for( int i=0; i<100; i++) ptr[i] = 0;
    // Draw a little shape
    for( int i=32; i<35; i++) ptr[i] = 255;
    for( int i=41; i<47; i++) ptr[i] = 255;
    ptr[52] = 255; ptr[54] = 255;

    // Find contour of image
    vtkSmartPointer<vtkMarchingSquares> msf =
vtkSmartPointer<vtkMarchingSquares>::New();
    msf->SetInputConnection( tempImage->GetProducerPort() );
    msf->SetNumberOfContours( 1 );
    msf->SetValue( 0, 128 );
    msf->Update();

  std::cout << "Number of Cells: " << msf->GetOutput()->GetNumberOfCells() << std::endl;
  std::cout << "Number of Points: " << msf->GetOutput()->GetNumberOfPoints() << std::endl;
  
    // Simplify it (Does this do anything?)
    vtkSmartPointer<vtkDecimatePolylineFilter> vs = vtkSmartPointer<vtkDecimatePolylineFilter>::New();
    vs->SetInputConnection( msf->GetOutputPort() );
  vs->SetTargetReduction(0.5);
    vs->Update();

  std::cout << "Number of Cells (Dec): " << vs->GetOutput()->GetNumberOfCells() << std::endl;
  std::cout << "Number of Points (Dec): " << vs->GetOutput()->GetNumberOfPoints() << std::endl;

    vtkSmartPointer<vtkPolyData> pd = vs->GetOutput();

#if 0
    pd->GetLines()->InitTraversal();
  
    vtkIdType *pts, npts;
  
    pd->GetLines()->GetNextCell(npts,pts);
  
   vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  
  for ( int i = 0;  i < static_cast<int>( npts-1 );  i ++)
    {
    points->InsertPoint(i,unorderedPolyData->GetPoints()->GetPoint(pts[i]));
    }
  
  vtkSmartPointer<vtkPolyData> orderedPolyData =
  vtkSmartPointer<vtkPolyData>::New();
  orderedPolyData->SetPoints( points );
  
#endif
  vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
  writer->SetInput(pd);
  writer->SetFileName("tmp1.vtk");
  writer->Update();
  
  return 1;
  
    // Debug Visualization
    vtkSmartPointer<vtkImageActor> imageActor =
vtkSmartPointer<vtkImageActor>::New();
    imageActor->SetInput( tempImage );

    vtkSmartPointer<vtkPolyDataMapper> mapper =
vtkSmartPointer<vtkPolyDataMapper>::New();

    vtkSmartPointer<vtkLookupTable> lut =
vtkSmartPointer<vtkLookupTable>::New();
    //set up LUT for contour polyline
    lut->SetNumberOfTableValues(1);
    lut->Build();
    lut->SetTableValue(0, 0.0, 1.0, 0.0, 1.0 );// Show contour in Green

    //add the lut to the mapper, then add to actor
    mapper->SetLookupTable( lut );
    mapper->SetColorModeToMapScalars();
    mapper->SetInput( pd );

    vtkSmartPointer<vtkActor> contourActor = vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor
= vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkOrientedGlyphContourRepresentation> contourRep =
        vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
    contourRep->GetLinesProperty()->SetColor(1, 0, 0); // Show contour

    vtkSmartPointer<vtkContourWidget> contourWidget =
        vtkSmartPointer<vtkContourWidget>::New();
    contourWidget->SetInteractor(renderWindowInteractor);
    contourWidget->SetRepresentation(contourRep);
    contourWidget->On();

    renderer->AddActor(contourActor);

    renderer->AddActor( imageActor );

    renderer->SetBackground( 0.0, 0.0, 0.45 );

    //set line interpolator
    vtkLinearContourLineInterpolator *lcli =
vtkLinearContourLineInterpolator::New();
    contourRep->SetLineInterpolator(lcli);

    contourWidget->Initialize(pd);
    contourWidget->Render();
    renderWindow->GetRenderers()->GetFirstRenderer()->ResetCamera();
    renderWindow->Render();

    renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

#if 0
//------------------------------------------------------------------------------------------

// Reorder points
// Assuming that we have an "unordered" polydata containing "good
polylines" and "unordered points"

unorderedPolyData->GetLines()->InitTraversal();

vtkIdType *pts, npts;

unorderedPolyData->GetLines()->GetNextCell(npts,pts);

vtkSmartPointer<vtkPoints> points =
vtkSmartPointer<vtkPoints>::New();

for ( int i = 0;  i < static_cast<int>( npts-1 );  i ++)
{
  points->InsertPoint(i,
                      unorderedPolyData->GetPoints()->GetPoint(pts[i]));
}

vtkSmartPointer<vtkPolyData> orderedPolyData =
vtkSmartPointer<vtkPolyData>::New();
orderedPolyData->SetPoints( points );

vtkDecimatePolylineFilter

//------------------------------------------------------------------------------------------
#endif

