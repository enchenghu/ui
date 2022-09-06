// Generated by gencpp from file arbe_msgs/arbeFusionResponsesVec.msg
// DO NOT EDIT!


#ifndef ARBE_MSGS_MESSAGE_ARBEFUSIONRESPONSESVEC_H
#define ARBE_MSGS_MESSAGE_ARBEFUSIONRESPONSESVEC_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>

#include <std_msgs/Header.h>
#include <arbe_msgs/arbeFusionResponse.h>

namespace arbe_msgs
{
template <class ContainerAllocator>
struct arbeFusionResponsesVec_
{
  typedef arbeFusionResponsesVec_<ContainerAllocator> Type;

  arbeFusionResponsesVec_()
    : header()
    , dt(0)
    , responses()  {
    }
  arbeFusionResponsesVec_(const ContainerAllocator& _alloc)
    : header(_alloc)
    , dt(0)
    , responses(_alloc)  {
  (void)_alloc;
    }



   typedef  ::std_msgs::Header_<ContainerAllocator>  _header_type;
  _header_type header;

   typedef uint64_t _dt_type;
  _dt_type dt;

   typedef std::vector< ::arbe_msgs::arbeFusionResponse_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::arbe_msgs::arbeFusionResponse_<ContainerAllocator> >::other >  _responses_type;
  _responses_type responses;





  typedef boost::shared_ptr< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> const> ConstPtr;

}; // struct arbeFusionResponsesVec_

typedef ::arbe_msgs::arbeFusionResponsesVec_<std::allocator<void> > arbeFusionResponsesVec;

typedef boost::shared_ptr< ::arbe_msgs::arbeFusionResponsesVec > arbeFusionResponsesVecPtr;
typedef boost::shared_ptr< ::arbe_msgs::arbeFusionResponsesVec const> arbeFusionResponsesVecConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >::stream(s, "", v);
return s;
}


template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator==(const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator1> & lhs, const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator2> & rhs)
{
  return lhs.header == rhs.header &&
    lhs.dt == rhs.dt &&
    lhs.responses == rhs.responses;
}

template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator!=(const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator1> & lhs, const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator2> & rhs)
{
  return !(lhs == rhs);
}


} // namespace arbe_msgs

namespace ros
{
namespace message_traits
{





template <class ContainerAllocator>
struct IsFixedSize< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> const>
  : TrueType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
{
  static const char* value()
  {
    return "e85897c08185dc25f773baf17831351f";
  }

  static const char* value(const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0xe85897c08185dc25ULL;
  static const uint64_t static_value2 = 0xf773baf17831351fULL;
};

template<class ContainerAllocator>
struct DataType< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
{
  static const char* value()
  {
    return "arbe_msgs/arbeFusionResponsesVec";
  }

  static const char* value(const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
{
  static const char* value()
  {
    return "std_msgs/Header header\n"
"uint64 dt\n"
"arbeFusionResponse[] responses\n"
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

  static const char* value(const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.header);
      stream.next(m.dt);
      stream.next(m.responses);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct arbeFusionResponsesVec_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::arbe_msgs::arbeFusionResponsesVec_<ContainerAllocator>& v)
  {
    s << indent << "header: ";
    s << std::endl;
    Printer< ::std_msgs::Header_<ContainerAllocator> >::stream(s, indent + "  ", v.header);
    s << indent << "dt: ";
    Printer<uint64_t>::stream(s, indent + "  ", v.dt);
    s << indent << "responses[]" << std::endl;
    for (size_t i = 0; i < v.responses.size(); ++i)
    {
      s << indent << "  responses[" << i << "]: ";
      s << std::endl;
      s << indent;
      Printer< ::arbe_msgs::arbeFusionResponse_<ContainerAllocator> >::stream(s, indent + "    ", v.responses[i]);
    }
  }
};

} // namespace message_operations
} // namespace ros

#endif // ARBE_MSGS_MESSAGE_ARBEFUSIONRESPONSESVEC_H
