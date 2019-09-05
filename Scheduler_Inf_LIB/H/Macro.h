//--------------------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
typedef enum {
	SYS_AVAILABLE ,
	SYS_RUNNING ,
	SYS_ABORTED ,
	SYS_SUCCESS ,
	SYS_ERROR ,
	SYS_WAITING
} MdlSts;
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_DEFINF_API( TYPE_NAME , VAR_NAME , API_NAME ) \
TYPE_NAME VAR_NAME; \
__declspec(dllexport) void API_NAME( TYPE_NAME api ) { \
	VAR_NAME = api; \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//VAR MACRO
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG0( TYPE_NAME , NAME , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( void ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( void ) { \
	rettype (*_API##NAME##)(); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG1( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 ) { \
	rettype (*_API##NAME##)( ARG1 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG2( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG3( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG4( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG5( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG6( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG7( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG8( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , TYPE_ARG8 , ARG8 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 , ARG8 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG9( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , TYPE_ARG8 , ARG8 , TYPE_ARG9 , ARG9 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 , ARG8 , ARG9 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG10( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , TYPE_ARG8 , ARG8 , TYPE_ARG9 , ARG9 , TYPE_ARG10 , ARG10 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 , ARG8 , ARG9 , ARG10 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG11( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , TYPE_ARG8 , ARG8 , TYPE_ARG9 , ARG9 , TYPE_ARG10 , ARG10 , TYPE_ARG11 , ARG11 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 , ARG8 , ARG9 , ARG10 , ARG11 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG12( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , TYPE_ARG8 , ARG8 , TYPE_ARG9 , ARG9 , TYPE_ARG10 , ARG10 , TYPE_ARG11 , ARG11 , TYPE_ARG12 , ARG12 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 , TYPE_ARG12 ARG12 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 , TYPE_ARG12 ARG12 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 , ARG8 , ARG9 , ARG10 , ARG11 , ARG12 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG13( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , TYPE_ARG8 , ARG8 , TYPE_ARG9 , ARG9 , TYPE_ARG10 , ARG10 , TYPE_ARG11 , ARG11 , TYPE_ARG12 , ARG12 , TYPE_ARG13 , ARG13 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 , TYPE_ARG12 ARG12 , TYPE_ARG13 ARG13 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 , TYPE_ARG12 ARG12 , TYPE_ARG13 ARG13 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 , ARG8 , ARG9 , ARG10 , ARG11 , ARG12 , ARG13 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VAR_ARG14( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , TYPE_ARG5 , ARG5 , TYPE_ARG6 , ARG6 , TYPE_ARG7 , ARG7 , TYPE_ARG8 , ARG8 , TYPE_ARG9 , ARG9 , TYPE_ARG10 , ARG10 , TYPE_ARG11 , ARG11 , TYPE_ARG12 , ARG12 , TYPE_ARG13 , ARG13 , TYPE_ARG14 , ARG14 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 , TYPE_ARG12 ARG12 , TYPE_ARG13 ARG13 , TYPE_ARG14 ARG14 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME _SCH_MACRO_##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 , TYPE_ARG5 ARG5 , TYPE_ARG6 ARG6 , TYPE_ARG7 ARG7 , TYPE_ARG8 ARG8 , TYPE_ARG9 ARG9 , TYPE_ARG10 ARG10 , TYPE_ARG11 ARG11 , TYPE_ARG12 ARG12 , TYPE_ARG13 ARG13 , TYPE_ARG14 ARG14 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 , ARG4 , ARG5 , ARG6 , ARG7 , ARG8 , ARG9 , ARG10 , ARG11 , ARG12 , ARG13 , ARG14 ); \
}







//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//VDN MACRO
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VDNx_ARG0( TYPE_NAME , NAME , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE_##NAME## ) ( void ); \
_API_DEFINE_##NAME _API_##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API_##NAME##( _API_DEFINE_##NAME api ) { \
	_API_##NAME## = api; \
} \
TYPE_NAME NAME##( void ) { \
	rettype (*_API_##NAME##)(); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VDN_ARG1( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME NAME##( TYPE_ARG1 ARG1 ) { \
	rettype (*_API##NAME##)( ARG1 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VDN_ARG2( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_VDN_ARG3( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , rettype ) \
typedef TYPE_NAME ( *_API_DEFINE##NAME## ) ( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 ); \
_API_DEFINE##NAME _API##NAME##; \
__declspec(dllexport) void _SCH_INF_DLL_API##NAME##( _API_DEFINE##NAME api ) { \
	_API##NAME## = api; \
} \
TYPE_NAME NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 ) { \
	rettype (*_API##NAME##)( ARG1 , ARG2 , ARG3 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------






//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//FDN MACRO
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_FDN_ARG1( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , rettype ) \
TYPE_NAME NAME( TYPE_ARG1 ARG1 ); \
__declspec(dllexport) TYPE_NAME _I##NAME##( TYPE_ARG1 ARG1 ) { \
	rettype NAME( ARG1 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_SCHEDULER_MACRO_FDN_ARG2( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , rettype ) \
TYPE_NAME NAME( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 ); \
__declspec(dllexport) TYPE_NAME _I##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 ) { \
	rettype NAME( ARG1 , ARG2 ); \
}
//--------------------------------------------------------------------------------------------------------------------------------------------
//#define DEFINE_SCHEDULER_MACRO_FDN_ARG4( TYPE_NAME , NAME , TYPE_ARG1 , ARG1 , TYPE_ARG2 , ARG2 , TYPE_ARG3 , ARG3 , TYPE_ARG4 , ARG4 , rettype ) \
//TYPE_NAME NAME( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 ); \
//__declspec(dllexport) TYPE_NAME _I##NAME##( TYPE_ARG1 ARG1 , TYPE_ARG2 ARG2 , TYPE_ARG3 ARG3 , TYPE_ARG4 ARG4 ) { \
//	rettype NAME( ARG1 , ARG2 , ARG3 , ARG4 ); \
//}
