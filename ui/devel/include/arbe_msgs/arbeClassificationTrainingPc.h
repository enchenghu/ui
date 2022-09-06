// Generated by gencpp from file arbe_msgs/arbeClassificationTrainingPc.msg
// DO NOT EDIT!


#ifndef ARBE_MSGS_MESSAGE_ARBECLASSIFICATIONTRAININGPC_H
#define ARBE_MSGS_MESSAGE_ARBECLASSIFICATIONTRAININGPC_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>

#include <std_msgs/Header.h>
#include <arbe_msgs/arbeNewPcMsg.h>
#include <arbe_msgs/arbeFusionResponse.h>

namespace arbe_msgs
{
template <class ContainerAllocator>
struct arbeClassificationTrainingPc_
{
  typedef arbeClassificationTrainingPc_<ContainerAllocator> Type;

  arbeClassificationTrainingPc_()
    : Header()
    , Pc()
    , fusionResponse()
    , matching_com_indices()  {
    }
  arbeClassificationTrainingPc_(const ContainerAllocator& _alloc)
    : Header(_alloc)
    , Pc(_alloc)
    , fusionResponse(_alloc)
    , matching_com_indices(_alloc)  {
  (void)_alloc;
    }



   typedef  ::std_msgs::Header_<ContainerAllocator>  _Header_type;
  _Header_type Header;

   typedef  ::arbe_msgs::arbeNewPcMsg_<ContainerAllocator>  _Pc_type;
  _Pc_type Pc;

   typedef  ::arbe_msgs::arbeFusionResponse_<ContainerAllocator>  _fusionResponse_type;
  _fusionResponse_type fusionResponse;

   typedef std::vector<uint16_t, typename ContainerAllocator::template rebind<uint16_t>::other >  _matching_com_indices_type;
  _matching_com_indices_type matching_com_indices;





  typedef boost::shared_ptr< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> const> ConstPtr;

}; // struct arbeClassificationTrainingPc_

typedef ::arbe_msgs::arbeClassificationTrainingPc_<std::allocator<void> > arbeClassificationTrainingPc;

typedef boost::shared_ptr< ::arbe_msgs::arbeClassificationTrainingPc > arbeClassificationTrainingPcPtr;
typedef boost::shared_ptr< ::arbe_msgs::arbeClassificationTrainingPc const> arbeClassificationTrainingPcConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >::stream(s, "", v);
return s;
}


template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator==(const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator1> & lhs, const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator2> & rhs)
{
  return lhs.Header == rhs.Header &&
    lhs.Pc == rhs.Pc &&
    lhs.fusionResponse == rhs.fusionResponse &&
    lhs.matching_com_indices == rhs.matching_com_indices;
}

template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator!=(const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator1> & lhs, const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator2> & rhs)
{
  return !(lhs == rhs);
}


} // namespace arbe_msgs

namespace ros
{
namespace message_traits
{





template <class ContainerAllocator>
struct IsFixedSize< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
{
  static const char* value()
  {
    return "e9b7fe5d9944489810bf1fbe1b2cac94";
  }

  static const char* value(const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0xe9b7fe5d99444898ULL;
  static const uint64_t static_value2 = 0x10bf1fbe1b2cac94ULL;
};

template<class ContainerAllocator>
struct DataType< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
{
  static const char* value()
  {
    return "arbe_msgs/arbeClassificationTrainingPc";
  }

  static const char* value(const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
{
  static const char* value()
  {
    return "std_msgs/Header Header\n"
"arbeNewPcMsg Pc\n"
"arbeFusionResponse fusionResponse\n"
"uint16[] matching_com_indices\n"
"\n"
"================================================================================\n"
"MSG: std_msgs/Header\n"
"# Standard metadata for higher-level stamped data types.\n"
"# This is generally used to communicate timestamped data \n"
"# in a particular coordinate frame.\n"
"# \n"
"# sequence ID: consecutively increasing ID \n"
"uint32 seq\n"
"#Two-integer timestamp that is expressed as:\n"
"# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n"
"# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n"
"# time-handling sugar is provided by the client library\n"
"time stamp\n"
"#Frame this data is associated with\n"
"string frame_id\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbeNewPcMsg\n"
"std_msgs/Header RosHeader\n"
"arbePcFrameHeader PcHeader\n"
"arbePcMetadata PcMetadata\n"
"arbePcBins Points\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbePcFrameHeader\n"
"uint64 time\n"
"uint16 frame_counter\n"
"uint8  is_4d\n"
"uint8  frame_type\n"
"uint16 udp_lost_packages\n"
"uint32 number_of_points\n"
"uint16 crd_count\n"
"arbeFrameTypes frame_types\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbeFrameTypes\n"
"uint8 Frame_Type_3d_short = 1\n"
"uint8 Frame_Type_3d_mid = 2\n"
"uint8 Frame_Type_3d_long = 3\n"
"uint8 Frame_Type_3d_ultra_long = 13\n"
"uint8 Frame_Type_4d_short = 4\n"
"uint8 Frame_Type_4d_mid = 5\n"
"uint8 Frame_Type_4d_long = 6\n"
"uint8 Frame_Type_4d_ultra_long = 16\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbePcMetadata\n"
"arbePcRes PcResolution\n"
"float32 range_offset\n"
"float32 range_res_in_db\n"
"uint32 azimuth_fft_size\n"
"uint32 elevation_fft_size\n"
"uint32 azimuth_fft_padding\n"
"uint32 elevation_fft_padding\n"
"\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbePcRes\n"
"float32 range_resolution\n"
"float32 doppler_resolution\n"
"float32 azimuth_coefficient\n"
"float32 elevation_coefficient\n"
"\n"
"\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbePcBins\n"
"uint16[] range_bin\n"
"int16[] doppler_signed_bin\n"
"int8[] azimuth_signed_bin\n"
"int8[] elevation_signed_bin\n"
"int16[] phase\n"
"int16[] power\n"
"\n"
"\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbeFusionResponse\n"
"bool valid\n"
"uint16 obj_id\n"
"uint32 query_id\n"
"uint8 fusionClass\n"
"uint16 obj_index\n"
"uint64 queryTime\n"
"float32 conf\n"
"uint8 tta_count\n"
"float32[] tta_scores\n"
"string class_str\n"
"arbeFusionClasses fusionClasses\n"
"\n"
"uint16  top\n"
"uint16  left\n"
"uint16  bottom\n"
"uint16  right\n"
"\n"
"================================================================================\n"
"MSG: arbe_msgs/arbeFusionClasses\n"
"uint16 FC_BIKE = 0\n"
"uint16 FC_TRUCK_BUS = 1\n"
"uint16 FC_VRU = 2\n"
"uint16 FC_CAR = 3\n"
"uint16 FC_NOT_CLASSIFIED = 4\n"
"uint16 NUM_CLASSES = 4\n"
;
  }

  static const char* value(const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.Header);
      stream.next(m.Pc);
      stream.next(m.fusionResponse);
      stream.next(m.matching_com_indices);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct arbeClassificationTrainingPc_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::arbe_msgs::arbeClassificationTrainingPc_<ContainerAllocator>& v)
  {
    s << indent << "Header: ";
    s << std::endl;
    Printer< ::std_msgs::Header_<ContainerAllocator> >::stream(s, indent + "  ", v.Header);
    s << indent << "Pc: ";
    s << std::endl;
    Printer< ::arbe_msgs::arbeNewPcMsg_<ContainerAllocator> >::stream(s, indent + "  ", v.Pc);
    s << indent << "fusionResponse: ";
    s << std::endl;
    Printer< ::arbe_msgs::arbeFusionResponse_<ContainerAllocator> >::stream(s, indent + "  ", v.fusionResponse);
    s << indent << "matching_com_indices[]" << std::endl;
    for (size_t i = 0; i < v.matching_com_indices.size(); ++i)
    {
      s << indent << "  matching_com_indices[" << i << "]: ";
      Printer<uint16_t>::stream(s, indent + "  ", v.matching_com_indices[i]);
    }
  }
};

} // namespace message_operations
} // namespace ros

#endif // ARBE_MSGS_MESSAGE_ARBECLASSIFICATIONTRAININGPC_H
