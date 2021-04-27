uint8 object_id
uint8 object_class
uint8 object_quali
float32 sizeX
float32 sizeY
float32 centerX
float32 centerY
uint16 object_flags
uint32 object_age 
float32 object_orient
float32 absvelX
float32 absvelY

#############################################################################################

# more properties available --> check class definition. use of ObjectEcuEtDyn class since
# this is the latest version. Compatibility with older software is guaranteed. Use of this
# class is recommended by Ibeo
# other properties are:


# uint16_t getObjectId() const { return m_objectId; }
# uint16_t getFlags() const { return m_flags; }
# bool trackedByStationaryModel() const {return (m_flags & Flags_TrackedByStationaryModel)==Flags_TrackedByStationaryModel; }
# bool mobile() const {return (m_flags & Flags_Mobile) == Flags_Mobile; }
# bool motionModelValidated() const {return (m_flags & Flags_Validated) == Flags_Validated; }
# uint32_t getObjectAge() const { return m_objectAge; }
# NTPTime getTimestamp() const { return m_timestamp; }
# uint16_t getObjectPredAge() const { return m_objectPredAge; }
# ObjectClass getClassification() const { return m_classification; }
# uint8_t getClassCertainty() const { return m_classCertainty; }
# uint32_t getClassAge() const { return m_classAge; }
# Point2dFloat getReserved0() const {return m_reserved0; }
# Point2dFloat getReserved1() const {return m_reserved1; }
# Point2dFloat getObjBoxCenter() const { return m_objBoxCenter; }
# Point2dFloat getObjBoxCenterSigma() const { return m_objBoxCenterSigma; }
# Point2dFloat getObjBoxSize() const { return m_objBoxSize; }
# Point2dFloat getReserved2() const {return m_reserved2; }
# IBEOSDK_DEPRECATED float getObjCourseAngle() const { return m_objBoxOrientation; }
# IBEOSDK_DEPRECATED float getObjCourseAngleSigma() const { return m_objBoxOrientationSigma; }
# float getObjBoxOrientation() const { return m_objBoxOrientation; }
# float getObjBoxOrientationSigma() const { return m_objBoxOrientationSigma; }
# Point2dFloat getRelVelocity() const { return m_relVelocity; }
# Point2dFloat getRelVelocitySigma() const { return m_relVelocitySigma; }
# Point2dFloat getAbsVelocity() const { return m_absVelocity; }
# Point2dFloat getAbsVelocitySigma() const { return m_absVelocitySigma; }
# uint64_t getReserved4() const { return m_reserved4; }
# float getReserved5() const { return m_reserved5; }
# float getReserved6() const { return m_reserved6; }
# uint16_t getReserved7() const { return m_reserved7; }
# uint8_t getNbOfContourPoints() const { return uint8_t(m_contourPoints.size()); }
# uint8_t getIdxOfClosestPoint() const { return m_idxOfClosestPoint; }
# RefPointBoxLocation getRefPointLocation() const { return m_refPointLocation; }
# Point2dFloat getRefPointCoords() const { return m_refPointCoords; }
# Point2dFloat getRefPointCoordsSigma() const { return m_refPointCoordsSigma; }
# float getRefPointPosCorrCoeffs() const { return m_refPointPosCorrCoeffs; }
# float getReserved8() const { return m_reserved8; }
# float getReserved9() const { return m_reserved9; }
# uint16_t getObjPriority() const { return m_objPriority; }
# float getObjExtMeasurement() const { return m_objExtMeasurement; }
# const std::vector<Point2dFloat>& getContourPoints() const { return m_contourPoints; }
# std::vector<Point2dFloat>& getContourPoints() { return m_contourPoints; }