#include <iostream>
#include <curl/curl.h>
#include "json.hpp"  // Include the JSON library

using json = nlohmann::json;

// Callback function to write received data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to retrieve high scores from Firebase
void getTopScores() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        // Your Firebase project URL with the .json extension
        std::string firebaseUrl = "https://midnight-dash-default-rtdb.asia-southeast1.firebasedatabase.app/highscores.json";

        curl_easy_setopt(curl, CURLOPT_URL, firebaseUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Allow insecure connections by disabling SSL verification (for development purposes)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Disable peer verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // Disable hostname verification

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse the JSON response
            auto jsonData = json::parse(readBuffer);
            std::vector<std::pair<std::string, int>> scores;

            for (auto& [key, value] : jsonData.items()) {
                std::string username = value["username"];
                int score = value["score"];
                scores.emplace_back(username, score);
            }

            // Sort scores in descending order
            std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

            // Display top 5 scores
            std::cout << "Top 5 Scores:\n";
            for (size_t i = 0; i < std::min(scores.size(), size_t(5)); ++i) {
                std::cout << (i + 1) << ". " << scores[i].first << ": " << scores[i].second << std::endl;
            }
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }
}

int main() {
    // Retrieve and display top scores from Firebase
    getTopScores();

    return 0;
}
