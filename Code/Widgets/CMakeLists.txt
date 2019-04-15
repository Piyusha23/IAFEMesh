PROJECT ( MIMXWidgets )

SET ( WIDGETS_SRCS 
    vtkMimxBoundingBoxWidget.cxx
    vtkMimxModPointWidget.cxx
    vtkMimxPolyDataWidget.cxx
    vtkMimxUnstructuredGridExtractWidget.cxx
    vtkMimxUnstructuredGridWidget.cxx
    vtkMimxExtractEdgeWidget.cxx
    vtkMimxExtractFaceWidget.cxx
    vtkMimxExtractCellWidget.cxx
    vtkMimxPlaceLocalAxesWidget.cxx
    vtkMimxTraceContourWidget.cxx
    vtkMimxExtractMultipleFaceWidget.cxx
    vtkMimxSelectCellsWidget.cxx
    vtkMimxSelectPointsWidget.cxx
    vtkMimxSelectCellsWidgetFEMesh.cxx
    vtkMimxCreateElementSetWidgetFEMesh.cxx
    vtkMimxCreateBuildingBlockFromPickWidget.cxx
#    vtkMimxColorCodeMeshSeedWidget.cxx
    vtkMimxViewMergePointsWidget.cxx
    vtkMimxLandmarkWidget.cxx
    vtkMimxImagePlaneWidget.cxx
    vtkMimxContourTraceWidget.cxx
    vtkMimxContourRepresentation.cxx
    vtkMimxContourTraceRepresentation.cxx
    vtkMimxContourLineInterpolator.cxx
    vtkMimxBoxWidget.cxx
#    vtkMultiContourWidget.cxx 
#    vtkMultiWidgetBase.cxx 
#    BTContour.cxx
)

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}    ${IAFEMMESH_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

ADD_LIBRARY( mimxWidgets ${WIDGETS_SRCS} )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMimxWidgetsConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMimxWidgetsConfigure.h
  )

INSTALL_TARGETS(/lib/IaFeMesh mimxWidgets)
INSTALL_FILES(/include/IaFeMesh/Widgets "(\\.h|\\.txx)$")

