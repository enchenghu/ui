;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeGUIsettings)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeGUIsettings (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEGUISETTINGS")
  (make-package "ARBE_MSGS::ARBEGUISETTINGS"))

(in-package "ROS")
;;//! \htmlinclude arbeGUIsettings.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeGUIsettings
  :super ros::object
  :slots (_header _mode _range _threshold3d _threshold4d _DynamicAzimuth _DynamicElevation _cam_euler_alpha _cam_euler_beta _cam_euler_gamma _isNtc3dOn _isNtc4dOn _isCfar3dOn _isCfar4dOn _isPhaseEnabled _isMetaDataEnabled _ColoringType _MinDoppler _MaxDoppler _showFloatingText _coloring_cc_min _coloring_cc_max _discardOutOfElContext _disp_processed_pc _disp_slam _noise_level_db _selectedAzimuthBin _tx_started _radar_id _radar_x_offset _radar_y_offset _radar_z_offset _radar_yaw_angle _radar_pitch_angle _localization_active _aggregation_active _slam_display _per_radar _color_obj_by_class _marker_text_size ))

(defmethod arbe_msgs::arbeGUIsettings
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:mode __mode) 0)
    ((:range __range) 0)
    ((:threshold3d __threshold3d) 0)
    ((:threshold4d __threshold4d) 0)
    ((:DynamicAzimuth __DynamicAzimuth) 0)
    ((:DynamicElevation __DynamicElevation) 0)
    ((:cam_euler_alpha __cam_euler_alpha) 0.0)
    ((:cam_euler_beta __cam_euler_beta) 0.0)
    ((:cam_euler_gamma __cam_euler_gamma) 0.0)
    ((:isNtc3dOn __isNtc3dOn) nil)
    ((:isNtc4dOn __isNtc4dOn) nil)
    ((:isCfar3dOn __isCfar3dOn) nil)
    ((:isCfar4dOn __isCfar4dOn) nil)
    ((:isPhaseEnabled __isPhaseEnabled) nil)
    ((:isMetaDataEnabled __isMetaDataEnabled) nil)
    ((:ColoringType __ColoringType) "")
    ((:MinDoppler __MinDoppler) 0.0)
    ((:MaxDoppler __MaxDoppler) 0.0)
    ((:showFloatingText __showFloatingText) nil)
    ((:coloring_cc_min __coloring_cc_min) 0.0)
    ((:coloring_cc_max __coloring_cc_max) 0.0)
    ((:discardOutOfElContext __discardOutOfElContext) nil)
    ((:disp_processed_pc __disp_processed_pc) nil)
    ((:disp_slam __disp_slam) nil)
    ((:noise_level_db __noise_level_db) 0)
    ((:selectedAzimuthBin __selectedAzimuthBin) 0)
    ((:tx_started __tx_started) nil)
    ((:radar_id __radar_id) 0)
    ((:radar_x_offset __radar_x_offset) 0.0)
    ((:radar_y_offset __radar_y_offset) 0.0)
    ((:radar_z_offset __radar_z_offset) 0.0)
    ((:radar_yaw_angle __radar_yaw_angle) 0.0)
    ((:radar_pitch_angle __radar_pitch_angle) 0.0)
    ((:localization_active __localization_active) nil)
    ((:aggregation_active __aggregation_active) nil)
    ((:slam_display __slam_display) (instance arbe_msgs::arbeSlamDisplaySettings :init))
    ((:per_radar __per_radar) (instance arbe_msgs::arbeSettingsPerRadar :init))
    ((:color_obj_by_class __color_obj_by_class) nil)
    ((:marker_text_size __marker_text_size) 0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _mode (round __mode))
   (setq _range (round __range))
   (setq _threshold3d (round __threshold3d))
   (setq _threshold4d (round __threshold4d))
   (setq _DynamicAzimuth (round __DynamicAzimuth))
   (setq _DynamicElevation (round __DynamicElevation))
   (setq _cam_euler_alpha (float __cam_euler_alpha))
   (setq _cam_euler_beta (float __cam_euler_beta))
   (setq _cam_euler_gamma (float __cam_euler_gamma))
   (setq _isNtc3dOn __isNtc3dOn)
   (setq _isNtc4dOn __isNtc4dOn)
   (setq _isCfar3dOn __isCfar3dOn)
   (setq _isCfar4dOn __isCfar4dOn)
   (setq _isPhaseEnabled __isPhaseEnabled)
   (setq _isMetaDataEnabled __isMetaDataEnabled)
   (setq _ColoringType (string __ColoringType))
   (setq _MinDoppler (float __MinDoppler))
   (setq _MaxDoppler (float __MaxDoppler))
   (setq _showFloatingText __showFloatingText)
   (setq _coloring_cc_min (float __coloring_cc_min))
   (setq _coloring_cc_max (float __coloring_cc_max))
   (setq _discardOutOfElContext __discardOutOfElContext)
   (setq _disp_processed_pc __disp_processed_pc)
   (setq _disp_slam __disp_slam)
   (setq _noise_level_db (round __noise_level_db))
   (setq _selectedAzimuthBin (round __selectedAzimuthBin))
   (setq _tx_started __tx_started)
   (setq _radar_id (round __radar_id))
   (setq _radar_x_offset (float __radar_x_offset))
   (setq _radar_y_offset (float __radar_y_offset))
   (setq _radar_z_offset (float __radar_z_offset))
   (setq _radar_yaw_angle (float __radar_yaw_angle))
   (setq _radar_pitch_angle (float __radar_pitch_angle))
   (setq _localization_active __localization_active)
   (setq _aggregation_active __aggregation_active)
   (setq _slam_display __slam_display)
   (setq _per_radar __per_radar)
   (setq _color_obj_by_class __color_obj_by_class)
   (setq _marker_text_size (round __marker_text_size))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:mode
   (&optional __mode)
   (if __mode (setq _mode __mode)) _mode)
  (:range
   (&optional __range)
   (if __range (setq _range __range)) _range)
  (:threshold3d
   (&optional __threshold3d)
   (if __threshold3d (setq _threshold3d __threshold3d)) _threshold3d)
  (:threshold4d
   (&optional __threshold4d)
   (if __threshold4d (setq _threshold4d __threshold4d)) _threshold4d)
  (:DynamicAzimuth
   (&optional __DynamicAzimuth)
   (if __DynamicAzimuth (setq _DynamicAzimuth __DynamicAzimuth)) _DynamicAzimuth)
  (:DynamicElevation
   (&optional __DynamicElevation)
   (if __DynamicElevation (setq _DynamicElevation __DynamicElevation)) _DynamicElevation)
  (:cam_euler_alpha
   (&optional __cam_euler_alpha)
   (if __cam_euler_alpha (setq _cam_euler_alpha __cam_euler_alpha)) _cam_euler_alpha)
  (:cam_euler_beta
   (&optional __cam_euler_beta)
   (if __cam_euler_beta (setq _cam_euler_beta __cam_euler_beta)) _cam_euler_beta)
  (:cam_euler_gamma
   (&optional __cam_euler_gamma)
   (if __cam_euler_gamma (setq _cam_euler_gamma __cam_euler_gamma)) _cam_euler_gamma)
  (:isNtc3dOn
   (&optional __isNtc3dOn)
   (if __isNtc3dOn (setq _isNtc3dOn __isNtc3dOn)) _isNtc3dOn)
  (:isNtc4dOn
   (&optional __isNtc4dOn)
   (if __isNtc4dOn (setq _isNtc4dOn __isNtc4dOn)) _isNtc4dOn)
  (:isCfar3dOn
   (&optional __isCfar3dOn)
   (if __isCfar3dOn (setq _isCfar3dOn __isCfar3dOn)) _isCfar3dOn)
  (:isCfar4dOn
   (&optional __isCfar4dOn)
   (if __isCfar4dOn (setq _isCfar4dOn __isCfar4dOn)) _isCfar4dOn)
  (:isPhaseEnabled
   (&optional __isPhaseEnabled)
   (if __isPhaseEnabled (setq _isPhaseEnabled __isPhaseEnabled)) _isPhaseEnabled)
  (:isMetaDataEnabled
   (&optional __isMetaDataEnabled)
   (if __isMetaDataEnabled (setq _isMetaDataEnabled __isMetaDataEnabled)) _isMetaDataEnabled)
  (:ColoringType
   (&optional __ColoringType)
   (if __ColoringType (setq _ColoringType __ColoringType)) _ColoringType)
  (:MinDoppler
   (&optional __MinDoppler)
   (if __MinDoppler (setq _MinDoppler __MinDoppler)) _MinDoppler)
  (:MaxDoppler
   (&optional __MaxDoppler)
   (if __MaxDoppler (setq _MaxDoppler __MaxDoppler)) _MaxDoppler)
  (:showFloatingText
   (&optional __showFloatingText)
   (if __showFloatingText (setq _showFloatingText __showFloatingText)) _showFloatingText)
  (:coloring_cc_min
   (&optional __coloring_cc_min)
   (if __coloring_cc_min (setq _coloring_cc_min __coloring_cc_min)) _coloring_cc_min)
  (:coloring_cc_max
   (&optional __coloring_cc_max)
   (if __coloring_cc_max (setq _coloring_cc_max __coloring_cc_max)) _coloring_cc_max)
  (:discardOutOfElContext
   (&optional __discardOutOfElContext)
   (if __discardOutOfElContext (setq _discardOutOfElContext __discardOutOfElContext)) _discardOutOfElContext)
  (:disp_processed_pc
   (&optional __disp_processed_pc)
   (if __disp_processed_pc (setq _disp_processed_pc __disp_processed_pc)) _disp_processed_pc)
  (:disp_slam
   (&optional __disp_slam)
   (if __disp_slam (setq _disp_slam __disp_slam)) _disp_slam)
  (:noise_level_db
   (&optional __noise_level_db)
   (if __noise_level_db (setq _noise_level_db __noise_level_db)) _noise_level_db)
  (:selectedAzimuthBin
   (&optional __selectedAzimuthBin)
   (if __selectedAzimuthBin (setq _selectedAzimuthBin __selectedAzimuthBin)) _selectedAzimuthBin)
  (:tx_started
   (&optional __tx_started)
   (if __tx_started (setq _tx_started __tx_started)) _tx_started)
  (:radar_id
   (&optional __radar_id)
   (if __radar_id (setq _radar_id __radar_id)) _radar_id)
  (:radar_x_offset
   (&optional __radar_x_offset)
   (if __radar_x_offset (setq _radar_x_offset __radar_x_offset)) _radar_x_offset)
  (:radar_y_offset
   (&optional __radar_y_offset)
   (if __radar_y_offset (setq _radar_y_offset __radar_y_offset)) _radar_y_offset)
  (:radar_z_offset
   (&optional __radar_z_offset)
   (if __radar_z_offset (setq _radar_z_offset __radar_z_offset)) _radar_z_offset)
  (:radar_yaw_angle
   (&optional __radar_yaw_angle)
   (if __radar_yaw_angle (setq _radar_yaw_angle __radar_yaw_angle)) _radar_yaw_angle)
  (:radar_pitch_angle
   (&optional __radar_pitch_angle)
   (if __radar_pitch_angle (setq _radar_pitch_angle __radar_pitch_angle)) _radar_pitch_angle)
  (:localization_active
   (&optional __localization_active)
   (if __localization_active (setq _localization_active __localization_active)) _localization_active)
  (:aggregation_active
   (&optional __aggregation_active)
   (if __aggregation_active (setq _aggregation_active __aggregation_active)) _aggregation_active)
  (:slam_display
   (&rest __slam_display)
   (if (keywordp (car __slam_display))
       (send* _slam_display __slam_display)
     (progn
       (if __slam_display (setq _slam_display (car __slam_display)))
       _slam_display)))
  (:per_radar
   (&rest __per_radar)
   (if (keywordp (car __per_radar))
       (send* _per_radar __per_radar)
     (progn
       (if __per_radar (setq _per_radar (car __per_radar)))
       _per_radar)))
  (:color_obj_by_class
   (&optional __color_obj_by_class)
   (if __color_obj_by_class (setq _color_obj_by_class __color_obj_by_class)) _color_obj_by_class)
  (:marker_text_size
   (&optional __marker_text_size)
   (if __marker_text_size (setq _marker_text_size __marker_text_size)) _marker_text_size)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; int32 _mode
    4
    ;; int32 _range
    4
    ;; int32 _threshold3d
    4
    ;; int32 _threshold4d
    4
    ;; int32 _DynamicAzimuth
    4
    ;; int32 _DynamicElevation
    4
    ;; float32 _cam_euler_alpha
    4
    ;; float32 _cam_euler_beta
    4
    ;; float32 _cam_euler_gamma
    4
    ;; bool _isNtc3dOn
    1
    ;; bool _isNtc4dOn
    1
    ;; bool _isCfar3dOn
    1
    ;; bool _isCfar4dOn
    1
    ;; bool _isPhaseEnabled
    1
    ;; bool _isMetaDataEnabled
    1
    ;; string _ColoringType
    4 (length _ColoringType)
    ;; float32 _MinDoppler
    4
    ;; float32 _MaxDoppler
    4
    ;; bool _showFloatingText
    1
    ;; float32 _coloring_cc_min
    4
    ;; float32 _coloring_cc_max
    4
    ;; bool _discardOutOfElContext
    1
    ;; bool _disp_processed_pc
    1
    ;; bool _disp_slam
    1
    ;; int8 _noise_level_db
    1
    ;; int32 _selectedAzimuthBin
    4
    ;; bool _tx_started
    1
    ;; int32 _radar_id
    4
    ;; float32 _radar_x_offset
    4
    ;; float32 _radar_y_offset
    4
    ;; float32 _radar_z_offset
    4
    ;; float32 _radar_yaw_angle
    4
    ;; float32 _radar_pitch_angle
    4
    ;; bool _localization_active
    1
    ;; bool _aggregation_active
    1
    ;; arbe_msgs/arbeSlamDisplaySettings _slam_display
    (send _slam_display :serialization-length)
    ;; arbe_msgs/arbeSettingsPerRadar _per_radar
    (send _per_radar :serialization-length)
    ;; bool _color_obj_by_class
    1
    ;; int32 _marker_text_size
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; int32 _mode
       (write-long _mode s)
     ;; int32 _range
       (write-long _range s)
     ;; int32 _threshold3d
       (write-long _threshold3d s)
     ;; int32 _threshold4d
       (write-long _threshold4d s)
     ;; int32 _DynamicAzimuth
       (write-long _DynamicAzimuth s)
     ;; int32 _DynamicElevation
       (write-long _DynamicElevation s)
     ;; float32 _cam_euler_alpha
       (sys::poke _cam_euler_alpha (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _cam_euler_beta
       (sys::poke _cam_euler_beta (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _cam_euler_gamma
       (sys::poke _cam_euler_gamma (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; bool _isNtc3dOn
       (if _isNtc3dOn (write-byte -1 s) (write-byte 0 s))
     ;; bool _isNtc4dOn
       (if _isNtc4dOn (write-byte -1 s) (write-byte 0 s))
     ;; bool _isCfar3dOn
       (if _isCfar3dOn (write-byte -1 s) (write-byte 0 s))
     ;; bool _isCfar4dOn
       (if _isCfar4dOn (write-byte -1 s) (write-byte 0 s))
     ;; bool _isPhaseEnabled
       (if _isPhaseEnabled (write-byte -1 s) (write-byte 0 s))
     ;; bool _isMetaDataEnabled
       (if _isMetaDataEnabled (write-byte -1 s) (write-byte 0 s))
     ;; string _ColoringType
       (write-long (length _ColoringType) s) (princ _ColoringType s)
     ;; float32 _MinDoppler
       (sys::poke _MinDoppler (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _MaxDoppler
       (sys::poke _MaxDoppler (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; bool _showFloatingText
       (if _showFloatingText (write-byte -1 s) (write-byte 0 s))
     ;; float32 _coloring_cc_min
       (sys::poke _coloring_cc_min (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _coloring_cc_max
       (sys::poke _coloring_cc_max (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; bool _discardOutOfElContext
       (if _discardOutOfElContext (write-byte -1 s) (write-byte 0 s))
     ;; bool _disp_processed_pc
       (if _disp_processed_pc (write-byte -1 s) (write-byte 0 s))
     ;; bool _disp_slam
       (if _disp_slam (write-byte -1 s) (write-byte 0 s))
     ;; int8 _noise_level_db
       (write-byte _noise_level_db s)
     ;; int32 _selectedAzimuthBin
       (write-long _selectedAzimuthBin s)
     ;; bool _tx_started
       (if _tx_started (write-byte -1 s) (write-byte 0 s))
     ;; int32 _radar_id
       (write-long _radar_id s)
     ;; float32 _radar_x_offset
       (sys::poke _radar_x_offset (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _radar_y_offset
       (sys::poke _radar_y_offset (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _radar_z_offset
       (sys::poke _radar_z_offset (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _radar_yaw_angle
       (sys::poke _radar_yaw_angle (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _radar_pitch_angle
       (sys::poke _radar_pitch_angle (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; bool _localization_active
       (if _localization_active (write-byte -1 s) (write-byte 0 s))
     ;; bool _aggregation_active
       (if _aggregation_active (write-byte -1 s) (write-byte 0 s))
     ;; arbe_msgs/arbeSlamDisplaySettings _slam_display
       (send _slam_display :serialize s)
     ;; arbe_msgs/arbeSettingsPerRadar _per_radar
       (send _per_radar :serialize s)
     ;; bool _color_obj_by_class
       (if _color_obj_by_class (write-byte -1 s) (write-byte 0 s))
     ;; int32 _marker_text_size
       (write-long _marker_text_size s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; int32 _mode
     (setq _mode (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _range
     (setq _range (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _threshold3d
     (setq _threshold3d (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _threshold4d
     (setq _threshold4d (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _DynamicAzimuth
     (setq _DynamicAzimuth (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _DynamicElevation
     (setq _DynamicElevation (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; float32 _cam_euler_alpha
     (setq _cam_euler_alpha (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _cam_euler_beta
     (setq _cam_euler_beta (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _cam_euler_gamma
     (setq _cam_euler_gamma (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; bool _isNtc3dOn
     (setq _isNtc3dOn (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _isNtc4dOn
     (setq _isNtc4dOn (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _isCfar3dOn
     (setq _isCfar3dOn (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _isCfar4dOn
     (setq _isCfar4dOn (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _isPhaseEnabled
     (setq _isPhaseEnabled (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _isMetaDataEnabled
     (setq _isMetaDataEnabled (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; string _ColoringType
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _ColoringType (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; float32 _MinDoppler
     (setq _MinDoppler (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _MaxDoppler
     (setq _MaxDoppler (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; bool _showFloatingText
     (setq _showFloatingText (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; float32 _coloring_cc_min
     (setq _coloring_cc_min (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _coloring_cc_max
     (setq _coloring_cc_max (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; bool _discardOutOfElContext
     (setq _discardOutOfElContext (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _disp_processed_pc
     (setq _disp_processed_pc (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _disp_slam
     (setq _disp_slam (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; int8 _noise_level_db
     (setq _noise_level_db (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _noise_level_db 127) (setq _noise_level_db (- _noise_level_db 256)))
   ;; int32 _selectedAzimuthBin
     (setq _selectedAzimuthBin (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; bool _tx_started
     (setq _tx_started (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; int32 _radar_id
     (setq _radar_id (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; float32 _radar_x_offset
     (setq _radar_x_offset (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _radar_y_offset
     (setq _radar_y_offset (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _radar_z_offset
     (setq _radar_z_offset (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _radar_yaw_angle
     (setq _radar_yaw_angle (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _radar_pitch_angle
     (setq _radar_pitch_angle (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; bool _localization_active
     (setq _localization_active (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _aggregation_active
     (setq _aggregation_active (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; arbe_msgs/arbeSlamDisplaySettings _slam_display
     (send _slam_display :deserialize buf ptr-) (incf ptr- (send _slam_display :serialization-length))
   ;; arbe_msgs/arbeSettingsPerRadar _per_radar
     (send _per_radar :deserialize buf ptr-) (incf ptr- (send _per_radar :serialization-length))
   ;; bool _color_obj_by_class
     (setq _color_obj_by_class (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; int32 _marker_text_size
     (setq _marker_text_size (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeGUIsettings :md5sum-) "0ab680bfc9fa5170d24dd6773926eca5")
(setf (get arbe_msgs::arbeGUIsettings :datatype-) "arbe_msgs/arbeGUIsettings")
(setf (get arbe_msgs::arbeGUIsettings :definition-)
      "std_msgs/Header header
int32 mode
int32 range
int32 threshold3d
int32 threshold4d
int32 DynamicAzimuth
int32 DynamicElevation
float32 cam_euler_alpha
float32 cam_euler_beta
float32 cam_euler_gamma
bool  isNtc3dOn
bool  isNtc4dOn
bool  isCfar3dOn
bool  isCfar4dOn
bool  isPhaseEnabled
bool  isMetaDataEnabled
string ColoringType
float32 MinDoppler
float32 MaxDoppler
bool showFloatingText
float32 coloring_cc_min
float32 coloring_cc_max
bool discardOutOfElContext
bool disp_processed_pc
bool disp_slam
int8 noise_level_db
int32 selectedAzimuthBin
bool tx_started
int32 radar_id
float32 radar_x_offset
float32 radar_y_offset
float32 radar_z_offset
float32 radar_yaw_angle
float32 radar_pitch_angle
bool localization_active
bool aggregation_active
arbeSlamDisplaySettings slam_display
arbeSettingsPerRadar per_radar
bool color_obj_by_class
int32 marker_text_size



================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
string frame_id

================================================================================
MSG: arbe_msgs/arbeSlamDisplaySettings
bool slam_on_camera
bool disp_dist_on_cam
bool color_by_class
bool show_classification
bool disp_funnel
bool disp_FS_on_cam
bool disp_slam_eco_mode

================================================================================
MSG: arbe_msgs/arbeSettingsPerRadar
uint8 n_radars
int8  radar_for_text
float32[] ant_height
float32[] ant_pitch
float32[] cam_euler_a
float32[] cam_euler_b
float32[] cam_euler_g

")



(provide :arbe_msgs/arbeGUIsettings "0ab680bfc9fa5170d24dd6773926eca5")


