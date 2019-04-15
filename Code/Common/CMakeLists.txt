PROJECT(MIMXCommon)

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}    ${IAFEMMESH_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

SET ( COMMON_SRCS 
    mimxRegisterVersorRigidFilter.cxx  
    mimxRegisterBSplineFilter.cxx 
    mimxTransformIO.cxx 
    mimxCheckTypeOfFile.cxx
    vtkContainer.cxx
    vtkLinkedListWrapper.cxx
    vtkMimxCurve.cxx
    vtkMimxSTLSource.cxx
    vtkMimxUGrid.cxx
    vtkMimxBoundingBoxSource.cxx 
    vtkLocalAxesActor.cxx
    vtkPVAxesActor.cxx  
    vtkMimxSolidStructuredGridSource.cxx
    vtkMimxPlanarStructuredGridSource.cxx
    vtkMeshQualityClass.cxx
    vtkMimxApplyNodeElementNumbers.cxx
    vtkMimxCreateIntervertebralDiscElementSets.cxx
    vtkMimxMergeNodesPointAndCellData.cxx
    vtkMimxMeshSeed.cxx
    vtkMimxActorBase.cxx
    vtkMimxSurfacePolyDataActor.cxx
    vtkMimxFeaturePolyDataActor.cxx
    vtkMimxUnstructuredGridActor.cxx
    vtkMimxErrorCallback.cxx
    vtkMimxImageActor.cxx
#    vtkMimxInteractorStyleUser.cxx
    vtkLinkedListWrapperTree.cxx  
    vtkMimxColorCodeMeshSeedActor.cxx
    vtkMimxMeshActor.cxx
    vtkMimxEntryValueChangedCallback.cxx
    vtkMimxBoundaryConditionActor.cxx
    vtkMimxCubeAxesActor.cxx
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMimxCommonConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMimxCommonConfigure.h
  )

ADD_LIBRARY( mimxCommon ${COMMON_SRCS} )

INSTALL_TARGETS(/lib/IaFeMesh mimxCommon)
INSTALL_FILES(/include/IaFeMesh/Common "(\\.h|\\.txx)$")


