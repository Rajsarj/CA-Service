#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <curl/curl.h>

void write_char_array(std::ostream& os, const char* string) {
    // faster than an ostream_iterator and std::copy
    os.write(string, strlen(string));
}

 struct memory {
   char *response;
   size_t size;
 };
 
 static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
 {
   std::cout << "....Callback Received !" << std::endl;
   size_t realsize = size * nmemb;
   struct memory *mem = (struct memory *)userp;
 
   char *ptr = (char *) realloc(mem->response, mem->size + realsize + 1);
   if(ptr == NULL)
     return 0;  /* out of memory! */
 
   mem->response = ptr;
   memcpy(&(mem->response[mem->size]), data, realsize);
   mem->size += realsize;
   mem->response[mem->size] = 0;
 
   
   return realsize;
 }

 size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }
    return newLength;
}


/*
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}*/

int ReadFile( char * buffer)
{
    //open file
  std::ifstream infile("./Test.txt");

  //get length of file
  infile.seekg(0, std::ios::end);
  size_t length = infile.tellg();
  infile.seekg(0, std::ios::beg);

  // don't overflow the buffer!
  if (length > sizeof (buffer))
  {
      length = sizeof (buffer);
  }

  //read file
  infile.read(buffer, length);
  return length;
}

int main(void)
{
  CURL *curl;
  CURLcode res;
  struct memory chunk;

/*
  std::stringstream ss;
        ss << "aufbau1,ort=halle ";
        ss << std::endl;

  std::string readBuffer = ss.str();
  const char *dataChar = readBuffer.c_str();
  std::cout << "Data : " << dataChar << std::endl;
  std::cout << "..Size : " << sizeof(dataChar) << std::endl;
*/
  char buffer[10000];
  size_t length = 0;
  {
      std::ifstream infile("./ML.csr");

      //get length of file
      infile.seekg(0, std::ios::end);
      length = infile.tellg();
      infile.seekg(0, std::ios::beg);

      // don't overflow the buffer!
      if (length > sizeof (buffer))
      {
          length = sizeof (buffer);
      }

      //read file
      infile.read(buffer, length);
  }

  std::cout << "Buffer : [" << buffer << "]" << std::endl;
  std::cout << "..Buffer Size : " << length << std::endl;



  struct curl_slist *headerlist = NULL;
  headerlist = curl_slist_append(headerlist, "Content-Type: application/x-protobuf");

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  std::string s;
  if(curl) 
  {
    curl_easy_setopt(curl, CURLOPT_URL, "http://3.235.138.65/ProgressRail?rs:client=Mogembo");
    
    //curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, cb);
    //curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_USERPWD, "admin:admin");

    //curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
    curl_easy_setopt(curl, CURLOPT_POST, true);
    //curl_easy_setopt(curl, CURLOPT_HEADER, true);

    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, sizeof(dataChar));
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataChar);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, length);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) 
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    else 
    {
      //printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
      //printf("Data : [%s]\n", chunk.response);
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    std::cout<< "[" << s <<"]"<<std::endl;
    std::ofstream out("./ML.pem");
    out << s;
    out.close();
  //write_char_array(std::cout, chunk.response);
    //std::cout << readBuffer << std::endl;
  }
  return 0;
}