PROJECT(MIMXKWCommon)

SET ( KWCommon_SRCS 
  vtkKWMimxApplication.cxx
  vtkKWMimxFEMeshMenuGroup.cxx
  vtkKWMimxImageMenuGroup.cxx
  vtkKWMimxMaterialPropertyMenuGroup.cxx
  vtkKWMimxMenuOptionGroup.cxx
  vtkKWMimxMainNotebook.cxx
  vtkKWMimxMainMenuGroup.cxx
  #vtkKWMimxMenuWindow.cxx
  vtkKWMimxSurfaceMenuGroup.cxx
  vtkKWMimxBBMenuGroup.cxx
  vtkKWMimxEvaluateMeshQualityGroup.cxx
  vtkKWMimxQualityMenuGroup.cxx
  vtkKWMimxCreateBBFromBoundsGroup.cxx
  vtkKWMimxEditBBGroup.cxx
  vtkKWMimxCreateFEMeshFromBBGroup.cxx
  vtkKWMimxViewProperties.cxx
  vtkKWMimxDeleteBoundaryConditionsGroup.cxx
  vtkKWMimxSaveVTKFEMeshGroup.cxx
  vtkKWMimxSaveVTKBBGroup.cxx
  vtkKWMimxCreateBBMeshSeedGroup.cxx
  vtkKWMimxCreateBBFromRubberBandPickGroup.cxx
  vtkKWMimxSaveSTLSurfaceGroup.cxx
  vtkKWMimxSaveVTKSurfaceGroup.cxx
  vtkKWMimxCreateFEMeshNodeSetGroup.cxx
  vtkKWMimxCreateElementSetPickFaceGroup.cxx
  vtkKWMimxCreateExtrudeFEMeshGroup.cxx
  vtkKWMimxCreateFEMeshElementSetGroup.cxx
  vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup.cxx
  vtkKWMimxSaveSurfaceGroup.cxx
  vtkKWMimxSaveBBGroup.cxx
  vtkKWMimxSaveMeshGroup.cxx
  vtkKWMimxSaveImageGroup.cxx
  vtkKWMimxEditBBMeshSeedGroup.cxx
  vtkKWMimxDeleteObjectGroup.cxx
  vtkKWMimxEditFEMeshLaplacianSmoothGroup.cxx
  vtkKWMimxCreateSurfaceFromContourGroup.cxx
  vtkKWMimxNodeElementNumbersGroup.cxx
  vtkKWMimxMergeMeshGroup.cxx
  vtkKWMimxExportAbaqusFEMeshGroup.cxx
  vtkKWMimxMergeBBGroup.cxx
  vtkKWMimxImageViewProperties.cxx
  vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.cxx
  vtkKWMimxReBinFEMeshMaterialPropertiesFromImageGroup.cxx
  vtkKWMimxMainWindow.cxx
  vtkKWMimxMainUserInterfacePanel.cxx
  vtkKWMimxViewPropertiesGroup.cxx
  vtkKWMimxDisplayPropertiesGroup.cxx
  vtkKWMimxConstMatPropElSetGroup.cxx
  vtkKWMimxBoundaryConditionsMenuGroup.cxx
  vtkKWMimxAssignBoundaryConditionsGroup.cxx
  vtkKWMimxEditElementSetNumbersGroup.cxx
  vtkKWMimxEditNodeSetNumbersGroup.cxx
  #vtkKWMimxEditNodeElementSetGroup.cxx
  vtkSlicerTheme.cxx
  vtkSlicerColor.cxx
  vtkSlicerFont.cxx
  vtkSlicerCheckRadioButtonIcons.cxx
  vtkSlicerIcons.cxx
  vtkKWMimxApplicationSettingsInterface.cxx
  vtkKWMimxDefineElSetGroup.cxx
  vtkKWMimxDisplayMatPropGroup.cxx
  vtkKWMimxViewPropertiesOptionGroup.cxx
  vtkKWMimxDefineNodeSetGroup.cxx
  #vtkKWMimxCreateElementSetFromSurface.cxx
  vtkKWMimxCreateSurfaceFromImageGroup.cxx
  vtkKWMimxLoadSurfaceGroup.cxx
  vtkKWMimxLoadImageGroup.cxx
  vtkKWMimxLoadBBGroup.cxx
  vtkKWMimxLoadMeshGroup.cxx
  vtkKWMimxImageThresholdGroup.cxx
  vtkKWMimxCreateVoxelMesh.cxx
  #vtkKWMimxMeshImprovementGroup.cxx
  #vtkKWMimxCreateIntervertebralDiscGroup.cxx
  #vtkKWMimxLigamentDefinitionGroup.cxx
  #vtkKWMimxMeshFromMultipleSurfacesGroup.cxx
  #vtkKWMimxAttachBuildingBlockToMeshGroup.cxx
  #vtkKWMimxAttachBuildingBlockToMeshMSGroup.cxx
  #vtkKWMimxSurfaceElementSetGroup.cxx
  #vtkKWMimxFacetDistanceMapGroup.cxx
  vtkKWMimxBooleanSurfaceGroup.cxx
  vtkKWMimxConnectivitySurfaceGroup.cxx
  #vtkKWMimxCreateSurfaceGroup.cxx
  vtkKWMimxSurfaceDensityGroup.cxx
  #vtkKWMimxTriangulateSurfaceGroup.cxx
  vtkKWMimxTransformSurfaceGroup.cxx
  vtkKWMimxEditSurfaceGroup.cxx
  vtkKWMimxSmoothSurfaceGroup.cxx
  #vtkKWMimxImageProcessingGroup.cxx
  #vtkKWMimxSegmentImageGroup.cxx
  #vtkKWMimxMergeSurfaceGroup.cxx
  #vtkKWMimxICPSurfaceGroup.cxx
  #vtkKWMimxSpecifyHypercubeTypeGroup.cxx
  vtkKWMimxImageManualSegmentGroup.cxx
  #vtkKWImage2D.cxx
  vtkKWMimxLoadFeatureGroup.cxx
  vtkKWMimxSaveFeatureGroup.cxx
  #vtkMimxIcons.cxx
  vtkKWMimxSaveSceneGroup.cxx
  vtkKWMimxGroupBase.cxx
  vtkKWMimxImageAutoSegmentGroup.cxx
  vtkKWMimxCropImageGroup.cxx
  vtkKWMimxCreateMaskFromFeatureGroup.cxx
  vtkKWMimxCreateFeatureFromMaskGroup.cxx
  #vtkKWMimxBridgeBlockGroup.cxx
  #vtkKWMimxProjectPointsGroup.cxx
)

SET(NO_WRAP_LIB_SRCS
  PolygonFill.cxx
  MyLineDraw.cxx
)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})




INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
KWWidgets_WRAP_TCL(mimxKWCommon KWCommon_TCL_SRCS "${KWCommon_SRCS}" "")

# Create the library. The library is built in static mode for convenience. 
# Check the 'Callbacks' example for more information about building it in
# shared mode, i.e. without the STATIC keyword (Win32 compilers requires
# an additional header file to setup DLL export symbols correctly).

ADD_LIBRARY(mimxKWCommon ${KWCommon_TCL_SRCS} ${KWCommon_SRCS} ${NO_WRAP_LIB_SRCS})
TARGET_LINK_LIBRARIES(mimxKWCommon ${KWWidgets_LIBRARIES})

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/IO ${IAFEMMESH_BINARY_DIR}/Code/IO )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Mesh    ${IAFEMMESH_BINARY_DIR}/Code/Mesh )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Utils/SurfaceBoolean ${IAFEMMESH_BINARY_DIR}/Code/Utils/SurfaceBoolean )
INCLUDE_DIRECTORIES( ${VTK_SOURCE_DIR}/Utilities/verdict/include ${VTK_KITS_DIR}/verdict )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Utils/GeoDesicPath)
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}    ${IAFEMMESH_BINARY_DIR} )  
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}    ${CMAKE_CURRENT_BINARY_DIR} )  

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMimxKWCommonConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMimxKWCommonConfigure.h
  )

#ADD_LIBRARY( mimxKWCommon ${KWCommon_SRCS} )

INSTALL_TARGETS(/lib/IaFeMesh mimxKWCommon)
INSTALL_FILES(/include/IaFeMesh/KWCommon "(\\.h|\\.txx)$")


