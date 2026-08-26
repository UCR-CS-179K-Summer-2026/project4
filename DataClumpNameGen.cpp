#include "DataClumpNameGen.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        // Handle memory allocation error
        return 0;
    }
    return newLength;
}

std::string DataClumpNameGen::getKey() {
    std::ifstream file;
    file.open("../../.env");

    if (!file.is_open()) {
        file.open("../.env");
        if(!file.is_open()) {
            file.open(".env");
            if(!file.is_open()) {
                std::cerr << "Failed to open .env file." << std::endl;
                return "";
            }
        }
    }

    std::string line;
    while (std::getline(file, line)) {
        const std::string prefix = "GOOGLE_GEMINI_API_KEY=";
        if (line.rfind(prefix, 0) == 0) {
            file.close();
            return line.substr(prefix.length());
        }
    }

    return "";
}

std::string DataClumpNameGen::generateName(const std::string& variableList) {
    std::string apiKey = getKey();
    if (apiKey.empty()) {
        std::cout << "GOOGLE_GEMINI_API_KEY is not set in .env file." << std::endl;
        throw std::runtime_error("GOOGLE_GEMINI_API_KEY is not set.");
    }

    CURLcode initialize = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (initialize != CURLE_OK) {
        std::cout << "Failed to initialize libcurl: " << curl_easy_strerror(initialize) << std::endl;
        throw std::runtime_error("Failed to initialize libcurl.");
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cout << "Failed to create CURL handle." << std::endl;
        curl_global_cleanup();
        throw std::runtime_error("Failed to create CURL handle.");
    }

    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3.5-flash-lite:generateContent";

    std::string prompt =
        "You are a code refactoring assistant. "
        "Given the following list of variable names, generate a concise "
        "and meaningful name for a struct or class that could encapsulate "
        "these variables. "
        "The name should be in PascalCase and should not include spaces "
        "or special characters. "
        "Variable names: " + variableList + ". "
        "Provide only the recommended name. Do not include any additional text or explanation.";

    std::string body =
        "{"
        "\"contents\":["
            "{"
                "\"parts\":["
                    "{"
                        "\"text\":\"" + prompt + "\""
                    "}"
                "]"
            "}"
        "]"
        "}";

    std::string responseBody;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("x-goog-api-key: " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cout << "Error: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return "DefaultName"; // Return a default name in case of error
    }

    long resCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resCode);
    if (resCode != 200) {
        std::cout << "Error: Received HTTP response code " << resCode << std::endl;
        std::cout << "Response body: " << responseBody << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return "DefaultName"; // Return a default name in case of error
    }

    json response = json::parse(responseBody);
    std::string generatedName = response["candidates"][0]["content"]["parts"][0]["text"];

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return generatedName;
}