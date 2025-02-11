//  SPDX-License-Identifier: MIT
//
//  ES-DE Frontend
//  ThemeData.cpp
//
//  Finds available themes on the file system and loads and parses these.
//  Basic error checking for valid elements and data types is done here,
//  with additional validation handled by the individual components.
//

#include "ThemeData.h"

#include "Log.h"
#include "Settings.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "utils/FileSystemUtil.h"
#include "utils/LocalizationUtil.h"
#include "utils/StringUtil.h"

#include <algorithm>
#include <pugixml.hpp>

// clang-format off
std::vector<std::string> ThemeData::sSupportedViews {
    {"all"},
    {"system"},
    {"gamelist"}};

std::vector<std::string> ThemeData::sSupportedMediaTypes {
    {"miximage"},
    {"marquee"},
    {"screenshot"},
    {"titlescreen"},
    {"cover"},
    {"backcover"},
    {"3dbox"},
    {"physicalmedia"},
    {"fanart"},
    {"video"}};

std::vector<std::string> ThemeData::sSupportedTransitions {
    {"systemToSystem"},
    {"systemToGamelist"},
    {"gamelistToGamelist"},
    {"gamelistToSystem"},
    {"startupToSystem"},
    {"startupToGamelist"}};

std::vector<std::string> ThemeData::sSupportedTransitionAnimations {
    {"builtin-instant"},
    {"builtin-slide"},
    {"builtin-fade"}};

std::vector<std::pair<std::string, std::string>> ThemeData::sSupportedFontSizes {
    {"medium", "medium"},
    {"large", "large"},
    {"small", "small"},
    {"x-large", "extra large"},
    {"x-small", "extra small"}};

std::vector<std::pair<std::string, std::string>> ThemeData::sSupportedAspectRatios {
    {"automatic", "automatic"},
    {"16:9", "16:9"},
    {"16:9_vertical", "16:9 vertical"},
    {"16:10", "16:10"},
    {"16:10_vertical", "16:10 vertical"},
    {"3:2", "3:2"},
    {"3:2_vertical", "3:2 vertical"},
    {"4:3", "4:3"},
    {"4:3_vertical", "4:3 vertical"},
    {"5:4", "5:4"},
    {"5:4_vertical", "5:4 vertical"},
    {"19.5:9", "19.5:9"},
    {"19.5:9_vertical", "19.5:9 vertical"},
    {"20:9", "20:9"},
    {"20:9_vertical", "20:9 vertical"},
    {"21:9", "21:9"},
    {"21:9_vertical", "21:9 vertical"},
    {"32:9", "32:9"},
    {"32:9_vertical", "32:9 vertical"},
    {"1:1", "1:1"}};

std::map<std::string, float> ThemeData::sAspectRatioMap {
    {"16:9", 1.7777f},
    {"16:9_vertical", 0.5625f},
    {"16:10", 1.6f},
    {"16:10_vertical", 0.625f},
    {"3:2", 1.5f},
    {"3:2_vertical", 0.6667f},
    {"4:3", 1.3333f},
    {"4:3_vertical", 0.75f},
    {"5:4", 1.25f},
    {"5:4_vertical", 0.8f},
    {"19.5:9", 2.1667f},
    {"19.5:9_vertical", 0.4615f},
    {"20:9", 2.2222f},
    {"20:9_vertical", 0.45f},
    {"21:9", 2.3703f},
    {"21:9_vertical", 0.4219f},
    {"32:9", 3.5555f},
    {"32:9_vertical", 0.2813f},
    {"1:1", 1.0f}};

std::vector<std::pair<std::string, std::string>> ThemeData::sSupportedLanguages {
    {"automatic", "automatic"},
    {"en_US", "ENGLISH (UNITED STATES)"},
    {"en_GB", "ENGLISH (UNITED KINGDOM)"},
    {"ca_ES", "CATALÀ"},
    {"de_DE", "DEUTSCH"},
    {"es_ES", "ESPAÑOL (ESPAÑA)"},
    {"fr_FR", "FRANÇAIS"},
    {"it_IT", "ITALIANO"},
    {"nl_NL", "NEDERLANDS"},
    {"pl_PL", "POLSKI"},
    {"pt_BR", "PORTUGUÊS (BRASIL)"},
    {"ro_RO", "ROMÂNĂ"},
    {"ru_RU", "РУССКИЙ"},
    {"sv_SE", "SVENSKA"},
    {"ja_JP", "日本語"},
    {"ko_KR", "한국어"},
    {"zh_CN", "简体中文"},
    {"zh_TW", "繁體中文"}};

std::map<std::string, std::map<std::string, std::string>> ThemeData::sPropertyAttributeMap
    // The data type is defined by the parent property.
    {
     {"badges",
      {{"customBadgeIcon", "badge"},
       {"customControllerIcon", "controller"}}},
     {"helpsystem",
      {{"customButtonIcon", "button"}}},
    };

std::map<std::string, std::map<std::string, ThemeData::ElementPropertyType>>
    ThemeData::sElementMap {
     {"carousel",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"type", STRING},
       {"staticImage", PATH},
       {"imageType", STRING},
       {"defaultImage", PATH},
       {"defaultFolderImage", PATH},
       {"maxItemCount", FLOAT},
       {"itemsBeforeCenter", UNSIGNED_INTEGER},
       {"itemsAfterCenter", UNSIGNED_INTEGER},
       {"itemStacking", STRING},
       {"selectedItemMargins", NORMALIZED_PAIR},
       {"selectedItemOffset", NORMALIZED_PAIR},
       {"itemSize", NORMALIZED_PAIR},
       {"itemScale", FLOAT},
       {"itemRotation", FLOAT},
       {"itemRotationOrigin", NORMALIZED_PAIR},
       {"itemAxisHorizontal", BOOLEAN},
       {"itemAxisRotation", FLOAT},
       {"imageFit", STRING},
       {"imageCropPos", NORMALIZED_PAIR},
       {"imageInterpolation", STRING},
       {"imageCornerRadius", FLOAT},
       {"imageColor", COLOR},
       {"imageColorEnd", COLOR},
       {"imageGradientType", STRING},
       {"imageSelectedColor", COLOR},
       {"imageSelectedColorEnd", COLOR},
       {"imageSelectedGradientType", STRING},
       {"imageBrightness", FLOAT},
       {"imageSaturation", FLOAT},
       {"itemTransitions", STRING},
       {"itemDiagonalOffset", FLOAT},
       {"itemHorizontalAlignment", STRING},
       {"itemVerticalAlignment", STRING},
       {"wheelHorizontalAlignment", STRING},
       {"wheelVerticalAlignment", STRING},
       {"horizontalOffset", FLOAT},
       {"verticalOffset", FLOAT},
       {"reflections", BOOLEAN},
       {"reflectionsOpacity", FLOAT},
       {"reflectionsFalloff", FLOAT},
       {"unfocusedItemOpacity", FLOAT},
       {"unfocusedItemSaturation", FLOAT},
       {"unfocusedItemDimming", FLOAT},
       {"fastScrolling", BOOLEAN},
       {"color", COLOR},
       {"colorEnd", COLOR},
       {"gradientType", STRING},
       {"text", STRING},
       {"textRelativeScale", FLOAT},
       {"textBackgroundCornerRadius", FLOAT},
       {"textColor", COLOR},
       {"textBackgroundColor", COLOR},
       {"textSelectedColor", COLOR},
       {"textSelectedBackgroundColor", COLOR},
       {"textHorizontalScrolling", BOOLEAN},
       {"textHorizontalScrollSpeed", FLOAT},
       {"textHorizontalScrollDelay", FLOAT},
       {"textHorizontalScrollGap", FLOAT},
       {"fontPath", PATH},
       {"fontSize", FLOAT},
       {"letterCase", STRING},
       {"letterCaseAutoCollections", STRING},
       {"letterCaseCustomCollections", STRING},
       {"lineSpacing", FLOAT},
       {"systemNameSuffix", BOOLEAN},
       {"letterCaseSystemNameSuffix", STRING},
       {"fadeAbovePrimary", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"grid",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"staticImage", PATH},
       {"imageType", STRING},
       {"defaultImage", PATH},
       {"defaultFolderImage", PATH},
       {"itemSize", NORMALIZED_PAIR},
       {"itemScale", FLOAT},
       {"itemSpacing", NORMALIZED_PAIR},
       {"scaleInwards", BOOLEAN},
       {"fractionalRows", BOOLEAN},
       {"itemTransitions", STRING},
       {"rowTransitions", STRING},
       {"unfocusedItemOpacity", FLOAT},
       {"unfocusedItemSaturation", FLOAT},
       {"unfocusedItemDimming", FLOAT},
       {"imageFit", STRING},
       {"imageCropPos", NORMALIZED_PAIR},
       {"imageInterpolation", STRING},
       {"imageRelativeScale", FLOAT},
       {"imageCornerRadius", FLOAT},
       {"imageColor", COLOR},
       {"imageColorEnd", COLOR},
       {"imageGradientType", STRING},
       {"imageSelectedColor", COLOR},
       {"imageSelectedColorEnd", COLOR},
       {"imageSelectedGradientType", STRING},
       {"imageBrightness", FLOAT},
       {"imageSaturation", FLOAT},
       {"backgroundImage", PATH},
       {"backgroundRelativeScale", FLOAT},
       {"backgroundCornerRadius", FLOAT},
       {"backgroundColor", COLOR},
       {"backgroundColorEnd", COLOR},
       {"backgroundGradientType", STRING},
       {"selectorImage", PATH},
       {"selectorRelativeScale", FLOAT},
       {"selectorCornerRadius", FLOAT},
       {"selectorLayer", STRING},
       {"selectorColor", COLOR},
       {"selectorColorEnd", COLOR},
       {"selectorGradientType", STRING},
       {"text", STRING},
       {"textRelativeScale", FLOAT},
       {"textBackgroundCornerRadius", FLOAT},
       {"textColor", COLOR},
       {"textBackgroundColor", COLOR},
       {"textSelectedColor", COLOR},
       {"textSelectedBackgroundColor", COLOR},
       {"textHorizontalScrolling", BOOLEAN},
       {"textHorizontalScrollSpeed", FLOAT},
       {"textHorizontalScrollDelay", FLOAT},
       {"textHorizontalScrollGap", FLOAT},
       {"fontPath", PATH},
       {"fontSize", FLOAT},
       {"letterCase", STRING},
       {"letterCaseAutoCollections", STRING},
       {"letterCaseCustomCollections", STRING},
       {"lineSpacing", FLOAT},
       {"systemNameSuffix", BOOLEAN},
       {"letterCaseSystemNameSuffix", STRING},
       {"fadeAbovePrimary", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"textlist",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"selectorWidth", FLOAT},
       {"selectorHeight", FLOAT},
       {"selectorHorizontalOffset", FLOAT},
       {"selectorVerticalOffset", FLOAT},
       {"selectorColor", COLOR},
       {"selectorColorEnd", COLOR},
       {"selectorGradientType", STRING},
       {"selectorImagePath", PATH},
       {"selectorImageTile", BOOLEAN},
       {"primaryColor", COLOR},
       {"secondaryColor", COLOR},
       {"selectedColor", COLOR},
       {"selectedSecondaryColor", COLOR},
       {"selectedBackgroundColor", COLOR},
       {"selectedSecondaryBackgroundColor", COLOR},
       {"selectedBackgroundMargins", NORMALIZED_PAIR},
       {"selectedBackgroundCornerRadius", FLOAT},
       {"textHorizontalScrolling", BOOLEAN},
       {"textHorizontalScrollSpeed", FLOAT},
       {"textHorizontalScrollDelay", FLOAT},
       {"textHorizontalScrollGap", FLOAT},
       {"fontPath", PATH},
       {"fontSize", FLOAT},
       {"horizontalAlignment", STRING},
       {"horizontalMargin", FLOAT},
       {"letterCase", STRING},
       {"letterCaseAutoCollections", STRING},
       {"letterCaseCustomCollections", STRING},
       {"lineSpacing", FLOAT},
       {"indicators", STRING},
       {"collectionIndicators", STRING},
       {"systemNameSuffix", BOOLEAN},
       {"letterCaseSystemNameSuffix", STRING},
       {"fadeAbovePrimary", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"image",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"maxSize", NORMALIZED_PAIR},
       {"cropSize", NORMALIZED_PAIR},
       {"cropPos", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"renderDuringTransitions", BOOLEAN},
       {"flipHorizontal", BOOLEAN},
       {"flipVertical", BOOLEAN},
       {"path", PATH},
       {"gameOverridePath", PATH},
       {"default", PATH},
       {"imageType", STRING},
       {"metadataElement", BOOLEAN},
       {"gameselector", STRING},
       {"gameselectorEntry", UNSIGNED_INTEGER},
       {"tile", BOOLEAN},
       {"tileSize", NORMALIZED_PAIR},
       {"tileHorizontalAlignment", STRING},
       {"tileVerticalAlignment", STRING},
       {"interpolation", STRING},
       {"cornerRadius", FLOAT},
       {"color", COLOR},
       {"colorEnd", COLOR},
       {"gradientType", STRING},
       {"scrollFadeIn", BOOLEAN},
       {"brightness", FLOAT},
       {"opacity", FLOAT},
       {"saturation", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"video",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"maxSize", NORMALIZED_PAIR},
       {"cropSize", NORMALIZED_PAIR},
       {"cropPos", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"path", PATH},
       {"default", PATH},
       {"defaultImage", PATH},
       {"imageType", STRING},
       {"metadataElement", BOOLEAN},
       {"gameselector", STRING},
       {"gameselectorEntry", UNSIGNED_INTEGER},
       {"iterationCount", UNSIGNED_INTEGER},
       {"onIterationsDone", STRING},
       {"audio", BOOLEAN},
       {"interpolation", STRING},
       {"imageCornerRadius", FLOAT},
       {"videoCornerRadius", FLOAT},
       {"color", COLOR},
       {"colorEnd", COLOR},
       {"gradientType", STRING},
       {"pillarboxes", BOOLEAN},
       {"pillarboxThreshold", NORMALIZED_PAIR},
       {"scanlines", BOOLEAN},
       {"delay", FLOAT},
       {"fadeInTime", FLOAT},
       {"scrollFadeIn", BOOLEAN},
       {"brightness", FLOAT},
       {"opacity", FLOAT},
       {"saturation", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"animation",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"maxSize", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"metadataElement", BOOLEAN},
       {"path", PATH},
       {"speed", FLOAT},
       {"direction", STRING},
       {"iterationCount", UNSIGNED_INTEGER},
       {"interpolation", STRING},
       {"cornerRadius", FLOAT},
       {"color", COLOR},
       {"colorEnd", COLOR},
       {"gradientType", STRING},
       {"brightness", FLOAT},
       {"opacity", FLOAT},
       {"saturation", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"badges",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"horizontalAlignment", STRING},
       {"direction", STRING},
       {"lines", UNSIGNED_INTEGER},
       {"itemsPerLine", UNSIGNED_INTEGER},
       {"itemMargin", NORMALIZED_PAIR},
       {"slots", STRING},
       {"controllerPos", NORMALIZED_PAIR},
       {"controllerSize", FLOAT},
       {"customBadgeIcon", PATH},
       {"customControllerIcon", PATH},
       {"folderLinkPos", NORMALIZED_PAIR},
       {"folderLinkSize", FLOAT},
       {"customFolderLinkIcon", PATH},
       {"badgeIconColor", COLOR},
       {"badgeIconColorEnd", COLOR},
       {"badgeIconGradientType", STRING},
       {"controllerIconColor", COLOR},
       {"controllerIconColorEnd", COLOR},
       {"controllerIconGradientType", STRING},
       {"folderLinkIconColor", COLOR},
       {"folderLinkIconColorEnd", COLOR},
       {"folderLinkIconGradientType", STRING},
       {"interpolation", STRING},
       {"opacity", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"text",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"text", STRING},
       {"systemdata", STRING},
       {"metadata", STRING},
       {"defaultValue", STRING},
       {"systemNameSuffix", BOOLEAN},
       {"letterCaseSystemNameSuffix", STRING},
       {"metadataElement", BOOLEAN},
       {"gameselector", STRING},
       {"gameselectorEntry", UNSIGNED_INTEGER},
       {"container", BOOLEAN},
       {"containerType", STRING},
       {"containerVerticalSnap", BOOLEAN},
       {"containerScrollSpeed", FLOAT},
       {"containerStartDelay", FLOAT},
       {"containerResetDelay", FLOAT},
       {"containerScrollGap", FLOAT},
       {"fontPath", PATH},
       {"fontSize", FLOAT},
       {"horizontalAlignment", STRING},
       {"verticalAlignment", STRING},
       {"color", COLOR},
       {"backgroundColor", COLOR},
       {"backgroundMargins", NORMALIZED_PAIR},
       {"backgroundCornerRadius", FLOAT},
       {"letterCase", STRING},
       {"lineSpacing", FLOAT},
       {"opacity", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"datetime",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"metadata", STRING},
       {"defaultValue", STRING},
       {"gameselector", STRING},
       {"gameselectorEntry", UNSIGNED_INTEGER},
       {"fontPath", PATH},
       {"fontSize", FLOAT},
       {"horizontalAlignment", STRING},
       {"verticalAlignment", STRING},
       {"color", COLOR},
       {"backgroundColor", COLOR},
       {"backgroundMargins", NORMALIZED_PAIR},
       {"backgroundCornerRadius", FLOAT},
       {"letterCase", STRING},
       {"lineSpacing", FLOAT},
       {"format", STRING},
       {"displayRelative", BOOLEAN},
       {"opacity", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"gamelistinfo",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"fontPath", PATH},
       {"fontSize", FLOAT},
       {"horizontalAlignment", STRING},
       {"verticalAlignment", STRING},
       {"color", COLOR},
       {"backgroundColor", COLOR},
       {"opacity", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"rating",
      {{"pos", NORMALIZED_PAIR},
       {"size", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"rotation", FLOAT},
       {"rotationOrigin", NORMALIZED_PAIR},
       {"stationary", STRING},
       {"hideIfZero", BOOLEAN},
       {"gameselector", STRING},
       {"gameselectorEntry", UNSIGNED_INTEGER},
       {"interpolation", STRING},
       {"color", COLOR},
       {"filledPath", PATH},
       {"unfilledPath", PATH},
       {"overlay", BOOLEAN},
       {"opacity", FLOAT},
       {"visible", BOOLEAN},
       {"zIndex", FLOAT}}},
     {"gameselector",
      {{"selection", STRING},
       {"gameCount", UNSIGNED_INTEGER},
       {"allowDuplicates", BOOLEAN}}},
     {"helpsystem",
      {{"pos", NORMALIZED_PAIR},
       {"posDimmed", NORMALIZED_PAIR},
       {"origin", NORMALIZED_PAIR},
       {"originDimmed", NORMALIZED_PAIR},
       {"textColor", COLOR},
       {"textColorDimmed", COLOR},
       {"iconColor", COLOR},
       {"iconColorDimmed", COLOR},
       {"fontPath", PATH},
       {"fontSize", FLOAT},
       {"fontSizeDimmed", FLOAT},
       {"entrySpacing", FLOAT},
       {"entrySpacingDimmed", FLOAT},
       {"iconTextSpacing", FLOAT},
       {"iconTextSpacingDimmed", FLOAT},
       {"letterCase", STRING},
       {"opacity", FLOAT},
       {"opacityDimmed", FLOAT},
       {"customButtonIcon", PATH}}},
     {"sound",
      {{"path", PATH}}}};
// clang-format on

ThemeData::ThemeData()
    : mCustomCollection {false}
{
    sCurrentTheme = sThemes.find(Settings::getInstance()->getString("Theme"));
    sVariantDefinedTransitions = "";
}

void ThemeData::loadFile(const std::map<std::string, std::string>& sysDataMap,
                         const std::string& path,
                         const ThemeTriggers::TriggerType trigger,
                         const bool customCollection)
{
    mCustomCollection = customCollection;
    mOverrideVariant = "";

    mPaths.push_back(path);

    ThemeException error;
    error << "ThemeData::loadFile(): ";
    error.setFiles(mPaths);

    if (!Utils::FileSystem::exists(path))
        throw error << "File does not exist";

    mViews.clear();
    mVariables.clear();

    mVariables.insert(sysDataMap.cbegin(), sysDataMap.cend());

    pugi::xml_document doc;
#if defined(_WIN64)
    pugi::xml_parse_result res {doc.load_file(Utils::String::stringToWideString(path).c_str())};
#else
    pugi::xml_parse_result res {doc.load_file(path.c_str())};
#endif
    if (!res)
        throw error << ": XML parsing error: " << res.description();

    pugi::xml_node root {doc.child("theme")};
    if (!root)
        throw error << ": Missing <theme> tag";

    // Check if there's an unsupported theme version tag.
    if (root.child("formatVersion") != nullptr)
        throw error << ": Unsupported <formatVersion> tag found";

    if (sCurrentTheme->second.capabilities.variants.size() > 0) {
        for (auto& variant : sCurrentTheme->second.capabilities.variants)
            mVariants.emplace_back(variant.name);

        if (std::find(mVariants.cbegin(), mVariants.cend(),
                      Settings::getInstance()->getString("ThemeVariant")) != mVariants.cend())
            mSelectedVariant = Settings::getInstance()->getString("ThemeVariant");
        else
            mSelectedVariant = mVariants.front();
        // Special shortcut variant to apply configuration to all defined variants.
        mVariants.emplace_back("all");

        if (trigger != ThemeTriggers::TriggerType::NONE) {
            auto overrides = getCurrentThemeSelectedVariantOverrides();
            if (overrides.find(trigger) != overrides.end())
                mOverrideVariant = overrides.at(trigger).first;
        }
    }

    if (sCurrentTheme->second.capabilities.colorSchemes.size() > 0) {
        for (auto& colorScheme : sCurrentTheme->second.capabilities.colorSchemes)
            mColorSchemes.emplace_back(colorScheme.name);

        if (std::find(mColorSchemes.cbegin(), mColorSchemes.cend(),
                      Settings::getInstance()->getString("ThemeColorScheme")) !=
            mColorSchemes.cend())
            mSelectedColorScheme = Settings::getInstance()->getString("ThemeColorScheme");
        else
            mSelectedColorScheme = mColorSchemes.front();
    }

    if (sCurrentTheme->second.capabilities.fontSizes.size() > 0) {
        for (auto& fontSize : sCurrentTheme->second.capabilities.fontSizes)
            mFontSizes.emplace_back(fontSize);

        if (std::find(mFontSizes.cbegin(), mFontSizes.cend(),
                      Settings::getInstance()->getString("ThemeFontSize")) != mFontSizes.cend())
            mSelectedFontSize = Settings::getInstance()->getString("ThemeFontSize");
        else
            mSelectedFontSize = mFontSizes.front();
    }

    sAspectRatioMatch = false;
    sThemeLanguage = "";

    if (sCurrentTheme->second.capabilities.aspectRatios.size() > 0) {
        if (std::find(sCurrentTheme->second.capabilities.aspectRatios.cbegin(),
                      sCurrentTheme->second.capabilities.aspectRatios.cend(),
                      Settings::getInstance()->getString("ThemeAspectRatio")) !=
            sCurrentTheme->second.capabilities.aspectRatios.cend())
            sSelectedAspectRatio = Settings::getInstance()->getString("ThemeAspectRatio");
        else
            sSelectedAspectRatio = sCurrentTheme->second.capabilities.aspectRatios.front();

        if (sSelectedAspectRatio == "automatic") {
            // Auto-detect the closest aspect ratio based on what's available in the theme config.
            sSelectedAspectRatio = "16:9";
            const float screenAspectRatio {Renderer::getScreenAspectRatio()};
            float diff {std::fabs(sAspectRatioMap["16:9"] - screenAspectRatio)};

            for (auto& aspectRatio : sCurrentTheme->second.capabilities.aspectRatios) {
                if (aspectRatio == "automatic")
                    continue;

                if (sAspectRatioMap.find(aspectRatio) != sAspectRatioMap.end()) {
                    const float newDiff {
                        std::fabs(sAspectRatioMap[aspectRatio] - screenAspectRatio)};
                    if (newDiff < 0.01f)
                        sAspectRatioMatch = true;
                    if (newDiff < diff) {
                        diff = newDiff;
                        sSelectedAspectRatio = aspectRatio;
                    }
                }
            }
        }
    }

    if (sCurrentTheme->second.capabilities.languages.size() > 0) {
        for (auto& language : sCurrentTheme->second.capabilities.languages)
            mLanguages.emplace_back(language);

        std::string langSetting {Settings::getInstance()->getString("ThemeLanguage")};
        if (langSetting == "automatic")
            langSetting = Utils::Localization::sCurrentLocale;

        // Check if there is an exact match.
        if (std::find(sCurrentTheme->second.capabilities.languages.cbegin(),
                      sCurrentTheme->second.capabilities.languages.cend(),
                      langSetting) != sCurrentTheme->second.capabilities.languages.cend()) {
            sThemeLanguage = langSetting;
        }
        else {
            // We assume all locales are in the correct format.
            const std::string currLanguage {langSetting.substr(0, 2)};
            // Select the closest matching locale (i.e. same language but possibly for a
            // different country).
            for (const auto& lang : sCurrentTheme->second.capabilities.languages) {
                if (lang.substr(0, 2) == currLanguage) {
                    sThemeLanguage = lang;
                    break;
                }
            }
            // If there is no match then fall back to the default language en_US, which is
            // mandatory for all themes that provide language support.
            if (sThemeLanguage == "")
                sThemeLanguage = "en_US";
        }
    }

    parseVariables(root);
    parseColorSchemes(root);
    parseFontSizes(root);
    parseLanguages(root);
    parseIncludes(root);
    parseViews(root);
    if (root.child("feature") != nullptr)
        throw error << ": Unsupported <feature> tag found";
    parseVariants(root);
    parseAspectRatios(root);
}

bool ThemeData::hasView(const std::string& view)
{
    auto viewIt = mViews.find(view);
    return (viewIt != mViews.cend());
}

const ThemeData::ThemeElement* ThemeData::getElement(const std::string& view,
                                                     const std::string& element,
                                                     const std::string& expectedType) const
{
    auto viewIt = mViews.find(view);
    if (viewIt == mViews.cend())
        return nullptr; // Not found.

    auto elemIt = viewIt->second.elements.find(element);
    if (elemIt == viewIt->second.elements.cend())
        return nullptr;

    // If expectedType is an empty string, then skip type checking.
    if (elemIt->second.type != expectedType && !expectedType.empty()) {
        LOG(LogWarning) << "ThemeData::getElement(): Requested element \"" << view << "." << element
                        << "\" has the wrong type, expected \"" << expectedType << "\", got \""
                        << elemIt->second.type << "\"";
        return nullptr;
    }

    return &elemIt->second;
}

void ThemeData::populateThemes()
{
    sThemes.clear();
    LOG(LogInfo) << "Checking for available themes...";

    // Check for themes first under the user theme directory (which is in the ES-DE home directory
    // by default), then under the data installation directory (Unix only) and last under the ES-DE
    // binary directory.
#if defined(__ANDROID__)
    const std::string userThemeDirectory {Utils::FileSystem::getInternalAppDataDirectory() +
                                          "/themes"};
#else
    const std::string defaultUserThemeDir {Utils::FileSystem::getAppDataDirectory() + "/themes"};
    const std::string userThemeDirSetting {Utils::FileSystem::expandHomePath(
        Settings::getInstance()->getString("UserThemeDirectory"))};
    std::string userThemeDirectory;

    if (userThemeDirSetting.empty()) {
        userThemeDirectory = defaultUserThemeDir;
    }
    else if (Utils::FileSystem::isDirectory(userThemeDirSetting) ||
             Utils::FileSystem::isSymlink(userThemeDirSetting)) {
        userThemeDirectory = userThemeDirSetting;
#if defined(_WIN64)
        LOG(LogInfo) << "Setting user theme directory to \""
                     << Utils::String::replace(userThemeDirectory, "/", "\\") << "\"";
#else
        LOG(LogInfo) << "Setting user theme directory to \"" << userThemeDirectory << "\"";
#endif
    }
    else {
        LOG(LogWarning) << "Requested user theme directory \"" << userThemeDirSetting
                        << "\" does not exist or is not a directory, reverting to \""
                        << defaultUserThemeDir << "\"";
        userThemeDirectory = defaultUserThemeDir;
    }
#endif

#if defined(__ANDROID__)
    const std::vector<std::string> themePaths {Utils::FileSystem::getProgramDataPath() + "/themes",
                                               Utils::FileSystem::getAppDataDirectory() + "/themes",
                                               userThemeDirectory};
#elif defined(__APPLE__)
    const std::vector<std::string> themePaths {
        Utils::FileSystem::getExePath() + "/themes",
        Utils::FileSystem::getExePath() + "/../Resources/themes", userThemeDirectory};
#elif defined(_WIN64) || defined(APPIMAGE_BUILD)
    const std::vector<std::string> themePaths {Utils::FileSystem::getExePath() + "/themes",
                                               userThemeDirectory};
#else
    const std::vector<std::string> themePaths {Utils::FileSystem::getExePath() + "/themes",
                                               Utils::FileSystem::getProgramDataPath() + "/themes",
                                               userThemeDirectory};
#endif

    for (auto path : themePaths) {
        if (!Utils::FileSystem::isDirectory(path))
            continue;

        Utils::FileSystem::StringList dirContent {Utils::FileSystem::getDirContent(path)};

        for (Utils::FileSystem::StringList::const_iterator it = dirContent.cbegin();
             it != dirContent.cend(); ++it) {
            if (Utils::FileSystem::isDirectory(*it)) {
                const std::string themeDirName {Utils::FileSystem::getFileName(*it)};
                if (themeDirName == "themes-list" ||
                    (themeDirName.length() >= 8 &&
                     Utils::String::toLower(themeDirName.substr(themeDirName.length() - 8, 8)) ==
                         "disabled"))
                    continue;
#if defined(_WIN64)
                LOG(LogDebug) << "Loading theme capabilities for \""
                              << Utils::String::replace(*it, "/", "\\") << "\"...";
#else
                LOG(LogDebug) << "Loading theme capabilities for \"" << *it << "\"...";
#endif
                ThemeCapability capabilities {parseThemeCapabilities((*it))};

                if (!capabilities.validTheme)
                    continue;

                std::string themeName;
                if (capabilities.themeName != "")
                    themeName.append(" (\"").append(capabilities.themeName).append("\")");

#if defined(_WIN64)
                LOG(LogInfo) << "Added theme \"" << Utils::String::replace(*it, "/", "\\") << "\""
                             << themeName;
#else
                LOG(LogInfo) << "Added theme \"" << *it << "\"" << themeName;
#endif
                int aspectRatios {0};
                int languages {0};
                if (capabilities.aspectRatios.size() > 0)
                    aspectRatios = static_cast<int>(capabilities.aspectRatios.size()) - 1;
                if (capabilities.languages.size() > 0)
                    languages = static_cast<int>(capabilities.languages.size()) - 1;
                LOG(LogDebug) << "Theme includes support for " << capabilities.variants.size()
                              << " variant" << (capabilities.variants.size() != 1 ? "s" : "")
                              << ", " << capabilities.colorSchemes.size() << " color scheme"
                              << (capabilities.colorSchemes.size() != 1 ? "s" : "") << ", "
                              << capabilities.fontSizes.size() << " font size"
                              << (capabilities.fontSizes.size() != 1 ? "s" : "") << ", "
                              << aspectRatios << " aspect ratio" << (aspectRatios != 1 ? "s" : "")
                              << ", " << capabilities.transitions.size() << " transition"
                              << (capabilities.transitions.size() != 1 ? "s" : "") << " and "
                              << languages << " language" << (languages != 1 ? "s" : "");

                Theme theme {*it, capabilities};
                sThemes[theme.getName()] = theme;
            }
        }
    }

    if (sThemes.empty()) {
        LOG(LogWarning) << "Couldn't find any themes, creating dummy entry";
        Theme theme {"no-themes", ThemeCapability()};
        sThemes[theme.getName()] = theme;
        sCurrentTheme = sThemes.begin();
    }
}

const std::string ThemeData::getSystemThemeFile(const std::string& system)
{
    if (sThemes.empty())
        getThemes();

    if (sThemes.empty())
        return "";

    std::map<std::string, Theme, StringComparator>::const_iterator theme {
        sThemes.find(Settings::getInstance()->getString("Theme"))};
    if (theme == sThemes.cend()) {
        // Currently configured theme is missing, attempt to load the default theme linear-es-de
        // instead, and if that's also missing then pick the first available one.
        bool defaultSetFound {true};

        theme = sThemes.find("linear-es-de");

        if (theme == sThemes.cend()) {
            theme = sThemes.cbegin();
            defaultSetFound = false;
        }

        LOG(LogWarning) << "Configured theme \"" << Settings::getInstance()->getString("Theme")
                        << "\" does not exist, loading" << (defaultSetFound ? " default " : " ")
                        << "theme \"" << theme->first << "\" instead";

        Settings::getInstance()->setString("Theme", theme->first);
        sCurrentTheme = sThemes.find(Settings::getInstance()->getString("Theme"));
    }

    return theme->second.getThemePath(system);
}

const std::string ThemeData::getFontSizeLabel(const std::string& fontSize)
{
    auto it = std::find_if(sSupportedFontSizes.cbegin(), sSupportedFontSizes.cend(),
                           [&fontSize](const std::pair<std::string, std::string>& entry) {
                               return entry.first == fontSize;
                           });
    if (it != sSupportedFontSizes.cend())
        return it->second;
    else
        return "invalid font size";
}

const std::string ThemeData::getAspectRatioLabel(const std::string& aspectRatio)
{
    auto it = std::find_if(sSupportedAspectRatios.cbegin(), sSupportedAspectRatios.cend(),
                           [&aspectRatio](const std::pair<std::string, std::string>& entry) {
                               return entry.first == aspectRatio;
                           });
    if (it != sSupportedAspectRatios.cend())
        return it->second;
    else
        return "invalid ratio";
}

const std::string ThemeData::getLanguageLabel(const std::string& language)
{
    auto it = std::find_if(sSupportedLanguages.cbegin(), sSupportedLanguages.cend(),
                           [&language](const std::pair<std::string, std::string>& entry) {
                               return entry.first == language;
                           });
    if (it != sSupportedLanguages.cend())
        return it->second;
    else
        return "invalid language";
}

void ThemeData::setThemeTransitions()
{
    auto setTransitionsFunc = [](int transitionAnim) {
        Settings::getInstance()->setInt("TransitionsSystemToSystem", transitionAnim);
        Settings::getInstance()->setInt("TransitionsSystemToGamelist", transitionAnim);
        Settings::getInstance()->setInt("TransitionsGamelistToGamelist", transitionAnim);
        Settings::getInstance()->setInt("TransitionsGamelistToSystem", transitionAnim);
        Settings::getInstance()->setInt("TransitionsStartupToSystem", transitionAnim);
        Settings::getInstance()->setInt("TransitionsStartupToGamelist", transitionAnim);
    };

    int transitionAnim {ViewTransitionAnimation::INSTANT};
    setTransitionsFunc(transitionAnim);

    const std::string& transitionsSetting {Settings::getInstance()->getString("ThemeTransitions")};
    std::string profile;
    size_t profileEntry {0};

    if (transitionsSetting == "automatic") {
        if (sVariantDefinedTransitions != "")
            profile = sVariantDefinedTransitions;
        else if (!sCurrentTheme->second.capabilities.transitions.empty())
            profile = sCurrentTheme->second.capabilities.transitions.front().name;
    }
    else {
        profile = transitionsSetting;
    }

    auto it = std::find_if(
        sCurrentTheme->second.capabilities.transitions.cbegin(),
        sCurrentTheme->second.capabilities.transitions.cend(),
        [&profile](const ThemeTransitions transitions) { return transitions.name == profile; });
    if (it != sCurrentTheme->second.capabilities.transitions.cend())
        profileEntry = static_cast<size_t>(
            std::distance(sCurrentTheme->second.capabilities.transitions.cbegin(), it) + 1);

    if (profileEntry != 0 &&
        sCurrentTheme->second.capabilities.transitions.size() > profileEntry - 1) {
        auto transitionMap =
            sCurrentTheme->second.capabilities.transitions[profileEntry - 1].animations;
        if (transitionMap.find(ViewTransition::SYSTEM_TO_SYSTEM) != transitionMap.end())
            Settings::getInstance()->setInt("TransitionsSystemToSystem",
                                            transitionMap[ViewTransition::SYSTEM_TO_SYSTEM]);
        if (transitionMap.find(ViewTransition::SYSTEM_TO_GAMELIST) != transitionMap.end())
            Settings::getInstance()->setInt("TransitionsSystemToGamelist",
                                            transitionMap[ViewTransition::SYSTEM_TO_GAMELIST]);
        if (transitionMap.find(ViewTransition::GAMELIST_TO_GAMELIST) != transitionMap.end())
            Settings::getInstance()->setInt("TransitionsGamelistToGamelist",
                                            transitionMap[ViewTransition::GAMELIST_TO_GAMELIST]);
        if (transitionMap.find(ViewTransition::GAMELIST_TO_SYSTEM) != transitionMap.end())
            Settings::getInstance()->setInt("TransitionsGamelistToSystem",
                                            transitionMap[ViewTransition::GAMELIST_TO_SYSTEM]);
        if (transitionMap.find(ViewTransition::STARTUP_TO_SYSTEM) != transitionMap.end())
            Settings::getInstance()->setInt("TransitionsStartupToSystem",
                                            transitionMap[ViewTransition::STARTUP_TO_SYSTEM]);
        if (transitionMap.find(ViewTransition::STARTUP_TO_GAMELIST) != transitionMap.end())
            Settings::getInstance()->setInt("TransitionsStartupToGamelist",
                                            transitionMap[ViewTransition::STARTUP_TO_GAMELIST]);
    }
    else if (transitionsSetting == "builtin-slide" || transitionsSetting == "builtin-fade") {
        if (std::find(sCurrentTheme->second.capabilities.suppressedTransitionProfiles.cbegin(),
                      sCurrentTheme->second.capabilities.suppressedTransitionProfiles.cend(),
                      transitionsSetting) ==
            sCurrentTheme->second.capabilities.suppressedTransitionProfiles.cend()) {
            if (transitionsSetting == "builtin-slide") {
                transitionAnim = static_cast<int>(ViewTransitionAnimation::SLIDE);
            }
            else if (transitionsSetting == "builtin-fade") {
                transitionAnim = static_cast<int>(ViewTransitionAnimation::FADE);
            }
            setTransitionsFunc(transitionAnim);
        }
    }
}

const std::map<ThemeTriggers::TriggerType, std::pair<std::string, std::vector<std::string>>>
ThemeData::getCurrentThemeSelectedVariantOverrides()
{
    const auto variantIter = std::find_if(
        sCurrentTheme->second.capabilities.variants.cbegin(),
        sCurrentTheme->second.capabilities.variants.cend(),
        [this](ThemeVariant currVariant) { return currVariant.name == mSelectedVariant; });

    if (variantIter != sCurrentTheme->second.capabilities.variants.cend() &&
        !(*variantIter).overrides.empty())
        return (*variantIter).overrides;
    else
        return ThemeVariant().overrides;
}

const void ThemeData::themeLoadedLogOutput()
{
    LOG(LogInfo) << "Finished loading theme \"" << sCurrentTheme->first << "\"";

    if (sSelectedAspectRatio != "") {
        const bool autoDetect {Settings::getInstance()->getString("ThemeAspectRatio") ==
                               "automatic"};
        const std::string match {sAspectRatioMatch ? "exact match " : "closest match "};

        LOG(LogInfo) << "Aspect ratio " << (autoDetect ? "automatically " : "manually ")
                     << "set to " << (autoDetect ? match : "") << "\""
                     << Utils::String::replace(sSelectedAspectRatio, "_", " ") << "\"";
    }

    if (sThemeLanguage != "") {
        LOG(LogInfo) << "Theme language set to \"" << sThemeLanguage << "\"";
    }
    else {
        LOG(LogInfo) << "Theme does not have multilingual support";
    }
}

unsigned int ThemeData::getHexColor(const std::string& str)
{
    ThemeException error;

    if (str == "")
        throw error << "Empty color property";

    const size_t length {str.size()};
    if (length != 6 && length != 8)
        throw error << "Invalid color property \"" << str
                    << "\" (must be 6 or 8 characters in length)";

    unsigned int value;
    std::stringstream ss;
    ss << str;
    ss >> std::hex >> value;

    if (length == 6)
        value = (value << 8) | 0xFF;

    return value;
}

std::string ThemeData::resolvePlaceholders(const std::string& in)
{
    if (in.empty())
        return in;

    const size_t variableBegin {in.find("${")};
    const size_t variableEnd {in.find("}", variableBegin)};

    if ((variableBegin == std::string::npos) || (variableEnd == std::string::npos))
        return in;

    std::string prefix {in.substr(0, variableBegin)};
    std::string replace {in.substr(variableBegin + 2, variableEnd - (variableBegin + 2))};
    std::string suffix {resolvePlaceholders(in.substr(variableEnd + 1).c_str())};

    return prefix + mVariables[replace] + suffix;
}

ThemeData::ThemeCapability ThemeData::parseThemeCapabilities(const std::string& path)
{
    ThemeCapability capabilities;
    std::vector<std::string> aspectRatiosTemp;
    std::vector<std::string> fontSizesTemp;
    std::vector<std::string> languagesTemp;
    bool hasTriggers {false};

    const std::string capFile {path + "/capabilities.xml"};

    if (Utils::FileSystem::isRegularFile(capFile) || Utils::FileSystem::isSymlink(capFile)) {
        capabilities.validTheme = true;

        pugi::xml_document doc;
#if defined(_WIN64)
        const pugi::xml_parse_result& res {
            doc.load_file(Utils::String::stringToWideString(capFile).c_str())};
#else
        const pugi::xml_parse_result& res {doc.load_file(capFile.c_str())};
#endif
        if (res.status == pugi::status_no_document_element) {
            LOG(LogDebug) << "Found a capabilities.xml file with no configuration";
        }
        else if (!res) {
            LOG(LogError) << "Couldn't parse capabilities.xml: " << res.description();
            return capabilities;
        }
        const pugi::xml_node& themeCapabilities {doc.child("themeCapabilities")};
        if (!themeCapabilities) {
            LOG(LogError) << "Missing <themeCapabilities> tag in capabilities.xml";
            return capabilities;
        }

        const pugi::xml_node& themeName {themeCapabilities.child("themeName")};
        if (themeName != nullptr)
            capabilities.themeName = themeName.text().get();

        for (pugi::xml_node aspectRatio {themeCapabilities.child("aspectRatio")}; aspectRatio;
             aspectRatio = aspectRatio.next_sibling("aspectRatio")) {
            const std::string& value {aspectRatio.text().get()};
            if (std::find_if(sSupportedAspectRatios.cbegin(), sSupportedAspectRatios.cend(),
                             [&value](const std::pair<std::string, std::string>& entry) {
                                 return entry.first == value;
                             }) == sSupportedAspectRatios.cend()) {
                LOG(LogWarning) << "Declared aspect ratio \"" << value
                                << "\" is not supported, ignoring entry in \"" << capFile << "\"";
            }
            else {
                if (std::find(aspectRatiosTemp.cbegin(), aspectRatiosTemp.cend(), value) !=
                    aspectRatiosTemp.cend()) {
                    LOG(LogWarning)
                        << "Aspect ratio \"" << value
                        << "\" is declared multiple times, ignoring entry in \"" << capFile << "\"";
                }
                else {
                    aspectRatiosTemp.emplace_back(value);
                }
            }
        }

        for (pugi::xml_node fontSize {themeCapabilities.child("fontSize")}; fontSize;
             fontSize = fontSize.next_sibling("fontSize")) {
            const std::string& value {fontSize.text().get()};
            if (std::find_if(sSupportedFontSizes.cbegin(), sSupportedFontSizes.cend(),
                             [&value](const std::pair<std::string, std::string>& entry) {
                                 return entry.first == value;
                             }) == sSupportedFontSizes.cend()) {
                LOG(LogWarning) << "Declared font size \"" << value
                                << "\" is not supported, ignoring entry in \"" << capFile << "\"";
            }
            else {
                if (std::find(fontSizesTemp.cbegin(), fontSizesTemp.cend(), value) !=
                    fontSizesTemp.cend()) {
                    LOG(LogWarning)
                        << "Font size \"" << value
                        << "\" is declared multiple times, ignoring entry in \"" << capFile << "\"";
                }
                else {
                    fontSizesTemp.emplace_back(value);
                }
            }
        }

        for (pugi::xml_node variant {themeCapabilities.child("variant")}; variant;
             variant = variant.next_sibling("variant")) {
            ThemeVariant readVariant;
            const std::string& name {variant.attribute("name").as_string()};
            if (name.empty()) {
                LOG(LogWarning)
                    << "Found <variant> tag without name attribute, ignoring entry in \"" << capFile
                    << "\"";
            }
            else if (name == "all") {
                LOG(LogWarning)
                    << "Found <variant> tag using reserved name \"all\", ignoring entry in \""
                    << capFile << "\"";
            }
            else {
                readVariant.name = name;
            }

            if (variant.child("label") == nullptr) {
                LOG(LogDebug)
                    << "No variant <label> tags found, setting label value to the variant name \""
                    << name << "\" for \"" << capFile << "\"";
                readVariant.labels.emplace_back(std::make_pair("en_US", name));
            }
            else {
                std::vector<std::pair<std::string, std::string>> labels;
                for (pugi::xml_node labelTag {variant.child("label")}; labelTag;
                     labelTag = labelTag.next_sibling("label")) {
                    std::string language {labelTag.attribute("language").as_string()};
                    if (language == "")
                        language = "en_US";
                    else if (std::find_if(sSupportedLanguages.cbegin(), sSupportedLanguages.cend(),
                                          [language](std::pair<std::string, std::string> currLang) {
                                              return currLang.first == language;
                                          }) == sSupportedLanguages.cend()) {
                        LOG(LogWarning) << "Declared language \"" << language
                                        << "\" not supported, setting label language to \"en_US\""
                                           " for variant name \""
                                        << name << "\" in \"" << capFile << "\"";
                        language = "en_US";
                    }
                    std::string labelValue {labelTag.text().as_string()};
                    if (labelValue == "") {
                        LOG(LogWarning) << "No variant <label> value defined, setting value to "
                                           "the variant name \""
                                        << name << "\" for \"" << capFile << "\"";
                        labelValue = name;
                    }
                    labels.emplace_back(std::make_pair(language, labelValue));
                }
                if (!labels.empty()) {
                    // Add the label languages in the order they are defined in sSupportedLanguages.
                    for (auto& language : sSupportedLanguages) {
                        if (language.first == "automatic")
                            continue;
                        const auto it =
                            std::find_if(labels.cbegin(), labels.cend(),
                                         [language](std::pair<std::string, std::string> currLabel) {
                                             return currLabel.first == language.first;
                                         });
                        if (it != labels.cend()) {
                            readVariant.labels.emplace_back(
                                std::make_pair((*it).first, (*it).second));
                        }
                    }
                }
            }

            const pugi::xml_node& selectableTag {variant.child("selectable")};
            if (selectableTag != nullptr) {
                const std::string& value {selectableTag.text().as_string()};
                if (value.front() == '0' || value.front() == 'f' || value.front() == 'F' ||
                    value.front() == 'n' || value.front() == 'N')
                    readVariant.selectable = false;
                else
                    readVariant.selectable = true;
            }

            for (pugi::xml_node overrideTag {variant.child("override")}; overrideTag;
                 overrideTag = overrideTag.next_sibling("override")) {
                if (overrideTag != nullptr) {
                    std::vector<std::string> mediaTypes;
                    const pugi::xml_node& mediaTypeTag {overrideTag.child("mediaType")};
                    if (mediaTypeTag != nullptr) {
                        std::string mediaTypeValue {mediaTypeTag.text().as_string()};
                        for (auto& character : mediaTypeValue) {
                            if (std::isspace(character))
                                character = ',';
                        }
                        mediaTypeValue = Utils::String::replace(mediaTypeValue, ",,", ",");
                        mediaTypes = Utils::String::delimitedStringToVector(mediaTypeValue, ",");

                        for (std::string& type : mediaTypes) {
                            if (std::find(sSupportedMediaTypes.cbegin(),
                                          sSupportedMediaTypes.cend(),
                                          type) == sSupportedMediaTypes.cend()) {
                                LOG(LogError) << "ThemeData::parseThemeCapabilities(): Invalid "
                                                 "override configuration, unsupported "
                                                 "\"mediaType\" value \""
                                              << type << "\"";
                                mediaTypes.clear();
                                break;
                            }
                        }
                    }

                    const pugi::xml_node& triggerTag {overrideTag.child("trigger")};
                    if (triggerTag != nullptr) {
                        const std::string& triggerValue {triggerTag.text().as_string()};
                        if (triggerValue == "") {
                            LOG(LogWarning) << "No <trigger> tag value defined for variant \""
                                            << readVariant.name << "\", ignoring entry in \""
                                            << capFile << "\"";
                        }
                        else if (triggerValue != "noVideos" && triggerValue != "noMedia") {
                            LOG(LogWarning) << "Invalid <useVariant> tag value \"" << triggerValue
                                            << "\" defined for variant \"" << readVariant.name
                                            << "\", ignoring entry in \"" << capFile << "\"";
                        }
                        else {
                            const pugi::xml_node& useVariantTag {overrideTag.child("useVariant")};
                            if (useVariantTag != nullptr) {
                                const std::string& useVariantValue {
                                    useVariantTag.text().as_string()};
                                if (useVariantValue == "") {
                                    LOG(LogWarning)
                                        << "No <useVariant> tag value defined for variant \""
                                        << readVariant.name << "\", ignoring entry in \"" << capFile
                                        << "\"";
                                }
                                else {
                                    hasTriggers = true;
                                    if (triggerValue == "noVideos") {
                                        readVariant
                                            .overrides[ThemeTriggers::TriggerType::NO_VIDEOS] =
                                            std::make_pair(useVariantValue,
                                                           std::vector<std::string>());
                                    }
                                    else if (triggerValue == "noMedia") {
                                        if (mediaTypes.empty())
                                            mediaTypes.emplace_back("miximage");
                                        readVariant
                                            .overrides[ThemeTriggers::TriggerType::NO_MEDIA] =
                                            std::make_pair(useVariantValue, mediaTypes);
                                    }
                                }
                            }
                            else {
                                LOG(LogWarning)
                                    << "Found an <override> tag without a corresponding "
                                       "<useVariant> tag, "
                                    << "ignoring entry for variant \"" << readVariant.name
                                    << "\" in \"" << capFile << "\"";
                            }
                        }
                    }
                }
                else {
                    LOG(LogWarning)
                        << "Found an <override> tag without a corresponding <trigger> tag, "
                        << "ignoring entry for variant \"" << readVariant.name << "\" in \""
                        << capFile << "\"";
                }
            }

            if (readVariant.name != "") {
                bool duplicate {false};
                for (auto& variant : capabilities.variants) {
                    if (variant.name == readVariant.name) {
                        LOG(LogWarning) << "Variant \"" << readVariant.name
                                        << "\" is declared multiple times, ignoring entry in \""
                                        << capFile << "\"";
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate)
                    capabilities.variants.emplace_back(readVariant);
            }
        }

        for (pugi::xml_node colorScheme {themeCapabilities.child("colorScheme")}; colorScheme;
             colorScheme = colorScheme.next_sibling("colorScheme")) {
            ThemeColorScheme readColorScheme;
            const std::string& name {colorScheme.attribute("name").as_string()};
            if (name.empty()) {
                LOG(LogWarning)
                    << "Found <colorScheme> tag without name attribute, ignoring entry in \""
                    << capFile << "\"";
            }
            else {
                readColorScheme.name = name;
            }

            if (colorScheme.child("label") == nullptr) {
                LOG(LogDebug) << "No colorScheme <label> tag found, setting label value to the "
                                 "color scheme name \""
                              << name << "\" for \"" << capFile << "\"";
                readColorScheme.labels.emplace_back(std::make_pair("en_US", name));
            }
            else {
                std::vector<std::pair<std::string, std::string>> labels;
                for (pugi::xml_node labelTag {colorScheme.child("label")}; labelTag;
                     labelTag = labelTag.next_sibling("label")) {
                    std::string language {labelTag.attribute("language").as_string()};
                    if (language == "")
                        language = "en_US";
                    else if (std::find_if(sSupportedLanguages.cbegin(), sSupportedLanguages.cend(),
                                          [language](std::pair<std::string, std::string> currLang) {
                                              return currLang.first == language;
                                          }) == sSupportedLanguages.cend()) {
                        LOG(LogWarning) << "Declared language \"" << language
                                        << "\" not supported, setting label language to \"en_US\""
                                           " for color scheme name \""
                                        << name << "\" in \"" << capFile << "\"";
                        language = "en_US";
                    }
                    std::string labelValue {labelTag.text().as_string()};
                    if (labelValue == "") {
                        LOG(LogWarning) << "No colorScheme <label> value defined, setting value to "
                                           "the color scheme name \""
                                        << name << "\" for \"" << capFile << "\"";
                        labelValue = name;
                    }
                    labels.emplace_back(std::make_pair(language, labelValue));
                }
                if (!labels.empty()) {
                    // Add the label languages in the order they are defined in sSupportedLanguages.
                    for (auto& language : sSupportedLanguages) {
                        if (language.first == "automatic")
                            continue;
                        const auto it =
                            std::find_if(labels.cbegin(), labels.cend(),
                                         [language](std::pair<std::string, std::string> currLabel) {
                                             return currLabel.first == language.first;
                                         });
                        if (it != labels.cend()) {
                            readColorScheme.labels.emplace_back(
                                std::make_pair((*it).first, (*it).second));
                        }
                    }
                }
            }

            if (readColorScheme.name != "") {
                bool duplicate {false};
                for (auto& colorScheme : capabilities.colorSchemes) {
                    if (colorScheme.name == readColorScheme.name) {
                        LOG(LogWarning) << "Color scheme \"" << readColorScheme.name
                                        << "\" is declared multiple times, ignoring entry in \""
                                        << capFile << "\"";
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate)
                    capabilities.colorSchemes.emplace_back(readColorScheme);
            }
        }

        for (pugi::xml_node language {themeCapabilities.child("language")}; language;
             language = language.next_sibling("language")) {
            const std::string& value {language.text().get()};
            if (std::find_if(sSupportedLanguages.cbegin(), sSupportedLanguages.cend(),
                             [&value](const std::pair<std::string, std::string>& entry) {
                                 return entry.first == value;
                             }) == sSupportedLanguages.cend()) {
                LOG(LogWarning) << "Declared language \"" << value
                                << "\" is not supported, ignoring entry in \"" << capFile << "\"";
            }
            else {
                if (std::find(languagesTemp.cbegin(), languagesTemp.cend(), value) !=
                    languagesTemp.cend()) {
                    LOG(LogWarning)
                        << "Language \"" << value
                        << "\" is declared multiple times, ignoring entry in \"" << capFile << "\"";
                }
                else {
                    languagesTemp.emplace_back(value);
                }
            }
        }

        if (languagesTemp.size() > 0 && std::find(languagesTemp.cbegin(), languagesTemp.cend(),
                                                  "en_US") == languagesTemp.cend()) {
            LOG(LogError) << "Theme has declared language support but is missing mandatory "
                          << "\"en_US\" entry in \"" << capFile << "\"";
            languagesTemp.clear();
        }

        for (pugi::xml_node transitions {themeCapabilities.child("transitions")}; transitions;
             transitions = transitions.next_sibling("transitions")) {
            std::map<ViewTransition, ViewTransitionAnimation> readTransitions;
            std::string name {transitions.attribute("name").as_string()};
            std::string label;
            bool selectable {true};

            if (name.empty()) {
                LOG(LogWarning)
                    << "Found <transitions> tag without name attribute, ignoring entry in \""
                    << capFile << "\"";
                name.clear();
            }
            else {
                if (std::find(sSupportedTransitionAnimations.cbegin(),
                              sSupportedTransitionAnimations.cend(),
                              name) != sSupportedTransitionAnimations.cend()) {
                    LOG(LogWarning)
                        << "Found <transitions> tag using reserved name attribute value \"" << name
                        << "\", ignoring entry in \"" << capFile << "\"";
                    name.clear();
                }
                else {
                    for (auto& transitionEntry : capabilities.transitions) {
                        if (transitionEntry.name == name) {
                            LOG(LogWarning)
                                << "Found <transitions> tag with previously used name attribute "
                                   "value \""
                                << name << "\", ignoring entry in \"" << capFile << "\"";
                            name.clear();
                        }
                    }
                }
            }

            if (name == "")
                continue;

            const pugi::xml_node& selectableTag {transitions.child("selectable")};
            if (selectableTag != nullptr) {
                const std::string& value {selectableTag.text().as_string()};
                if (value.front() == '0' || value.front() == 'f' || value.front() == 'F' ||
                    value.front() == 'n' || value.front() == 'N')
                    selectable = false;
            }

            for (auto& currTransition : sSupportedTransitions) {
                const pugi::xml_node& transitionTag {transitions.child(currTransition.c_str())};
                if (transitionTag != nullptr) {
                    const std::string& transitionValue {transitionTag.text().as_string()};
                    if (transitionValue.empty()) {
                        LOG(LogWarning) << "Found <" << currTransition
                                        << "> transition tag without any value, "
                                           "ignoring entry in \""
                                        << capFile << "\"";
                    }
                    else if (std::find(sSupportedTransitionAnimations.cbegin(),
                                       sSupportedTransitionAnimations.cend(),
                                       currTransition) != sSupportedTransitionAnimations.cend()) {
                        LOG(LogWarning)
                            << "Invalid <" << currTransition << "> transition tag value \""
                            << transitionValue << "\", ignoring entry in \"" << capFile << "\"";
                    }
                    else {
                        ViewTransitionAnimation transitionAnim {ViewTransitionAnimation::INSTANT};
                        if (transitionValue == "slide")
                            transitionAnim = ViewTransitionAnimation::SLIDE;
                        else if (transitionValue == "fade")
                            transitionAnim = ViewTransitionAnimation::FADE;

                        if (currTransition == "systemToSystem")
                            readTransitions[ViewTransition::SYSTEM_TO_SYSTEM] = transitionAnim;
                        else if (currTransition == "systemToGamelist")
                            readTransitions[ViewTransition::SYSTEM_TO_GAMELIST] = transitionAnim;
                        else if (currTransition == "gamelistToGamelist")
                            readTransitions[ViewTransition::GAMELIST_TO_GAMELIST] = transitionAnim;
                        else if (currTransition == "gamelistToSystem")
                            readTransitions[ViewTransition::GAMELIST_TO_SYSTEM] = transitionAnim;
                        else if (currTransition == "startupToSystem")
                            readTransitions[ViewTransition::STARTUP_TO_SYSTEM] = transitionAnim;
                        else if (currTransition == "startupToGamelist")
                            readTransitions[ViewTransition::STARTUP_TO_GAMELIST] = transitionAnim;
                    }
                }
            }

            if (!readTransitions.empty()) {
                // If startupToSystem and startupToGamelist are not defined, then set them
                // to the same values as systemToSystem and gamelistToGamelist respectively,
                // assuming those transitions have been defined.
                if (readTransitions.find(ViewTransition::STARTUP_TO_SYSTEM) ==
                    readTransitions.cend()) {
                    if (readTransitions.find(ViewTransition::SYSTEM_TO_SYSTEM) !=
                        readTransitions.cend())
                        readTransitions[ViewTransition::STARTUP_TO_SYSTEM] =
                            readTransitions[ViewTransition::SYSTEM_TO_SYSTEM];
                }
                if (readTransitions.find(ViewTransition::STARTUP_TO_GAMELIST) ==
                    readTransitions.cend()) {
                    if (readTransitions.find(ViewTransition::GAMELIST_TO_GAMELIST) !=
                        readTransitions.cend())
                        readTransitions[ViewTransition::STARTUP_TO_GAMELIST] =
                            readTransitions[ViewTransition::GAMELIST_TO_GAMELIST];
                }

                ThemeTransitions transition;
                transition.name = name;

                std::vector<std::pair<std::string, std::string>> labels;
                for (pugi::xml_node labelTag {transitions.child("label")}; labelTag;
                     labelTag = labelTag.next_sibling("label")) {
                    std::string language {labelTag.attribute("language").as_string()};
                    if (language == "")
                        language = "en_US";
                    else if (std::find_if(sSupportedLanguages.cbegin(), sSupportedLanguages.cend(),
                                          [language](std::pair<std::string, std::string> currLang) {
                                              return currLang.first == language;
                                          }) == sSupportedLanguages.cend()) {
                        LOG(LogWarning) << "Declared language \"" << language
                                        << "\" not supported, setting label language to \"en_US\""
                                           " for transition name \""
                                        << name << "\" in \"" << capFile << "\"";
                        language = "en_US";
                    }
                    std::string labelValue {labelTag.text().as_string()};
                    if (labelValue != "")
                        labels.emplace_back(std::make_pair(language, labelValue));
                }
                if (!labels.empty()) {
                    // Add the label languages in the order they are defined in sSupportedLanguages.
                    for (auto& language : sSupportedLanguages) {
                        if (language.first == "automatic")
                            continue;
                        const auto it =
                            std::find_if(labels.cbegin(), labels.cend(),
                                         [language](std::pair<std::string, std::string> currLabel) {
                                             return currLabel.first == language.first;
                                         });
                        if (it != labels.cend()) {
                            transition.labels.emplace_back(
                                std::make_pair((*it).first, (*it).second));
                        }
                    }
                }

                transition.selectable = selectable;
                transition.animations = std::move(readTransitions);
                capabilities.transitions.emplace_back(std::move(transition));
            }
        }

        for (pugi::xml_node suppressTransitionProfiles {
                 themeCapabilities.child("suppressTransitionProfiles")};
             suppressTransitionProfiles;
             suppressTransitionProfiles =
                 suppressTransitionProfiles.next_sibling("suppressTransitionProfiles")) {
            std::vector<std::string> readSuppressProfiles;

            for (pugi::xml_node entries {suppressTransitionProfiles.child("entry")}; entries;
                 entries = entries.next_sibling("entry")) {
                const std::string& entryValue {entries.text().as_string()};

                if (std::find(sSupportedTransitionAnimations.cbegin(),
                              sSupportedTransitionAnimations.cend(),
                              entryValue) != sSupportedTransitionAnimations.cend()) {
                    capabilities.suppressedTransitionProfiles.emplace_back(entryValue);
                }
                else {
                    LOG(LogWarning)
                        << "Found suppressTransitionProfiles <entry> tag with invalid value \""
                        << entryValue << "\", ignoring entry in \"" << capFile << "\"";
                }
            }

            // Sort and remove any duplicates.
            if (capabilities.suppressedTransitionProfiles.size() > 1) {
                std::sort(capabilities.suppressedTransitionProfiles.begin(),
                          capabilities.suppressedTransitionProfiles.end());
                auto last = std::unique(capabilities.suppressedTransitionProfiles.begin(),
                                        capabilities.suppressedTransitionProfiles.end());
                capabilities.suppressedTransitionProfiles.erase(
                    last, capabilities.suppressedTransitionProfiles.end());
            }
        }
    }
    else {
        capabilities.validTheme = false;
        LOG(LogWarning)
            << "No capabilities.xml file found, this does not appear to be a valid theme: \""
#if defined(_WIN64)
            << Utils::String::replace(path, "/", "\\") << "\"";
#else
            << path << "\"";
#endif
    }

    // Add the aspect ratios in the order they are defined in sSupportedAspectRatios so they
    // always show up in the same order in the UI Settings menu.
    if (!aspectRatiosTemp.empty()) {
        // Add the "automatic" aspect ratio if there is at least one entry.
        capabilities.aspectRatios.emplace_back(sSupportedAspectRatios.front().first);
        for (auto& aspectRatio : sSupportedAspectRatios) {
            if (std::find(aspectRatiosTemp.cbegin(), aspectRatiosTemp.cend(), aspectRatio.first) !=
                aspectRatiosTemp.cend()) {
                capabilities.aspectRatios.emplace_back(aspectRatio.first);
            }
        }
    }

    // Add the languages in the order they are defined in sSupportedLanguages so they always
    // show up in the same order in the UI Settings menu.
    if (!languagesTemp.empty()) {
        // Add the "automatic" language if there is at least one entry.
        capabilities.languages.emplace_back(sSupportedLanguages.front().first);
        for (auto& language : sSupportedLanguages) {
            if (std::find(languagesTemp.cbegin(), languagesTemp.cend(), language.first) !=
                languagesTemp.cend()) {
                capabilities.languages.emplace_back(language.first);
            }
        }
    }

    // Add the font sizes in the order they are defined in sSupportedFontSizes so they always
    // show up in the same order in the UI Settings menu.
    if (!fontSizesTemp.empty()) {
        for (auto& fontSize : sSupportedFontSizes) {
            if (std::find(fontSizesTemp.cbegin(), fontSizesTemp.cend(), fontSize.first) !=
                fontSizesTemp.cend()) {
                capabilities.fontSizes.emplace_back(fontSize.first);
            }
        }
    }

    if (hasTriggers) {
        for (auto& variant : capabilities.variants) {
            for (auto it = variant.overrides.begin(); it != variant.overrides.end();) {
                const auto variantIter =
                    std::find_if(capabilities.variants.begin(), capabilities.variants.end(),
                                 [it](ThemeVariant currVariant) {
                                     return currVariant.name == (*it).second.first;
                                 });
                if (variantIter == capabilities.variants.end()) {
                    LOG(LogWarning)
                        << "The <useVariant> tag value \"" << (*it).second.first
                        << "\" does not match any defined variants, ignoring entry in \"" << capFile
                        << "\"";
                    it = variant.overrides.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }

    return capabilities;
}

void ThemeData::parseIncludes(const pugi::xml_node& root)
{
    for (pugi::xml_node node {root.child("include")}; node; node = node.next_sibling("include")) {
        ThemeException error;
        error << "ThemeData::parseIncludes(): ";
        error.setFiles(mPaths);

        // Check if there's an unsupported theme version tag.
        if (root.child("formatVersion") != nullptr)
            throw error << ": Unsupported <formatVersion> tag found";

        std::string relPath {resolvePlaceholders(node.text().as_string())};
        std::string path {Utils::FileSystem::resolveRelativePath(relPath, mPaths.back(), true)};

        if (!ResourceManager::getInstance().fileExists(path)) {
            // For explicit paths, throw an error if the file couldn't be found, but only
            // print a debug message if it was set using a variable.
            if (relPath == node.text().get()) {
                throw error << " -> \"" << relPath << "\" not found (resolved to \"" << path
                            << "\")";
            }
            else {
                if (!(Settings::getInstance()->getBool("DebugSkipMissingThemeFiles") ||
                      (mCustomCollection && Settings::getInstance()->getBool(
                                                "DebugSkipMissingThemeFilesCustomCollections")))) {
#if defined(_WIN64)
                    LOG(LogDebug) << Utils::String::replace(error.message, "/", "\\")
                                  << ": Couldn't find file \"" << node.text().get() << "\" "
                                  << ((node.text().get() != path) ?
                                          "which resolves to \"" +
                                              Utils::String::replace(path, "/", "\\") + "\"" :
#else
                    LOG(LogDebug) << error.message << ": Couldn't find file \"" << node.text().get()
                                  << "\" "
                                  << ((node.text().get() != path) ?
                                          "which resolves to \"" + path + "\"" :
#endif
                                          "");
                }
                continue;
            }
        }

        error << " -> \"" << relPath << "\"";

        mPaths.push_back(path);

        pugi::xml_document includeDoc;
#if defined(_WIN64)
        pugi::xml_parse_result result {
            includeDoc.load_file(Utils::String::stringToWideString(path).c_str())};
#else
        pugi::xml_parse_result result {includeDoc.load_file(path.c_str())};
#endif
        if (!result)
            throw error << ": Error parsing file: " << result.description();

        pugi::xml_node theme {includeDoc.child("theme")};
        if (!theme)
            throw error << ": Missing <theme> tag";

        parseTransitions(theme);
        parseVariables(theme);
        parseColorSchemes(theme);
        parseFontSizes(theme);
        parseLanguages(theme);
        parseIncludes(theme);
        parseViews(theme);
        if (theme.child("feature") != nullptr)
            throw error << ": Unsupported <feature> tag found";
        parseVariants(theme);
        parseAspectRatios(theme);

        mPaths.pop_back();
    }
}

void ThemeData::parseVariants(const pugi::xml_node& root)
{
    if (sCurrentTheme == sThemes.end())
        return;

    if (mSelectedVariant == "")
        return;

    ThemeException error;
    error << "ThemeData::parseVariants(): ";
    error.setFiles(mPaths);

    for (pugi::xml_node node {root.child("variant")}; node; node = node.next_sibling("variant")) {
        if (!node.attribute("name"))
            throw error << ": <variant> tag missing \"name\" attribute";

        const std::string delim {" \t\r\n,"};
        const std::string nameAttr {node.attribute("name").as_string()};
        size_t prevOff {nameAttr.find_first_not_of(delim, 0)};
        size_t off {nameAttr.find_first_of(delim, prevOff)};
        std::string viewKey;
        while (off != std::string::npos || prevOff != std::string::npos) {
            viewKey = nameAttr.substr(prevOff, off - prevOff);
            prevOff = nameAttr.find_first_not_of(delim, off);
            off = nameAttr.find_first_of(delim, prevOff);

            if (std::find(mVariants.cbegin(), mVariants.cend(), viewKey) == mVariants.cend()) {
                throw error << ": <variant> value \"" << viewKey
                            << "\" is not defined in capabilities.xml";
            }

            const std::string variant {mOverrideVariant.empty() ? mSelectedVariant :
                                                                  mOverrideVariant};

            if (variant == viewKey || viewKey == "all") {
                parseTransitions(node);
                parseVariables(node);
                parseColorSchemes(node);
                parseFontSizes(node);
                parseLanguages(node);
                parseIncludes(node);
                parseViews(node);
                parseAspectRatios(node);
            }
        }
    }
}

void ThemeData::parseColorSchemes(const pugi::xml_node& root)
{
    if (sCurrentTheme == sThemes.end())
        return;

    if (mSelectedColorScheme == "")
        return;

    ThemeException error;
    error << "ThemeData::parseColorSchemes(): ";
    error.setFiles(mPaths);

    for (pugi::xml_node node {root.child("colorScheme")}; node;
         node = node.next_sibling("colorScheme")) {
        if (!node.attribute("name"))
            throw error << ": <colorScheme> tag missing \"name\" attribute";

        const std::string delim {" \t\r\n,"};
        const std::string nameAttr {node.attribute("name").as_string()};
        size_t prevOff {nameAttr.find_first_not_of(delim, 0)};
        size_t off {nameAttr.find_first_of(delim, prevOff)};
        std::string viewKey;
        while (off != std::string::npos || prevOff != std::string::npos) {
            viewKey = nameAttr.substr(prevOff, off - prevOff);
            prevOff = nameAttr.find_first_not_of(delim, off);
            off = nameAttr.find_first_of(delim, prevOff);

            if (std::find(mColorSchemes.cbegin(), mColorSchemes.cend(), viewKey) ==
                mColorSchemes.cend()) {
                throw error << ": <colorScheme> value \"" << viewKey
                            << "\" is not defined in capabilities.xml";
            }

            if (mSelectedColorScheme == viewKey) {
                parseVariables(node);
                parseIncludes(node);
            }
        }
    }
}

void ThemeData::parseFontSizes(const pugi::xml_node& root)
{
    if (sCurrentTheme == sThemes.end())
        return;

    if (mSelectedFontSize == "")
        return;

    ThemeException error;
    error << "ThemeData::parseFontSizes(): ";
    error.setFiles(mPaths);

    for (pugi::xml_node node {root.child("fontSize")}; node; node = node.next_sibling("fontSize")) {
        if (!node.attribute("name"))
            throw error << ": <fontSize> tag missing \"name\" attribute";

        const std::string delim {" \t\r\n,"};
        const std::string nameAttr {node.attribute("name").as_string()};
        size_t prevOff {nameAttr.find_first_not_of(delim, 0)};
        size_t off {nameAttr.find_first_of(delim, prevOff)};
        std::string viewKey;
        while (off != std::string::npos || prevOff != std::string::npos) {
            viewKey = nameAttr.substr(prevOff, off - prevOff);
            prevOff = nameAttr.find_first_not_of(delim, off);
            off = nameAttr.find_first_of(delim, prevOff);

            if (std::find(mFontSizes.cbegin(), mFontSizes.cend(), viewKey) == mFontSizes.cend()) {
                throw error << ": <fontSize> value \"" << viewKey
                            << "\" is not defined in capabilities.xml";
            }

            if (mSelectedFontSize == viewKey) {
                parseVariables(node);
                parseIncludes(node);
            }
        }
    }
}

void ThemeData::parseAspectRatios(const pugi::xml_node& root)
{
    if (sCurrentTheme == sThemes.end())
        return;

    if (sSelectedAspectRatio == "")
        return;

    ThemeException error;
    error << "ThemeData::parseAspectRatios(): ";
    error.setFiles(mPaths);

    for (pugi::xml_node node {root.child("aspectRatio")}; node;
         node = node.next_sibling("aspectRatio")) {
        if (!node.attribute("name"))
            throw error << ": <aspectRatio> tag missing \"name\" attribute";

        const std::string delim {" \t\r\n,"};
        const std::string nameAttr {node.attribute("name").as_string()};
        size_t prevOff {nameAttr.find_first_not_of(delim, 0)};
        size_t off {nameAttr.find_first_of(delim, prevOff)};
        std::string viewKey;
        while (off != std::string::npos || prevOff != std::string::npos) {
            viewKey = nameAttr.substr(prevOff, off - prevOff);
            prevOff = nameAttr.find_first_not_of(delim, off);
            off = nameAttr.find_first_of(delim, prevOff);

            if (std::find(sCurrentTheme->second.capabilities.aspectRatios.cbegin(),
                          sCurrentTheme->second.capabilities.aspectRatios.cend(),
                          viewKey) == sCurrentTheme->second.capabilities.aspectRatios.cend()) {
                throw error << ": <aspectRatio> value \"" << viewKey
                            << "\" is not defined in capabilities.xml";
            }

            if (sSelectedAspectRatio == viewKey) {
                parseVariables(node);
                parseColorSchemes(node);
                parseFontSizes(node);
                parseLanguages(node);
                parseIncludes(node);
                parseViews(node);
            }
        }
    }
}

void ThemeData::parseTransitions(const pugi::xml_node& root)
{
    ThemeException error;
    error << "ThemeData::parseTransitions(): ";
    error.setFiles(mPaths);

    const pugi::xml_node& transitions {root.child("transitions")};
    if (transitions != nullptr) {
        const std::string& transitionsValue {transitions.text().as_string()};
        if (std::find_if(sCurrentTheme->second.capabilities.transitions.cbegin(),
                         sCurrentTheme->second.capabilities.transitions.cend(),
                         [&transitionsValue](const ThemeTransitions transitions) {
                             return transitions.name == transitionsValue;
                         }) == sCurrentTheme->second.capabilities.transitions.cend()) {
            throw error << ": <transitions> value \"" << transitionsValue
                        << "\" is not matching any defined transitions";
        }
        sVariantDefinedTransitions = transitionsValue;
    }
}

void ThemeData::parseLanguages(const pugi::xml_node& root)
{
    if (sCurrentTheme == sThemes.end())
        return;

    if (sThemeLanguage == "")
        return;

    ThemeException error;
    error << "ThemeData::parseLanguages(): ";
    error.setFiles(mPaths);

    for (pugi::xml_node node {root.child("language")}; node; node = node.next_sibling("language")) {
        if (!node.attribute("name"))
            throw error << ": <language> tag missing \"name\" attribute";

        const std::string delim {" \t\r\n,"};
        const std::string nameAttr {node.attribute("name").as_string()};
        size_t prevOff {nameAttr.find_first_not_of(delim, 0)};
        size_t off {nameAttr.find_first_of(delim, prevOff)};
        std::string viewKey;
        while (off != std::string::npos || prevOff != std::string::npos) {
            viewKey = nameAttr.substr(prevOff, off - prevOff);
            prevOff = nameAttr.find_first_not_of(delim, off);
            off = nameAttr.find_first_of(delim, prevOff);

            if (std::find(mLanguages.cbegin(), mLanguages.cend(), viewKey) == mLanguages.cend()) {
                throw error << ": <language> value \"" << viewKey
                            << "\" is not defined in capabilities.xml";
            }

            if (sThemeLanguage == viewKey) {
                parseVariables(node);
                parseIncludes(node);
            }
        }
    }
}

void ThemeData::parseVariables(const pugi::xml_node& root)
{
    ThemeException error;
    error.setFiles(mPaths);

    for (pugi::xml_node node {root.child("variables")}; node;
         node = node.next_sibling("variables")) {

        for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it) {
            const std::string key {it->name()};
            const std::string val {resolvePlaceholders(it->text().as_string())};

            if (!val.empty()) {
                if (mVariables.find(key) != mVariables.end())
                    mVariables[key] = val;
                else
                    mVariables.insert(std::pair<std::string, std::string>(key, val));
            }
        }
    }
}

void ThemeData::parseViews(const pugi::xml_node& root)
{
    ThemeException error;
    error << "ThemeData::parseViews(): ";
    error.setFiles(mPaths);

    // Parse views.
    for (pugi::xml_node node {root.child("view")}; node; node = node.next_sibling("view")) {
        if (!node.attribute("name"))
            throw error << ": View missing \"name\" attribute";

        const std::string delim {" \t\r\n,"};
        const std::string nameAttr {node.attribute("name").as_string()};
        size_t prevOff {nameAttr.find_first_not_of(delim, 0)};
        size_t off {nameAttr.find_first_of(delim, prevOff)};
        std::string viewKey;
        while (off != std::string::npos || prevOff != std::string::npos) {
            viewKey = nameAttr.substr(prevOff, off - prevOff);
            prevOff = nameAttr.find_first_not_of(delim, off);
            off = nameAttr.find_first_of(delim, prevOff);

            if (std::find(sSupportedViews.cbegin(), sSupportedViews.cend(), viewKey) !=
                sSupportedViews.cend()) {
                ThemeView& view {
                    mViews.insert(std::pair<std::string, ThemeView>(viewKey, ThemeView()))
                        .first->second};
                parseView(node, view);
            }
            else {
                throw error << ": Unsupported \"" << viewKey << "\" view style defined";
            }
        }
    }
}

void ThemeData::parseView(const pugi::xml_node& root, ThemeView& view)
{
    ThemeException error;
    error << "ThemeData::parseView(): ";
    error.setFiles(mPaths);

    for (pugi::xml_node node {root.first_child()}; node; node = node.next_sibling()) {
        if (!node.attribute("name"))
            throw error << ": Element of type \"" << node.name() << "\" missing \"name\" attribute";

        auto elemTypeIt = sElementMap.find(node.name());
        if (elemTypeIt == sElementMap.cend())
            throw error << ": Unknown element type \"" << node.name() << "\"";

        const std::string delim {" \t\r\n,"};
        const std::string nameAttr {node.attribute("name").as_string()};
        size_t prevOff {nameAttr.find_first_not_of(delim, 0)};
        size_t off {nameAttr.find_first_of(delim, prevOff)};
        while (off != std::string::npos || prevOff != std::string::npos) {
            std::string elemKey {nameAttr.substr(prevOff, off - prevOff)};
            prevOff = nameAttr.find_first_not_of(delim, off);
            off = nameAttr.find_first_of(delim, prevOff);

            // Add the element type as a prefix to avoid name collisions between different
            // component types.
            elemKey = std::string {node.name()} + "_" + elemKey;

            parseElement(
                node, elemTypeIt->second,
                view.elements.insert(std::pair<std::string, ThemeElement>(elemKey, ThemeElement()))
                    .first->second);
        }
    }
}

void ThemeData::parseElement(const pugi::xml_node& root,
                             const std::map<std::string, ElementPropertyType>& typeMap,
                             ThemeElement& element)
{
    ThemeException error;
    error << "ThemeData::parseElement(): ";
    error.setFiles(mPaths);
    element.type = root.name();

    if (root.attribute("extra") != nullptr)
        throw error << ": Unsupported \"extra\" attribute found for element of type \""
                    << element.type << "\"";

    for (pugi::xml_node node {root.first_child()}; node; node = node.next_sibling()) {
        auto typeIt = typeMap.find(node.name());
        if (typeIt == typeMap.cend())
            throw error << ": Unknown property type \"" << node.name()
                        << "\" for element of type \"" << root.name() << "\"";

        std::string str {resolvePlaceholders(node.text().as_string())};

        // Handle the special case with mutually exclusive system variables, for example
        // system.fullName.autoCollections and system.fullName.noCollections which can never
        // exist at the same time. A backspace is assigned in SystemData to flag the
        // variables that do not apply and if it's encountered here we simply skip the
        // property.
        if (str == "\b")
            continue;

        // Strictly enforce that there are no blank values in the theme configuration.
        if (str == "")
            throw error << ": Property \"" << typeIt->first << "\" for element \"" << element.type
                        << "\" has no value defined";

        std::string nodeName {node.name()};

        // If an attribute exists, then replace nodeName with its name.
        auto attributeEntry = sPropertyAttributeMap.find(element.type);
        if (attributeEntry != sPropertyAttributeMap.end()) {
            auto attribute = attributeEntry->second.find(typeIt->first);
            if (attribute != attributeEntry->second.end()) {
                if (node.attribute(attribute->second.c_str()) == nullptr) {
                    throw error << ": Unknown attribute \"" << node.first_attribute().name()
                                << "\" for property \"" << typeIt->first << "\" (element \""
                                << attributeEntry->first << "\")";
                }
                else {
                    // Add the attribute name as a prefix to avoid potential name collisions.
                    nodeName = attribute->second + "_" +
                               node.attribute(attribute->second.c_str()).as_string("");
                }
            }
        }

        switch (typeIt->second) {
            case NORMALIZED_RECT: {
                glm::vec4 val;

                auto splits = Utils::String::delimitedStringToVector(str, " ");
                if (splits.size() == 2) {
                    val = glm::vec4 {static_cast<float>(atof(splits.at(0).c_str())),
                                     static_cast<float>(atof(splits.at(1).c_str())),
                                     static_cast<float>(atof(splits.at(0).c_str())),
                                     static_cast<float>(atof(splits.at(1).c_str()))};
                }
                else if (splits.size() == 4) {
                    val = glm::vec4 {static_cast<float>(atof(splits.at(0).c_str())),
                                     static_cast<float>(atof(splits.at(1).c_str())),
                                     static_cast<float>(atof(splits.at(2).c_str())),
                                     static_cast<float>(atof(splits.at(3).c_str()))};
                }

                element.properties[node.name()] = val;
                break;
            }
            case NORMALIZED_PAIR: {
                size_t divider = str.find(' ');
                if (divider == std::string::npos)
                    throw error << ": Invalid normalized pair value \"" << str.c_str()
                                << "\" for property \"" << node.name() << "\"";

                std::string first {str.substr(0, divider)};
                std::string second {str.substr(divider, std::string::npos)};

                glm::vec2 val {static_cast<float>(atof(first.c_str())),
                               static_cast<float>(atof(second.c_str()))};

                element.properties[node.name()] = val;
                break;
            }
            case STRING: {
                element.properties[node.name()] = str;
                break;
            }
            case PATH: {
                std::string path;

                if (!str.empty() && str.front() == ':')
                    path = ResourceManager::getInstance().getResourcePath(str);
                else
                    path = Utils::FileSystem::resolveRelativePath(str, mPaths.back(), true);

                if (!ResourceManager::getInstance().fileExists(path)) {
                    std::stringstream ss;
                    // For explicit paths, print a warning if the file couldn't be found, but
                    // only print a debug message if it was set using a variable.
                    if (str == node.text().as_string()) {
#if defined(_WIN64)
                        LOG(LogWarning) << Utils::String::replace(error.message, "/", "\\")
                                        << ": Couldn't find file \"" << node.text().get() << "\" "
                                        << ((node.text().get() != path) ?
                                                "which resolves to \"" +
                                                    Utils::String::replace(path, "/", "\\") + "\"" :
#else
                        LOG(LogWarning)
                            << error.message << ": Couldn't find file \"" << node.text().get()
                            << "\" "
                            << ((node.text().get() != path) ? "which resolves to \"" + path + "\"" :
#endif
                                                "")
                                        << " (element type \"" << element.type << "\", name \""
                                        << root.attribute("name").as_string() << "\", property \""
                                        << nodeName << "\")";
                    }
                    else if (!(Settings::getInstance()->getBool("DebugSkipMissingThemeFiles") ||
                               (mCustomCollection &&
                                Settings::getInstance()->getBool(
                                    "DebugSkipMissingThemeFilesCustomCollections")))) {
#if defined(_WIN64)
                        LOG(LogDebug) << Utils::String::replace(error.message, "/", "\\")
                                      << ": Couldn't find file \"" << node.text().get() << "\" "
                                      << ((node.text().get() != path) ?
                                              "which resolves to \"" +
                                                  Utils::String::replace(path, "/", "\\") + "\"" :
#else
                        LOG(LogDebug)
                            << error.message << ": Couldn't find file \"" << node.text().get()
                            << "\" "
                            << ((node.text().get() != path) ? "which resolves to \"" + path + "\"" :
#endif
                                              "")
                                      << " (element type \"" << element.type << "\", name \""
                                      << root.attribute("name").as_string() << "\", property \""
                                      << nodeName << "\")";
                    }
                }
                element.properties[nodeName] = path;
                break;
            }
            case COLOR: {
                try {
                    element.properties[node.name()] = getHexColor(str);
                }
                catch (ThemeException& e) {
                    throw error << ": " << e.what();
                }
                break;
            }
            case UNSIGNED_INTEGER: {
                unsigned int integerVal {static_cast<unsigned int>(strtoul(str.c_str(), 0, 0))};
                element.properties[node.name()] = integerVal;
                break;
            }
            case FLOAT: {
                float floatVal {static_cast<float>(strtod(str.c_str(), 0))};
                element.properties[node.name()] = floatVal;
                break;
            }
            case BOOLEAN: {
                bool boolVal = false;
                // Only look at the first character.
                if (str.size() > 0) {
                    if (str.front() == '1' || str.front() == 't' || str.front() == 'T' ||
                        str.front() == 'y' || str.front() == 'Y')
                        boolVal = true;
                }

                element.properties[node.name()] = boolVal;
                break;
            }
            default: {
                throw error << ": Unknown ElementPropertyType for \""
                            << root.attribute("name").as_string() << "\", property " << node.name();
            }
        }
    }
}

#if defined(GETTEXT_DUMMY_ENTRIES)
void ThemeData::gettextMessageCatalogEntries()
{
    // sSupportedFontSizes
    _("medium");
    _("large");
    _("small");
    _("extra small");
    _("extra large");

    // sSupportedAspectRatios
    _("automatic");
    _("16:9 vertical");
    _("16:10 vertical");
    _("3:2 vertical");
    _("4:3 vertical");
    _("5:4 vertical");
    _("19.5:9 vertical");
    _("20:9 vertical");
    _("21:9 vertical");
    _("32:9 vertical");
}
#endif
