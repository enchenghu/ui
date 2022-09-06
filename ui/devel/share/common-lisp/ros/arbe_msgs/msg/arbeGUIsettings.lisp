; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeGUIsettings.msg.html

(cl:defclass <arbeGUIsettings> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (mode
    :reader mode
    :initarg :mode
    :type cl:integer
    :initform 0)
   (range
    :reader range
    :initarg :range
    :type cl:integer
    :initform 0)
   (threshold3d
    :reader threshold3d
    :initarg :threshold3d
    :type cl:integer
    :initform 0)
   (threshold4d
    :reader threshold4d
    :initarg :threshold4d
    :type cl:integer
    :initform 0)
   (DynamicAzimuth
    :reader DynamicAzimuth
    :initarg :DynamicAzimuth
    :type cl:integer
    :initform 0)
   (DynamicElevation
    :reader DynamicElevation
    :initarg :DynamicElevation
    :type cl:integer
    :initform 0)
   (cam_euler_alpha
    :reader cam_euler_alpha
    :initarg :cam_euler_alpha
    :type cl:float
    :initform 0.0)
   (cam_euler_beta
    :reader cam_euler_beta
    :initarg :cam_euler_beta
    :type cl:float
    :initform 0.0)
   (cam_euler_gamma
    :reader cam_euler_gamma
    :initarg :cam_euler_gamma
    :type cl:float
    :initform 0.0)
   (isNtc3dOn
    :reader isNtc3dOn
    :initarg :isNtc3dOn
    :type cl:boolean
    :initform cl:nil)
   (isNtc4dOn
    :reader isNtc4dOn
    :initarg :isNtc4dOn
    :type cl:boolean
    :initform cl:nil)
   (isCfar3dOn
    :reader isCfar3dOn
    :initarg :isCfar3dOn
    :type cl:boolean
    :initform cl:nil)
   (isCfar4dOn
    :reader isCfar4dOn
    :initarg :isCfar4dOn
    :type cl:boolean
    :initform cl:nil)
   (isPhaseEnabled
    :reader isPhaseEnabled
    :initarg :isPhaseEnabled
    :type cl:boolean
    :initform cl:nil)
   (isMetaDataEnabled
    :reader isMetaDataEnabled
    :initarg :isMetaDataEnabled
    :type cl:boolean
    :initform cl:nil)
   (ColoringType
    :reader ColoringType
    :initarg :ColoringType
    :type cl:string
    :initform "")
   (MinDoppler
    :reader MinDoppler
    :initarg :MinDoppler
    :type cl:float
    :initform 0.0)
   (MaxDoppler
    :reader MaxDoppler
    :initarg :MaxDoppler
    :type cl:float
    :initform 0.0)
   (showFloatingText
    :reader showFloatingText
    :initarg :showFloatingText
    :type cl:boolean
    :initform cl:nil)
   (coloring_cc_min
    :reader coloring_cc_min
    :initarg :coloring_cc_min
    :type cl:float
    :initform 0.0)
   (coloring_cc_max
    :reader coloring_cc_max
    :initarg :coloring_cc_max
    :type cl:float
    :initform 0.0)
   (discardOutOfElContext
    :reader discardOutOfElContext
    :initarg :discardOutOfElContext
    :type cl:boolean
    :initform cl:nil)
   (disp_processed_pc
    :reader disp_processed_pc
    :initarg :disp_processed_pc
    :type cl:boolean
    :initform cl:nil)
   (disp_slam
    :reader disp_slam
    :initarg :disp_slam
    :type cl:boolean
    :initform cl:nil)
   (noise_level_db
    :reader noise_level_db
    :initarg :noise_level_db
    :type cl:fixnum
    :initform 0)
   (selectedAzimuthBin
    :reader selectedAzimuthBin
    :initarg :selectedAzimuthBin
    :type cl:integer
    :initform 0)
   (tx_started
    :reader tx_started
    :initarg :tx_started
    :type cl:boolean
    :initform cl:nil)
   (radar_id
    :reader radar_id
    :initarg :radar_id
    :type cl:integer
    :initform 0)
   (radar_x_offset
    :reader radar_x_offset
    :initarg :radar_x_offset
    :type cl:float
    :initform 0.0)
   (radar_y_offset
    :reader radar_y_offset
    :initarg :radar_y_offset
    :type cl:float
    :initform 0.0)
   (radar_z_offset
    :reader radar_z_offset
    :initarg :radar_z_offset
    :type cl:float
    :initform 0.0)
   (radar_yaw_angle
    :reader radar_yaw_angle
    :initarg :radar_yaw_angle
    :type cl:float
    :initform 0.0)
   (radar_pitch_angle
    :reader radar_pitch_angle
    :initarg :radar_pitch_angle
    :type cl:float
    :initform 0.0)
   (localization_active
    :reader localization_active
    :initarg :localization_active
    :type cl:boolean
    :initform cl:nil)
   (aggregation_active
    :reader aggregation_active
    :initarg :aggregation_active
    :type cl:boolean
    :initform cl:nil)
   (slam_display
    :reader slam_display
    :initarg :slam_display
    :type arbe_msgs-msg:arbeSlamDisplaySettings
    :initform (cl:make-instance 'arbe_msgs-msg:arbeSlamDisplaySettings))
   (per_radar
    :reader per_radar
    :initarg :per_radar
    :type arbe_msgs-msg:arbeSettingsPerRadar
    :initform (cl:make-instance 'arbe_msgs-msg:arbeSettingsPerRadar))
   (color_obj_by_class
    :reader color_obj_by_class
    :initarg :color_obj_by_class
    :type cl:boolean
    :initform cl:nil)
   (marker_text_size
    :reader marker_text_size
    :initarg :marker_text_size
    :type cl:integer
    :initform 0))
)

(cl:defclass arbeGUIsettings (<arbeGUIsettings>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeGUIsettings>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeGUIsettings)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeGUIsettings> is deprecated: use arbe_msgs-msg:arbeGUIsettings instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'mode-val :lambda-list '(m))
(cl:defmethod mode-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:mode-val is deprecated.  Use arbe_msgs-msg:mode instead.")
  (mode m))

(cl:ensure-generic-function 'range-val :lambda-list '(m))
(cl:defmethod range-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:range-val is deprecated.  Use arbe_msgs-msg:range instead.")
  (range m))

(cl:ensure-generic-function 'threshold3d-val :lambda-list '(m))
(cl:defmethod threshold3d-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:threshold3d-val is deprecated.  Use arbe_msgs-msg:threshold3d instead.")
  (threshold3d m))

(cl:ensure-generic-function 'threshold4d-val :lambda-list '(m))
(cl:defmethod threshold4d-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:threshold4d-val is deprecated.  Use arbe_msgs-msg:threshold4d instead.")
  (threshold4d m))

(cl:ensure-generic-function 'DynamicAzimuth-val :lambda-list '(m))
(cl:defmethod DynamicAzimuth-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:DynamicAzimuth-val is deprecated.  Use arbe_msgs-msg:DynamicAzimuth instead.")
  (DynamicAzimuth m))

(cl:ensure-generic-function 'DynamicElevation-val :lambda-list '(m))
(cl:defmethod DynamicElevation-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:DynamicElevation-val is deprecated.  Use arbe_msgs-msg:DynamicElevation instead.")
  (DynamicElevation m))

(cl:ensure-generic-function 'cam_euler_alpha-val :lambda-list '(m))
(cl:defmethod cam_euler_alpha-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:cam_euler_alpha-val is deprecated.  Use arbe_msgs-msg:cam_euler_alpha instead.")
  (cam_euler_alpha m))

(cl:ensure-generic-function 'cam_euler_beta-val :lambda-list '(m))
(cl:defmethod cam_euler_beta-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:cam_euler_beta-val is deprecated.  Use arbe_msgs-msg:cam_euler_beta instead.")
  (cam_euler_beta m))

(cl:ensure-generic-function 'cam_euler_gamma-val :lambda-list '(m))
(cl:defmethod cam_euler_gamma-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:cam_euler_gamma-val is deprecated.  Use arbe_msgs-msg:cam_euler_gamma instead.")
  (cam_euler_gamma m))

(cl:ensure-generic-function 'isNtc3dOn-val :lambda-list '(m))
(cl:defmethod isNtc3dOn-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:isNtc3dOn-val is deprecated.  Use arbe_msgs-msg:isNtc3dOn instead.")
  (isNtc3dOn m))

(cl:ensure-generic-function 'isNtc4dOn-val :lambda-list '(m))
(cl:defmethod isNtc4dOn-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:isNtc4dOn-val is deprecated.  Use arbe_msgs-msg:isNtc4dOn instead.")
  (isNtc4dOn m))

(cl:ensure-generic-function 'isCfar3dOn-val :lambda-list '(m))
(cl:defmethod isCfar3dOn-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:isCfar3dOn-val is deprecated.  Use arbe_msgs-msg:isCfar3dOn instead.")
  (isCfar3dOn m))

(cl:ensure-generic-function 'isCfar4dOn-val :lambda-list '(m))
(cl:defmethod isCfar4dOn-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:isCfar4dOn-val is deprecated.  Use arbe_msgs-msg:isCfar4dOn instead.")
  (isCfar4dOn m))

(cl:ensure-generic-function 'isPhaseEnabled-val :lambda-list '(m))
(cl:defmethod isPhaseEnabled-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:isPhaseEnabled-val is deprecated.  Use arbe_msgs-msg:isPhaseEnabled instead.")
  (isPhaseEnabled m))

(cl:ensure-generic-function 'isMetaDataEnabled-val :lambda-list '(m))
(cl:defmethod isMetaDataEnabled-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:isMetaDataEnabled-val is deprecated.  Use arbe_msgs-msg:isMetaDataEnabled instead.")
  (isMetaDataEnabled m))

(cl:ensure-generic-function 'ColoringType-val :lambda-list '(m))
(cl:defmethod ColoringType-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:ColoringType-val is deprecated.  Use arbe_msgs-msg:ColoringType instead.")
  (ColoringType m))

(cl:ensure-generic-function 'MinDoppler-val :lambda-list '(m))
(cl:defmethod MinDoppler-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:MinDoppler-val is deprecated.  Use arbe_msgs-msg:MinDoppler instead.")
  (MinDoppler m))

(cl:ensure-generic-function 'MaxDoppler-val :lambda-list '(m))
(cl:defmethod MaxDoppler-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:MaxDoppler-val is deprecated.  Use arbe_msgs-msg:MaxDoppler instead.")
  (MaxDoppler m))

(cl:ensure-generic-function 'showFloatingText-val :lambda-list '(m))
(cl:defmethod showFloatingText-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:showFloatingText-val is deprecated.  Use arbe_msgs-msg:showFloatingText instead.")
  (showFloatingText m))

(cl:ensure-generic-function 'coloring_cc_min-val :lambda-list '(m))
(cl:defmethod coloring_cc_min-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:coloring_cc_min-val is deprecated.  Use arbe_msgs-msg:coloring_cc_min instead.")
  (coloring_cc_min m))

(cl:ensure-generic-function 'coloring_cc_max-val :lambda-list '(m))
(cl:defmethod coloring_cc_max-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:coloring_cc_max-val is deprecated.  Use arbe_msgs-msg:coloring_cc_max instead.")
  (coloring_cc_max m))

(cl:ensure-generic-function 'discardOutOfElContext-val :lambda-list '(m))
(cl:defmethod discardOutOfElContext-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:discardOutOfElContext-val is deprecated.  Use arbe_msgs-msg:discardOutOfElContext instead.")
  (discardOutOfElContext m))

(cl:ensure-generic-function 'disp_processed_pc-val :lambda-list '(m))
(cl:defmethod disp_processed_pc-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:disp_processed_pc-val is deprecated.  Use arbe_msgs-msg:disp_processed_pc instead.")
  (disp_processed_pc m))

(cl:ensure-generic-function 'disp_slam-val :lambda-list '(m))
(cl:defmethod disp_slam-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:disp_slam-val is deprecated.  Use arbe_msgs-msg:disp_slam instead.")
  (disp_slam m))

(cl:ensure-generic-function 'noise_level_db-val :lambda-list '(m))
(cl:defmethod noise_level_db-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:noise_level_db-val is deprecated.  Use arbe_msgs-msg:noise_level_db instead.")
  (noise_level_db m))

(cl:ensure-generic-function 'selectedAzimuthBin-val :lambda-list '(m))
(cl:defmethod selectedAzimuthBin-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:selectedAzimuthBin-val is deprecated.  Use arbe_msgs-msg:selectedAzimuthBin instead.")
  (selectedAzimuthBin m))

(cl:ensure-generic-function 'tx_started-val :lambda-list '(m))
(cl:defmethod tx_started-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tx_started-val is deprecated.  Use arbe_msgs-msg:tx_started instead.")
  (tx_started m))

(cl:ensure-generic-function 'radar_id-val :lambda-list '(m))
(cl:defmethod radar_id-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar_id-val is deprecated.  Use arbe_msgs-msg:radar_id instead.")
  (radar_id m))

(cl:ensure-generic-function 'radar_x_offset-val :lambda-list '(m))
(cl:defmethod radar_x_offset-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar_x_offset-val is deprecated.  Use arbe_msgs-msg:radar_x_offset instead.")
  (radar_x_offset m))

(cl:ensure-generic-function 'radar_y_offset-val :lambda-list '(m))
(cl:defmethod radar_y_offset-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar_y_offset-val is deprecated.  Use arbe_msgs-msg:radar_y_offset instead.")
  (radar_y_offset m))

(cl:ensure-generic-function 'radar_z_offset-val :lambda-list '(m))
(cl:defmethod radar_z_offset-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar_z_offset-val is deprecated.  Use arbe_msgs-msg:radar_z_offset instead.")
  (radar_z_offset m))

(cl:ensure-generic-function 'radar_yaw_angle-val :lambda-list '(m))
(cl:defmethod radar_yaw_angle-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar_yaw_angle-val is deprecated.  Use arbe_msgs-msg:radar_yaw_angle instead.")
  (radar_yaw_angle m))

(cl:ensure-generic-function 'radar_pitch_angle-val :lambda-list '(m))
(cl:defmethod radar_pitch_angle-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar_pitch_angle-val is deprecated.  Use arbe_msgs-msg:radar_pitch_angle instead.")
  (radar_pitch_angle m))

(cl:ensure-generic-function 'localization_active-val :lambda-list '(m))
(cl:defmethod localization_active-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:localization_active-val is deprecated.  Use arbe_msgs-msg:localization_active instead.")
  (localization_active m))

(cl:ensure-generic-function 'aggregation_active-val :lambda-list '(m))
(cl:defmethod aggregation_active-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:aggregation_active-val is deprecated.  Use arbe_msgs-msg:aggregation_active instead.")
  (aggregation_active m))

(cl:ensure-generic-function 'slam_display-val :lambda-list '(m))
(cl:defmethod slam_display-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:slam_display-val is deprecated.  Use arbe_msgs-msg:slam_display instead.")
  (slam_display m))

(cl:ensure-generic-function 'per_radar-val :lambda-list '(m))
(cl:defmethod per_radar-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:per_radar-val is deprecated.  Use arbe_msgs-msg:per_radar instead.")
  (per_radar m))

(cl:ensure-generic-function 'color_obj_by_class-val :lambda-list '(m))
(cl:defmethod color_obj_by_class-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:color_obj_by_class-val is deprecated.  Use arbe_msgs-msg:color_obj_by_class instead.")
  (color_obj_by_class m))

(cl:ensure-generic-function 'marker_text_size-val :lambda-list '(m))
(cl:defmethod marker_text_size-val ((m <arbeGUIsettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:marker_text_size-val is deprecated.  Use arbe_msgs-msg:marker_text_size instead.")
  (marker_text_size m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeGUIsettings>) ostream)
  "Serializes a message object of type '<arbeGUIsettings>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let* ((signed (cl:slot-value msg 'mode)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'range)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'threshold3d)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'threshold4d)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'DynamicAzimuth)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'DynamicElevation)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'cam_euler_alpha))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'cam_euler_beta))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'cam_euler_gamma))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'isNtc3dOn) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'isNtc4dOn) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'isCfar3dOn) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'isCfar4dOn) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'isPhaseEnabled) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'isMetaDataEnabled) 1 0)) ostream)
  (cl:let ((__ros_str_len (cl:length (cl:slot-value msg 'ColoringType))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_str_len) ostream))
  (cl:map cl:nil #'(cl:lambda (c) (cl:write-byte (cl:char-code c) ostream)) (cl:slot-value msg 'ColoringType))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'MinDoppler))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'MaxDoppler))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'showFloatingText) 1 0)) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'coloring_cc_min))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'coloring_cc_max))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'discardOutOfElContext) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'disp_processed_pc) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'disp_slam) 1 0)) ostream)
  (cl:let* ((signed (cl:slot-value msg 'noise_level_db)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'selectedAzimuthBin)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'tx_started) 1 0)) ostream)
  (cl:let* ((signed (cl:slot-value msg 'radar_id)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'radar_x_offset))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'radar_y_offset))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'radar_z_offset))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'radar_yaw_angle))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'radar_pitch_angle))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'localization_active) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'aggregation_active) 1 0)) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'slam_display) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'per_radar) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'color_obj_by_class) 1 0)) ostream)
  (cl:let* ((signed (cl:slot-value msg 'marker_text_size)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeGUIsettings>) istream)
  "Deserializes a message object of type '<arbeGUIsettings>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'mode) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'range) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'threshold3d) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'threshold4d) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'DynamicAzimuth) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'DynamicElevation) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'cam_euler_alpha) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'cam_euler_beta) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'cam_euler_gamma) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:slot-value msg 'isNtc3dOn) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'isNtc4dOn) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'isCfar3dOn) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'isCfar4dOn) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'isPhaseEnabled) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'isMetaDataEnabled) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:let ((__ros_str_len 0))
      (cl:setf (cl:ldb (cl:byte 8 0) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'ColoringType) (cl:make-string __ros_str_len))
      (cl:dotimes (__ros_str_idx __ros_str_len msg)
        (cl:setf (cl:char (cl:slot-value msg 'ColoringType) __ros_str_idx) (cl:code-char (cl:read-byte istream)))))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'MinDoppler) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'MaxDoppler) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:slot-value msg 'showFloatingText) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'coloring_cc_min) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'coloring_cc_max) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:slot-value msg 'discardOutOfElContext) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'disp_processed_pc) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'disp_slam) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'noise_level_db) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'selectedAzimuthBin) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:setf (cl:slot-value msg 'tx_started) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'radar_id) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'radar_x_offset) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'radar_y_offset) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'radar_z_offset) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'radar_yaw_angle) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'radar_pitch_angle) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:slot-value msg 'localization_active) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'aggregation_active) (cl:not (cl:zerop (cl:read-byte istream))))
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'slam_display) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'per_radar) istream)
    (cl:setf (cl:slot-value msg 'color_obj_by_class) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'marker_text_size) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeGUIsettings>)))
  "Returns string type for a message object of type '<arbeGUIsettings>"
  "arbe_msgs/arbeGUIsettings")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeGUIsettings)))
  "Returns string type for a message object of type 'arbeGUIsettings"
  "arbe_msgs/arbeGUIsettings")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeGUIsettings>)))
  "Returns md5sum for a message object of type '<arbeGUIsettings>"
  "0ab680bfc9fa5170d24dd6773926eca5")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeGUIsettings)))
  "Returns md5sum for a message object of type 'arbeGUIsettings"
  "0ab680bfc9fa5170d24dd6773926eca5")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeGUIsettings>)))
  "Returns full string definition for message of type '<arbeGUIsettings>"
  (cl:format cl:nil "std_msgs/Header header~%int32 mode~%int32 range~%int32 threshold3d~%int32 threshold4d~%int32 DynamicAzimuth~%int32 DynamicElevation~%float32 cam_euler_alpha~%float32 cam_euler_beta~%float32 cam_euler_gamma~%bool  isNtc3dOn~%bool  isNtc4dOn~%bool  isCfar3dOn~%bool  isCfar4dOn~%bool  isPhaseEnabled~%bool  isMetaDataEnabled~%string ColoringType~%float32 MinDoppler~%float32 MaxDoppler~%bool showFloatingText~%float32 coloring_cc_min~%float32 coloring_cc_max~%bool discardOutOfElContext~%bool disp_processed_pc~%bool disp_slam~%int8 noise_level_db~%int32 selectedAzimuthBin~%bool tx_started~%int32 radar_id~%float32 radar_x_offset~%float32 radar_y_offset~%float32 radar_z_offset~%float32 radar_yaw_angle~%float32 radar_pitch_angle~%bool localization_active~%bool aggregation_active~%arbeSlamDisplaySettings slam_display~%arbeSettingsPerRadar per_radar~%bool color_obj_by_class~%int32 marker_text_size~%~%~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeSlamDisplaySettings~%bool slam_on_camera~%bool disp_dist_on_cam~%bool color_by_class~%bool show_classification~%bool disp_funnel~%bool disp_FS_on_cam~%bool disp_slam_eco_mode~%~%================================================================================~%MSG: arbe_msgs/arbeSettingsPerRadar~%uint8 n_radars~%int8  radar_for_text~%float32[] ant_height~%float32[] ant_pitch~%float32[] cam_euler_a~%float32[] cam_euler_b~%float32[] cam_euler_g~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeGUIsettings)))
  "Returns full string definition for message of type 'arbeGUIsettings"
  (cl:format cl:nil "std_msgs/Header header~%int32 mode~%int32 range~%int32 threshold3d~%int32 threshold4d~%int32 DynamicAzimuth~%int32 DynamicElevation~%float32 cam_euler_alpha~%float32 cam_euler_beta~%float32 cam_euler_gamma~%bool  isNtc3dOn~%bool  isNtc4dOn~%bool  isCfar3dOn~%bool  isCfar4dOn~%bool  isPhaseEnabled~%bool  isMetaDataEnabled~%string ColoringType~%float32 MinDoppler~%float32 MaxDoppler~%bool showFloatingText~%float32 coloring_cc_min~%float32 coloring_cc_max~%bool discardOutOfElContext~%bool disp_processed_pc~%bool disp_slam~%int8 noise_level_db~%int32 selectedAzimuthBin~%bool tx_started~%int32 radar_id~%float32 radar_x_offset~%float32 radar_y_offset~%float32 radar_z_offset~%float32 radar_yaw_angle~%float32 radar_pitch_angle~%bool localization_active~%bool aggregation_active~%arbeSlamDisplaySettings slam_display~%arbeSettingsPerRadar per_radar~%bool color_obj_by_class~%int32 marker_text_size~%~%~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeSlamDisplaySettings~%bool slam_on_camera~%bool disp_dist_on_cam~%bool color_by_class~%bool show_classification~%bool disp_funnel~%bool disp_FS_on_cam~%bool disp_slam_eco_mode~%~%================================================================================~%MSG: arbe_msgs/arbeSettingsPerRadar~%uint8 n_radars~%int8  radar_for_text~%float32[] ant_height~%float32[] ant_pitch~%float32[] cam_euler_a~%float32[] cam_euler_b~%float32[] cam_euler_g~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeGUIsettings>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     4
     4
     4
     4
     4
     4
     4
     4
     4
     1
     1
     1
     1
     1
     1
     4 (cl:length (cl:slot-value msg 'ColoringType))
     4
     4
     1
     4
     4
     1
     1
     1
     1
     4
     1
     4
     4
     4
     4
     4
     4
     1
     1
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'slam_display))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'per_radar))
     1
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeGUIsettings>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeGUIsettings
    (cl:cons ':header (header msg))
    (cl:cons ':mode (mode msg))
    (cl:cons ':range (range msg))
    (cl:cons ':threshold3d (threshold3d msg))
    (cl:cons ':threshold4d (threshold4d msg))
    (cl:cons ':DynamicAzimuth (DynamicAzimuth msg))
    (cl:cons ':DynamicElevation (DynamicElevation msg))
    (cl:cons ':cam_euler_alpha (cam_euler_alpha msg))
    (cl:cons ':cam_euler_beta (cam_euler_beta msg))
    (cl:cons ':cam_euler_gamma (cam_euler_gamma msg))
    (cl:cons ':isNtc3dOn (isNtc3dOn msg))
    (cl:cons ':isNtc4dOn (isNtc4dOn msg))
    (cl:cons ':isCfar3dOn (isCfar3dOn msg))
    (cl:cons ':isCfar4dOn (isCfar4dOn msg))
    (cl:cons ':isPhaseEnabled (isPhaseEnabled msg))
    (cl:cons ':isMetaDataEnabled (isMetaDataEnabled msg))
    (cl:cons ':ColoringType (ColoringType msg))
    (cl:cons ':MinDoppler (MinDoppler msg))
    (cl:cons ':MaxDoppler (MaxDoppler msg))
    (cl:cons ':showFloatingText (showFloatingText msg))
    (cl:cons ':coloring_cc_min (coloring_cc_min msg))
    (cl:cons ':coloring_cc_max (coloring_cc_max msg))
    (cl:cons ':discardOutOfElContext (discardOutOfElContext msg))
    (cl:cons ':disp_processed_pc (disp_processed_pc msg))
    (cl:cons ':disp_slam (disp_slam msg))
    (cl:cons ':noise_level_db (noise_level_db msg))
    (cl:cons ':selectedAzimuthBin (selectedAzimuthBin msg))
    (cl:cons ':tx_started (tx_started msg))
    (cl:cons ':radar_id (radar_id msg))
    (cl:cons ':radar_x_offset (radar_x_offset msg))
    (cl:cons ':radar_y_offset (radar_y_offset msg))
    (cl:cons ':radar_z_offset (radar_z_offset msg))
    (cl:cons ':radar_yaw_angle (radar_yaw_angle msg))
    (cl:cons ':radar_pitch_angle (radar_pitch_angle msg))
    (cl:cons ':localization_active (localization_active msg))
    (cl:cons ':aggregation_active (aggregation_active msg))
    (cl:cons ':slam_display (slam_display msg))
    (cl:cons ':per_radar (per_radar msg))
    (cl:cons ':color_obj_by_class (color_obj_by_class msg))
    (cl:cons ':marker_text_size (marker_text_size msg))
))
