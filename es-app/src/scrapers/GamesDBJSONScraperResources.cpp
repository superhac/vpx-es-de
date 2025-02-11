//  SPDX-License-Identifier: MIT
//
//  ES-DE Frontend
//  GamesDBJSONScraperResources.cpp
//
//  Functions specifically for scraping from thegamesdb.net
//  Called from GamesDBJSONScraper.
//
//  Downloads these resource files to the scrapers folder in the application data directory:
//  gamesdb_developers.json
//  gamesdb_genres.json
//  gamesdb_publishers.json
//

#if defined(_MSC_VER) // MSVC compiler.
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#endif

#include "scrapers/GamesDBJSONScraperResources.h"

#include "Log.h"
#include "utils/FileSystemUtil.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include <chrono>
#include <fstream>
#include <memory>
#include <thread>

using namespace rapidjson;

namespace
{
    constexpr char GamesDBAPIKey[] {
        "e42de8fb0cdcea89fec60f70cd565122f34f5c6228be3e9ae247ba409779d9d5"};

    constexpr int MAX_WAIT_MS {90000};
    constexpr int POLL_TIME_MS {500};
    constexpr int MAX_WAIT_ITER {MAX_WAIT_MS / POLL_TIME_MS};

    constexpr char SCRAPER_RESOURCES_DIR[] {"scrapers"};
    constexpr char DEVELOPERS_JSON_FILE[] {"thegamesdb_developers.json"};
    constexpr char PUBLISHERS_JSON_FILE[] {"thegamesdb_publishers.json"};
    constexpr char GENRES_JSON_FILE[] {"thegamesdb_genres.json"};
    constexpr char DEVELOPERS_ENDPOINT[] {"/Developers"};
    constexpr char PUBLISHERS_ENDPOINT[] {"/Publishers"};
    constexpr char GENRES_ENDPOINT[] {"/Genres"};

    std::string genFilePath(const std::string& file_name)
    {
        return Utils::FileSystem::getGenericPath(getScrapersResouceDir() + "/" + file_name);
    }

    void ensureScrapersResourcesDir()
    {
        std::string path {getScrapersResouceDir()};
        if (!Utils::FileSystem::exists(path))
            Utils::FileSystem::createDirectory(path);
    }

} // namespace

std::string getScrapersResouceDir()
{
    return Utils::FileSystem::getAppDataDirectory() + "/" + SCRAPER_RESOURCES_DIR;
}

std::string TheGamesDBJSONRequestResources::getApiKey() const { return GamesDBAPIKey; }

void TheGamesDBJSONRequestResources::prepare()
{
    if (checkLoaded())
        return;

    if (loadResource(gamesdb_new_developers_map, "developers", genFilePath(DEVELOPERS_JSON_FILE)) &&
        !gamesdb_developers_resource_request)
        gamesdb_developers_resource_request = fetchResource(DEVELOPERS_ENDPOINT);

    if (loadResource(gamesdb_new_publishers_map, "publishers", genFilePath(PUBLISHERS_JSON_FILE)) &&
        !gamesdb_publishers_resource_request)
        gamesdb_publishers_resource_request = fetchResource(PUBLISHERS_ENDPOINT);

    if (loadResource(gamesdb_new_genres_map, "genres", genFilePath(GENRES_JSON_FILE)) &&
        !gamesdb_genres_resource_request)
        gamesdb_genres_resource_request = fetchResource(GENRES_ENDPOINT);
}

void TheGamesDBJSONRequestResources::ensureResources()
{
    if (checkLoaded())
        return;

    for (int i = 0; i < MAX_WAIT_ITER; ++i) {

        if (gamesdb_developers_resource_request &&
            saveResource(gamesdb_developers_resource_request.get(), gamesdb_new_developers_map,
                         "developers", genFilePath(DEVELOPERS_JSON_FILE)))
            gamesdb_developers_resource_request.reset(nullptr);

        if (gamesdb_publishers_resource_request &&
            saveResource(gamesdb_publishers_resource_request.get(), gamesdb_new_publishers_map,
                         "publishers", genFilePath(PUBLISHERS_JSON_FILE)))
            gamesdb_publishers_resource_request.reset(nullptr);

        if (gamesdb_genres_resource_request &&
            saveResource(gamesdb_genres_resource_request.get(), gamesdb_new_genres_map, "genres",
                         genFilePath(GENRES_JSON_FILE)))
            gamesdb_genres_resource_request.reset(nullptr);

        if (!gamesdb_developers_resource_request && !gamesdb_publishers_resource_request &&
            !gamesdb_genres_resource_request)
            return;

        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_TIME_MS));
    }
    LOG(LogError) << "Timed out while waiting for resources\n";
}

bool TheGamesDBJSONRequestResources::checkLoaded()
{
    return !gamesdb_new_genres_map.empty() && !gamesdb_new_developers_map.empty() &&
           !gamesdb_new_publishers_map.empty();
}

bool TheGamesDBJSONRequestResources::saveResource(HttpReq* req,
                                                  std::unordered_map<int, std::string>& resource,
                                                  const std::string& resource_name,
                                                  const std::string& file_name)
{

    if (req == nullptr) {
        LOG(LogError) << "HTTP request pointer was null\n";
        return true;
    }
    if (req->status() == HttpReq::REQ_IN_PROGRESS) {
        return false; // Not ready: wait some more.
    }
    if (req->status() != HttpReq::REQ_SUCCESS) {
        LOG(LogError) << "Resource request for " << file_name << " failed:\n\t"
                      << req->getErrorMsg();
        return true; // Request failed, resetting request..
    }

    ensureScrapersResourcesDir();

    std::ofstream fout {file_name, std::ios::binary};
    fout << req->getContent();
    fout.close();
    loadResource(resource, resource_name, file_name);
    return true;
}

std::unique_ptr<HttpReq> TheGamesDBJSONRequestResources::fetchResource(const std::string& endpoint)
{
    std::string path {"https://api.thegamesdb.net/v1"};
    path.append(endpoint).append("?apikey=").append(getApiKey());

    return std::unique_ptr<HttpReq>(new HttpReq(path, true));
}

int TheGamesDBJSONRequestResources::loadResource(std::unordered_map<int, std::string>& resource,
                                                 const std::string& resource_name,
                                                 const std::string& file_name)
{
    std::ifstream fin {file_name};
    if (!fin.good())
        return 1;

    std::stringstream buffer;
    buffer << fin.rdbuf();
    Document doc;
    doc.Parse(buffer.str().c_str());

    if (doc.HasParseError()) {
        std::string err = std::string("TheGamesDBJSONRequest - "
                                      "Error parsing JSON for resource file ") +
                          file_name + ":\n\t" + GetParseError_En(doc.GetParseError());
        LOG(LogError) << err;
        return 1;
    }

    if (!doc.HasMember("data") || !doc["data"].HasMember(resource_name.c_str()) ||
        !doc["data"][resource_name.c_str()].IsObject()) {
        std::string err = "TheGamesDBJSONRequest - Response had no resource data\n";
        LOG(LogError) << err;
        return 1;
    }
    auto& data = doc["data"][resource_name.c_str()];

    for (Value::ConstMemberIterator it = data.MemberBegin(); it != data.MemberEnd(); ++it) {
        auto& entry = it->value;
        if (!entry.IsObject() || !entry.HasMember("id") || !entry["id"].IsInt() ||
            !entry.HasMember("name") || !entry["name"].IsString())
            continue;

        resource[entry["id"].GetInt()] = entry["name"].GetString();
    }
    return resource.empty();
}
