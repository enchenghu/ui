#include <chrono>			//c++11日期和时间库		
#include <functional>		//参见 注1
#include <memory>			//c++内存管理库，使用智能指针必须包含此头文件
#include <string>

#include "rclcpp/rclcpp.hpp"	//ROS2，C++接口的头文件
#include "std_msgs/msg/string.hpp"	//string类型的msg头文件

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

//定义一个节点类的子类
class MinimalPublisher : public rclcpp::Node
{
  public:
	  	//构造函数：初始化节点名 minimal_publisher，然后创建了一个publisher和定时器
	    MinimalPublisher(): Node("minimal_publisher"), count_(0)
	    {
	    	//创建一个发布者，发布的话题名为topic，话题消息是String，保存消息的队列长度是10
      		publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
	      	//创建一个定时器，定时500ms，定时触发回调函数timer_callback。
	      	//这里用到了<functional>的一个特性
	      	timer_ = this->create_wall_timer(500ms, std::bind(&MinimalPublisher::timer_callback, this));
	    }

  private:
  		//定时触发回调函数
	    void timer_callback()
	    {
	    	//定义消息
		    auto message = std_msgs::msg::String();
		    message.data = "Hello, world! " + std::to_string(count_++);
		    //打印日志
		    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
		    //发布
		    publisher_->publish(message);
	    }
    
    	//定义定时器和发布者指针
	    rclcpp::TimerBase::SharedPtr timer_;
	    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
	    
	    size_t count_;
  };

  int main(int argc, char * argv[])
  {
	    rclcpp::init(argc, argv);
	    rclcpp::spin(std::make_shared<MinimalPublisher>());
	    rclcpp::shutdown();
	    return 0;
  }
