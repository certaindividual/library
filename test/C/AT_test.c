/**
 * @brief A simple file to check the library and enable debugging
 */

/*
 *    AT_test.c
 *    ===================
 *
 *    Created on: 2009-06-08
 *    Creator: kongruencja
 *
 *    Copyright 2006, 2010 The libamtrack team
 *
 *    This file is part of the AmTrack program (libamtrack.sourceforge.net).
 *
 *    AmTrack is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    AmTrack is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with AmTrack (file: copying.txt).
 *    If not, see <http://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "AT.h"
#include "config.h"

int main(){

	const double 	a = 2, b = 3;
	double 			result = AT_test_fun(a, b);

	char			version_tag[255];
	strcpy(version_tag, VERSION);		// from config.h <-- ./configure
	strcat(version_tag, "r");
	strcat(version_tag, SVN_REVISION);  // from config.h <-- ./configure

	if(result == a + b){
		printf("libamtrack %s is working.\n", version_tag);
		return EXIT_SUCCESS;
	}else{
		printf("libamtrack %s is NOT working properly [problem in calling AT_test_fun()].\n", version_tag);
		return EXIT_FAILURE;
	}
};

