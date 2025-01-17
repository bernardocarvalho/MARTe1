/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id$
 *
**/

/**
 * VxWorks Motorola 68000 implementation
 */
#ifndef SYSTEM_VX_68K_H
#define SYSTEM_VX_68K_H

#if defined(_VX68K)

#define PPC_PLATFORM

extern "C"
{
#include <vplsServLib.h>
}
#include <vxWorks.h>
#include <vxLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <logLib.h>
#include <semLib.h>
#include <sockLib.h>
#include <selectLib.h>
#include <errnoLib.h>
#include <stdio.h>
#include <setjmp.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
//#include <typeinfo>
//#include <io.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/ioctl.h>
#include <ioLib.h>
#include <dirent.h>
#include <stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "GenDefs.h"

#include <float.h>
#include <math.h>
#include <iv.h>
#include <vxLib.h>
#include <intLib.h>
#include <fcntl.h>
#include <hostLib.h>
#include <symLib.h>

// compatibility to OS2 IBM sockets
// sock_init() moved to InternetAddress.cpp

#define _EXTERN_ extern

#define sock_errno() errno
#define psock_errno  perror
#define soclose      close


#define __thread
#define __thread_decl

//#define  vsnprintf  _vsnprintf
#define HANDLE int
#define HFILE HANDLE
#define HMTX  SEM_ID
//#define HEV   HANDLE
#define TID   HANDLE
//#define HFILE HANDLE
//#define APIRET DWORD

#define  SEM_INDEFINITE_WAIT 0xFFFFFFFF
//#define  INFINITE 0xFFFFFFFF

#if 0
VERY DANGEROUS ! (FILIPPO comment)
static inline int     vsnprintf (char *p, size_t s, const char *f, va_list v){
    vsprintf(p,f,v);
}
#endif

/**  implemented in BString.cpp */
int snprintf(char *s, size_t  n, const char *fmt, ...);

/**  implemented in BString.cpp */
int vsnprintf(char *s, size_t  n, const char *fmt, va_list ap);

struct  hostent {
        char    *h_name;        /* official name of host */
        char    **h_aliases;    /* alias list */
        int     h_addrtype;     /* host address type */
        int     h_length;       /* length of address */
        char    **h_addr_list;  /* list of addresses from name server */
#define h_addr  h_addr_list[0]  /* address, for backward compatiblity */
};

struct  servent {
        char    *s_name;        /* official service name */
        char    **s_aliases;    /* alias list */
        int     s_port;         /* port # */
        char    *s_proto;       /* protocol to use */
};


static inline struct hostent  *gethostbyname(char *name) { return NULL; }

static inline struct hostent  *gethostbyaddr(const char *name, int len , int type) { return NULL; }

static inline struct servent *getservbyname(const char *name,  const  char *proto) {  return NULL; }

static inline struct servent *getservbyport(int port, const char *proto) { return NULL; }


inline int strncasecmp(const char *s1, const char *s2, int n ){
    if (s1 == s2) return 0;
    if (s1 == NULL) return -1;
    if (s2 == NULL) return 1;

     register char c1= toupper(*s1);
     register char c2= toupper(*s2);
     while ((c1!=0) && (c2!=0) && (n>0)){
         if (c1>c2) return 1;
         if (c1<c2) return -1;
         s1++;
         s2++;
         n--;
         c1= toupper(*s1);
         c2= toupper(*s2);
     }
     if (n==0) return 0;
     if ((c1==0) && (c2==0)) return 0;
     if (c1==0) return -1;
     return 1;
}

#define IEEE_FLOAT_NAN 0x7fC00000
#define IEEE_FLOAT_INF 0x7f800000
#define IEEE_FLOAT_INF_NEG 0xFf800000

#endif
#endif

