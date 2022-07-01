#include "../environment.h"

int Environment::get_gpu_device_choice ()
{
    if (using_gpu())
        return gpu_device_id_;
    else
        return -1;  // GPU was not requested so return an invalid device ID -1
}

void Environment::set_use_gpu(bool yes)
{
    use_gpu_ = yes;
}

bool Environment::using_gpu()
{
    return use_gpu_;
}

