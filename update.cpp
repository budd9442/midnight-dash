#include <iostream>
#include <curl/curl.h>
#include <string>
#include <sstream>

// Function to write the response data to a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// Function to fetch the current content from the Pastebin paste
std::string fetchPasteContent(const std::string& pasteUrl) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, pasteUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request and check the result
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        // Cleanup
        curl_easy_cleanup(curl);
    }
    return response;
}

// Function to update the Pastebin paste with new content
void updatePasteContent(const std::string& pasteUrl, const std::string& newContent) {
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        // Prepare the URL with the password
        std::string postUrl = pasteUrl + "?api_dev_key=LqrM7gN523";

        // Set the URL for the POST request
        curl_easy_setopt(curl, CURLOPT_URL, postUrl.c_str());

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, newContent.c_str());

        // Perform the request and check the result
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        else
            std::cout << "Paste updated successfully." << std::endl;

        // Cleanup
        curl_easy_cleanup(curl);
    }
}

int main() {
    // URL of the paste and its raw content
    std::string pasteUrl = "https://pastebin.com/raw/d2f8GTeW";

    // Fetch current content from the Pastebin paste
    std::string currentContent = fetchPasteContent(pasteUrl);

    // Append new content to the existing content
    std::string newContent = currentContent + "\nAppended Text!";

    // Update the Pastebin paste with the new content
    updatePasteContent(pasteUrl, newContent);

    return 0;
}
