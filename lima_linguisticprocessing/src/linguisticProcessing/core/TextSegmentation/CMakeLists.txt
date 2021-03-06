#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_TEXTSEGMENTATION_EXPORTING)

########### next target ###############

SET(lima-lp-textsegmentation_LIB_SRCS
   SegmentationData.cpp
   SegmentationDataXmlLogger.cpp
   SegmentationResultsLoader.cpp
   SegmentFeatureExtractor.cpp
   SegmentFeaturesDumper.cpp
   SentenceBoundariesFinder.cpp
   ParagraphBoundariesFinder.cpp
   SentenceBoundariesXmlLogger.cpp
# TemporalSegmentationFeatures has to be moved to a new library just because otherwise it does not compile under Windows
#   TemporalSegmentationFeatures.cpp
   CRFSegmentLoader.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-textsegmentation)

target_link_libraries(lima-lp-textsegmentation lima-common-factory lima-common-misc lima-common-data lima-common-fsaaccess lima-common-mediaticdata lima-common-time lima-common-factory lima-common-xmlconfigurationfiles lima-common-processunitframework lima-common-mediaprocessors lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-annotationgraph lima-lp-linguisticanalysisstructure ${QT_LIBRARIES} lima-common-qslog)

set_target_properties(lima-lp-textsegmentation PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-textsegmentation DESTINATION lib)

########### install files ###############

install(FILES 
  SegmentationData.h 
  SegmentationDataXmlLogger.h 
  SegmentationResultsLoader.h 
  SegmentFeatureExtractor.h
  SegmentFeaturesDumper.h
  SentenceBoundariesFinder.h
  ParagraphBoundariesFinder.h
  SentenceBoundariesXmlLogger.h
  TemporalSegmentationFeatures.h
  CRFSegmentLoader.h
  TextSegmentationExport.h
  DESTINATION include/linguisticProcessing/core/TextSegmentation)
