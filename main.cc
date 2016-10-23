// --------------------------------------------------------------------------
//  _____                    ________   __
// |  __ \                   |  ___\ \ / /
// | |  \/_ __ ___  ___ _ __ | |_   \ V /          Open Source Tools for
// | | __| '__/ _ \/ _ \ '_ \|  _|  /   \            Automated Algorithmic
// | |_\ \ | |  __/  __/ | | | |   / /^\ \             Currency Trading
//  \____/_|  \___|\___|_| |_\_|   \/   \/
//
// --------------------------------------------------------------------------

// Copyright (C) 2014, 2016 Anthony Green <anthony@atgreen.org>
// Distrubuted under the terms of the GPL v3 or later.

// This progam is responsible for publishing OANDA currency exchange
// ticks through an ActiveMQ message broker.

#include <cstdlib>
#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include <activemq/library/ActiveMQCPP.h>
#include <activemq/core/ActiveMQConnectionFactory.h>

using namespace activemq;
using namespace cms;
using namespace std;

#define AMQ_URL "tcp://broker-amq-tcp:61616?wireFormat=openwire"

static Session *session;
static MessageProducer *producer;

struct MemoryStruct {
  char *memory;
  size_t size;
};

static void fatal (const char *msg)
{
  fprintf (stderr, "%s\n", msg);
  exit (1);
}

static size_t httpCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  struct MemoryStruct *mem = (struct MemoryStruct *) userp;
  char *ptr;
  static bool first_connect = true;
  
  if (first_connect)
    {
      std::cout << "SUCCESS" << std::endl;
      first_connect = false;
    }
  
  // Allocate new space for our buffer...
  mem->memory = ptr = (char *) realloc (mem->memory, mem->size + (size * nmemb));
  if (ptr == NULL)
    fatal ("Out of memory");
  
  // Copy the new data to the end of our buffer...
  memcpy (&(ptr[mem->size - 1]), contents, size * nmemb);
  mem->size += size * nmemb;
  ptr[mem->size - 1] = 0;

  do
    {
      // Scan for json delimiters
      int start = 0, brace = 0, end;
      while (ptr[start] && ptr[start] != '{') start++;
      if (ptr[start] == 0)
	return size * nmemb;;
      end = start;
      do {
	switch (ptr[end])
	  {
	  case 0: // incomplete. wait for more.
	    return size * nmemb;
	  case '{': brace++; break;
	  case '}': brace--; break;
	  default: break;
	  }
	end++;
      } while (brace != 0);

      // Temporatily NULL terminate the substring, and parse the json.
      char oldc = ptr[end];
      ptr[end] = 0;
      json_object *jobj = json_tokener_parse (&ptr[start]);

      if (json_object_object_get_ex (jobj, "tick", NULL))
	{
	  std::auto_ptr<TextMessage> message(session->createTextMessage(ptr));
	  producer->send(message.get());
	}
      else
	{
	  if (! json_object_object_get_ex (jobj, "heartbeat", NULL))
	    fprintf (stderr, "Unrecognized data from OANDA: %s\n", &ptr[start]);
	}

      json_object_put (jobj);
  
      // Restore the character we temporarily NULLd
      ptr[end] = oldc;

      // Copy the rest back to the beginning and try again.
      memmove (mem->memory,
	       &ptr[end], 
	       mem->size - end);
      mem->size -= end;
    } while (mem->size > 0);
  
  return size * nmemb;
}

char *getenv_checked (const char *e)
{
  char *v = getenv (e);
  if (!v)
    {
      fprintf (stderr, "ERROR: environment variable '%s' not set.\n", e);
      exit (1);
    }

  return v;
}

int main(void)
{
  CURL *curl_handle;
  CURLcode res;
  char authHeader[100];
  char url[100];
  static char *domain;
  static char *accessToken;
  static char *accounts;

  struct curl_slist *chunk = NULL;

  Connection *connection;
  Destination *destination;

  std::cout << GFX_VERSION_STR "  [git commit " GFX_GIT_COMMIT_HASH "]\nCopyright (C) 2014, 2016  Anthony Green\n" << std::endl;

  domain = getenv_checked ("OANDA_STREAM_DOMAIN");
  accessToken = getenv_checked ("OANDA_ACCESS_TOKEN");
  accounts = getenv_checked ("OANDA_ACCOUNT_ID");
  
  printf ("Program started by User %d\n", getuid());

  activemq::library::ActiveMQCPP::initializeLibrary();

  std::cout << "Connecting to " AMQ_URL " : ";

  try {
      
    // Create a ConnectionFactory
    std::auto_ptr<ConnectionFactory> 
      connectionFactory(ConnectionFactory::createCMSConnectionFactory(AMQ_URL));

    // Create a Connection
    connection = connectionFactory->createConnection(getenv_checked ("AMQ_USER"),
						     getenv_checked ("AMQ_PASSWORD"));
    connection->start();

    session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
    destination = session->createTopic("OANDA.TICK");

    producer = session->createProducer(destination);
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

  } catch (CMSException& e) {

    std::cout << "FAILED" << std::endl;
    // This seems to always print "Success".
    // std::cout << e.getMessage() << std::endl;
    exit (1);
  }

  if (snprintf(authHeader, 100, 
	       "Authorization: Bearer %s", accessToken) >= 100)
    exit(1);
					 
  if (snprintf(url, 100, 
	       "%s/v1/prices?accountId=%s&instruments=USD_CAD,GBP_USD,EUR_JPY", 
	       domain, accounts) >= 100)
    exit(1);

  std::cout << "SUCCESS\nConnecting to " << url << " : ";

  struct MemoryStruct mchunk;
  mchunk.memory = (char *) malloc(1);
  *(mchunk.memory) = 0;
  mchunk.size = 1;

  curl_global_init(CURL_GLOBAL_ALL);

  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, httpCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&mchunk);
  chunk = curl_slist_append(chunk, authHeader);
  res = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);
  res = curl_easy_perform(curl_handle);

  /* check for errors */ 
  if(res != CURLE_OK) 
    {
      fprintf (stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
  else 
    {
      printf ("Disconnected from stream\n");
    }

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);

  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();

  printf ("Program ended\n");

  return 0;
}
