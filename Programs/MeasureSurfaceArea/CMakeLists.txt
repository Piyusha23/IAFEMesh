PROJECT(MeasureSurfaceArea)

  
INCLUDE_DIRECTORIES( ${MeasureSurfaceArea_SOURCE_DIR} ${MeasureSurfaceArea_BINARY_DIR} )


 
ADD_EXECUTABLE(MeasureSurfaceArea MeasureSurfaceArea.cxx )
#ADD_EXECUTABLE(MarchingSquaresTest MarchingSquaresTest.cxx )
#ADD_EXECUTABLE(OctreeFilterTest OctreeFilterTest.cxx )

LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(MeasureSurfaceArea vtkIO vtkHybrid vtkFiltering vtkCommon)
#TARGET_LINK_LIBRARIES(MarchingSquaresTest vtkIO vtkHybrid vtkFiltering vtkCommon)
#TARGET_LINK_LIBRARIES(OctreeFilterTest vtkIO vtkHybrid vtkFiltering vtkCommon)

INSTALL(TARGETS MeasureSurfaceArea RUNTIME DESTINATION bin)
