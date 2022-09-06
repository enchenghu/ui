;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeSlamDisplaySettings)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeSlamDisplaySettings (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBESLAMDISPLAYSETTINGS")
  (make-package "ARBE_MSGS::ARBESLAMDISPLAYSETTINGS"))

(in-package "ROS")
;;//! \htmlinclude arbeSlamDisplaySettings.msg.html


(defclass arbe_msgs::arbeSlamDisplaySettings
  :super ros::object
  :slots (_slam_on_camera _disp_dist_on_cam _color_by_class _show_classification _disp_funnel _disp_FS_on_cam _disp_slam_eco_mode ))

(defmethod arbe_msgs::arbeSlamDisplaySettings
  (:init
   (&key
    ((:slam_on_camera __slam_on_camera) nil)
    ((:disp_dist_on_cam __disp_dist_on_cam) nil)
    ((:color_by_class __color_by_class) nil)
    ((:show_classification __show_classification) nil)
    ((:disp_funnel __disp_funnel) nil)
    ((:disp_FS_on_cam __disp_FS_on_cam) nil)
    ((:disp_slam_eco_mode __disp_slam_eco_mode) nil)
    )
   (send-super :init)
   (setq _slam_on_camera __slam_on_camera)
   (setq _disp_dist_on_cam __disp_dist_on_cam)
   (setq _color_by_class __color_by_class)
   (setq _show_classification __show_classification)
   (setq _disp_funnel __disp_funnel)
   (setq _disp_FS_on_cam __disp_FS_on_cam)
   (setq _disp_slam_eco_mode __disp_slam_eco_mode)
   self)
  (:slam_on_camera
   (&optional __slam_on_camera)
   (if __slam_on_camera (setq _slam_on_camera __slam_on_camera)) _slam_on_camera)
  (:disp_dist_on_cam
   (&optional __disp_dist_on_cam)
   (if __disp_dist_on_cam (setq _disp_dist_on_cam __disp_dist_on_cam)) _disp_dist_on_cam)
  (:color_by_class
   (&optional __color_by_class)
   (if __color_by_class (setq _color_by_class __color_by_class)) _color_by_class)
  (:show_classification
   (&optional __show_classification)
   (if __show_classification (setq _show_classification __show_classification)) _show_classification)
  (:disp_funnel
   (&optional __disp_funnel)
   (if __disp_funnel (setq _disp_funnel __disp_funnel)) _disp_funnel)
  (:disp_FS_on_cam
   (&optional __disp_FS_on_cam)
   (if __disp_FS_on_cam (setq _disp_FS_on_cam __disp_FS_on_cam)) _disp_FS_on_cam)
  (:disp_slam_eco_mode
   (&optional __disp_slam_eco_mode)
   (if __disp_slam_eco_mode (setq _disp_slam_eco_mode __disp_slam_eco_mode)) _disp_slam_eco_mode)
  (:serialization-length
   ()
   (+
    ;; bool _slam_on_camera
    1
    ;; bool _disp_dist_on_cam
    1
    ;; bool _color_by_class
    1
    ;; bool _show_classification
    1
    ;; bool _disp_funnel
    1
    ;; bool _disp_FS_on_cam
    1
    ;; bool _disp_slam_eco_mode
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; bool _slam_on_camera
       (if _slam_on_camera (write-byte -1 s) (write-byte 0 s))
     ;; bool _disp_dist_on_cam
       (if _disp_dist_on_cam (write-byte -1 s) (write-byte 0 s))
     ;; bool _color_by_class
       (if _color_by_class (write-byte -1 s) (write-byte 0 s))
     ;; bool _show_classification
       (if _show_classification (write-byte -1 s) (write-byte 0 s))
     ;; bool _disp_funnel
       (if _disp_funnel (write-byte -1 s) (write-byte 0 s))
     ;; bool _disp_FS_on_cam
       (if _disp_FS_on_cam (write-byte -1 s) (write-byte 0 s))
     ;; bool _disp_slam_eco_mode
       (if _disp_slam_eco_mode (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; bool _slam_on_camera
     (setq _slam_on_camera (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _disp_dist_on_cam
     (setq _disp_dist_on_cam (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _color_by_class
     (setq _color_by_class (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _show_classification
     (setq _show_classification (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _disp_funnel
     (setq _disp_funnel (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _disp_FS_on_cam
     (setq _disp_FS_on_cam (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _disp_slam_eco_mode
     (setq _disp_slam_eco_mode (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeSlamDisplaySettings :md5sum-) "137b1eba43f6932882c483d9b1c16de3")
(setf (get arbe_msgs::arbeSlamDisplaySettings :datatype-) "arbe_msgs/arbeSlamDisplaySettings")
(setf (get arbe_msgs::arbeSlamDisplaySettings :definition-)
      "bool slam_on_camera
bool disp_dist_on_cam
bool color_by_class
bool show_classification
bool disp_funnel
bool disp_FS_on_cam
bool disp_slam_eco_mode

")



(provide :arbe_msgs/arbeSlamDisplaySettings "137b1eba43f6932882c483d9b1c16de3")


