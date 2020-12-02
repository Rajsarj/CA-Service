#include <iostream>
#include <string>
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(void)
{
    printf("...TEst");

  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();

    return 1;
}