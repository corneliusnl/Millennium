#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef _DEBUG
	#define TRACE(msg) 			printf(msg)
	#define TRACE1(msg, x)			printf(msg,x)
	#define TRACE2(msg, x1, x2)		printf(msg, x1, x2)
	#define TRACE3(msg, x1, x2, x3) 	printf(msg, x1, x2, x3)
	#define TRACE4(msg, x1, x2, x3, x4) 	printf(msg, x1, x2, x3, x4)
	#define TRACE5(msg, x1, x2, x3, x4, x5)	printf(msg, x1, x2, x3, x4, x5)

#else
	#define TRACE(msg) 			;
	#define TRACE1(msg, x)			;
	#define TRACE2(msg, x1, x2)		;
	#define TRACE3(msg, x1, x2, x3) 	;
	#define TRACE4(msg, x1, x2, x3, x4) 	;
	#define TRACE5(msg, x1, x2, x3, x4, x5)	;

#endif

#endif /* _TRACE_H_ */
