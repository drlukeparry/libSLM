#include <string>
#include <algorithm>

#include "Utils.h"
#include "Model.h"

using namespace slm;

/*
 * Model Definition
 */

BuildStyle::BuildStyle() : id(0),
                           laserPower(0.0),
                           laserFocus(0.0),
                           laserSpeed(0.0),
                           pointDistance(0),
                           pointExposureTime(0),
                           laserId(1),
                           laserMode(1),
                           jumpSpeed(0),
                           jumpDelay(0)

{

}

BuildStyle::~BuildStyle()
{

}

void BuildStyle::setStyle(uint64_t bid,
              uint64_t focus,
              float power,
              uint64_t pExpTime,
              uint64_t pDistTime,
              float speed,
              uint64_t lId,
              LaserMode lMode)
{
    // Convenience function
    id = bid;
    laserFocus = focus;
    laserPower = power;
    laserSpeed = speed;
    pointExposureTime = pExpTime;
    pointDistance = pDistTime;
    laserId = lId;
    laserMode = lMode;

}


Model::Model() : id(0),
                 topSliceNum(0)
{
}

Model::Model(uint64_t mid, uint64_t topSliceNum) : id(mid),
                                                   topSliceNum(topSliceNum)
{
}

Model::~Model()
{
}

std::string Model::getNameAsString() const
{
    return UTF16toASCII(name);
}

std::string Model::getBuildStyleNameAsString() const
{
    return UTF16toASCII(buildStyleName);
}

std::string Model::getBuildStlyeDescriptionAsString() const
{
    return UTF16toASCII(buildStyleDescription);
}

void Model::clear()
{
    this->mBuildStyles.clear();
}

BuildStyle::Ptr Model::getBuildStyleById(const uint64_t bid) const
{
    auto result = std::find_if(std::begin(mBuildStyles), std::end(mBuildStyles),
                             [&](BuildStyle::Ptr bstyle){return bstyle->id == bid;});

    if (result != std::end(mBuildStyles)) {
        return *result;
    } else {
        return BuildStyle::Ptr(nullptr);
    }

}

int64_t Model::addBuildStyle(BuildStyle::Ptr bstyle)
{
    if(!bstyle)
        return -1;

    auto result = std::find_if(std::begin(mBuildStyles), std::end(mBuildStyles),
                             [&](BuildStyle::Ptr b){return bstyle->id == b->id;});

    if (result != std::end(mBuildStyles))
        return -1;

    mBuildStyles.push_back(bstyle);

    return mBuildStyles.size();
}

#if 0
std::vector<BuildStyle::Ptr> Model::getBuildStyles() const
{
    std::vector<BuildStyle::Ptr> bstyles;
    bstyles.reserve(buildStyles.size());

    // Range iterator
    for(auto const& imap: buildStyles)
        bstyles.push_back(imap.second);

    return bstyles;
}

BuildStyle::Ptr Model::getBuildStyleById(const uint64_t bid) const
{
    if(buildStyles.count(bid))
        return buildStyles.at(bid);
    else
        return BuildStyle::Ptr(nullptr);
}

void Model::setBuildStyles(const BStyleMap &bstyles)
{
    buildStyles = bstyles;
}

int64_t Model::addBuildStyle(BuildStyle::Ptr bstyle)
{
    if(!bstyle)
        return -1;

    if(!buildStyles.count(bstyle->id))
        buildStyles.insert(BStyleMap::value_type(bstyle->id, bstyle));

    return buildStyles.size();
}
#endif
