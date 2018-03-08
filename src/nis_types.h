#ifndef NIS_TYPES
#define NIS_TYPES

typedef void *NISHandle;
typedef void (*nis_callback_t)(char *const nisData, uint16_t lenData);

enum TokResult { 
		TOK_RESULT_OK, 
		TOK_RESULT_NOT_CONNECTED, 
		TOK_RESULT_READ_ERROR, 
		TOK_RESULT_GENERIC_ERROR 
};

enum ReaderResult { 
		READER_RESULT_OK, 
		READER_RESULT_GENERIC_ERROR 
};

enum { NIS_BACKEND_NONE = 0, NIS_BACKEND_PCSC = 1 };

#endif
