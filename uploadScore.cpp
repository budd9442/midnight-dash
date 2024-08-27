
#include <iostream>
#include <curl/curl.h>
using namespace std;

int main(int argc, char *argv[])  //./uploadscore <name> <score>
{
    string username = argv[1]; // first argument
    string score = argv[2]; // second argument

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        std::string firebaseUrl = "https://midnight-dash-default-rtdb.asia-southeast1.firebasedatabase.app/highscores.json"; 
        std::string jsonData = "{\"username\":\"" + username + "\", \"score\":" + (score) + "}";

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
