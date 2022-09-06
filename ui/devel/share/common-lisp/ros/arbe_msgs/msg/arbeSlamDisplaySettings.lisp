; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeSlamDisplaySettings.msg.html

(cl:defclass <arbeSlamDisplaySettings> (roslisp-msg-protocol:ros-message)
  ((slam_on_camera
    :reader slam_on_camera
    :initarg :slam_on_camera
    :type cl:boolean
    :initform cl:nil)
   (disp_dist_on_cam
    :reader disp_dist_on_cam
    :initarg :disp_dist_on_cam
    :type cl:boolean
    :initform cl:nil)
   (color_by_class
    :reader color_by_class
    :initarg :color_by_class
    :type cl:boolean
    :initform cl:nil)
   (show_classification
    :reader show_classification
    :initarg :show_classification
    :type cl:boolean
    :initform cl:nil)
   (disp_funnel
    :reader disp_funnel
    :initarg :disp_funnel
    :type cl:boolean
    :initform cl:nil)
   (disp_FS_on_cam
    :reader disp_FS_on_cam
    :initarg :disp_FS_on_cam
    :type cl:boolean
    :initform cl:nil)
   (disp_slam_eco_mode
    :reader disp_slam_eco_mode
    :initarg :disp_slam_eco_mode
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass arbeSlamDisplaySettings (<arbeSlamDisplaySettings>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeSlamDisplaySettings>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeSlamDisplaySettings)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeSlamDisplaySettings> is deprecated: use arbe_msgs-msg:arbeSlamDisplaySettings instead.")))

(cl:ensure-generic-function 'slam_on_camera-val :lambda-list '(m))
(cl:defmethod slam_on_camera-val ((m <arbeSlamDisplaySettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:slam_on_camera-val is deprecated.  Use arbe_msgs-msg:slam_on_camera instead.")
  (slam_on_camera m))

(cl:ensure-generic-function 'disp_dist_on_cam-val :lambda-list '(m))
(cl:defmethod disp_dist_on_cam-val ((m <arbeSlamDisplaySettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:disp_dist_on_cam-val is deprecated.  Use arbe_msgs-msg:disp_dist_on_cam instead.")
  (disp_dist_on_cam m))

(cl:ensure-generic-function 'color_by_class-val :lambda-list '(m))
(cl:defmethod color_by_class-val ((m <arbeSlamDisplaySettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:color_by_class-val is deprecated.  Use arbe_msgs-msg:color_by_class instead.")
  (color_by_class m))

(cl:ensure-generic-function 'show_classification-val :lambda-list '(m))
(cl:defmethod show_classification-val ((m <arbeSlamDisplaySettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:show_classification-val is deprecated.  Use arbe_msgs-msg:show_classification instead.")
  (show_classification m))

(cl:ensure-generic-function 'disp_funnel-val :lambda-list '(m))
(cl:defmethod disp_funnel-val ((m <arbeSlamDisplaySettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:disp_funnel-val is deprecated.  Use arbe_msgs-msg:disp_funnel instead.")
  (disp_funnel m))

(cl:ensure-generic-function 'disp_FS_on_cam-val :lambda-list '(m))
(cl:defmethod disp_FS_on_cam-val ((m <arbeSlamDisplaySettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:disp_FS_on_cam-val is deprecated.  Use arbe_msgs-msg:disp_FS_on_cam instead.")
  (disp_FS_on_cam m))

(cl:ensure-generic-function 'disp_slam_eco_mode-val :lambda-list '(m))
(cl:defmethod disp_slam_eco_mode-val ((m <arbeSlamDisplaySettings>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:disp_slam_eco_mode-val is deprecated.  Use arbe_msgs-msg:disp_slam_eco_mode instead.")
  (disp_slam_eco_mode m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeSlamDisplaySettings>) ostream)
  "Serializes a message object of type '<arbeSlamDisplaySettings>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'slam_on_camera) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'disp_dist_on_cam) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'color_by_class) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'show_classification) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'disp_funnel) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'disp_FS_on_cam) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'disp_slam_eco_mode) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeSlamDisplaySettings>) istream)
  "Deserializes a message object of type '<arbeSlamDisplaySettings>"
    (cl:setf (cl:slot-value msg 'slam_on_camera) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'disp_dist_on_cam) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'color_by_class) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'show_classification) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'disp_funnel) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'disp_FS_on_cam) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'disp_slam_eco_mode) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeSlamDisplaySettings>)))
  "Returns string type for a message object of type '<arbeSlamDisplaySettings>"
  "arbe_msgs/arbeSlamDisplaySettings")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeSlamDisplaySettings)))
  "Returns string type for a message object of type 'arbeSlamDisplaySettings"
  "arbe_msgs/arbeSlamDisplaySettings")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeSlamDisplaySettings>)))
  "Returns md5sum for a message object of type '<arbeSlamDisplaySettings>"
  "137b1eba43f6932882c483d9b1c16de3")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeSlamDisplaySettings)))
  "Returns md5sum for a message object of type 'arbeSlamDisplaySettings"
  "137b1eba43f6932882c483d9b1c16de3")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeSlamDisplaySettings>)))
  "Returns full string definition for message of type '<arbeSlamDisplaySettings>"
  (cl:format cl:nil "bool slam_on_camera~%bool disp_dist_on_cam~%bool color_by_class~%bool show_classification~%bool disp_funnel~%bool disp_FS_on_cam~%bool disp_slam_eco_mode~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeSlamDisplaySettings)))
  "Returns full string definition for message of type 'arbeSlamDisplaySettings"
  (cl:format cl:nil "bool slam_on_camera~%bool disp_dist_on_cam~%bool color_by_class~%bool show_classification~%bool disp_funnel~%bool disp_FS_on_cam~%bool disp_slam_eco_mode~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeSlamDisplaySettings>))
  (cl:+ 0
     1
     1
     1
     1
     1
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeSlamDisplaySettings>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeSlamDisplaySettings
    (cl:cons ':slam_on_camera (slam_on_camera msg))
    (cl:cons ':disp_dist_on_cam (disp_dist_on_cam msg))
    (cl:cons ':color_by_class (color_by_class msg))
    (cl:cons ':show_classification (show_classification msg))
    (cl:cons ':disp_funnel (disp_funnel msg))
    (cl:cons ':disp_FS_on_cam (disp_FS_on_cam msg))
    (cl:cons ':disp_slam_eco_mode (disp_slam_eco_mode msg))
))
