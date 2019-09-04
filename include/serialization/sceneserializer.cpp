#include <string>
#include <filesystem>
#include <cstdio>
#include <iostream>
#include <fstream>

#include "../3rdParty/rapidjson/rapidjson.h"
#include "../3rdParty/rapidjson/document.h"
#include "../3rdParty/rapidjson/prettywriter.h"
#include "../3rdParty/rapidjson/filewritestream.h"
#include "../3rdParty/rapidjson/istreamwrapper.h"

#include "sceneserializer.h"
#include "entityserializer.h"

using namespace hserialization;

void hsceneserializer::serializeScene(ecs::Entity* sceneEntity, std::filesystem::path scenePath, const std::string& spath, bool pretty) {
    scenePath /= std::filesystem::path(spath.c_str());
    
    // TODO: better logging util (dbg macro from github?)
    std::cerr << "Saving current scene to: " << scenePath.string() << '\n';
    
    rapidjson::Document doc;
    doc.SetObject();
    hentityserializer::serializeEntity(sceneEntity, doc, doc.GetAllocator());
    
    FILE* fp = fopen(scenePath.string().c_str(), "wb");
    char buf[65536];
    rapidjson::FileWriteStream os(fp, buf, sizeof(buf));
    if (pretty) {
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        doc.Accept(writer);
    } else {
        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        doc.Accept(writer);
    }
        
    fclose(fp);
}

void hsceneserializer::deserializeScene(ecs::EntityManager& entityMgr, std::filesystem::path scenePath, const std::string& spath) {
    scenePath /= std::filesystem::path(spath.c_str());
    
    std::cerr << "Loading scene located at: " << scenePath.string() << '\n';
    
    rapidjson::Document doc;
    std::ifstream ifs(scenePath.string());
    rapidjson::IStreamWrapper isw(ifs);
    doc.ParseStream(isw);
    
    hentityserializer::deserializeEntity(nullptr, entityMgr, doc);
}
