/*
  chktld -- Check to make sure a TLD exists

  Very simple program demonstrating the use of res_query() to verify a
  top-level domain exists.  Accepts strings that may or may not have a
  terminating ".".

  Assumes:

  - An active Internet connection
  - A configured DNS resolver

  Returns:

  -  0 if the supplied TLD exists
  -  1 if the supplied TLD does not exist
  - -1 if an error occured

  Limitations:

  - Doesn't deal with TLDs that have embedded (escaped) "."s
  - Doesn't actually look at the response to make sure the SOA
    returned matches the question.

* Copyright (c) 2006 ICANN. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of ICANNnor the names of its contributors may be 
*       used to endorse or promote products derived from this software 
*       without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ICANN AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ICANN OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/
  

#define BIND_8_COMPAT

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>

int main( int argc, char *argv[] ){
	int rc;
	unsigned char answer[ 512 ];
	char *dot;

	if( argc != 2 ){
		printf( "\nUsage: %s <tld>\n", basename( argv[ 0 ]));
		printf( "\nWhere\n\t<tld> is the top-level-domain to check\n" );
		printf( "\nReturns\n" );
		printf( "\t 0 if <tld> exists\n" );
		printf( "\t 1 if <tld> does not exist\n" );
		printf( "\t-1 if an error occured\n" );
		printf( "\nRequires a configured DNS resolver\n\n" );
		exit( -1 );
	}

	dot = strchr( argv[ 1 ], '.' );
	if( dot != NULL && dot[ 1 ] != '\0' ){
		fprintf( stderr, "TLDs must not contain '.'\n" );
		exit( -1 );
	}

	if( res_init()){
		fprintf( stderr, "Unable to initialize DNS resolver\n" );
		exit( -1 );
	}

	rc = res_query( argv[ 1 ], C_IN, T_SOA, answer, sizeof( answer ));
	if( rc > 0 ){
		printf( "TLD \"%s\" exists\n", argv[ 1 ]);
		exit( 0 );
	}
	
	switch( h_errno ){
	case HOST_NOT_FOUND: /* Authoritative Answer Host not found */
	case NO_DATA: /* Valid name, no data record of requested type */
		printf( "TLD \"%s\" does not exist\n", argv[ 1 ]);
		exit( 1 );
		break;
		
	case TRY_AGAIN: /* Non-Authoritative Host not found, or SERVERFAIL */
		fprintf( stderr, "DNS server failed.\n" );
		break;
			
	case NO_RECOVERY: /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
		fprintf( stderr, "FORMERR, NOTIMP, REFUSED, or other error\n" );
		break;
			
	default:
		fprintf( stderr, "Unknown error: %d\n", h_errno );
	}
	exit( -1 );
}
