// Generated by gencpp from file arbe_msgs/arbePcFrameHeader.msg
// DO NOT EDIT!


#ifndef ARBE_MSGS_MESSAGE_ARBEPCFRAMEHEADER_H
#define ARBE_MSGS_MESSAGE_ARBEPCFRAMEHEADER_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>

#include <arbe_msgs/arbeFrameTypes.h>

namespace arbe_msgs
{
template <class ContainerAllocator>
struct arbePcFrameHeader_
{
  typedef arbePcFrameHeader_<ContainerAllocator> Type;

  arbePcFrameHeader_()
    : time(0)
    , frame_counter(0)
    , is_4d(0)
    , frame_type(0)
    , udp_lost_packages(0)
    , number_of_points(0)
    , crd_count(0)
    , frame_types()  {
    }
  arbePcFrameHeader_(const ContainerAllocator& _alloc)
    : time(0)
    , frame_counter(0)
    , is_4d(0)
    , frame_type(0)
    , udp_lost_packages(0)
    , number_of_points(0)
    , crd_count(0)
    , frame_types(_alloc)  {
  (void)_alloc;
    }



   typedef uint64_t _time_type;
  _time_type time;

   typedef uint16_t _frame_counter_type;
  _frame_counter_type frame_counter;

   typedef uint8_t _is_4d_type;
  _is_4d_type is_4d;

   typedef uint8_t _frame_type_type;
  _frame_type_type frame_type;

   typedef uint16_t _udp_lost_packages_type;
  _udp_lost_packages_type udp_lost_packages;

   typedef uint32_t _number_of_points_type;
  _number_of_points_type number_of_points;

   typedef uint16_t _crd_count_type;
  _crd_count_type crd_count;

   typedef  ::arbe_msgs::arbeFrameTypes_<ContainerAllocator>  _frame_types_type;
  _frame_types_type frame_types;





  typedef boost::shared_ptr< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> const> ConstPtr;

}; // struct arbePcFrameHeader_

typedef ::arbe_msgs::arbePcFrameHeader_<std::allocator<void> > arbePcFrameHeader;

typedef boost::shared_ptr< ::arbe_msgs::arbePcFrameHeader > arbePcFrameHeaderPtr;
typedef boost::shared_ptr< ::arbe_msgs::arbePcFrameHeader const> arbePcFrameHeaderConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >::stream(s, "", v);
return s;
}


template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator==(const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator1> & lhs, const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator2> & rhs)
{
  return lhs.time == rhs.time &&
    lhs.frame_counter == rhs.frame_counter &&
    lhs.is_4d == rhs.is_4d &&
    lhs.frame_type == rhs.frame_type &&
    lhs.udp_lost_packages == rhs.udp_lost_packages &&
    lhs.number_of_points == rhs.number_of_points &&
    lhs.crd_count == rhs.crd_count &&
    lhs.frame_types == rhs.frame_types;
}

template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator!=(const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator1> & lhs, const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator2> & rhs)
{
  return !(lhs == rhs);
}


} // namespace arbe_msgs

namespace ros
{
namespace message_traits
{





template <class ContainerAllocator>
struct IsFixedSize< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
{
  static const char* value()
  {
    return "ea7e8a42429d20d4636665fbdb9eab60";
  }

  static const char* value(const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0xea7e8a42429d20d4ULL;
  static const uint64_t static_value2 = 0x636665fbdb9eab60ULL;
};

template<class ContainerAllocator>
struct DataType< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
{
  static const char* value()
  {
    return "arbe_msgs/arbePcFrameHeader";
  }

  static const char* value(const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
{
  static const char* value()
  {
    return "uint64 time\n"
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
;
  }

  static const char* value(const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.time);
      stream.next(m.frame_counter);
      stream.next(m.is_4d);
      stream.next(m.frame_type);
      stream.next(m.udp_lost_packages);
      stream.next(m.number_of_points);
      stream.next(m.crd_count);
      stream.next(m.frame_types);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct arbePcFrameHeader_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::arbe_msgs::arbePcFrameHeader_<ContainerAllocator>& v)
  {
    s << indent << "time: ";
    Printer<uint64_t>::stream(s, indent + "  ", v.time);
    s << indent << "frame_counter: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.frame_counter);
    s << indent << "is_4d: ";
    Printer<uint8_t>::stream(s, indent + "  ", v.is_4d);
    s << indent << "frame_type: ";
    Printer<uint8_t>::stream(s, indent + "  ", v.frame_type);
    s << indent << "udp_lost_packages: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.udp_lost_packages);
    s << indent << "number_of_points: ";
    Printer<uint32_t>::stream(s, indent + "  ", v.number_of_points);
    s << indent << "crd_count: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.crd_count);
    s << indent << "frame_types: ";
    s << std::endl;
    Printer< ::arbe_msgs::arbeFrameTypes_<ContainerAllocator> >::stream(s, indent + "  ", v.frame_types);
  }
};

} // namespace message_operations
} // namespace ros

#endif // ARBE_MSGS_MESSAGE_ARBEPCFRAMEHEADER_H
