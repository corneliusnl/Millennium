// ------------------------------------------------------------
// String.h Include File
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------

//
// Function Prototypes
//
extern char *strcpy	(char *dest, const char *src);
extern char	*strncpy	(char *dest, const char *src, size_t maxlen);
extern char	*strcat	(char *dest, const char *src);
extern char	*strncat	(char *dest, const char *src, size_t maxlen);
extern long	strcmp	(const char *s1, const char *s2);
extern long	strncmp	(const char *s1, const char *s2, size_t maxlen);
extern long	strlen	(const char *s);
extern char	*strchr	(const char *s, char c);
