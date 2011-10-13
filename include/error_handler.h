#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#define ERROR(e) printf("ERROR: %s\n", e)
#define FORMAT_ERROR printf("ERROR: Bad command format.\n")
#define IO_OPEN_ERROR printf("ERROR: Couldn't open file.\n")
#define IO_READ_ERROR printf("ERROR: Couldn't read from file.\n")
#define IO_WRITE_ERROR printf("ERROR: Couldn't write in file.\n")

#endif
