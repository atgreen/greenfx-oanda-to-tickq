// --------------------------------------------------------------------------
//  _____                    ________   __
// |  __ \                   |  ___\ \ / /
// | |  \/_ __ ___  ___ _ __ | |_   \ V /          Open Source Tools for
// | | __| '__/ _ \/ _ \ '_ \|  _|  /   \            Automated Algorithmic
// | |_\ \ | |  __/  __/ | | | |   / /^\ \             Currency Trading
//  \____/_|  \___|\___|_| |_\_|   \/   \/
//
// --------------------------------------------------------------------------

// Copyright (C) 2014, 2016  Anthony Green <green@spindazzle.org>
// Distrubuted under the terms of the GPL v3 or later.

// This progam is responsible for publishing OANDA currency exchange ticks
// through an ActiveMQ message broker.

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

static char *domain;
static char *accessToken;
static char *accounts;

static Session *session;
static MessageProducer *producer;

struct MemoryStruct {
  char *memory;
  size_t size;
};

static void fatal (const char *msg)
{
  syslog (LOG_ERR, msg);
  exit (1);
}

static size_t httpCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  struct MemoryStruct *mem = (struct MemoryStruct *) userp;
  mem->memory = (char *) realloc (mem->memory, mem->size + (size * nmemb) + 1);
  if (mem->memory == NULL)
    fatal ("Out of memory");
  
  memcpy (&(mem->memory[mem->size]), contents, size * nmemb);
  mem->size += size * nmemb;
  mem->memory[mem->size] = 0;
  int length;
  char *eol = strchr(mem->memory, '\r');

  while (eol)
    {
      length = eol - mem->memory;
      *eol = 0;

      json_object *jobj = json_tokener_parse (mem->memory);
      
      if (json_object_object_get_ex (jobj, "tick", NULL))
	{
	  syslog (LOG_NOTICE, "%s", mem->memory); 
	  std::auto_ptr<TextMessage> message(session->createTextMessage(mem->memory));
	  producer->send(message.get());
	}
      else
	{
	  if (! json_object_object_get_ex (jobj, "heartbeat", NULL))
	    syslog (LOG_ERR, "Unrecognized data from OANDA: %s", mem->memory);
	}
      eol++; length++;
      while (*eol == '\n' || *eol == '\r')
	{ eol++; length++; }
      
      mem->size -= length;
      memcpy (mem->memory, eol, mem->size);

      eol = strchr(mem->memory, '\r');
    }

  return size * nmemb;
}

char *getenv_checked (const char *e)
{
  char *v = getenv (e);
  if (!v)
    {
      fprintf (stderr, "ERROR: environment variable '%s' not set.", e);
      exit (1);
    }

  return v;
}

void config()
{
  domain = getenv_checked ("OANDA_STREAM_DOMAIN");
  accessToken = getenv_checked ("OANDA_ACCESS_TOKEN");
  accounts = getenv_checked ("OANDA_ACCOUNT_ID");
}

int main(void)
{
  CURL *curl_handle;
  CURLcode res;
  char authHeader[100];
  char url[100];
  struct curl_slist *chunk = NULL;

  string brokerURI = getenv_checked ("GREENFX_AMQ_TCP_PORT_61616_TCP");
  Connection *connection;
  Destination *destination;

  config();
  
  openlog ("oanda-ticks", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  syslog (LOG_NOTICE, "Program started by User %d", getuid());

  activemq::library::ActiveMQCPP::initializeLibrary();

  try {
      
    // Create a ConnectionFactory
    std::auto_ptr<ConnectionFactory> 
      connectionFactory(ConnectionFactory::createCMSConnectionFactory(brokerURI));

    // Create a Connection
    connection = connectionFactory->createConnection(getenv_checked ("AMQ_USER"),
						     getenv_checked ("AMQ_PASSWORD"));
    connection->start();

    syslog (LOG_NOTICE, "A");
      
    session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
    destination = session->createTopic("OANDA.TICK");

    producer = session->createProducer(destination);
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

  } catch (CMSException& e) {

    syslog (LOG_ERR, e.getStackTraceString().c_str());
    exit (1);

  }

  if (snprintf(authHeader, 100, 
	       "Authorization: Bearer %s", accessToken) >= 100)
    exit(1);
  if (snprintf(url, 100, 
	       "%s/v1/prices?accountIds=%s&instruments=USD_CAD", 
	       domain, accounts) >= 100)
    exit(1);

  syslog (LOG_NOTICE, ">> %s", url);

  struct MemoryStruct mchunk;
  mchunk.memory = (char *) malloc(1);
  mchunk.size = 0;

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
      syslog (LOG_ERR, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }
  else 
    {
      syslog (LOG_NOTICE, "disconnected from stream");
    }

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);

  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();

  syslog (LOG_NOTICE, "Program ended");

  return 0;
}

