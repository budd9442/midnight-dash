
#include <iostream>
#include <curl/curl.h>
using namespace std;

int main()
{
    std::string username = "Player1";
    int score = 1000;

    CURL *curl;
    CURLcode res;
    // curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    curl = curl_easy_init();
    if (curl)
    {
        std::string firebaseUrl = "https://midnight-dash-default-rtdb.asia-southeast1.firebasedatabase.app/highscores.json";
        std::string jsonData = "{\"username\":\"" + username + "\", \"score\":" + std::to_string(score) + "}";

        curl_easy_setopt(curl, CURLOPT_URL, firebaseUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable peer verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable hostname verification

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    std::cout << "High score uploaded!" << std::endl;

    return 0;
}
