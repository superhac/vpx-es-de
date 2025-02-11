//  SPDX-License-Identifier: MIT
//
//  ES-DE Frontend
//  LocalizationUtil.cpp
//
//  Localization functions.
//  Provides support for translations using gettext/libintl.
//

#include "utils/LocalizationUtil.h"

#include "Log.h"
#include "Settings.h"
#include "resources/ResourceManager.h"
#include "utils/StringUtil.h"

#include <SDL2/SDL_locale.h>

#include <algorithm>
#include <iostream>

#if defined(_WIN64)
#include <Windows.h>
#endif

namespace Utils
{
    namespace Localization
    {
        // clang-format off
        // When adding a new locale, then make sure to also update ThemeData::sSupportedLanguages.
        const std::vector<std::pair<std::string, std::string>> sSupportedLocales {{{"en"}, {"US"}},
                                                                                  {{"en"}, {"GB"}},
                                                                                  {{"ca"}, {"ES"}},
                                                                                  {{"de"}, {"DE"}},
                                                                                  {{"es"}, {"ES"}},
                                                                                  {{"fr"}, {"FR"}},
                                                                                  {{"it"}, {"IT"}},
                                                                                  {{"nl"}, {"NL"}},
                                                                                  {{"pl"}, {"PL"}},
                                                                                  {{"pt"}, {"BR"}},
                                                                                  {{"ro"}, {"RO"}},
                                                                                  {{"ru"}, {"RU"}},
                                                                                  {{"sv"}, {"SE"}},
                                                                                  {{"ja"}, {"JP"}},
                                                                                  {{"ko"}, {"KR"}},
                                                                                  {{"zh"}, {"CN"}},
                                                                                  {{"zh"}, {"TW"}}};
        // clang-format on

        std::string sCurrentLocale {"en_US"};
        float sMenuTitleScaleFactor {1.0f};
#if defined(_WIN64)
        unsigned long sLocaleID {0};
#endif

        const char* pgettextBuiltin(const char* msgctxt, const char* msgid)
        {
            // This is an unbelievable hack but it's actually done pretty much the same way in
            // the gettext.h header where a macro is used to wrap around the libintl functionality.
            // Why this function is simply not part of libintl itself is anyone's guess, as that
            // would be the logical thing to do.
            std::string lookup;
            lookup.append(msgctxt).append("\004").append(msgid);
            const char* translation {gettext(lookup.c_str())};
            if (translation == lookup.c_str())
                return msgid;
            else
                return translation;
        }

        const char* npgettextBuiltin(const char* msgctxt,
                                     const char* msgid1,
                                     const char* msgid2,
                                     unsigned long int n)
        {
            std::string lookup;
            lookup.append(msgctxt).append("\004").append(msgid1);
            const char* translation {ngettext(lookup.c_str(), msgid2, n)};
            if (translation == lookup.c_str())
                return msgid1;
            else
                return translation;
        }

        std::pair<std::string, std::string> getLocale()
        {
#if defined(_WIN64)
            std::wstring localeNameWide(LOCALE_NAME_MAX_LENGTH, '\0');
            if (GetUserDefaultLocaleName(&localeNameWide[0], LOCALE_NAME_MAX_LENGTH) == 0)
                return std::make_pair("en", "US");

            std::string localeName {Utils::String::wideStringToString(localeNameWide)};
            localeName.erase(localeName.find('\0'));

            // This should never happen, but who knows with Windows.
            if (localeName.empty())
                return std::make_pair("en", "US");

            std::vector<std::string> localeVector;

            // Of course Windows doesn't follow standards and names locales with dashes
            // instead of underscores, such as "sv-SE" instead of "sv_SE". But who knows
            // if this is consistent, so we check for underscores as an extra precaution.
            if (localeName.find("_") != std::string::npos)
                localeVector = Utils::String::delimitedStringToVector(localeName, "_");
            else
                localeVector = Utils::String::delimitedStringToVector(localeName, "-");

            if (localeVector.size() == 1)
                return std::make_pair(localeVector[0], "");
            else
                return std::make_pair(localeVector[0], localeVector[1]);
#else
            // SDL_GetPreferredLocales() does not seem to always return accurate results
            // on Windows but for all other operating systems we use it.
            SDL_Locale* preferredLocales {SDL_GetPreferredLocales()};

            if (preferredLocales == nullptr)
                return std::make_pair("en", "US");

            std::string language {preferredLocales->language};
            std::string country;
            if (preferredLocales->country != nullptr)
                country = preferredLocales->country;

            SDL_free(preferredLocales);
            return std::make_pair(language, country);
#endif
        }

        void setLocale()
        {
            // Only detect locale once (on application startup).
            if (Settings::getInstance()->getString("DetectedLocale") == "") {
                const std::pair<std::string, std::string> detectedLocale {getLocale()};
                if (detectedLocale.second == "")
                    Settings::getInstance()->setString("DetectedLocale", detectedLocale.first);
                else {
                    Settings::getInstance()->setString(
                        "DetectedLocale", detectedLocale.first + "_" + detectedLocale.second);
                }
            }

            sMenuTitleScaleFactor = 1.0f;
            sCurrentLocale = "en_US";
            std::string languageSetting {Settings::getInstance()->getString("ApplicationLanguage")};
            std::vector<std::string> localeVector;
            std::pair<std::string, std::string> localePair;

            if (languageSetting == "automatic") {
                localeVector = Utils::String::delimitedStringToVector(
                    Settings::getInstance()->getString("DetectedLocale"), "_");
            }
            else {
                localeVector = Utils::String::delimitedStringToVector(languageSetting, "_");
            }
            if (localeVector.size() == 1)
                localePair = std::make_pair(localeVector[0], "");
            else
                localePair = std::make_pair(localeVector[0], localeVector[1]);

            std::string locale;
            std::string localePairCombined;

            if (localePair.second == "")
                localePairCombined = localePair.first;
            else
                localePairCombined = localePair.first + "_" + localePair.second;

            if (std::find(sSupportedLocales.cbegin(), sSupportedLocales.cend(), localePair) !=
                sSupportedLocales.cend()) {
                locale = localePairCombined;
                LOG(LogInfo) << "Application language set to \"" << locale << "\"";
            }
            else {
                for (auto& localeEntry : sSupportedLocales) {
                    if (localeEntry.first == localePair.first) {
                        LOG(LogInfo) << "No support for language \"" << localePairCombined
                                     << "\", falling back to closest match \""
                                     << localeEntry.first + "_" + localeEntry.second << "\"";
                        locale = localeEntry.first + "_" + localeEntry.second;
                        break;
                    }
                }
            }

            if (locale == "") {
                LOG(LogInfo) << "No support for language \"" << localePairCombined
                             << "\", falling back to default \"en_US\"";
                locale = "en_US";
            }

            // Language-specific menu title scale factor.
            if (localePair.first == "ca")
                sMenuTitleScaleFactor = 0.92f;
            else if (localePair.first == "de")
                sMenuTitleScaleFactor = 0.92f;
            else if (localePair.first == "es")
                sMenuTitleScaleFactor = 0.90f;
            else if (localePair.first == "fr")
                sMenuTitleScaleFactor = 0.90f;
            else if (localePair.first == "it")
                sMenuTitleScaleFactor = 0.94f;
            else if (localePair.first == "nl")
                sMenuTitleScaleFactor = 0.94f;
            else if (localePair.first == "pl")
                sMenuTitleScaleFactor = 0.94f;
            else if (localePair.first == "pt")
                sMenuTitleScaleFactor = 0.90f;
            else if (localePair.first == "ro")
                sMenuTitleScaleFactor = 0.94f;
            else if (localePair.first == "ru")
                sMenuTitleScaleFactor = 0.94f;
            else if (localePair.first == "sv")
                sMenuTitleScaleFactor = 0.87f;
            else if (localePair.first == "ja")
                sMenuTitleScaleFactor = 0.94f;
            else if (localePair.first == "ko")
                sMenuTitleScaleFactor = 0.96f;
            else if (localePair.first == "zh")
                sMenuTitleScaleFactor = 0.94f;

            std::string localePath;
            localePath.append("/")
                .append(locale)
                .append("/LC_MESSAGES/")
                .append(locale)
                .append(".mo");

            // If the message catalog file is not found then an emergency shutdown will be
            // initiated by ResourceManager.
            std::string objectPath {
                ResourceManager::getInstance().getResourcePath(":/locale" + localePath)};

            // This makes it possible to override the message catalog with a file in the
            // application data directory.
            if (objectPath.length() > localePath.length())
                objectPath = objectPath.substr(0, objectPath.length() - localePath.length());

#if defined(_WIN64)
            _configthreadlocale(_DISABLE_PER_THREAD_LOCALE);
            sLocaleID = LocaleNameToLCID(Utils::String::stringToWideString(locale).c_str(),
                                         LOCALE_ALLOW_NEUTRAL_NAMES);
            SetThreadLocale(sLocaleID);
#else
            setenv("LANGUAGE", locale.c_str(), 1);
            setenv("LANG", locale.c_str(), 1);
            // For some bizarre reason we need to first set the locale to en_US.UTF-8 before
            // we set it to the requested locale as some specific locales like pt_BR and zh_CN
            // otherwise won't work consistently. This must be some kind of library or OS bug as
            // it only happens on regular Linux, and not on macOS, Windows, Android or FreeBSD.
            setlocale(LC_MESSAGES, std::string {"en_US.UTF-8"}.c_str());

            setlocale(LC_MESSAGES, std::string {locale + ".UTF-8"}.c_str());
#endif
            textdomain(locale.c_str());
            bindtextdomain(locale.c_str(), objectPath.c_str());
            bind_textdomain_codeset(locale.c_str(), "UTF-8");
            sCurrentLocale = locale;
        }

#if defined(_WIN64)
        void setThreadLocale()
        {
            // This is essentially a workaround for what seems to be a bug in the libintl library
            // where _configthreadlocale(_DISABLE_PER_THREAD_LOCALE) does not have the expected
            // effect. If gettext() is called from a child thread and the string has not been
            // translated already from the main thread then it will not get translated to the
            // selected locale. If however the string is first translated on the main thread and
            // then translated again from the child thread it works correctly. However, if calling
            // SetThreadLocale() from the child thread before attempting any translations then
            // everything behaves as expected which feels like a cleaner workaround than having
            // to make every single translation from the main thread that we want to use in any
            // child threads.

            if (sLocaleID == 0)
                return;

            SetThreadLocale(sLocaleID);
        }
#endif

    } // namespace Localization

} // namespace Utils
