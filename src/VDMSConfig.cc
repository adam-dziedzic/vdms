/**
 * @file   VDMSConfig.cc
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <map>
#include <sstream>
#include <fstream>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <jsoncpp/json/json.h>

#include "VDMSConfig.h"

#define DEFAULT_PATH_ROOT        "db"
#define DEFAULT_PATH_PMGD        "graph"
#define DEFAULT_PATH_IMAGES      "images"
#define DEFAULT_PATH_JPG         "jpg"
#define DEFAULT_PATH_PNG         "png"
#define DEFAULT_PATH_TDB         "tdb"
#define DEFAULT_PATH_BLOBS       "blobs"
#define DEFAULT_PATH_VIDEOS      "videos"
#define DEFAULT_PATH_DESCRIPTORS "descriptors"

using namespace VDMS;

VDMSConfig* VDMSConfig::cfg;

bool VDMSConfig::init(std::string config_file)
{
    if(cfg)
        return false;

    cfg = new VDMSConfig(config_file);
    return true;
}

void VDMSConfig::destroy()
{
    if (cfg) {
        delete cfg;
        cfg = NULL;
    }
}

VDMSConfig* VDMSConfig::instance()
{
    if(cfg)
        return cfg;

    std::cout << "ERROR: Config not init" << std::endl;
    return NULL;
}

VDMSConfig::VDMSConfig(std::string config_file)
{
    Json::Reader reader;
    std::ifstream file(config_file);

    bool parsingSuccessful = reader.parse(file, json_config);

    if (!parsingSuccessful){
        std::cout << "Error parsing config file." << std::endl;
        std::cout << "Exiting..." << std::endl;
        exit(0);
    }

    build_dirs();
}

int VDMSConfig::get_int_value(std::string val, int def)
{
    return json_config.get(val, def).asInt();
}

std::string VDMSConfig::get_string_value(std::string val, std::string def)
{
    return json_config.get(val, def).asString();
}

// This method will check if the dir exists,
// and create the dir if it does not exist.
int VDMSConfig::create_dir(std::string path)
{
    struct stat sb;
    while (1)
        if (stat(path.c_str(), &sb) == 0)
            if (sb.st_mode & S_IFDIR)
                return 0;
            else
                return EEXIST;
        else if (errno != ENOENT)
            return errno;
        else if (mkdir(path.c_str(), 0777) == 0)
            return 0;
        else if (errno != EEXIST)
            return errno;
}

void VDMSConfig::check_or_create(std::string path)
{
    if (create_dir(path) == 0){
        return;
    }
    else{
        std::cout << "Cannot open/create directories structure." << std::endl;
        std::cout << "Failed dir: " << path << std::endl;
        std::cout << "Check paths and permissions." << std::endl;
        std::cout << "Exiting..." << std::endl;
        exit(0);
    }
}

void VDMSConfig::build_dirs()
{
    // Root
    path_root = get_string_value(PARAM_DB_ROOT, DEFAULT_PATH_ROOT);
    check_or_create(path_root);

    // PMGD
    path_pmgd = path_root + "/" + DEFAULT_PATH_PMGD;
    path_pmgd = get_string_value(PARAM_DB_PMGD, path_pmgd);
    check_or_create(path_pmgd);

    // IMAGES
    path_images = path_root + "/" + DEFAULT_PATH_IMAGES;
    path_images = get_string_value(PARAM_DB_IMAGES, path_images);
    check_or_create(path_images);

    // IMAGES - PNG
    path_png = path_images + "/" + DEFAULT_PATH_PNG;
    path_png = get_string_value(PARAM_DB_PNG, path_png);
    check_or_create(path_png);

    // IMAGES - JPG
    path_jpg = path_images + "/" + DEFAULT_PATH_JPG;
    path_jpg = get_string_value(PARAM_DB_JPG, path_jpg);
    check_or_create(path_jpg);

    // IMAGES - TDB
    path_tdb = path_images + "/" + DEFAULT_PATH_TDB;
    path_tdb = get_string_value(PARAM_DB_TDB, path_tdb);
    check_or_create(path_tdb);

    // BLOBS
    path_blobs = path_root + "/" + DEFAULT_PATH_BLOBS;
    path_blobs = get_string_value(PARAM_DB_BLOBS, path_blobs);
    check_or_create(path_blobs);

    // VIDEOS
    path_videos = path_root + "/" + DEFAULT_PATH_VIDEOS;
    path_videos = get_string_value(PARAM_DB_VIDEOS, path_videos);
    check_or_create(path_videos);

    // DESCRIPTORS
    path_descriptors = path_root + "/" + DEFAULT_PATH_DESCRIPTORS;
    path_descriptors = get_string_value(PARAM_DB_DESCRIPTORS, path_descriptors);
    check_or_create(path_descriptors);
}
