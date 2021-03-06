/*
    Copyright (C) 2013 Andrey Uzunov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file structures.h
 * @author Andrey Uzunov
 */
 
#include "mhd2spdy_structures.h"


void
free_uri(struct URI * uri)
{
  if(NULL != uri)
  {
    free(uri->full_uri);
    free(uri->scheme);
    free(uri->host_and_port);
    //free(uri->host_and_port_for_connecting);
    free(uri->host);
    free(uri->path);
    free(uri->path_and_more);
    free(uri->query);
    free(uri->fragment);
    uri->port = 0;
    free(uri);
  }
}

int
init_parse_uri(regex_t * preg)
{
  // RFC 2396
  // ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
      /*
        scheme    = $2
      authority = $4
      path      = $5
      query     = $7
      fragment  = $9
      */
  
  return regcomp(preg, "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?", REG_EXTENDED);
}

void
deinit_parse_uri(regex_t * preg)
{
  regfree(preg);
}
  
int
parse_uri(regex_t * preg, char * full_uri, struct URI ** uri)
{
  int ret;
  char *colon;
  long long port;
  size_t nmatch = 10;
  regmatch_t pmatch[10];

  if (0 != (ret = regexec(preg, full_uri, nmatch, pmatch, 0)))
    return ret;
    
  *uri = au_malloc(sizeof(struct URI));
  if(NULL == *uri)
    return -200;
    
  (*uri)->full_uri = strdup(full_uri);
  
  asprintf(&((*uri)->scheme), "%.*s",pmatch[2].rm_eo - pmatch[2].rm_so, &full_uri[pmatch[2].rm_so]);
  asprintf(&((*uri)->host_and_port), "%.*s",pmatch[4].rm_eo - pmatch[4].rm_so, &full_uri[pmatch[4].rm_so]);
  asprintf(&((*uri)->path), "%.*s",pmatch[5].rm_eo - pmatch[5].rm_so, &full_uri[pmatch[5].rm_so]);
  asprintf(&((*uri)->path_and_more), "%.*s",pmatch[9].rm_eo - pmatch[5].rm_so, &full_uri[pmatch[5].rm_so]);
  asprintf(&((*uri)->query), "%.*s",pmatch[7].rm_eo - pmatch[7].rm_so, &full_uri[pmatch[7].rm_so]);
  asprintf(&((*uri)->fragment), "%.*s",pmatch[9].rm_eo - pmatch[9].rm_so, &full_uri[pmatch[9].rm_so]);
  
  colon = strrchr((*uri)->host_and_port, ':');
  if(NULL == colon)
  {
    (*uri)->host = strdup((*uri)->host_and_port);
    /*if(0 == strcasecmp("http", uri->scheme))
    {
      uri->port = 80;
      asprintf(&(uri->host_and_port_for_connecting), "%s:80", uri->host_and_port);
    }
    else if(0 == strcasecmp("https", uri->scheme))
    {
      uri->port = 443;
      asprintf(&(uri->host_and_port_for_connecting), "%s:443", uri->host_and_port);
    }
    else
    {
      PRINT_INFO("no standard scheme!");
      */(*uri)->port = 0;
      /*uri->host_and_port_for_connecting = strdup(uri->host_and_port);
    }*/
    return 0;
  }
  
  port = atoi(colon  + 1);
  if(port<1 || port >= 256 * 256)
  {
    free_uri(*uri);
    return -100;
  }
  (*uri)->port = port;
  asprintf(&((*uri)->host), "%.*s", (int)(colon - (*uri)->host_and_port), (*uri)->host_and_port);
  
  return 0;
}

void
free_proxy(struct Proxy *proxy)
{
  //PRINT_INFO("free proxy called");
  free(proxy->http_body);
  free_uri(proxy->uri);
	free(proxy->url);
	free(proxy->http_uri);
	free(proxy);
}

void *au_malloc(size_t size)
{
  void *new_memory;
  
  new_memory = malloc(size);
  if(NULL != new_memory)
  {
    glob_opt.global_memory += size;
    memset(new_memory, 0, size);
  }
  return new_memory;
}
