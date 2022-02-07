#ifndef SLM_MODEL_H_HEADER_HAS_BEEN_INCLUDED
#define SLM_MODEL_H_HEADER_HAS_BEEN_INCLUDED

#include "SLM_Export.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace slm
{

enum LaserMode {
    CW     = 0,
    PULSE  = 1
};

class SLM_EXPORT BuildStyle
{
    // Model can access protected data/methods
    friend class Model;

public:

    typedef std::shared_ptr<BuildStyle> Ptr;

    BuildStyle();
    ~BuildStyle();

    void setStyle(uint64_t bid,
                  float focus,
                  float power,
                  uint64_t pExpTime,
                  uint64_t pDistTime,
                  float speed = 0.f,
                  uint64_t laserId = 1,
                  LaserMode laserMode = LaserMode::PULSE);

    void setName(const std::u16string &str) { name = str;}
    void setDescription(const std::u16string &str) { description = str;}


public:
    uint64_t  id;
    uint64_t  laserId;
    uint64_t  laserMode;


    float   laserPower;
    float   laserFocus;
    float   laserSpeed;

    uint64_t  pointDistance;
    uint64_t  pointDelay;
    uint64_t  pointExposureTime;
    uint64_t  jumpSpeed;
    uint64_t  jumpDelay;

    std::u16string   name;
    std::u16string   description;
};

class SLM_EXPORT Model
{
public:
    Model();
    Model(uint64_t mid, uint64_t topSliceNum);
    ~Model();

public:
    typedef std::shared_ptr<Model> Ptr;
    typedef std::map<uint64_t, BuildStyle::Ptr> BStyleMap;

    enum BuildStyleID
    {
        CoreContour_Volume           = 1, /** core contour on volume */
        CoreContour_Overhang         = 2, /** core contour on powder */
        HollowShell1Contour_Volume   = 3,/** shell1 contour on volume */
        HollowShell1Contour_Overhang = 4,/** shell1 contour on powder */
        HollowShell2Contour_Volume   = 5,/** shell2 contour on volume */
        HollowShell2Contour_Overhang = 6,/** shell2 contour on powder */
        CoreOverhangHatch            = 7, /** core hatch on powder */
        CoreNormalHatch              = 8, /** core hatch on volume */
        CoreContourHatch             = 9, /** core contour hatch */
        HollowShell1OverhangHatch    = 10, /** shell1 hatch on powder */
        HollowShell1NormalHatch      = 11,  /** shell1 hatch on volume */
        HollowShell1ContourHatch     = 12, /** shell1 contour hatch */
        HollowShell2OverhangHatch    = 13, /** shell2 hatch on powder */
        HollowShell2NormalHatch      = 14,  /** shell2 hatch on volume */
        HollowShell2ContourHatch     = 15, /** shell2 contour hatch */
        SupportContourVolume         = 16, /** support contour */
        SupportHatch                 = 17,  /** support hatch */
        PointSequence                = 18, /** point sequence */
        ExternalSupports             = 19, /** Externe Stützen */
        CoreContourHatchOverhang     = 20, /** HollowCore Konturversatz - Overhang */
        HollowShell1ContourHatchOverhang = 21, /** HollowShell1  - Overhang */
        HollowShell2ContourHatchOverhang = 22, /** HollowShell2 Konturversatz - Overhang */
    };

    void clear();

    /**
     * Getters
     */
     uint64_t getId() const {  return id;}
     uint64_t getTopSlice() const { return topSliceNum; }

     std::u16string getName() const { return name; }
     std::string getNameAsString() const;

     std::u16string getBuildStyleName() const { return buildStyleName; }
     std::string getBuildStyleNameAsString() const;

     std::u16string getBuildStyleDescription() const { return buildStyleDescription; }
     std::string getBuildStlyeDescriptionAsString() const;


    /*
     * Build Style Getters
     */
     void setBuildStyles(const std::vector<BuildStyle::Ptr> &bstyles) { mBuildStyles = bstyles; }

     std::vector<BuildStyle::Ptr> getBuildStyles() const { return mBuildStyles; }
     BuildStyle::Ptr getBuildStyleById(const uint64_t bid) const;
     std::vector<BuildStyle::Ptr>  & buildStylesRef() { return mBuildStyles; }

    /**
     * Setters
     */
    int64_t addBuildStyle(BuildStyle::Ptr bstyle);
    //void setBuildStyles(const BStyleMap &bstyles);

    void setId(const uint64_t val) { id = val;}
    void setTopSlice(const uint64_t val) { topSliceNum = val;}
    void setName(const std::u16string &str) { name = str;}
    void setBuildStlyeName(const std::u16string &str) { buildStyleName = str;}
    void setBuildStlyeDescription(const std::u16string &str) { buildStyleDescription = str;}

protected:
    uint64_t    id;
    uint64_t    topSliceNum;
    std::u16string   name;
    std::u16string   buildStyleName;
    std::u16string   buildStyleDescription;
    //BStyleMap buildStyles;

    std::vector<BuildStyle::Ptr> mBuildStyles;
};

} // End of SLM Namespace

#endif // SLM_MODEL_H_HEADER_HAS_BEEN_INCLUDED
