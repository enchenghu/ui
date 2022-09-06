/*
 * Copyright (C) 2019 Arbe - http://www.arberobotics.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Arbe nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "Utils.h"
#include <arbe_msgs/arbeSlamMsg.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include "vis_utils.hpp"

static bool is_slam_valid = false;
static bool discardOutOfElevation = false;
static bool aggregate_pc = false;
static bool reset_local_frame = false;

bool get_slam_valid()
{
		return is_slam_valid;
}
bool set_slam_valid(bool valid)
{
		is_slam_valid = valid;
}

void set_discard_out_of_el_context(bool flag)
{
        discardOutOfElevation = flag;
}
bool get_discard_out_of_el_context()
{
        return discardOutOfElevation;
}

void set_aggregation(bool agg)
{
        aggregate_pc = agg;
}
bool get_aggregation()
{
        return aggregate_pc;
}

void set_reset_mapping(bool flag)
{
        reset_local_frame = flag;
}

bool get_reset_mapping()
{
        return reset_local_frame;
}
