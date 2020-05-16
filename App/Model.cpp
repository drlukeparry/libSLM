#include <codecvt>
#include <locale>
#include <string>

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
                           pointExposureTime(0)

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
              float speed)
{
    // Convenience function
    id = bid;
    laserFocus = focus;
    laserPower = power;
    laserSpeed = speed;
    pointExposureTime = pExpTime;
    pointDistance = pDistTime;
}


Model::Model() : id(0),
                 topSliceNum(0)
{
}

Model::Model(uint64_t mid, uint64_t sliceNum) : id(mid),
                                                topSliceNum(sliceNum)
{
}

Model::~Model()
{
}

std::string Model::getNameAsString() const
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> converter;

    return converter.to_bytes(name);
}


void Model::clear()
{
    this->buildStyles.clear();
}

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
