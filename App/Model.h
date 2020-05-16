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

class SLM_EXPORT BuildStyle
{
    // Model can access protected data/methods
    friend class Model;

public:

    typedef std::shared_ptr<BuildStyle> Ptr;

    BuildStyle();
    ~BuildStyle();

    void setStyle(uint64_t bid,
                  uint64_t focus,
                  float power,
                  uint64_t pExpTime,
                  uint64_t pDistTime,
                  float speed = 0.f);

public:
    uint64_t  id;

    float   laserPower;
    float   laserFocus;
    float   laserSpeed;

    uint64_t  pointDistance;
    uint64_t  pointExposureTime;
};

class SLM_EXPORT Model
{
public:
    Model();
    Model(uint64_t mid, uint64_t sliceNum);
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
        /** Reserved for future use */
        idReserved23 = 23,
        idReserved24 = 24,
        idReserved25 = 25,
        idReserved26 = 26,
        idReserved27 = 27,
        idReserved28 = 28,
        idReserved29 = 29,
        idReserved30 = 30,
        idReserved31 = 31,
        idReserved32 = 32,
        /** Max enum value (all ID’s are smaller than this)
        * WILL BE REMOVED IN FUTURE VERSION */
        MAX = 33
    };

    void clear();

    /**
     * Getters
     */
     uint64_t getId() const {  return id;}
     uint64_t getTopSlice() const { return topSliceNum; }
     std::u16string getName() const { return name; }

     std::string getNameAsString() const;


    /*
     ** Build Style Getters
     */
     std::vector<BuildStyle::Ptr> getBuildStyles() const; // const{ return this->buildStyles.values(); }
     BuildStyle::Ptr getBuildStyleById(const uint64_t bid) const;// const { return this->buildStyles.value(bid);}

    /**
     * Setters
     */
    int64_t addBuildStyle(BuildStyle::Ptr bstyle);
    void setBuildStyles(const BStyleMap &bstyles);

    void setId(const uint64_t val) { id = val;}
    void setTopSlice(const uint64_t val) { topSliceNum = val;}
    void setName(const std::u16string &str) { name = str;}


protected:
    uint64_t    id;
    uint64_t    topSliceNum;
    std::u16string   name;
    BStyleMap buildStyles;
};

} // End of SLM Namespace

#endif // SLM_MODEL_H_HEADER_HAS_BEEN_INCLUDED
