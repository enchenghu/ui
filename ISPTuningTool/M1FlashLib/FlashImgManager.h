#ifndef FLASH_IMG_MANAGER_H
#define FLASH_IMG_MANAGER_H

#include "FlashImage.h"
#include <functional>
#include <memory>
#include <vector>


using FlashImagePtr = std::unique_ptr<FlashImage>;

// singleton class
class FlashImgManager
{
public:
    static int create(FlashImg *img)
    {
        if(img == nullptr)
            return ERROR_PARAM_NULL;
        auto tmpImg = std::make_unique<FlashImage>();
        getInstance()._flashImages.push_back(std::move(tmpImg));
        *img = reinterpret_cast<FlashImg>(getInstance()._flashImages.back().get());
        return ERROR_NO_ERR;
    }

    static int create(FlashImg *img, const char *path)
    {
        if (img == nullptr || path == nullptr) 
            return ERROR_PARAM_NULL;
        auto tmpImg = std::make_unique<FlashImage>();
        int  res    = tmpImg->readFromImageFile(path);
        if (res != ERROR_NO_ERR) 
            return res;
        getInstance()._flashImages.push_back(std::move(tmpImg));
        *img = reinterpret_cast<FlashImg>(getInstance()._flashImages.back().get());
        return res;
    }

    static int create(FlashImg *img, int deviceIndex)
    {
        if (img == nullptr) 
            return ERROR_PARAM_NULL;
        auto tmpImg = std::make_unique<FlashImage>();
        int  res    = tmpImg->readFromDevice(deviceIndex);
        if (res != ERROR_NO_ERR) 
            return res;
        getInstance()._flashImages.push_back(std::move(tmpImg));
        *img = reinterpret_cast<FlashImg>(getInstance()._flashImages.back().get());
        return res;
    }

    static int release(FlashImg img)
    {
        if (img == nullptr) {
            return ERROR_PARAM_NULL;
        }
        for (auto it = getInstance()._flashImages.begin(); it != getInstance()._flashImages.end(); ++it) {
            if (img == reinterpret_cast<FlashImg>(it->get())) {
                getInstance()._flashImages.erase(it);
                return ERROR_NO_ERR;
            }
        }
        return ERROR_IMG_INVALID;
    }

    template <typename Func, typename... Args>
    static int execute(FlashImg img, Func func, Args &&... args)
    {
        if (img == nullptr) 
            return ERROR_PARAM_NULL;
        if (valid(img)) {
            return std::bind(func, getFlashImage(img).get(), std::forward<Args>(args)...)();
        }
        return ERROR_IMG_INVALID;
    }

private:
    FlashImgManager() { }

    static FlashImgManager &getInstance()
    {
        static FlashImgManager instance;
        return instance;
    }

    static bool valid(FlashImg img)
    {
        for (auto &flashImg : getInstance()._flashImages) {
            if (img == reinterpret_cast<FlashImg>(flashImg.get())) {
                return true;
            }
        }
        return false;
    }

    static FlashImagePtr &getFlashImage(FlashImg img)
    {
        if (valid(img)) {
            for (auto &flashImg : getInstance()._flashImages) {
                if (img == reinterpret_cast<FlashImg>(flashImg.get())) {
                    return flashImg;
                }
            }
        }
        static FlashImagePtr nullPtr;
        return nullPtr;
    }

    std::vector<FlashImagePtr> _flashImages;
};

#endif // FLASH_IMG_MANAGER_H