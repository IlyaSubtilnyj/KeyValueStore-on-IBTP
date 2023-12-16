#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Function to generate a random string of a given length
unsigned char* generate_string(int length) {
	static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
	unsigned char* randomString = (unsigned char*)malloc(sizeof(char) * (length + 1));
	if (randomString) {
		for (int i = 0; i < length; i++) {
			int key = rand() % (int)(sizeof(charset) - 1);
			randomString[i] = charset[key];
		}
		randomString[length] = '\0';
		return randomString;
	}
	else {
		printf("No memory");
		exit(1);
	}
}