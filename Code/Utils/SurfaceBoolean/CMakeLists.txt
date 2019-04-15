CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#Change PROJECT_NAME to the name of your project
PROJECT(BooleanOperationPolyData)


#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  vtkFiltering
  vtkGraphics
  vtkIO
  vtkRendering
)

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above.
#You can build more than one executable per project
add_library( vtkMimxBoolean
  vtkBooleanOperationPolyDataFilter.cxx
  vtkImplicitPolyData.cxx
  vtkIntersectionPolyDataFilter.cxx
  vtkDistancePolyDataFilter.cxx
)

SET(CurrentExe "PolyDataBooleanOperationFilterExample")
ADD_EXECUTABLE(${CurrentExe}
  BooleanOperationPolyDataFilterExample.cxx
)
TARGET_LINK_LIBRARIES(${CurrentExe} vtkMimxBoolean ${Libraries})

SET(CurrentExe "PipelineExample")
ADD_EXECUTABLE(${CurrentExe}
  PipelineExample.cxx
)
TARGET_LINK_LIBRARIES(${CurrentExe} vtkMimxBoolean ${Libraries})

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
#ADD_TEST(PolyDataBooleanOperationFilter ${CurrentExe})
