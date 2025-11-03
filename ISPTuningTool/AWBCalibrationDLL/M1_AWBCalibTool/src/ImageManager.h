#include "Image.h"
#include "M1AWBTypeDef.h"
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
using ImagePtr = std::shared_ptr<Image>;
class ImageManager
{
public:
    static int create(Img *img)
    {
        if (img == nullptr)
            return ERROR_PARAM_NULL;
        auto tmpImg = std::make_shared<Image>();
        getInstance()._Images.push_back(std::move(tmpImg));
        *img = reinterpret_cast<Img>(getInstance()._Images.back().get());
        return ERROR_NO_ERR;
    }

    static int release(Img img)
    {
        if (img == nullptr)
            return ERROR_PARAM_NULL;

        for (auto it = getInstance()._Images.begin(); it != getInstance()._Images.end(); ++it)
        {
            if (img == reinterpret_cast<Img>(it->get()))
            {
                getInstance()._Images.erase(it);
                return ERROR_NO_ERR;
            }
        }
        return ERROR_IMG_INVALID;
    }

    static int releaseGroup(int groupId)
    {
        for (auto it = getInstance()._Images.begin(); it != getInstance()._Images.end();)
        {
            int ID=it->get()->groupId();
            if (ID == groupId)
            {
                it=getInstance()._Images.erase(it);
            }else
            {
                it++;
            }
        }
        return ERROR_NO_ERR;
    }

    static int clearAll()
    {
        getInstance()._Images.clear();
        return ERROR_IMG_INVALID;
    }

    template <typename Func, typename... Args>
    static int execute(Img img, Func func, Args &&...args)
    {
        if (img == nullptr)
            return ERROR_PARAM_NULL;
        if (valid(img))
        {
            return std::bind(func, getImage(img).get(), std::forward<Args>(args)...)();
        }
        return ERROR_IMG_INVALID;
    }

    static int setImageInfo(int groupId, Img img, uint16_t *data, int dLength, ImageInfo info)
    {
        if (img == nullptr||data==nullptr)
            return ERROR_PARAM_NULL;
        auto _img = getImage(img);
        if (_img == nullptr)
            return ERROR_IMG_INVALID;
        return _img.get()->initImage(groupId, data, dLength, info);
    }
    static int calculateWBGain(Img img)
    {
        if (img == nullptr)
            return ERROR_PARAM_NULL;
        auto _img = getImage(img);
        if (_img == nullptr)
            return ERROR_IMG_INVALID;
        return _img.get()->autoCalibration();
    }
    static int addROI(Img img, int64_t left, int64_t top, int64_t right, int64_t bottom, RoiType type)
    {
        if (img == nullptr)
            return ERROR_PARAM_NULL;
        auto _img = getImage(img);
        if (_img == nullptr)
            return ERROR_IMG_INVALID;
        return _img.get()->addImageROI(left, top, right, bottom, type);
    }
    static int getImageROI(Img img, int64_t *left, int64_t *top, int64_t *right, int64_t *bottom, int size)
    {
        if (img == nullptr || left == nullptr || top == nullptr || right == nullptr || bottom == nullptr)
            return ERROR_PARAM_NULL;
        auto _img = getImage(img);
        if (_img == nullptr)
            return ERROR_IMG_INVALID;
        std::vector<int64_t> Left;
        std::vector<int64_t> Top;
        std::vector<int64_t> Right;
        std::vector<int64_t> Bottom;
        auto res = _img.get()->getImageROI(Left, Top, Right, Bottom, size);
        if (res == ERROR_NO_ERR)
        {
            memcpy(left, Left.data(), sizeof(int64_t) * size);
            memcpy(top, Top.data(), sizeof(int64_t) * size);
            memcpy(right, Right.data(), sizeof(int64_t) * size);
            memcpy(bottom, Bottom.data(), sizeof(int64_t) * size);
        }
        return res;
    }
    static int getImageAfterAWBCalib(Img img, uint16_t *data, uint32_t dLength)
    {
        if (img == nullptr)
            return ERROR_PARAM_NULL;
        auto _img = getImage(img);
        if (_img == nullptr)
            return ERROR_IMG_INVALID;
        return _img.get()->getImageAfterAWBCalib(data, dLength);
    }
    static int calGreenHighLumaROI(int groupId, Img img)
    {
        if (img == nullptr)
            return ERROR_PARAM_NULL;
        auto _img = getImage(img);
        if (_img == nullptr)
            return ERROR_IMG_INVALID;
        return _img.get()->calGreenHighLumaROI();
    }

    

private:
    ImageManager() {}

    static ImageManager &getInstance()
    {
        static ImageManager instance;
        return instance;
    }

    static bool valid(Img img)
    {
        for (int i = 0; i < getInstance()._Images.size(); ++i)
        {
            if (img == reinterpret_cast<Img>(getInstance()._Images.at(i).get()))
            {
                return true;
            }
        }
        return false;
    }

    static ImagePtr &getImage(Img img)
    {
        if (valid(img)) {
            for (int i = 0; i < getInstance()._Images.size(); ++i)
            {
                if (img == reinterpret_cast<Img>(getInstance()._Images.at(i).get()))
                {
                    return getInstance()._Images.at(i);
                }
            }
        }
        static ImagePtr nullPtr;
        return nullPtr;
    }


    std::vector<ImagePtr> _Images; // all image
};
